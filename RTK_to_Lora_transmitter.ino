#include<HardwareSerial.h>
HardwareSerial LoRa(1);
HardwareSerial RTK(2);

String str="";
char a;
int timeNMEA= 32;
float latitude= 27.99;
float longitude=88.99;
float altitudenmea=0;
int fixQuality = 100;
int sattelite =0;
int hdopNMEA=0;
int checksum=32;
int ellipsoidheight=0;
String validity="A";
String direction1="N";
String direction2="W";
int speedknots=0;
int truecourse;
int dateNMEA=26/03/2021;
int variation;
void setup() {
// put your setup code here, to run once:
Serial.begin(115200);
LoRa.begin(115200, SERIAL_8N1, 16, 17);
RTK.begin(115200, SERIAL_8N1, 2, 4);

}

void loop() {
timeNMEA=timeNMEA+random(10);
latitude=latitude+random(10);
longitude=longitude+random(10);
sattelite=random(10);
hdopNMEA=random(100);
altitudenmea=random(122);
ellipsoidheight=random(3000);
speedknots=random(330);
truecourse=random(100);
variation=random(21);

// put your main code here, to run repeatedly:
//String sat="$GPRMC, "+String(timeNMEA)+", "+validity+", "+String(latitude)+", "+direction1+", "+String(longitude)+", "+direction2+", "+String(speedknots)+", "+String(truecourse)+", "+String(dateNMEA)+", "+String(variation)+", "+direction2+", "+String(checksum);
//Serial.println("$GPGGA, "+timeNMEA+", "+latitude+", "+longitude+", "+fixQuality+", "+sattelite+", "hdopnmea+", "+altitudenmea[i]+", "+ellipsoidHeight", "+0+", "+0+", "+checksum[i]);
//Serial.println("$GPRMC, "+String(timeNMEA)+", "+validity+", "+String(latitude)+", "+direction1+", "+String(longitude)+", "+direction2+", "+String(speedknots)+", "+String(truecourse)+", "+String(dateNMEA)+", "+String(variation)+", "+direction2+", "+String(checksum));
//;

// RTK.print("$GPGGA, "+timeNMEA+", "+latitude+", "+longitude+", "+fixQuality+", "+sattelite+", "hdopnmea+", "+altitudenmea[i]+", "+ellipsoidHeight", "+0+", "+0+", "+checksum[i]);




if (RTK.available())
{
a = RTK.read();

LoRa.print(a);
Serial.print(a);
str="";
}
}
