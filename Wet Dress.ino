#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include "Adafruit_MAX31855.h"

//create servo objects
Servo Burn;

//create an array for the conditions of the servos
boolean burpValve;

//define the pin to regulate the emergency burp valve
int Emergency = 3;

//define the pin used for the ignition relay
int LIT = 4;

float TC [2]= {0,0};

//declare SPI pins for thermocouple breakout
int MAXDO = 50;
int TCCS [2] = {49,48};
int MAXCS;
int MAXCLK = 52;

float PT [2] = {0,0};

//create arrays for temperature and pressure values across 
//the run tank and combustion chamber
//initialize thermocouple breakout board
Adafruit_MAX31855 thermocouple1(MAXCLK, TCCS[0], MAXDO);
Adafruit_MAX31855 thermocouple2(MAXCLK, TCCS[1], MAXDO);
//Adafruit_MAX31855 thermocouple3(MAXCLK, TCCS[2], MAXDO);
//Adafruit_MAX31855 thermocouple4(MAXCLK, TCCS[3], MAXDO);

Adafruit_MAX31855 Thermocouples [2] = {thermocouple1,thermocouple2}; //create thermocouple array



//define pins for sensor data
byte Pins[3] = {A0,A1,A2};

//define condition for ignition to open ball valve
int ARMED = 4; //put switch at pin 4 

//create numerical code for fire command
int FIRE = 0;

//create a variable for the critical parameter of the oxidizer
String CRIT;
String Data = "";



////////////////////////////////////////////////


void setup() {
  //begin serial communication for debugging
  Serial.begin(9600);

  //begin communication with load cells
  Serial1.begin(9600);
  Serial2.begin(9600);
  
  //set valve conditions to "closed"
  burpValve = false;
  
  
  //attach valves to PWM pins
  Burn.attach(5);
  Burn.write(0);
  Serial.println("BALL VALVE INITIALIZED");

  Serial.println("STABLIZING THERMOCOUPLES...");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.println("THERMOCOUPLES STABILIZED");

  //initialize pressure transducers
  pinMode(Pins,INPUT);
  Serial.println("SENSORS READY");
  CRIT = "STABLE";

}




////////////////////////////////////////////////




void loop() {

    //Serial.print("Starting");
    CRIT = "STABLE";
    for(int j = 0; j < 2; j++){
      MAXCS = TCCS[j]; //change CS line depending on which thermocouple data is taken from
      TC[j] = Thermocouples[j].readCelsius(); //take temperature data in celsius
    }
    //Serial.println("Taking pressure...");
    for(int k = 0; k < 2; k++){
      PT[k] = ((analogRead(Pins[k]) - 100) / 820) * 5076; //take pressure data from transducers, adjust for factory calibration 
    }
    
    //Serial.println("Checking for critical temperature");
    for (int l = 0; l < 2; l++){ //test for critical temperature: open solenoid if above threshold, close if below
      if(TC[l] >= 309.5){
          burpValve = true;
          CRIT = "TEMP";
          Serial.println("TEMPERATURE CRITICAL");
      }else{
        burpValve = false;
      }
    }

    //Serial.println("Checking for critical pressure");
    for (int m = 0; m < 2; m++){ //test for critical pressure: open solenoid if above threshold, close if below
      if(PT[m] >= 7240){
        burpValve == true;
        CRIT = "PRESSURE";
        Serial.println("PRESSURE CRITICAL");
      }else{
        burpValve = false;
      }
    }

    
    if(burpValve == true){ //control solenoid based on valve condition
      digitalWrite(Emergency,LOW);
    }else if (burpValve == false){
      digitalWrite(Emergency,HIGH);
    }

  ////Data processing////

  String Data = "";
  for (int n = 0; n < 2; n++){
    Data+=TC[n];
    Data+=" ";
  }
  for (int x = 0; x < 2; x++){
    Data+=PT[x];
    Data+=" ";
  }
  Data+=CRIT;

  //append load cell data
  Data+=Serial1.read();
  Data+=" ";
  Data+=Serial2.read();
  
  ////Data transmission////

  LoRa.beginPacket();
  LoRa.print(Data);
  LoRa.endPacket();
  Serial.println(Data);
  int packetSize = LoRa.parsePacket();
  if (packetSize) { // read packet
    while (LoRa.available()) {
      FIRE = ((int)LoRa.read());
    }
  }
}

//interrupt that 
void Ignition(){
  while (digitalRead(4)==HIGH){  //only capable of ignition if the safety lock on the engine has been disarmed
    if (FIRE = 1){    //if FIRE command matches the encoded value we determine
      Burn.write(180);  //open ball valve
      digitalWrite(LIT,HIGH);   //ignite the solid fuel grain
    }else{
      Burn.write(0); //if FIRE command does not match encoded value, keep the ball valve closed
    }
  }
}
 

