#include<HardwareSerial.h>
HardwareSerial LoRa(1);
char a;
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
LoRa.begin(19200, SERIAL_8N1, 16, 17);
}

void loop() {
  // put your main code here, to run repeatedly:
if(LoRa.available())
{
char a= LoRa.read();
Serial.print(a);
//LoRa.write(a);
}
}
