#include <RF24.h>
#include <Wire.h>
#include <Servo.h>
#include "Adafruit_MAX31855.h"

//create servo objects for ball valve control
Servo Burn;

//create an array for the conditions of the servos
boolean Valves [2];

//define the pin to toggle the emergency burp valve
int Emergency = 3;

//define the pin used for the ignition relay
int LIT = 4;

//create arrays for temperature and pressure values across 
//the run tank and combustion chamber
float TC [4]= {0,0,0,0};
float PT [3] = {0,0,0};

//declare SPI pins for thermocouple breakout
int MAXDO = 50;
int TCCS [4] = {47,48,49,50};
int MAXCS;
int MAXCLK = 52;

//initialize thermocouple breakout board
Adafruit_MAX31855 thermocouple1(MAXCLK, TCCS[0], MAXDO);
Adafruit_MAX31855 thermocouple2(MAXCLK, TCCS[1], MAXDO);
Adafruit_MAX31855 thermocouple3(MAXCLK, TCCS[2], MAXDO);
Adafruit_MAX31855 thermocouple4(MAXCLK, TCCS[3], MAXDO);

Adafruit_MAX31855 Thermocouples [4] = {thermocouple1,thermocouple2,thermocouple3,thermocouple4};



//define pins for sensor data
byte Pins[3] = {A0,A1,A2};

//define condition for ignition to open ball valve
int ARMED = 4; //put switch at pin 4 

//define a specific value as our launch command code
int LIVE = 420;

//radio communication setup
bool radioNumber = 0;
RF24 radio(7,8);
byte addresses[][6] = {"1Node","2Node"}; 
bool role=0;


////////////////////////////////////////////////


void setup() {
  Serial.begin(9600);
  
  //set valve conditions to "closed"
  for(int i = 0; i < 2; i++){
  Valves[i] = false;
  }
  
  //ensure ignition pin is turned off
  digitalWrite(LIT,LOW);
  
  //attach valves to PWM pins
  Burn.attach(5);
  Serial.println("BALL VALVE INITIALIZED");

  Serial.println("STABLIZING THERMOCOUPLES...");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.println("THERMOCOUPLES STABILIZED");
  Serial.println("SENSORS READY");


  //establish communication
  radio.begin();
  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data
  radio.startListening();

  Serial.println("RADIO READY");
  Serial.println("SYSTEM READY");
}




////////////////////////////////////////////////




void loop() {

  //do not vent in flight, therefore do not transmit/receive data after launch command has been issued
  while(FIRE = 0){  
    for(int j = 0; j < 2; j++){
      MAXCS = TCCS[j];  //define CS pin for each thermocouple in the run tank
      TC[j] = Thermocouples[j].readCelsius(); //take run tank temperatures
      MAXCS = TCCS[j+2]; //define CS pin for each thermocouple in the combustion chamber
      TC[j+2] = Thermocouples[j+2].readCelsius(); //take combustion chamber temperatures
    }
    
    for(int k = 0; k < 3; k++){
      PT[k] = analogRead(Pins[k]);
      PT[k] = map(PT[k],0,1023,0,10000);
    }
  
    for (int l = 0; l < 2; l++){ //test for critical temperature: open solenoid if above threshold, close if below
      if(TC[l] >= 309.5){
          Valves[0] = true;
          Serial.println("TEMPERATURE CRITICAL");
      }else{
        Valves[0] = false;
      }
    }

    for (int m = 0; m < 3; m++){ //test for critical pressure: open solenoid if above threshold, close if below
      if(PT[m] >= 7240){
        Valves[0] == true;
        Serial.println("PRESSURE CRITICAL");
      }else{
        Valves[1] = false;
      }
    }
    
    if(Valves[0] == true){
      digitalWrite(Emergency,LOW);
    }else if (Valves[0] == false){
      digitalWrite(Emergency,HIGH);
    }
  
    if(Valves[1] == false){
      Burn.write(0);
    }else if(Valves[1] == true){
      Burn.write(180);
    }
  
    radio.stopListening();        // First, stop listening so we can talk.
    Serial.println(F("Now sending"));
    for (int a = 0; a < 4; a++){

      unsigned long temp = TC[a];                             // Take the temperature, and send it.  This will block until complete
      if (!radio.write( &temp, sizeof(unsigned long) )){
        Serial.println(F("failed"));
        }else{
        radio.write(&temp, sizeof(unsigned long));
      }
    }
    
    for (int b = 0; b < 3; b++){
      unsigned long Pressure = PT[b];                             // Take the pressure, and send it.  This will block until complete
      if (!radio.write( &Pressure, sizeof(unsigned long) )){
        Serial.println(F("failed"));
      }else{
        radio.write(&Pressure, sizeof(unsigned long));
      }
    }
    radio.startListening();                                    // Now, continue listening
    
    int FIRE;               // Set up a timeout period, get the current microseconds
    
    //determine if launch signal has been sent
      if (digitalRead(ARMED) == HIGH){ 
        if( radio.available()){
                                                                    // Variable for the received command
          while (radio.available()) {                                   // While there is data ready
            radio.read( &FIRE, sizeof(int) );             // Get the fire command message
          }
        }
        
      if (FIRE == LIVE){    //if fire command matches the value we determine
        Valves[1] = true;   //open the ball valve 
        digitalWrite(LIT,HIGH);   //ignite the solid fuel grain
      }
    }  
  }
  radio.stopListening();
} 
