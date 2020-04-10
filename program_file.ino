#include <Wire.h>
#include <MPU6050.h>
MPU6050 mpu;
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#include "MAX30100_PulseOximeter.h"
PulseOximeter pox;

#include "ThingSpeak.h"

#include <WiFi.h>

#include <ESP32Lib.h>
#include <Ressources/CodePage437_8x8.h>
#include <Ressources/CodePage437_8x14.h>
#include <Ressources/CodePage437_8x16.h>
#include <Ressources/CodePage437_8x19.h>
#include <Ressources/CodePage437_9x16.h>
#include <Ressources/Font6x8.h>
void onBeatDetected()
{
    Serial.println("Beat!");
    
    vga.print("Beat!");
    
}
//pin configuration
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 32;
const int vsyncPin = 33;

char ssid[] = "zubair";   // your network SSID (name) 
char pass[] = "mirza777";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;
VGA3Bit vga;
unsigned long myChannelNumber = 676199;
const char * myWriteAPIKey = "79FSSM4XYF44ABOI";

// Initialize our values

int count=0;
String myStatus = "";

void setup() 
{
  Serial.begin(115200);
  Serial.println("Initialize MPU6050");
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(100);
  }
   if (!pox.begin()) {
        Serial.println("FAILED");
        
    } else {
        Serial.println("SUCCESS");
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);


    pox.setOnBeatDetectedCallback(onBeatDetected);

    

pinMode(1,OUTPUT);// buzzer pin
  
  Serial.println("Initialize MPU6050");

  
 vga.init(vga.MODE640x400, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
 
  vga.setFont(Font6x8);
 
  vga.setTextColor(vga.RGB(255, 0, 0), vga.RGB(0, 0, 255));
 
  vga.println("COVID-19");
  for (int i = 0; i < 256; i++)
    vga.print((char)i);
  vga.println();
 
ThingSpeak.begin(client);

}

void loop()
{
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, heartRate);
  ThingSpeak.setField(3, count);
  float temp = mpu.readTemperature();
  float heartRate = pox.getHeartRate();
  vga.print(pox.getHeartRate());
    vga.print("Bps");
  
  Serial.print(" Temp = ");
  Serial.print(temp);
  Serial.println(" *C");
  pox.update();
   if (temp>38){
   vga.println("fever detected");
   digitalWrite(1,HIGH);
    delay(100);
    digitalWrite(1,LOW);
    delay(100);
    Serial.println("fever");
    Serial.println ("Might be suspect");
    
  
    }
    if (pox.getHeartRate()<75){
    digitalWrite(1,HIGH);
    delay(100);
    digitalWrite(1,LOW);
    delay(100);
    vga.print("breath shortness");
    vga.print("S2+");
      
    }
   int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  if(temp > 38 && heartRate > 75){
    count++;
    digitalWrite(1,HIGH);// buzzer high
    vga.print("This patient might be covid");
  }
   
    
  
    pox.update();

delay(20000);

} 
    
    
