String readString;
int strIndex;
int strstatus;
String SSID1, password;
void setup() 
{
  Serial.begin(9600);
  Serial.println("Simple serial echo test"); // so I can keep track of what is loaded
}

void loop() 
{

  while (Serial.available()) 
  {
    char c = Serial.read();  //gets one byte from serial buffer
    readString += c; //makes the String readString
    delay(2);  //slow looping to allow buffer to fill with next character
  }

  if (readString.length() >0) {
    
    if(readString.startsWith("$EZ_RTK|SET-MODE|ROVER", 0))
    {
      Serial.println("Thanks for using EZ_RTK, your RTK has been configured as a ROVER ");
      readString="";
    }
    else if(readString.startsWith("$EZ_RTK|SET-MODE|BASE", 0))
    {
      Serial.println("Thanks for using EZ_RTK, your RTK has been configured as a BASE");
      readString="";
    }
    else if(readString.startsWith("$EZ_RTK|SET-WIFI|", 0))
    {
      strIndex=readString.lastIndexOf('|');
      SSID1=readString.substring(17, strIndex);
      password=readString.substring(strIndex+1);
      Serial.println("Thanks for using EZ_RTK, your settings for Wifi has been configured as follows:\nSSID : "+SSID1+"\nPassword : "+password);
      readString="";
    }
    else
    {
      Serial.print("Invalid Command : ");
      Serial.println(readString);  //so you can see the captured String
    }
    readString="";
  }
}
