//#include <Arduino.h>

#include <MPU6050_tockn.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include<Adafruit_GrayOLED.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "BluetoothSerial.h"
#include <ArduinoJson.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define buzzer 6
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
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
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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


//can be generated  using "image2cpp", select arduino code in dropdownList
/*
const unsigned char myBitmap [] PROGMEM = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
*/

//buzzer for confirming status
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

void handleRoot() 
{
 String page="<h1>GPS to Web Server</h1><h3>Data:</h3><h4>"+String(dataPacket)+"</h4>"; //Read HTML contents
 server.send(200, "text/html", page); //Send web page
}

void bluetoothSetup()
{
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The bluetooth Channel Initialised!");
  display.clearDisplay();
  display.setTextSize(2);
  //display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Bluetooth Initiated");
  display.display();
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
  errorBuzzer();
  }
  else
  {
  //Serial.println("Success!! Correction data is being streamed via LoRa ");
  successBuzzer();
  
  }
  
}

void Oled_setup()
{
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
  successBuzzer();

//ESP32 As access point IP: 192.168.4.1
  WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP("ESPWebServer", "12345678");    //Password length minimum 8 char

//Comment below code if you are using Access point only
//ESP32 connects to your wifi -----------------------------------
//  WiFi.mode(WIFI_STA); //Connect to your wifi
// WiFi.begin(ssid, password);

  WiFiClient Client;

  Serial.println("WiFi Hotspot succesfully created \n Host name : ESPWebSerer\nPassword : 1234abcd");
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("IP Address : "+String(myIP));
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("IP Address : "+String(myIP));
  delay(2000);


  delay(1000);
  Serial.println("Connected to : "+String(Client.read()));
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("Connected to : "+String(Client.read()));
  delay(2000);
}

//returns string that contains the required iformation in following sequence lat,lng,date,time,speed(kmph),altitude(mtr),
/*String NMEA_parser()
{
  while(RTK.available()>0)
  {
    gps.encode(RTK.read());
    Serial.print("Latitude : ");
    Serial.println(gps.location.lat(), 6); // Latitude in degrees (double)
    Serial.print("Longitude : ");
    Serial.println(gps.location.lng(), 6); // Longitude in degrees (double)
    Serial.print("Raw latitude : ");
    Serial.print(gps.location.rawLat().negative ? "-" : "+");
    Serial.println(gps.location.rawLat().deg); // Raw latitude in whole degrees
    Serial.print("Raw latitude (in billionths) : ");
    Serial.println(gps.location.rawLat().billionths);// ... and billionths (u16/u32)
    Serial.print("Raw longitude : ");
    Serial.print(gps.location.rawLng().negative ? "-" : "+");
    Serial.println(gps.location.rawLng().deg); // Raw longitude in whole degrees
    Serial.print("Raw longitude (in billionths) : ");
    Serial.println(gps.location.rawLng().billionths);// ... and billionths (u16/u32)
    Serial.print("Date : ");
    Serial.println(gps.date.value()); // Raw date in DDMMYY format (u32)
    Serial.print("Year : ");
    Serial.println(gps.date.year()); // Year (2000+) (u16)
    Serial.print("Month : ");
    Serial.println(gps.date.month()); // Month (1-12) (u8)
    Serial.print("Day : ");
    Serial.println(gps.date.day()); // Day (1-31) (u8)
    Serial.print("Time : ");
    Serial.println(gps.time.value()); // Raw time in HHMMSSCC format (u32)
    Serial.print("Hour : ");
    Serial.println(gps.time.hour()); // Hour (0-23) (u8)
    Serial.print("Minute : ");
    Serial.println(gps.time.minute()); // Minute (0-59) (u8)
    Serial.print("Second : ");
    Serial.println(gps.time.second()); // Second (0-59) (u8)
    Serial.print("Centisecond : ");
    Serial.println(gps.time.centisecond()); // 100ths of a second (0-99) (u8)
    Serial.print("Speed (100ths of knots) : ");
    Serial.println(gps.speed.value()); // Raw speed in 100ths of a knot (i32)
    Serial.print("Speed  (Knots) : ");
    Serial.println(gps.speed.knots()); // Speed in knots (double)
    Serial.print("Speed (miles per hour) : ");
    Serial.println(gps.speed.mph()); // Speed in miles per hour (double)
    Serial.print("Speed (meters per hour) : ");
    Serial.println(gps.speed.mps()); // Speed in meters per second (double)
    Serial.print("Speed (kilometers per hour) : ");
    Serial.println(gps.speed.kmph()); // Speed in kilometers per hour (double)
    Serial.print("Course (100ths of a degree) : ");
    Serial.println(gps.course.value()); // Raw course in 100ths of a degree (i32)
    Serial.print("Course (degrees) : ");
    Serial.println(gps.course.deg()); // Course in degrees (double)
    Serial.print("Raw altitude (centimeters) : ");
    Serial.println(gps.altitude.value()); // Raw altitude in centimeters (i32)
    Serial.print("Raw altitude (meters) : ");
    Serial.println(gps.altitude.meters()); // Altitude in meters (double)
    Serial.print("Raw altitude (miles) : ");
    Serial.println(gps.altitude.miles()); // Altitude in miles (double)
    Serial.print("Raw altitude (kilometer) : ");
    Serial.println(gps.altitude.kilometers()); // Altitude in kilometers (double)
    Serial.print("Raw altitude (feet) : ");
    Serial.println(gps.altitude.feet()); // Altitude in feet (double)
    Serial.print("No of Satellite : ");
    Serial.println(gps.satellites.value()); // Number of satellites in use (u32)
    Serial.print("Elevation angle : ");
    Serial.println(gps.hdop.value()); // Horizontal Dim. of Precision (100ths-i32)
    //String dataPacketNmea=String(gps.location.rawLat().negative ? "-" : "+")+String(gps.location.rawLat().deg)+"\t"+String(gps.location.rawLng().negative ? "-" : "+")+String(gps.location.rawLat().deg)+"\t"+String(gps.date.value())+"\t"+String(gps.time.value())+"\t"+String(gps.speed.kmph())+"\t"+String(gps.altitude.meters())+"\t"+String(gps.satellites.value())+"\t"+String(gps.hdop.value());
    String dataPacketNMEA= String(gps.location.rawLat().negative ? "-" : "+")+String(gps.location.rawLat().deg)+"\t";
    dataPacketNMEA=dataPacketNMEA+String(gps.location.rawLng().negative ? "-" : "+")+String(gps.location.rawLat().deg)+"\t";
    dataPacketNMEA=dataPacketNMEA+String(gps.date.value())+"\t";
    dataPacketNMEA=dataPacketNMEA+String(gps.time.value())+"\t";
    dataPacketNMEA=dataPacketNMEA+String(gps.speed.kmph())+"\t";
    dataPacketNMEA=dataPacketNMEA+String(gps.altitude.meters())+"\t";
    dataPacketNMEA=dataPacketNMEA+String(gps.satellites.value())+"\t";
    dataPacketNMEA=dataPacketNMEA+String(gps.hdop.value());
    String json_packet="Work ongoing";
    return(dataPacketNMEA);
  
  }
};
*/

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
  if(Serial.readString()=="Rover")
  {
    Oled_setup();
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor (0,0);
    display.print("ROVER");
    display.display();
    delay(2000);
    LoRa_RTK_setup();
    IMU_setup();
    bluetoothSetup();
    successBuzzer();
    Mode=1;
  }
  else if(Serial.readString()=="Base Station")
  {
    Oled_setup();
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor (0,0);
    display.print("BASE STATION");
    display.display();
    delay(2000);
    LoRa_RTK_setup();
    hotspot_setup();
    successBuzzer();
    Mode=0;
  }
  else
  {
    Oled_setup();
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor (0,0);
    display.print("Invalid Selection");
    display.display();
    delay(2000);
    Serial.println("Invalid input please try again (case sensitive)");
    errorBuzzer();
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
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  //digitalWrite (muxSelect,1); muxSelect to be decclared as  per pin availabilty (RTK serial multiplexing)
  //dataPacket_rover=LoRa.ReadString();
  //RTK.println(LoRa.readString());
  //parse the datapacket_Rover
  display.println("Latitude : latitudeValude\nLongitude : longitudeVaue\nAltitude : altitudeValue");
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
    angularOffsetsWindow();
  }
  else if(x!=0)
  {
    BaseStatusWindow();
  }

}
