#include <LoRa.h>

const int EMATCH_RELAY_PIN = 5;  // Connect to signal line of relay that switches firing current; change as needed.
const int IGNITE_MAX_TIME = 2000; // Maximum time, in milliseconds, that relay is to be kept closed while attempting to light the ematch.
long int IgnitingStart = 0;  // Time relay to ignite ematch was closed

void setup() {
  /* Initialize radio */
  LoRa.begin(915E6);

  // For testing:
  Serial.begin(9600);

  pinMode(EMATCH_RELAY_PIN, OUTPUT);
}

void loop() {
  String command;
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) { //get full RF command
      command += LoRa.read();
    }
    Serial.println(command);
  }

  // Fire ematch if asked to do so
  if (command[0] == 51) {
    digitalWrite(EMATCH_RELAY_PIN, HIGH);
    IgnitingStart = millis();
  }

  // Open ematch-firing relay if max ignition time is up
  if (IgnitingStart && (millis() - IgnitingStart) > IGNITE_MAX_TIME) {
    digitalWrite(EMATCH_RELAY_PIN, LOW);
    IgnitingStart = 0;
  }
}
