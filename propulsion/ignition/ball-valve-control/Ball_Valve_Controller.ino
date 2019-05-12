#include <SPI.h>
#include <LoRa.h>

char INITIAL;
char COMMAND;

void setup() {
  Serial.begin(9600);
  Serial.println("BEGINNING SETUP");
  if (!LoRa.begin(915E6)) {
    Serial.println("UNABLE TO INITIALIZE COMMUNICATION");
    while (1);
  }
  Serial.println("SETUP COMPLETE");
}

void getCommand() {
  COMMAND = Serial.read();
}

void sendCommand() {
    LoRa.beginPacket();
    LoRa.print(COMMAND);
    LoRa.endPacket();
    Serial.println("COMMAND SENT");
    INITIAL = COMMAND;
}

void loop() {
  Serial.println("STARTING");
  Serial.println("READY TO RECEIVE USER INPUT");
  Serial.println("F: OPEN");
  Serial.println("R: CLOSE");
  while (COMMAND == INITIAL){
    getCommand();
    if (COMMAND != INITIAL){
      sendCommand();
    }
    switch (COMMAND) {
      case 'F':
        Serial.println("BALL VALVE OPENED");
        Serial.println("IGNITION SEQUENCE AUTHORIZED");
        break;
      case 'R':
        Serial.println("BALL VALVE CLOSED");
        break;
    }
  }
}
