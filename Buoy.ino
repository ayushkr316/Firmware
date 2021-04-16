#include<TinyGPS++.h>
#include<HardwareSerial.h>

HardwareSerial LoRa(1);
HardwareSerial GPS(2);
TinyGPSPlus gps;
String Packet;

char gpsdat;
void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  GPS.begin(9600, SERIAL_8N1, 2,4);
  LoRa.begin(115200, SERIAL_8N1, 16, 17);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  while (GPS.available())
  {
    gps.encode(GPS.read());
    //gpsdat=GPS.read();
    //Serial.print(gpsdat);

      Packet="Buoy2|"+String(gps.location.rawLat().negative ? "-" : "+")+String(gps.location.rawLat().deg)+"|"+String(gps.location.rawLng().negative ? "-" : "+")+String(gps.location.rawLng().deg)+"|"+String(gps.date.value())+"|"+String(gps.time.value())+"|Hydrophone Signal status|}";
//    Serial.println("Latitude : "+String(gps.location.lat(), 6)); // Latitude in degrees (double)
//    Serial.println("Longitude : "+String(gps.location.lng(), 6)); // Longitude in degrees (double)
//    Serial.print("Latitude Raw : "+String(gps.location.rawLat().negative ? "-" : "+"));
//    Serial.println(gps.location.rawLat().deg); // Raw latitude in whole degrees
//    //Serial.println(gps.location.rawLat().billionths);// ... and billionths (u16/u32)
//    Serial.print(gps.location.rawLng().negative ? "-" : "+");
//    Serial.println(gps.location.rawLng().deg); // Raw longitude in whole degrees
//    Serial.println(gps.location.rawLng().billionths);// ... and billionths (u16/u32)
//    Serial.println(gps.date.value()); // Raw date in DDMMYY format (u32)
//    Serial.println(gps.date.year()); // Year (2000+) (u16)
//    Serial.println(gps.date.month()); // Month (1-12) (u8)
//    Serial.println(gps.date.day()); // Day (1-31) (u8)
//    Serial.println(gps.time.value()); // Raw time in HHMMSSCC format (u32)
//    Serial.println(gps.time.hour()); // Hour (0-23) (u8)
//    Serial.println(gps.time.minute()); // Minute (0-59) (u8)
//    Serial.println(gps.time.second()); // Second (0-59) (u8)
//    Serial.println(gps.time.centisecond()); // 100ths of a second (0-99) (u8)
//    Serial.println(gps.speed.value()); // Raw speed in 100ths of a knot (i32)
//    Serial.println(gps.speed.knots()); // Speed in knots (double)
//    Serial.println(gps.speed.mph()); // Speed in miles per hour (double)
//    Serial.println(gps.speed.mps()); // Speed in meters per second (double)
//    Serial.println(gps.speed.kmph()); // Speed in kilometers per hour (double)
//    Serial.println(gps.course.value()); // Raw course in 100ths of a degree (i32)
//    Serial.println(gps.course.deg()); // Course in degrees (double)
//    Serial.println(gps.altitude.value()); // Raw altitude in centimeters (i32)
//    Serial.println(gps.altitude.meters()); // Altitude in meters (double)
//    Serial.println(gps.altitude.miles()); // Altitude in miles (double)
//    Serial.println(gps.altitude.kilometers()); // Altitude in kilometers (double)
//    Serial.println(gps.altitude.feet()); // Altitude in feet (double)
//    Serial.println(gps.satellites.value()); // Number of satellites in use (u32)
//    Serial.println(gps.hdop.value()); // Horizontal Dim. of Precision (100ths-i32)
      LoRa.print(Packet);
      Serial.println(Packet);
      delay(3000);
  }
}
