#include "FS.h"
#include <WiFi.h>
#include "ESPAsyncWebServer.h"

#include <SPI.h>
#include <SD.h>
#include "OneButton.h"

// For PX1122r
#include <HardwareSerial.h>

#define chipSelect 5
int i, SessionMode=0;
String ClickStatus="";
int timer=0;
int randNum=0;
char* ssid = "$EZ_RTK";
char* password = "1234567890";

String globe = "Error: PX1122R Module not connected.";
String str0,str1;
const unsigned long eventInterval = 2000;
unsigned long previousTime = 0;
int ck = 0;

AsyncWebServer server(80);
HardwareSerial MySerial(1);
//// Set your Static IP address
//IPAddress local_IP(192, 168, 5, 1);
//// Set your Gateway IP address
//IPAddress gateway(192, 168, 1, 1);
//// Set your Subnet mask
//IPAddress subnet(255, 255, 255, 0);

OneButton button(21, true);  

File myFile;

String fileName;
int fileValue=0;

void hostServer();
void SD_Card_Setup();
void parser();
void createJsonFile()
{
  //gps.encode(RTK.read());
  //fileName = filename.toString();//String(gps.time.value())+"_"+String(gps.date.value())+"_UTC_Time_zone.txt";
  fileName="/EZ_RTK_"+String(random(0,300))+".json";
  Serial.println("Surveying session STARTED!!\n");
  Serial.println("Your file name is : "+String(fileName));
  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) 
  {
    Serial.println("Succesfully created the file\n");
  } 
  else 
  {
    // if the file didn't open, print an error:
    Serial.println("No file to open.....");
  }
  if (!SD.exists(fileName))
  Serial.println("NO SUCH FILE FOUND");
}

void logJson()
{
  //starting logging JSON
  Serial.println("Opening file for appendng new data!!");
  myFile = SD.open(fileName,FILE_APPEND);
  //myFile.seek(2);
  if(myFile)
  {
    Serial.println("File opened for appending new data");
    myFile.println ("{\nTime : String(gps.time.value())");
    myFile.println ("Date : String(gps.date.value())");
    myFile.println ("Latitude : String(gps.location.rawLat().deg)");
    myFile.println ("Longitude : String(gps.location.rawLng().deg)");
    myFile.println ("Speed : String(gps.speed.kmph())");
    myFile.println ("Altitude : String(gps.altitude.meters())");
    myFile.println ("Sattelites : String(gps.satellites.value())");
    myFile.println ("HDOP : String(gps.hdop.value())");
    myFile.println ("Time : String(gps.time.value())\n}");
    myFile.println("!");
    Serial.println("Data logged successfully...\n\n");
    myFile.close();
  }
  
  else{
    Serial.println("\nPlease start a session before logging the data!!!");
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

void deleteFile(fs::FS &fs, String path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void setup() {
  Serial.begin(115200);
  MySerial.begin(9600, SERIAL_8N1, 16, 17);//rx,tx
  
  SD_Card_Setup();

  WiFi.mode(WIFI_MODE_APSTA);
  Serial.println("Setting AP However you can connect as Station as well…");

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  hostServer();
    
  button.attachDoubleClick(doubleclick);            // link the function to be called on a doubleclick event.
  button.attachClick(singleclick);                  // link the function to be called on a singleclick event.
  button.attachLongPressStop(longclick);
  

  if(!SD.begin(chipSelect))
  {
    Serial.println("Card Mount Failed!");
    return;
  }
  else
  {
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
}


void loop()
{
   button.tick();
   delay(10);
   parser();
//   hostServer();

//    Code for reading NMEA Data from PX or a simulator
  
  if(MySerial.available()){
    char temp = MySerial.read();
    unsigned long currentTime = millis();

    if(currentTime - previousTime >= eventInterval){
        if(ck==0){
          ck = 1;
          globe = str0;
          str0 = "";
          Serial.print(globe);
        }
        else if(ck==1){
          ck = 0;
          globe = str1;
          str1 = "";
          Serial.print(globe);
        }
        previousTime = currentTime;
    }
    else{
      if(ck == 0){
        str0 += temp;  
      }
      else{
        str1 += temp;  
      }
    }
  }
}

void SD_Card_Setup()
{
   while (!Serial);

  Serial.println("Initializing SD card...");

  if (!SD.begin(chipSelect)) 
  {
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
  Serial.print("\n\nLong click - ");
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
    Serial.println("Surveying session TERMINATED\nYour data logged inside file : "+String(fileName));
    fileName="";
  }
    else 
    {

      Serial.println("Surveying session Terminated \nNo file found.");

    }
  }
}
void parser()
{
  int strIndex=0;
  String SSID1, PASS;
  char ssid[30],c,d;
  char password[30];
  String command;

  while (Serial.available()) 
  {
    c = Serial.read();  //gets one byte from serial buffer
    command += c; //makes the String command
    delay(8);  //slow looping to allow buffer to fill with next character
  }

  if (command.length() > 0) 
  {
    if(command.equals("$EZ_RTK|SET-MODE|ROVER"))
    {
      Serial.println("Thanks for using EZ_RTK, your RTK has been configured as a ROVER ");
      command="";
    }
    else if(command.equals("$EZ_RTK|SET-MODE|BASE"))
    {
      Serial.println("Thanks for using EZ_RTK, your RTK has been configured as a BASE");
      command="";
    }
    else if(command.startsWith("$EZ_RTK|SET-WIFI|", 0))
    {
      Serial.println("Command : "+command);
      strIndex=command.lastIndexOf('|');
      SSID1=command.substring(17, strIndex);
      PASS=command.substring(strIndex+1);
      Serial.println("SSID="+SSID1);//String value parsed
      Serial.println("Password="+PASS);//String value parsed
     // SSID1.toCharArray(ssid, SSID1.length()+1);
      //ssid[]=SSID1;
      //password[]=Password;
      
        SSID1.toCharArray(ssid, SSID1.length()+1);
        PASS.toCharArray(password, PASS.length()+1);
        //SSID and Password broken into character array from string and then printed below
      
      Serial.println("Thanks for using EZ_RTK, your settings for Wifi has been configured as follows:\nSSID :"+String(ssid)+"."+"\nPassword :"+String(password)+".");
      Serial.println();
      Serial.print("Connecting to ");
      Serial.println("'"+String(ssid)+"'");
      if (WiFi.status() == WL_CONNECTED)
      {
        Serial.println("Wifi already connected");
      }
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) 
        {
          WiFi.begin(ssid, password);
            delay(1000);
            timer++;
            Serial.print(".");
            if (timer==10&&WiFi.status() != WL_CONNECTED)
            {
              Serial.println("\nTimeout no wifi connected!!\n\n");
              timer=0;
              break;
            }
            if (WiFi.status() == WL_CONNECTED)
            {
              Serial.println("WiFi connection estabilished with "+String(ssid));
              Serial.print("IP Address : ");
              Serial.println(WiFi.localIP());

              hostServer();
              Serial.println("Server hosted");
              Serial.println();
            }
        }
      
      command="";
    }
  }
}

void hostServer()
{
   server.begin();
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", "ESP 32 web server");
    });

    server.on("/files", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String files = listDir(SD, "/", 0); //list files in directory
        request->send(200, "text/plain", files);
    });

    server.on("/download", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        int fnsstart = request->url().lastIndexOf('/');
        String fn = request->url().substring(fnsstart);
        Serial.println("Download File: "+fn);
        // ... and finally
        request->send(SD, fn, String(), true);
    });

    server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        int fnsstart = request->url().lastIndexOf('/');
        String fn = request->url().substring(fnsstart);
        //Serial.println("Download File: "+fn);
        // ... and finally
        deleteFile(SD, fn);
        String files = listDir(SD, "/", 0);
        request->send(200, "text/plain", files); //send back remaining files
    }); 
}
