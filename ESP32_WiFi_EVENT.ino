#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPLs6SXPNni"
#define BLYNK_DEVICE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "zegyPanevAvkGE22bssFCOpPvR_d1H-g"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "zegyPanevAvkGE22bssFCOpPvR_d1H-g";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "HUAWEI P2000";
char pass[] = "Palm99234";

int VentOn = 0;
int Mode = 0;
char buff[100];
int vel = 100; 
int respirationRate = 20;
int inspireTime = 1000;
int expireTime = 2000;
int IRatio = 1;
int ERatio = 4;
int ie_ratio = 1; // 1 = 1:2, 2 = 1:3, 3 = 1:4, 4 = 2:1
int td = 400;

WidgetTerminal terminal(V16);
BlynkTimer timer;

void sendUptime()
{
  Blynk.virtualWrite(V7, analogRead(32));
  sprintf(buff,"Sensor:%d",analogRead(32));
  terminal.println(buff);
}

BLYNK_WRITE(V0)
{
 VentOn = param.asInt();
 digitalWrite(2,VentOn);
 Serial.print("VentOn: ");
 Serial.println(VentOn);
 SendPackage();
 }

BLYNK_WRITE(V14)
{
 Mode = param.asInt();
 digitalWrite(15,Mode);
 Serial.print("Mode: ");
 Serial.println(Mode);
 SendPackage();
 }

BLYNK_WRITE(V15)
{
 respirationRate = param.asInt();
 Serial.print("RR: ");
 Serial.println(respirationRate);
 SendPackage();
 }

BLYNK_WRITE(V17)
{
 ie_ratio = param.asInt();
 Serial.print("IE: ");
 Serial.println(ie_ratio);
 SendPackage();
 }


void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(2,OUTPUT);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000, sendUptime);
}

void loop()
{
  Blynk.run();
  timer.run();
  //terminal.print("Hello world eVentillator");
}

void SendPackage()
{
    Serial2.print("A");
    Serial2.print(respirationRate);
    Serial2.print("B");
    Serial2.print(td);
    Serial2.print("C");
    Serial2.print(VentOn);
    Serial2.print("D"); 
    Serial2.print(ie_ratio);
    Serial2.print("E");
    Serial2.print(Mode);
    Serial2.print("F");
    
}
