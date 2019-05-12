#include <SPI.h>
#include <LoRa.h>

//declare pins for fueling, venting, and disconnect/reset switches
const int fuelSwitch = 4;
const int ventSwitch = 5;
const int dcSwitch = 6;
const int resetSwitch = 7;

//VARIABLES FOR IGNITION SAFETY AND BUTTON
const int ignitionLock = 8;
const int ignitionSwitch = 9;
int tOpen = 0;

//set variables for the condition of each switch
//this corresponds to the desired state of the analogous solenoid
String CommandV;
String CommandD;
String CommandF;
String BV;

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
  pinMode(BVForward, INPUT);
  pinMode(BVReverse, INPUT);
  pinMode(ignitionSwitch, INPUT);
}

void ventCom(){
  if (digitalRead(ventSwitch) == HIGH){
    CommandV = "V"; //indicate venting command was sent
  }
}

void dcCom(){
  if (digitalRead(dcSwitch) == HIGH){
    CommandD = "D"; //indicate DC command was sent
  }
}

void resetCom(){
  if (digitalRead(resetSwitch) == HIGH){
    CommandD = "N"; //indicate reset variable was sent
  }
}

void fuelCom() {
  if (digitalRead(fuelSwitch) == LOW){
    CommandF = "B"; //indicate fueling variable was sent
  }else if (digitalRead(fuelSwitch) == HIGH){
    CommandF = "S"; //indicate stop fueling varaible was sent
  }
}

void BV() {
  if (ignitionLock !=) {
    if (digitalRead(BVForward == HIGH){
      BV = "F"; //indicate ball valve was driven forward
    }else if (digitalRead(BVReverse == HIGH){
      BV = "R"; //indicate ball valve was driven in reverse
    }
  }
 }

void ignitionCom() {
  if (digitalRead(ignitionLock == HIGH){
    if (digitalRead(ignitionSwitch == HIGH){
      BV = "I"; //indicate ignition variable was sent
    }
  }
}

void getCom() { //check states, send commands to open/close valves to rocket
  ventCom();
  dcCom();
  resetCom();
  fuelCom(); 
  BV();
}

void loop() {
  CommandV = "X"; //set states to default holding state variables
  CommandD = "X";
  CommandF = "X";
  BV = "X";
  getCom(); //detect desired states
  if (digitalRead(igntionLock) == HIGH){ //only execute ignition function if key switch is turned to ON position
    ignitionCom();
  }
  Command = CommandV + CommandD + CommandF + BV; //compile data packet to send to solenoid actuator
  Serial.println(Command); //DEBUG: display what you sent as the command
  LoRa.beginPacket(); //start transmission
  LoRa.print(Command); //just gonna send it, asuhh dudes
  LoRa.endPacket(); //end transmission
}
