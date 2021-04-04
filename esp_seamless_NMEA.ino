#include <HardwareSerial.h>
#include "FS.h"
#include "SPIFFS.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "MD5.h"
#include "SD.h"
#include <MD5Builder.h>


//char* ssid = "Zuarifurnituresls5.1";
//char* password = "bedrock@8540";

char* ssid = "Rinchin's device";
char* password = "airbud17";

String globe="";
String server_store="";

HardwareSerial MySerial(1);
AsyncWebServer server(80);


#define FORMAT_SPIFFS_IF_FAILED true
#define chipSelect 5

MD5Builder _md5;

int timeNMEA= 32;
float latitude= 27.99;
float longitude=88.99;
float altitudenmea=0;
int fixQuality = 100;
int sattelite =0;
int hdopNMEA=0;
int checksum1=32;
int ellipsoidheight=0;
String validity="A";
String direction1="N";
String direction2="W";
int speedknots=0;
int truecourse;
int dateNMEA=26/03/2021;
int variation;

String md5(String str) {
  _md5.begin();
  _md5.add(String(str));
  _md5.calculate();
  return _md5.toString();
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
            //Serial.print("\tSIZE: ");
            //Serial.println(file.size());
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
  MySerial.begin(115200, SERIAL_8N1, 16, 17);//rx,tx

  
//  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
//      Serial.println("SPIFFS Mount Failed");
//      return;
//    }

//  if(!SD.begin(chipSelect)){
//    Serial.println("Card Mount Failed!");
//    return;
//  }
//  else{
//    Serial.println("Card mounted successfully!");
//  }
//  uint8_t cardType = SD.cardType();
   
//  if(cardType == CARD_NONE){
//    Serial.println("No SD card attached");
//    return;
//  }
  
//  writeFile(SD, "/sd_hello1.txt", "Hello ");
//  writeFile(SD, "/sd_hello2.txt", "Hello ");
//  writeFile(SD, "/sd_hello3.txt", "Hello ");

//  listDir(SD, "/", 0);
     
//  writeFile(SPIFFS, "/hello1.txt", "Server test file content"); //create files
//  writeFile(SPIFFS, "/hello2.txt", "Server test file content"); //create files
//  writeFile(SPIFFS, "/hello3.txt", "Server test file content"); //create files
//  writeFile(SPIFFS, "/hello4.txt", "Server test file content"); //create files

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

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
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

  server.on("/livedata", HTTP_GET, [](AsyncWebServerRequest *request){
//        
//    timeNMEA=timeNMEA+random(10);
//    latitude=latitude+random(10);
//    longitude=longitude+random(10);
//    sattelite=random(10);
//    hdopNMEA=random(100);
//    altitudenmea=random(122);
//    ellipsoidheight=random(3000);
//    speedknots=random(330);
//    truecourse=random(100);
//    variation=random(21);
//    
//    String test = "$GPRMC, "+String(timeNMEA)+", "+validity+", "+String(latitude)+", "+direction1+", "+String(longitude)+", "+direction2+", "+String(speedknots)+", "+String(truecourse)+", "+String(dateNMEA)+", "+String(variation)+", "+direction2+", "+String(checksum1);

    request->send(200, "text/plain", globe); 
  });

 
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

void loop() {
 
//  if (Serial.available()) {
//    String comm=Serial.readString();
//    if(comm[0]=='$') {
//      if(checksum(comm)) {
//        Serial.println("ACK: "+comm);  
//      } else{
//        Serial.println("NACK: Error in command transmission");
//      }
//    }
//  }
//  if(Serial.available()){
    if(MySerial.available()){
      char temp = MySerial.read();
      if(temp=='$'){
        Serial.print(globe); 
        delay(1000);
//        server_store = globe;
        globe = temp; 
      }
      else{
        globe += temp;
      }
//    
  }
}
