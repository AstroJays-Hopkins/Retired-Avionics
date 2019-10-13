#include <SPI.h>
#include <LoRa.h>

//declare pins for fueling, venting, and disconnect/reset switches
const int fuelSwitch = 4;
const int ventSwitch = 5;

//VARIABLES FOR IGNITION SAFETY AND BUTTON
const int ignitionLock = 7;
const int ignitionSwitch = 3;
int tOpen = 0;

const int BVSwitch = 6;

//set variables for the condition of each switch
//this corresponds to the desired state of the analogous solenoid
int8_t CommandV;
int8_t CommandD;
int8_t CommandF;
int8_t BValve;
int8_t trimKey;

//variable to store list of desired states
int8_t Command[3];

//variable to save last valve command on button to prevent repetitive signal sending? -> only "switch" commands processed
bool prevBVCommand = false;
bool prevIgnitionCommand = false;
bool prevFuelCommand = true;
bool prevVentCommand = true;
bool newCommand = false; 

void setup() {
  //begin RF communiation
  LoRa.begin(915E6);
  LoRa.setTxPower(2);
  Serial.begin(9600);
  //set all switch pins to receive voltage
  pinMode(BVSwitch, INPUT);
  pinMode(ventSwitch, INPUT);
  pinMode(fuelSwitch, INPUT);
  pinMode(ignitionSwitch, INPUT);
  pinMode(ignitionLock, INPUT);
}

void ventCom(){
  if (digitalRead(ventSwitch) == LOW and prevVentCommand == true){
    CommandV = 1; //indicate venting command was sent
    prevVentCommand = false;
    newCommand = true;
  } 
  else if(digitalRead(ventSwitch) == HIGH and prevVentCommand == false){
    CommandV = 0;
    prevVentCommand = true;
    newCommand = true;
  }
}

void fuelCom() {
  if (digitalRead(fuelSwitch) == LOW and prevFuelCommand == true){
    CommandF = 1; //indicate fueling variable was sent
    prevFuelCommand = false;
    newCommand = true;
  }else if (digitalRead(fuelSwitch) == HIGH and prevFuelCommand == false){
    CommandF = 0; //indicate stop fueling varaible was sent
    prevFuelCommand = true;
    newCommand = true;
  }
}

void BV() {
  if (Serial.available()) { // The testing of valve via serial input, which can happen EVEN IF IGNITION LOCK IS LOW
    trimKey = Serial.read();
    if (trimKey == 1 or trimKey == 0){
      BValve = trimKey; //indicate ball valve was driven in the direction determined by the 
      newCommand = true;
    }
  }
  if (true) { // Control of bValve through button is only possible when ignition lock is UNLOCKED
    if (digitalRead(BVSwitch) == HIGH and prevBVCommand == false){
      BValve = 1;
      prevBVCommand = true;
      newCommand = true;
    }
    else if(digitalRead(BVSwitch) == LOW and prevBVCommand == true){
      BValve = 0;
      prevBVCommand = false;
      newCommand = true;
    }
  }
}

void ignitionCom() {
  if (digitalRead(ignitionLock) == HIGH and digitalRead(ignitionSwitch) == HIGH and prevIgnitionCommand != true){ 
    //only execute ignition function if key switch is turned to ON position
    BValve = 3; //indicate ignition variable was sent
    prevIgnitionCommand = true;
    newCommand = true;
  }
}

void getCom() { //check states, send commands to open/close valves to rocket
  ventCom();
  fuelCom(); 
  BV();
  ignitionCom();
}

void loop() {
  CommandV = 2; //set states to default holding state variables
  CommandF = 2;
  BValve = 2;
  trimKey = 2;

  getCom(); //detect desired states
  int8_t Command[3] = {CommandV, CommandF, BValve}; //compile data packet to send to solenoid actuator
  // Serial.println(Command); //DEBUG: display what you sent as the command
  if(newCommand){
    for(int i=0; i<3; i++){
      Serial.print(Command[i]);
      Serial.println();
    }
    Serial.println("New Command");
    Serial.println("------------------------------");
    newCommand = false;
    
  }
}
