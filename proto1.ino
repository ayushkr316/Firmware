#include "FS.h"
#include "SPIFFS.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "MD5.h"
//#include "SD.h"
#include <MD5Builder.h>
#include <SPI.h>
#include <SD.h>
#include "OneButton.h"
#include <ArduinoJson.h>
//#include <TinyGPS++>
#define chipSelect 5
int i, SessionMode=0;
String ClickStatus="";

char* ssid = "Zuarifurnituresls5.1";
char* password = "bedrock@8540";

AsyncWebServer server(80);

#define FORMAT_SPIFFS_IF_FAILED true
#define chipSelect 5

OneButton button(13, true);  

//json vaiable/object
File myFile;

String fileName ="/something.txt"; //json file name
int fileValue=0;



MD5Builder _md5;

String md5(String str) {
    _md5.begin();
    _md5.add(String(str));
    _md5.calculate();
    return _md5.toString();
}

void createJsonFile()
{
  //gps.encode(RTK.read());
 //fileName = "NEW.txt";//String(gps.time.value())+"_"+String(gps.date.value())+"_UTC_Time_zone.txt";
  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) {
    for (i = 0; i <= 7; i++) {
      myFile.print(i);
      myFile.println("/");
    }
    myFile.close();
    Serial.println("Data written");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Surveying Session started\nNo file to open...");
  }
}
void SD_Card_Setup();
void logJson()
{
  //starting logging JSON
  Serial.println("loggingstarted");
  myFile = SD.open(fileName,FILE_APPEND);
  //myFile.seek(2);
  if(myFile)
  {
    Serial.println("File opened for appending new data");
  }
  DynamicJsonDocument  dataPacketJson(200);
  {
    Serial.println("logging started");
    dataPacketJson["Latitude"] = "gps.location.rawLat().deg";
    dataPacketJson["Longitude"] = "gps.location.rawLng().deg";
    dataPacketJson["Date"] = "gps.date.value()";
    dataPacketJson["Time"] = "gps.time.value()";
    dataPacketJson["Speed"] = "gps.speed.kmph()";
    dataPacketJson["Altitude"] = "gps.altitude.meters()";
    dataPacketJson["Sattelites"] = "gps.satellites.value()";
    dataPacketJson["HDOP"]= "gps.hdop.value()";
    Serial.println("data packet created");
    if (myFile) 
    {
      Serial.println("File opened and logging started");
     //writting data on the file
      Serial.print("Filename --> ");
      Serial.println(fileName);
      Serial.println("Writting JSON on the file ...");
      //myFile.seek(2);
      serializeJsonPretty(dataPacketJson, myFile);
      Serial.println("...");
      myFile.close();
    }
  }
}
String listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    String files = "";

    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return "failed to open directory";
    }
    if(!root.isDirectory()){
        //Serial.println(" - not a directory");
        return "not a directory";;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            files = String(files + file.name() + " ");
        }
        file = root.openNextFile();
    }

  return files;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void setup() {
    Serial.begin(115200);
    SD_Card_Setup();
  button.attachDoubleClick(doubleclick);            // link the function to be called on a doubleclick event.
  button.attachClick(singleclick);                  // link the function to be called on a singleclick event.
  button.attachLongPressStop(longclick);
  
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    if(!SD.begin(chipSelect)){
        Serial.println("Card Mount Failed!");
        return;
    }
    else{
        Serial.println("Card mounted successfully!");
    }
    uint8_t cardType = SD.cardType();
    
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }
  
    listDir(SD, "/", 0);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", "ESP 32 web server");
    });

    server.on("/files", HTTP_GET, [](AsyncWebServerRequest *request){
        String files = listDir(SD, "/", 0); //list files in directory
        request->send(200, "text/plain", files);
    });

    server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request){
        int fnsstart = request->url().lastIndexOf('/');
        String fn = request->url().substring(fnsstart);
        Serial.println("Download File: "+fn);
        // ... and finally
        request->send(SD, fn, String(), true);
    });

    server.on("/command", HTTP_GET, [](AsyncWebServerRequest *request){
        if(checksum(request->url())){
            request->send(200, "text/plain", "ACK");  
        }
        else{
            request->send(200, "text/plain", "NACK: Error in command transmission");  
        }
    });

    server.begin();
}

bool checksum(String commhash){
    int fnsstart = commhash.indexOf('$');
    int l= commhash.length() - 32;
    String hash = commhash.substring(l, commhash.length());
    String command = commhash.substring(fnsstart, l);

    if(hash == md5(command)){
        return true;
    }
    else{
        return false;
    }
}

void loop(){
   button.tick();
   delay(20);
    if (Serial.available()) {
        String comm=Serial.readString();
        Serial.println(comm);
        if(comm[0]=='$') {
            if(checksum(comm)) {
                Serial.println("ACK");  
            } else{
                Serial.println("NACK: Error in command transmission");
            }
        }
    }
}

void SD_Card_Setup()
{
   while (!Serial);

  Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset or reopen this serial monitor after fixing your issue!");
    while (true);
  }

  Serial.println("SD card initialised!");
}


void doubleclick()                                  // what happens when button is double-pressed
{                                
 Serial.println("Double click - Data to be logged"); 
 ClickStatus="DoubleClick";
 logJson();                            
} 
 
void singleclick()                                  // what happens when the button is clicked
{                                 
  Serial.println("Single click - window to be changed");
  ClickStatus="SingleClick";                             
}
 
void longclick()                                    // what happens when button is long-pressed
{                                   
  Serial.println("Long click - Session creation and destruction");
  ClickStatus="LongClick";
  SessionMode=!SessionMode;
  if (SessionMode==1)
  {
    createJsonFile(); 
                             
  }
  if (SessionMode==0)
  {
   if (SD.exists(fileName)) 
   {

    Serial.println(String(fileName)+" Exists");

  }
    else 
    {

      Serial.println("Surveying session Terminated \nNo file found.");

    }
  }
}
