#include <LoRa.h>

//declare pins for solenoid relays
const int fuelRelay = 4;
const int ventRelay = 5;
const int dcRelay = 6;
const int resetRelay = 7;

void setup() {
  Serial.begin(9600);
  
  //begin RF communication
  LoRa.begin(915E6);

  //set relay pins to output voltage
  pinMode(ventRelay,OUTPUT);
  pinMode(dcRelay,OUTPUT);
  pinMode(resetRelay,OUTPUT);
  pinMode(fuelRelay,OUTPUT);

  //set default relay states to LOW (default)
  digitalWrite(ventRelay,LOW);
  digitalWrite(dcRelay,LOW);
  digitalWrite(resetRelay,LOW);
  digitalWrite(fuelRelay,LOW);
}

void vent() { //open venting solenoid
  digitalWrite(ventRelay,HIGH);
  Serial.println("VENTING");
}

void DC() { //disconnect fueling line
  digitalWrite(dcRelay,HIGH);
  Serial.println("DISCONNECTING FUELING LINE");
}

void RESET() { //reset fueling line linear actuator
  digitalWrite(resetRelay,HIGH);
  Serial.println("RESETTING FUELING LINE");
}

void FUEL() { //open fueling valve
  digitalWrite(fuelRelay,HIGH);
  Serial.println("FUELING VALVE OPEN");
}

void DONE() { //close fueling valve
  digitalWrite(fuelRelay,LOW);
  Serial.println("FUELING VALVE CLOSED");
}

void CLOSE() { //close all valves
  digitalWrite(dcRelay,LOW);
  digitalWrite(resetRelay,LOW);
  digitalWrite(ventRelay,LOW);
  digitalWrite(fuelRelay,LOW);
}

void loop() {
  //set variable for received RF command
  String rfInput;
  
  int packetSize = LoRa.parsePacket();
  if (packetSize){ //receive RF commands
    while (LoRa.available()) {
      rfInput += (char)LoRa.read(); //append string with each solenoid state condition
    }  
  }
  Serial.println(rfInput); //DEBUG: display RF command

  //Check states
  if(rfInput[1] == "V"){
    vent();
  }
  
  if(rfInput[2] == "D"){
    DC();
  }else if(rfInput[2] == "R"){
    RESET();
  }

  if(rfInput[3] == "F"){
    FUEL();
  }else if(rfInput[3] == "C"){
    DONE();
  }
  
  if(rfInput[0] == "I"){
    CLOSE();
    Serial.println("FIRING");
    
  }
}
