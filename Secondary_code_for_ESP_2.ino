#include <TinyGPS++.h>
#include<BluetoothSerial.h>
#include<HardwareSerial.h>

#define MUX 13 //MUX =1 means Rover, MUX=2 means Base station

HardwareSerial RTK(1);
HardwareSerial LoRa(2);
BluetoothSerial BT;
TinyGPSPlus gnss;
String command="";


void setup() 
{
  // put your setup code here, to run once:
Serial.begin(115200);
RTK.begin(115200, SERIAL_8N1, 16, 17);
LoRa.begin(115200, SERIAL_8N1, 4, 5);
BT.begin("$EZ_RTK");
pinMode(MUX, OUTPUT);
digitalWrite(MUX, 1); //by default in Rover mode
gnss.encode(RTK.read());
}

void loop() {
  // put your main code here, to run repeatedly:
  
if(LoRa.available())
{
  RTK.write(LoRa.read()); //correction data from base
}
while (BT.available())
{
  char d = BT.read();  //gets one byte from serial buffer
  command += d; //appends to the String 
 }
 BT.println("\n\nLatitude : "+String(gnss.location.rawLat().deg));
BT.println("Longitude : "+String(gnss.location.rawLng().deg));
 Serial.println("\n\nLatitude : "+String(gnss.location.rawLat().deg));
 Serial.println("Longitude : "+String(gnss.location.rawLng().deg));
 if (command.length() > 0) 
  {
    if(command.equals("$EZ_RTK|SET-MODE|ROVER"))
    {
      Serial.println("Thanks for using EZ_RTK, your RTK has been configured as a ROVER ");
      digitalWrite(MUX,1); //MUX configured to be in Rover
      command="";
    }
    else if(command.equals("$EZ_RTK|SET-MODE|BASE"))
    {
      Serial.println("Thanks for using EZ_RTK, your RTK has been configured as a BASE");
      digitalWrite(MUX,0); //MUX configured to be in Base station
      command="";
    }
    else if(command.startsWith("$EZ_RTK|SET-WIFI|", 0))
    {
      Serial.println("Command : "+command);
      command="";
    }
  }
}
