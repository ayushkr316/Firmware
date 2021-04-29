#include "WiFi.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include<BluetoothSerial.h>
#include "FS.h"
#include "SD.h"
String readString;
int strIndex, buff=0;
int strstatus;
String SSID1, PASS;
char ssid[30],c,d;
int mode1=0;
char password[30];
String globe = "Error: PX1122R Module not connected.";
String str0 = ""; //buffer for NMEA
String str1 = ""; //buffer 2 for NMEA
const int chipSelect = 5;
int ck = 0;
int timer=0;// for wifi timeout
const unsigned long eventInterval = 2000;
unsigned long previousTime = 0;

BluetoothSerial BT;
AsyncWebServer server(80);
HardwareSerial MySerial(1);


//File system
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

// hosting the server
void hostServer(){
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "ESP 32 web server");
  });

  server.on("/command", HTTP_GET, [](AsyncWebServerRequest *request){
//    if(checksum(request->url())){
//      request->send(200, "text/plain", "ACK");  
//    }
//    else{
//      request->send(200, "text/plain", "NACK: Error in command transmission");  
//    }
  });

  server.on("/livedata", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", globe); 
  });
  
  server.begin();
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("Simple serial echo test"); // so I can keep track of what is loaded
  BT.begin("rtk");
 /// esp_get_free_heap_size();
}

void loop() 
{

  while (Serial.available()) 
  {
    mode1=1;
    c = Serial.read();  //gets one byte from serial buffer
    readString += c; //makes the String readString
    delay(8);  //slow looping to allow buffer to fill with next character
  }
  while(BT.available())
  {
    buff=1;
    
    d = BT.read();  //gets one byte from serial buffer
    readString += d; //makes the String readString
    delay(8);  //slow looping to allow buffer to fill with next character
  }
  if(buff==1)
  { 
    Serial.println("WiFi credentials recieved from Bluetooth");//nDisconnecting Bluetooth to avoid interferences...");
//    BT.disconnect();
//    Serial.println("Bluetooth Disconnected"); 
    buff=0;
  }

  if (readString.length() > 0) 
  {
    
    if(readString.equals("$EZ_RTK|SET-MODE|ROVER"))
    {
      Serial.println("Thanks for using EZ_RTK, your RTK has been configured as a ROVER ");
      readString="";
    }
    else if(readString.equals("$EZ_RTK|SET-MODE|BASE"))
    {
      Serial.println("Thanks for using EZ_RTK, your RTK has been configured as a BASE");
      readString="";
    }
    else if(readString.startsWith("$EZ_RTK|SET-WIFI|", 0))
    {
      Serial.println("Command : "+readString);
      strIndex=readString.lastIndexOf('|');
      SSID1=readString.substring(17, strIndex);
      PASS=readString.substring(strIndex+1);
      Serial.println("SSID="+SSID1);//String value parsed
      Serial.println("Password="+PASS);//String value parsed
     // SSID1.toCharArray(ssid, SSID1.length()+1);
      //ssid[]=SSID1;
      //password[]=Password;
      if (mode1 == 1) 
      {
        SSID1.toCharArray(ssid, SSID1.length()+1);
        PASS.toCharArray(password, PASS.length());
        mode1=0;
       } 
       else 
       {
        SSID1.toCharArray(ssid, SSID1.length()+1);
        PASS.toCharArray(password, PASS.length()-1);
        }
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
              BT.println(WiFi.localIP());
              hostServer();
              Serial.println("Server hosted");
              Serial.println();
            }
        }
      
      
      readString="";
    }
    else
    {
      Serial.print("Invalid Command : ");
      Serial.println(readString);  //so you can see the captured String
    }
    readString="";
  }
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
