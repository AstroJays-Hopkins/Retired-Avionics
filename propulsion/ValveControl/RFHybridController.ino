#include <SPI.h>
#include <LoRa.h>

//declare pins for fueling, venting, and disconnect/reset switches
const int fuelSwitch = 4;
const int ventSwitch = 5;

//VARIABLES FOR IGNITION SAFETY AND BUTTON
const int ignitionLock = 6;
const int ignitionSwitch = 7;
int tOpen = 0;

//set variables for the condition of each switch
//this corresponds to the desired state of the analogous solenoid
String CommandV;
String CommandD;
String CommandF;
String BValve;
String trimKey;

//variable to store list of desired states
String Command;

void setup() {
  //begin RF communiation
  LoRa.begin(915E6);
  Serial.begin(9600);
  //set all switch pins to receive voltage
  pinMode(ventSwitch, INPUT);
  pinMode(fuelSwitch, INPUT);
  pinMode(ignitionSwitch, INPUT);
  pinMode(ignitionLock, INPUT);
}

void ventCom(){
  if (digitalRead(ventSwitch) == LOW){
    CommandV = "V"; //indicate venting command was sent
  } 
}

void fuelCom() {
  if (digitalRead(fuelSwitch) == LOW){
    CommandF = "C"; //indicate fueling variable was sent
  }else if (digitalRead(fuelSwitch) == HIGH){
    CommandF = "O"; //indicate stop fueling varaible was sent
  }
}

void BV() {
  if (ignitionLock != HIGH) {
    if (trimKey == "F"){
      BValve = "F"; //indicate ball valve was driven forward
    }else if (trimKey == "R"){
      BValve = "R"; //indicate ball valve was driven in reverse
    }
  }
 }

void ignitionCom() {
  if (digitalRead(ignitionLock == HIGH)){
    if (digitalRead(ignitionSwitch == LOW)){
      BValve = "I"; //indicate ignition variable was sent
    }
  }
}

void getCom() { //check states, send commands to open/close valves to rocket
  ventCom();
  fuelCom(); 
  BV();
}

void loop() {
  CommandV = "X"; //set states to default holding state variables
  CommandF = "X";
  BValve = "X";
  trimKey = Serial.read();
  getCom(); //detect desired states
  if (digitalRead(ignitionLock) == HIGH){ //only execute ignition function if key switch is turned to ON position
    ignitionCom();
  }
  Command = CommandV + CommandF + BValve; //compile data packet to send to solenoid actuator
  Serial.println(Command); //DEBUG: display what you sent as the command
  LoRa.beginPacket(); //start transmission
  LoRa.print(Command); //just gonna send it, asuhh dudes
  LoRa.endPacket(); //end transmission
}
