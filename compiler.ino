#include <WiFi.h>
//#include <WiFiClient.h>
#include <WebServer.h>
//#include "ESPAsyncWebServer.h"

int i=0;

//#include "Sattelite.h"
HardwareSerial RTK(1); //(rx,tx)
WebServer server(80); // Object of WebServer(HTTP port, 80 is defult)
// SSID & Password
const char* ssid = "Amir"; //SSID of Ayush's Phone
const char* password = "123456787"; //Password of Ayush's Phone

String myPage="";
String RTKdata="";
/////////////////////////////////////////////

// Handle root url (/)
void handle_root()
{

// HTML to be available on the webserver on the IP
//String myPage = MAIN_page;
/*String myPage = myPage+
"<h1><b>LOCATION DATA FROM SATTELITE TO WEBSERVER</b></h1>
<h2> GNSS DATA UPTO 2cm ACCURACY</h2>
<h3>Data:</h3>
<h4>"+String(data)+"</h4>";
*/

myPage="<h1>GNSS DATA FROM SATTELITE TO WEBSERVER</h1><h2>"+String(Serial.readString())+"</h2>";

/*{ i++;
//RTKdata=RTK.readString();
RTKdata=i;
myPage=myPage+"<p>";
myPage=myPage+RTKdata+"</p>";
}*/
server.send(200, "text/html", myPage);
}
/*
void rFresh()
{
RTKdata=RTK.readString();
String RTKVal = RTKdata;
server.send(200, "text/plane", RTKVal);//Send RTK corrected values only to client ajax request

}
*/
void setup() {
Serial.begin(115200);
Serial.print("Try Connecting to ");
Serial.println("Zuarifurnituresls5.1");

// Connecting to ayush's wifi hotspot
WiFi.begin(ssid, "123456787");

// Checking wi-fi is connected to wi-fi network
while (WiFi.status() != WL_CONNECTED) {
delay(700);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected successfully");
Serial.print("Got IP: ");
Serial.println(WiFi.localIP()); //Show ESP32 IP on serial

server.on("/", handle_root);
// server.on("/readSatteliteRTK", rFresh);//To get update of sattelite rtk values
server.begin();
Serial.println("HTTP server started");
Serial.println("Authorised person can now access SATTELITE DATA with 1 cm accuracy");
delay(100);
// HardwareSerial RTK(1); //(rx,tx)
//RTK.begin(115200);
}

void loop()
{
server.handleClient();
//String x= Serial.readString();
}
