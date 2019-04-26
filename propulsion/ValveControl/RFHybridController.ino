#include <SPI.h>
#include <LoRa.h>

//declare pins for fueling, venting, and disconnect/reset switches
const int fuelSwitch = 4;
const int ventSwitch = 5;
const int dcSwitch = 6;
const int resetSwitch = 7;

//VARIABLE FOR IGNITION BUTTON
const int ignitionSwitch = 8;

//set variables for the condition of each switch
//this corresponds to the desired state of the analogous solenoid
String IGNITION;
String Command1;
String Command2;
String Command3;

//variable to store list of desired states
String Command;

void setup() {
  //begin RF communiation
  LoRa.begin(915E6);
  Serial.begin(9600);
  //set all switch pins to receive voltage
  pinMode(ventSwitch, INPUT);
  pinMode(dcSwitch, INPUT);
  pinMode(resetSwitch, INPUT);
  pinMode(fuelSwitch, INPUT);
  pinMode(ignitionSwitch, INPUT);
}

void ventCom(){
  if (digitalRead(ventSwitch) == HIGH){
    Command1 = "V"; //indicate venting command was sent
  }
}

void dcCom(){
  if (digitalRead(dcSwitch) == HIGH){
    Command2 = "D"; //indicate DC command was sent
  }
}

void resetCom(){
  if (digitalRead(resetSwitch) == HIGH){
    Command2 = "R"; //indicate reset variable was sent
  }
}

void fuelCom() {
  if (digitalRead(fuelSwitch) == LOW){
    Command3 = "F"; //send ignition variable
  }else{
    Command3 = "C";
  }
}

void ignitionCom() {
  if (digitalRead(ignitionSwitch == HIGH){
    IGNITION = "I"; //set ignition variable 
  }
}

void getCom() {
  ventCom();
  dcCom();
  resetCom();
  fuelCom(); //check states, send commands to open/close valves to rocket
  Command = IGNITION + Command1 + Command2 + Command3;
}

void loop() {
  IGNITION = "X"; //set default state variables that correspond to valve default states
  Command1 = "X";
  Command2 = "X";
  Command3 = "X";
  getCom(); //check for desired states
  Serial.println(Command); //DEBUG: display what you sent as the command
  LoRa.beginPacket(); //start transmission
  LoRa.print(Command); //just gonna send it, asuhh dudes
  LoRa.endPacket(); //end transmission
}
