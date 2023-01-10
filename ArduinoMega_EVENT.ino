#include "CytronMotorDriver.h"
#include <SoftwareSerial.h>
#include "HX711.h"
#include <SimpleKalmanFilter.h> 

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

SimpleKalmanFilter simpleKalmanFilter(4, 4, 0.01);
HX711 scale;

// Configure the motor driver.
CytronMD motor(PWM_DIR, 11, 12);  // PWM = Pin 3, DIR = Pin 4.
#define ZeroPin       22//Limit Switch 1
#define MaxPin        28 // Limit Switch 2

int vel =100;
int VentOn=0;
int Mode =0;
float estimated_value;
int respirationrate=20;
int inspiretime =1000;
int exspiretime=2000;
int IRatio=1;
int ERatio=3;
int ie_ratio=1;// 1=1:2,2=1:3,3=1:4,4=2:1
int td =400;
String readData;
String d1Data, d2Data, d3Data,d4Data, modeData, ventOnData,ieData;
char c;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  pinMode(ZeroPin, INPUT_PULLUP);
  pinMode(MaxPin, INPUT_PULLUP);
  inspiretime= inspiret(respirationrate,IRatio,ERatio);
  exspiretime= exspiret(respirationrate,IRatio,ERatio);
  Serial.print("In time");
  Serial.println(inspiretime);
  Serial.print("EX time");
  Serial.println(exspiretime);
  pinMode(13,OUTPUT);
}

void loop() {
  receivePackage();
  Serial.println(digitalRead(23));
  
  if(VentOn==1 && Mode ==0){
    VentilatorRun();
  }
  if(Mode ==1 && VentOn ==1){
    AssisVentilatorRun();
    digitalWrite(13,HIGH);
    Serial.println("Assis mode");
   }
  else{
   VentilatorStop();
   digitalWrite(13,LOW);
  }
   
}

void VentilatorRun() {

   while (digitalRead(ZeroPin) == HIGH)
      motor.setSpeed(-110);
      motor.setSpeed(0);
      //Serial.println("on");
      Serial.print("Max position");
      delay(inspiretime);
  while (digitalRead(MaxPin) == HIGH)
      motor.setSpeed(110);
      motor.setSpeed(0);
      Serial.print("Zero position");
      delay(exspiretime);

}
 
void VentilatorStop(){
  motor.setSpeed(0);
 }
 
void AssisVentilatorRun(){
  if (scale.is_ready()) {
    long reading = scale.read();
    float estimated_value = simpleKalmanFilter.updateEstimate(reading);
    Serial.println(estimated_value);
  if (estimated_value < -2300000){
    VentilatorRun();
    Serial.println("Assist Mode");
  } else{
    VentilatorStop();
  }
}
  
  else {
    Serial.println("HX711 not found.");
  }
}  

int inspiret(int(bpm),int(iratio),int(eratio)){
  float Tin=0;
  Tin=((60/bpm)*1000/(iratio+eratio))*iratio;
  return (Tin);
}
   
int exspiret(int(bpm),int(iratio),int(eratio)){
  float Ein=0;
  Ein=((60/bpm)*1000/(iratio+eratio))*eratio;
  return (Ein);
}

void receivePackage(){
  readData = "";  
  while (Serial2.available()) {
   delay(10);  //delay to allow byte to arrive in input buffer
   c = Serial2.read();
   readData += c;
   Serial.println(readData);
}
 
//Serial.println("In received");

//if (readData.length() == 12  || readData.length() == 18 || readData.length() == 19 || readData.length() == 21 || readData.length() == 20 ) {   //small
if (readData.length() >  10  && readData.length()<  50) {   //small
//if (readData.length()  >2) {
  Serial.print("Received:");
  Serial.println(readData.length());
  if (readData.indexOf("B") >= 0 && readData.indexOf("A") >= 0) {
         d1Data = readData.substring(readData.indexOf('A') + 1, readData.indexOf('B'));
  }
  if (readData.indexOf('C') >= 0 && readData.indexOf('B') >= 0) {
         d2Data = readData.substring(readData.indexOf('B') + 1, readData.indexOf('C'));
  }
  if (readData.indexOf('D') >= 0 && readData.indexOf('C') >= 0) {
         d3Data = readData.substring(readData.indexOf('C') + 1, readData.indexOf('D'));
  }
  if (readData.indexOf('E') >= 0 && readData.indexOf('D') >= 0) {
         d4Data = readData.substring(readData.indexOf('D') + 1, readData.indexOf('E'));
  }
  if (readData.indexOf('F') >= 0 && readData.indexOf('E') >= 0) {
         modeData = readData.substring(readData.indexOf('E') + 1, readData.indexOf('F'));
  }

 if (readData.indexOf('G') >= 0 && readData.indexOf('F') >= 0) {
         ventOnData = readData.substring(readData.indexOf('F') + 1, readData.indexOf('G'));
  }

 if (readData.indexOf('H') >= 0 && readData.indexOf('G') >= 0) {
         ieData = readData.substring(readData.indexOf('G') + 1, readData.indexOf('H'));
  }

//A RR IE C Td D VENT ON R

   respirationrate   = d1Data.toInt();  //bpm8-20
   td   = d2Data.toInt();  //volume 1-4
   VentOn  = d3Data.toInt();  //peep 400-100 x10
   ie_ratio = d4Data.toInt();  //trig 0,1
   Mode = modeData.toInt();
   
   Serial.print("BPM:");
   Serial.println(respirationrate);
   Serial.print(" ie ratio:");
   Serial.println(ie_ratio);
   Serial.print(" tidal volume:");
   Serial.println(td);
   Serial.print(" Venton:");
   Serial.println(VentOn);

  switch (ie_ratio){
    case 1:
           inspiretime= inspiret(respirationrate,1,2);
           exspiretime= exspiret(respirationrate,1,2);
           break;
    case 2:
           inspiretime=inspiret(respirationrate,1,3);
           exspiretime=exspiret(respirationrate,1,3);
           break;
    case 3:
           inspiretime=inspiret(respirationrate,1,4);
           exspiretime=exspiret(respirationrate,1,4);
           break;
    case 4:
           inspiretime=inspiret(respirationrate,2,1);
           exspiretime=exspiret(respirationrate,2,1);
           break;
   }
  // Mode   = modeData.toInt();
  //ventOn = ventOnData.toInt();
  // ie      = ieData.toInt();
  
  Serial.print("BPM");
  Serial.println(respirationrate);
  Serial.print("D1");
// Serial.println(respirationrate);
  
   readData="";
   delay(10);
}
}
