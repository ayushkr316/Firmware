#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

void setup() {
  // put your setup code here, to run once:
 
  Serial.begin(9600);
SerialBT.begin("ESP32test");
Serial.println("Bluetooth channel Initialised !!");
SerialBT.println("THE CONNECTION ESTABILISHED");
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println(SerialBT.readString());
}
