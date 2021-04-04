//#include <Arduino.h>

#include <MPU6050_tockn.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "BluetoothSerial.h"
#include <ArduinoJson.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//#define buzzer 6
//#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define chipSelect 11 //Auxilary pin for chipselect in sd card
#define pushButton 5

const char* ssid = "RTK_Device";
const char* password = "1234abcd";

HardwareSerial RTK (1);
HardwareSerial LoRa(2);

BluetoothSerial SerialBT;
TinyGPSPlus gps;
WebServer server(80);
MPU6050 mpu6050(Wire);

//json vaiable/object
File NMEALogger;



// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String dataPacket;
String fileName =""; //json file name
int  x=0; // Aux variable for switching window
int tactileSwitch=0;
int Mode;//auxilary variable for selecting base and rover.. mode=1 for rover & mode= 0 for base
bool connection = false;
int barStartX = 40;
int barStartY = 10;
int pushButtonCurrentStatus;
int pushButtonLastStatus = 0;
int startPressedTime = 0;
int stopPressedTime = 0;
int toggleMode=0;
int fileValue=0;
void BaseStation();
void Rover();



//buzzer for confirming status
/*
void successBuzzer()
{
 for (int i=0; i<15; i++)
  {
    digitalWrite (buzzer, 1);
    delay(100);
    digitalWrite(buzzer,0);
    delay(100);
  }
}


void errorBuzzer()
{
  for (int i=0; i<15; i++)
  {
    digitalWrite (buzzer, 1);
    delay(800);
    digitalWrite(buzzer,0);
    delay(200);
  }
}
*/
//
/*
void statusLights(String Rssi);

//for wifi bars
void WifiTranmissionRSSI()
{
  //Gets RSS and compares rss values, commands bar generation
  //More on RSSI: https://www.metageek.com/training/resources/understanding-rssi.html
  long rssi = WiFi.RSSI();
  if (connection) {
    if (rssi < -30 && rssi > -67) 
    { //Excellent signal range
      statusLights("HIGH");
    } 
    else if (rssi < -67 && rssi > -70) 
    { //Good signal range
      statusLights("MID");
    }
     else if (rssi < -70 && rssi > -80) 
    { //Poor signal range
      statusLights("LOW");
    } 
    else 
    {
      statusLights("BAD"); //unusable range
    }

  }
}
*/

/*
void angularOffsetsWindow()
{
    
   
    delay(2000);
    display.clearDisplay();
    display.setTextSize(2);
    //display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Angular Offsets");
    display.display();
    display.startscrollright(0x00, 0x00);
    display.setCursor(0, 25);
    display.setTextSize(1);
    //display.setTextColor(WHITE);
    display.print("angleX : ");display.println(mpu6050.getAngleX());
    display.print("angleY : ");display.println(mpu6050.getAngleY());
    display.print("angleZ : ");display.println(mpu6050.getAngleZ());
    display.display();
}

void BaseStatusWindow()
{
  if (!LoRa.available())
  {
    display.setCursor(0, 0);
   // display.setTextColor(WHITE);
    display.setTextSize(2);
    display.println("Communication LOST with Base station");
    display.display();
    display.startscrollright(0x00, 0x00);
  }
  else
  {
    display.setCursor(0, 0);
    //display.setTextColor(WHITE);
    display.setTextSize(2);
    display.println("Communication ESTABLISHED with Base station");
    display.display();
    display.startscrollright(0x00, 0x00);
    display.setCursor(8,20);
    //display.setTextColor(WHITE);
    display.setTextSize(1);
    display.println(RTK.readString()); 
  }
}
*/
void handleRoot() 
{
 String page="<h1>GPS to Web Server</h1><h3>Data:</h3><h4>"+String(dataPacket)+"</h4>"; //Read HTML contents
 server.send(200, "text/html", page); //Send web page
}

void bluetoothSetup()
{
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The bluetooth Channel Initialised!");
  //display.clearDisplay();
  //display.setTextSize(2);
  //display.setTextColor(WHITE);
  //display.setCursor(0,0);
  //display.println("Bluetooth Initiated");
  //display.display();
}

/*
void DrawWifiStatus(int16_t startX, int16_t startY, int bars = 4, int spacingX = 9, int bar_width = 7, int first_bar_height = 4, int bar_height_increment = 3)
{
  ///Draws RSSI (wifi) bars
  //Parameters:
  //StartX,StartY: x and y coordinates of first bar
  //Bars: number of bars
  //Spacing X: spacing between bars
  //bar_width: each bar width
  //first_bar_height: height of the first bar 
  //bar_height_increment: incremented height per bar.
  
  int height_increment = 0;
  for (int bar = 0; bar < bars; bar++) 
  {
    display.fillRect(startX, startY , bar_width, first_bar_height + height_increment, WHITE);
    startY = startY - bar_height_increment; //readjust postion based on height of the bar
    height_increment = height_increment + bar_height_increment;
    startX = startX + spacingX;
  }

}
*/

void LoRa_RTK_setup()
{
  Serial.print("Intialising LoRa and RTK Serial channels");
  for(int i=0; i<4; i++)
  {
  Serial.print(".");
  delay(500);
  }
  
  RTK.begin(115200);  //(baud-rate, protocol, RX pin, TX pin)(9,10 = rx, tx)
  LoRa.begin(115200); //(baud-rate, protocol, RX pin, TX pin)(16,17 = rx, tx)  
  
  Serial.println("");
  Serial.println("RTK and LoRa channels initialised");
  
  
  if (!LoRa.available())
  {
  //Serial.println("Failed!! No data recieved from LoRa Device");
  //errorBuzzer();
  }
  else
  {
  //Serial.println("Success!! Correction data is being streamed via LoRa ");
  //successBuzzer();
  
  }
  
}

void Oled_setup()
{/*
  Serial.print("Initialising OLED Display");
  for(int i=0; i<4; i++)
  {
  Serial.print(".");
  delay(500);
  Serial.println("");
  }
  

//  SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);// Don't proceed, loop forever
  }
    delay(2000); // Pause for 2 seconds
 
  // Clear the buffer.
   display.clearDisplay(); //for Clearing the display
  // display.drawBitmap(0, 0, myBitmap, 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
   display.display();
   delay(2000); // Pause for 2 seconds
   Serial.println("Display Initialised");
   successBuzzer();
   */
   
}

void hotspot_setup()
{
  Serial.println("Initialising Hotspot");
  for(int i=0; i<4; i++)
  {
    Serial.print(".");
    delay(500);
    Serial.println("");
  }
  Serial.println("Booting Sketch... ");
  //successBuzzer();

//ESP32 As access point IP: 192.168.4.1
  //WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP("ESPWebServer", "12345678");    //Password length minimum 8 char

//Comment below code if you are using Access point only
//ESP32 connects to your wifi -----------------------------------
//  WiFi.mode(WIFI_STA); //Connect to your wifi
// WiFi.begin(ssid, password);

  WiFiClient Client;

  Serial.println("WiFi Hotspot succesfully created \n Host name : ESPWebSerer\nPassword : 1234abcd");
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("IP Address : "+String(myIP));
  //display.clearDisplay();
  //display.setTextSize(2);
  //display.setCursor(0,0);
  //display.println("IP Address : "+String(myIP));
  //delay(2000);


  delay(1000);
  Serial.println("Connected to : "+String(Client.read()));
  //display.clearDisplay();
  //display.setTextSize(2);
  //display.setCursor(0,0);
 // display.println("Connected to : "+String(Client.read()));
  //delay(2000);
}



void createJsonFile()
{
  gps.encode(RTK.read());
  fileName = String(gps.time.value())+"_"+String(gps.date.value())+".jso";
  NMEALogger = SD.open(fileName, FILE_WRITE);
  if (!NMEALogger) 
  {
    Serial.println("Failed to create file");
    return;
  }
}
String pressButton()
{
  pushButtonCurrentStatus = digitalRead(pushButton);
  if (pushButtonLastStatus == 1 && pushButtonCurrentStatus == 0)
  {
    startPressedTime=millis();
  }
else if (pushButtonLastStatus == 0 && pushButtonCurrentStatus == 1)
{
  stopPressedTime=millis();
}
long pressedDuration = stopPressedTime-startPressedTime;
if (pressedDuration<1000)
{
  Serial.println("A short press is detected");
  return("SHORT_PRESS");
}
if (pressedDuration>1000)
{
  Serial.println("A long press is detected");
  return("LONG_PRESS");
  toggleMode=!toggleMode;
}
pushButtonLastStatus=pushButtonCurrentStatus;
return "";
}

void logJson()
{
  DynamicJsonDocument  dataPacketJson(200);
  if (pressButton()="SHORT_PRESS")
  {
    dataPacketJson["Latitude"] = gps.location.rawLat().deg;
    dataPacketJson["Longitude"] = gps.location.rawLng().deg;
    dataPacketJson["Date"] = gps.date.value();
    dataPacketJson["Time"] = gps.time.value();
    dataPacketJson["Speed"] = gps.speed.kmph();
    dataPacketJson["Altitude"] = gps.altitude.meters();
    dataPacketJson["Sattelites"] = gps.satellites.value();
    dataPacketJson["HDOP"]= gps.hdop.value();
    if (NMEALogger) 
    {
     //writting data on the file
      Serial.print(F("Filename --> "));
      Serial.println(fileName);
      Serial.print(F("Writting JSON on the file ..."));
      serializeJsonPretty(dataPacketJson, NMEALogger);
      Serial.print(F("..."));
    }
  }
}
/*Already embedded in logJson function
void JsonDataPacket( )
{
  gps.encode(RTK.read());
  NMEALogger = SD.open(fileName, FILE_WRITE);
  if (!NMEALogger) 
  {
    Serial.println(F("Failed to create file"));
    return;
  }
  DynamicJsonDocument  dataPacketJson(200);
  dataPacketJson["Latitude"] = gps.location.rawLat().deg;
  dataPacketJson["Longitude"] = gps.location.rawLng().deg;
  dataPacketJson["Date"] = gps.date.value();
  dataPacketJson["Time"] = gps.time.value();
  dataPacketJson["Speed"] = gps.speed.kmph();
  dataPacketJson["Altitude"] = gps.altitude.meters();
  dataPacketJson["Sattelites"] = gps.satellites.value();
  dataPacketJson["HDOP"]= gps.hdop.value();
  if (NMEALogger) 
  {
    //writting data on the file
    Serial.print(F("Filename --> "));
    Serial.println(fileName);
    Serial.print(F("Writting JSON on the file ..."));
    serializeJsonPretty(dataPacketJson, NMEALogger);
    Serial.print(F("..."));

    // close the file: File must be closed everytime properly before opening other file.
    NMEALogger.close();
    Serial.println(F("Done writting!"));
  } 
  else 
  {
    // if the file didn't open, throw an error:
    Serial.print(F("Error opening the file !!!"));
    Serial.println(fileName);
  }
}
*/

// for printing the recorded json
void printJsonFile() 
{
  // Open file for reading
  File file = SD.open(fileName);
  if (!file)
  {
      Serial.println(F("Failed to read file"));
      return;
  }
 
  // Extract each characters by one by one
  while (file.available()) 
  {
    Serial.print((char) file.read());
    Serial.println();

    // Close the file
    file.close();
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



void RTK_operation()
{
  if (pressButton()=="LONG_PRESS" && toggleMode==1)
  {
    createJsonFile();
    //if from now any single tap on switch is observed the NMEA data will be logged into sd-card  
    logJson();
  }
  if (pressButton()=="LONG_PRESS" && toggleMode==0)
  {
    
    // close the file: File must be closed everytime properly before opening other file.
    NMEALogger.close();
    Serial.println(F("Done writting!")); 
    if(!NMEALogger) 
    {
      // if the file didn't open, throw an error:
      Serial.print(F("Data cant be logged : "));
      Serial.println(fileName);
    }
  }
  
}

//for wifi bars
/*
void statusLights(String Rssi) 
{
  //generates bars according to rssi results
  if (Rssi == "HIGH") {
    display.clearDisplay();
    DrawWifiStatus(barStartX, barStartY, 4);
  } else if (Rssi == "MID") {
    display.clearDisplay();
    DrawWifiStatus(barStartX, barStartY, 3);
  } else if (Rssi == "LOW") {
    display.clearDisplay();
    DrawWifiStatus(barStartX, barStartY, 2);
  } else if (Rssi == "BAD") { //unusable
    display.clearDisplay();
    DrawWifiStatus(barStartX, barStartY,  1);
  }
  display.display();

}
*/


void IMU_setup()
{
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Enter your Choice between Base and Rover : ");
  Serial.println("1) Base Station \n2) Rover");
  delay(1000);
 while(1)
 {
  if (Serial.available())
  {
  if(Serial.readString()=="Rover\n")
  {
    Serial.println("To be configured in rover mode"); 
    /*
    Oled_setup();
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor (0,0);
    display.print("ROVER");
    display.display();
    delay(2000);*/
    LoRa_RTK_setup();
    IMU_setup();
    bluetoothSetup();
    //successBuzzer();
    Mode=1;
    break;
  }
  else if(Serial.readString()=="Base Station\n")
  {/*
    Oled_setup();
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor (0,0);
    display.print("BASE STATION");
    display.display();
    delay(2000);*/
    LoRa_RTK_setup();
    hotspot_setup();
    //successBuzzer();
    Mode=0;
    break;
  }
  else
  {/*
    Oled_setup();
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor (0,0);
    display.print("Invalid Selection");
    display.display();
    delay(2000);*/
    Serial.println("Invalid input please try again (case sensitive)");
//    errorBuzzer();
      
  
    
  }
  }
 }
}

void loop() 
{

  if(Mode==1)
  {
    Rover();
  }
  if(Mode==0)
  {
    BaseStation();
  }
  //previous code
  /*
  server.handleClient();
  delay(100);
    mpu6050.update();

    RTK.println(LoRa.read());

    dataPacket = mpu6050.getAngleX()+String("\t")+mpu6050.getAngleY()+String("\t")+mpu6050.getAngleZ()+String("\t")+LoRa.read();
    
    Serial.println(mpu6050.getAngleX()+String("\t ")+mpu6050.getAngleY()+String("\t ")+mpu6050.getAngleZ());
    */
  
}

void BaseStation()
{
  server.handleClient();
  LoRa.println(RTK.readString());
  dataPacket=RTK.readString();
}

void Rover()
{/*
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  //digitalWrite (muxSelect,1); muxSelect to be decclared as  per pin availabilty (RTK serial multiplexing)
  //dataPacket_rover=LoRa.ReadString();
  //RTK.println(LoRa.readString());
  //parse the datapacket_Rover
  display.println("Latitude : latitudeValude\nLongitude : longitudeVaue\nAltitude : altitudeValue");*/
  tactileSwitch=digitalRead(pushButton);

  Serial.println(mpu6050.getAngleX()+String("\t ")+mpu6050.getAngleY()+String("\t ")+mpu6050.getAngleZ());

  //printing data on bluetooth terminal
  SerialBT.println("Latitude : latitudeValude\nLongitude : longitudeVaue\nAltitude : altitudeValue");

  //Reading data from bluetooth terminal passthrough scope
  if (SerialBT.available()) 
  {
  Serial.println(SerialBT.read());
  }

  if (tactileSwitch==1)
  {
    x=!x;
  }
  if (x==0)
  {
    //angularOffsetsWindow();
  }
  else if(x!=0)
  {
    //BaseStatusWindow();
  }

}
