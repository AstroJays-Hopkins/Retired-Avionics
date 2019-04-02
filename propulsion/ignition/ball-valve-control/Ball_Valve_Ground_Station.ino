#include <SPI.h>
#include <LoRa.h>

//define initial state and desired command state variables
char INITIAL;
char COMMAND;

void setup() {
  Serial.begin(9600);
  Serial.println("BEGINNING SETUP"); //comment out
  
  //Begin rf communication
  if (!LoRa.begin(915E6)) {
    Serial.println("UNABLE TO INITIALIZE COMMUNICATION");
    while (1);
  }
  Serial.println("SETUP COMPLETE"); //comment out
}

//take serial input from computer to determine desired state
void getCommand() {
  COMMAND = Serial.read();
}

//send command for desired state to ball valve controller
void sendCommand() {
    LoRa.beginPacket();
    LoRa.print(COMMAND);
    LoRa.endPacket();
    Serial.println("COMMAND SENT");
    INITIAL = COMMAND; //indicate that the controller has reached its desired state
}

void loop() {
  Serial.println("STARTING"); //comment out
  Serial.println("READY TO RECEIVE USER INPUT");
  Serial.println("F: OPEN"); //state options for ball valve positions
  Serial.println("R: CLOSE");
  while (COMMAND == INITIAL){
    getCommand();
    if (COMMAND != INITIAL){
      sendCommand();
    }
    switch (COMMAND) {
      case 'F':
        Serial.println("BALL VALVE OPENED");
        Serial.println("IGNITION SEQUENCE AUTHORIZED"); //will be replaced with digitalWrite(pin,HIGH) for hot fire
        break;
      case 'R':
        Serial.println("BALL VALVE CLOSED");
        break;
    }
  }
}
