#include <SPI.h>
#include <Wire.h>
#include "Adafruit_MAX31855.h"

float TC [2]= {0,0};

//declare SPI pins for thermocouple breakout
int MAXDO = 50;
int TCCS [2] = {49,48};
int MAXCLK = 52;

float PT [2] = {0,0};

//initialize thermocouple breakout board
Adafruit_MAX31855 thermocouple1(MAXCLK, TCCS[0], MAXDO);
Adafruit_MAX31855 thermocouple2(MAXCLK, TCCS[1], MAXDO);

Adafruit_MAX31855 Thermocouples [2] = {thermocouple1,thermocouple2}; //create thermocouple array

//define pins for pressure transducer data
byte Pins[3] = {A0,A1};

//create a variable for the critical parameter of the oxidizer
String CRIT;
String Data = "";

////////////////////////////////////////////////


void setup() {
  //Begin serial communication for debugging
  Serial.begin(115200);
 
  //Initialize I2C line between ground station and sensing board
  Wire.begin(8);
  Wire.onRequest(requestEvent); // register event

  //Initalize thermocouples
  Serial.println("STABLIZING THERMOCOUPLES...");
  // wait for MAX chip to stabilize
  delay(1000);
  Serial.println("THERMOCOUPLES STABILIZED");
  
  //Initialize pressure transducers
  pinMode(Pins,INPUT);
  Serial.println("SENSORS READY");
  CRIT = "STABLE";
  
  Serial.println("Initialization Complete");
  delay(1000);
}


////////////////////////////////////////////////




void loop() {

  Serial.println("Starting");
  
  //reset critical condition
  CRIT = "STABLE";

  //take temperature
  Serial.println("Taking temperature...");
  for(int j = 0; j < 2; j++){
    TC[j] = Thermocouples[j].readCelsius(); //take temperature data in celsius
  }
  
  //take pressure
  Serial.println("Taking pressure...");
  for(int k = 0; k < 2; k++){
    PT[k] = ((analogRead(Pins[k]) - 100.00) / 820.00) * 5076.00; //take pressure data from transducers, adjust for factory calibration 
  }
  
  Serial.println("Checking for critical temperature");
  for (int l = 0; l < 2; l++){ //test for critical temperature: open solenoid if above threshold, close if below
    if(TC[l] >= 309.5){
        CRIT = "TEMPERATURE";
        Serial.println("TEMPERATURE CRITICAL");
    }
  }

  Serial.println("Checking for critical pressure");
  for (int m = 0; m < 2; m++){ //test for critical pressure: open solenoid if above threshold, close if below
    if(PT[m] >= 7240){
      CRIT = "PRESSURE";
      Serial.println("PRESSURE CRITICAL");
    }
  }
  
  ////Data formatting////
  //append thermocouple data
  for (int n = 0; n < 2; n++){
    Data+=TC[n];
    Data+=" ";
  }
  //append pressure transducer data
  for (int x = 0; x < 2; x++){
    Data+=PT[x];
    Data+=" ";
  }

  //append critical condition
  Data+=CRIT;
  Data+=" ";

  //append terminating character
  Data+=" x";
}

void requestEvent() {
  Wire.write(Data.c_str());
}
