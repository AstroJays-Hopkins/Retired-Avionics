#include <Wire.h> //blue wire on 2 pin connector to analog 4
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
#include "IntersemaBaro.h"
const int chipSelect = 10;

//%%%%%%%%%%%%%%%%%%%%%%%% ALTIMETER SETUP %%%%%%%%%%%%%%%%%%%%%%%%//
Intersema::BaroPressure_MS5607B baro(true);
// Altitude variables
float avg_alt;
float alt0;
float altitude;
float old_Alt;
float new_Alt;
unsigned long T0;
unsigned long T1;
unsigned long T;

//create state machine for deployment stages
enum states{
  None,
  Setup,
  Launchpad,
  Thrust,
  Descent,
  Deployment
};
//set initial state
states flight = None;
//states timer = None;

//%%%%%%%%%%%%%%%%%%%%%%%% RECOVERY PIN SETUP %%%%%%%%%%%%%%%%%%%%%%%%//
//const int SAFETY = 7;
const int SEP = 5;
const int R = 3;

//%%%%%%%%%%%%%%%%%%%%%%%% PAYLOAD PIN SETUP %%%%%%%%%%%%%%%%%%%%%%%%//
const int PL = 8;
int loop_counter = 0;


void setup() {
//begin communication
Serial.begin(9600); //set to 115200 when using GPS
pinMode(R, OUTPUT);
pinMode(SEP, OUTPUT);
//pinMode(SAFETY, INPUT);
pinMode(PL, OUTPUT);
digitalWrite(R, LOW);
digitalWrite(SEP, LOW);
digitalWrite(PL, LOW);

Serial.println("Booted up");
//altimeter
baro.init();
Serial.println("Initialized");
//set starting altitude
alt0 = 0;
altitude = 0;
avg_alt = 0;
int num_points = 50;
for (int i=0; i<num_points; i++)
    {
      alt0 += baro.getHeightCentiMeters()/30.48;
      delay(10);
    }
alt0 /= num_points;            //normalize to the number of samples collected
//set initial states
flight = Setup;
//timer = None;
//initialize datalogging
  if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
    }
    else{
      File dataFile = SD.open("Flight.txt", FILE_WRITE);
      if (dataFile){
          dataFile.println("Beginning New Flight");
          dataFile.close();
          Serial.println("New Datalog Created");
        }
      else{
          Serial.println("error opening file Flight.txt");
          return;
        }
      Serial.println("Initialization Complete");
    }
  
}


void loop() {
  ////////// Altimeter //////////
  altitude = baro.getHeightCentiMeters()/30.48 - alt0;
  avg_alt += (altitude - avg_alt)/5;
  ////////// Recovery and Payload Activation //////////
  
  //ALTMETER BASED DEPLOYMENT LOOP//
  
  switch(flight) {
    case Setup:
      Serial.println("SETUP");
        if(avg_alt > 10){
          flight = Launchpad;
        }
      break;
      
    case Launchpad:
      Serial.println("LAUNCHPAD");
      old_Alt = avg_alt;
      if(old_Alt > 50){
        T0 = millis();
        flight = Thrust;
        //timer = Thrust;
      }
      break;
    
    case Thrust:
      Serial.println("THRUST");
      new_Alt = avg_alt;
      T = millis();
      if ((new_Alt - old_Alt)/(millis() - T) < 0){
        digitalWrite(PL, HIGH);
        digitalWrite(SEP, HIGH);
        flight = Descent;
      }
      old_Alt = new_Alt;
      break;
      
    case Descent:
      Serial.println("DESCENT");
      if (avg_alt < 1600){
        digitalWrite(R, HIGH);
        Serial.println("DEPLOYMENT");
      }
      break;

    case Deployment:
      Serial.println("DEPLOYED");
      Serial.println("Waiting to be recovered...");
      delay(5000);
      break;
  }

  
  //TIMER BASED DEPLOYMENT LOOP//
 /* switch(timer){
    case Thrust:
      T1 = millis();
      if(T1 > T0 + 20000){
        digitalWrite(SEP, HIGH);
        digitalWrite(PL, HIGH);
        T0 = millis();
        timer = Descent;
      }
      break;
      
    case Descent:
      T1 = millis();
      if (T1 > T0 + 60000){
        digitalWrite(R, HIGH);
      }
  }
*/
  
//Debug//
   if (loop_counter % 20 == 0) {
    Serial.println("Altitude:");
    Serial.println(avg_alt);
    }
 
 loop_counter++;
//datalogging
 String data = "";
 data = altitude;
 File dataFile = SD.open("Flight.txt", FILE_WRITE);
 if (dataFile){
  dataFile.println(data);
  dataFile.close();
 }
 else{
  Serial.println("error opening file Flight.txt");
 }
 }

