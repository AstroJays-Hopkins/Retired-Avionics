#include <SPI.h>
#include <LoRa.h>

const int inputs[4] = {4,5,6,7};
int commands[3];

void setup() {
  for(int i=0; i<4; i++){     pinMode(inputs[i], INPUT);   }
  Serial.begin(9600);
  while (!Serial);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(2);
}

void loop() {
  Serial.println("Sending packet: ");

  // send packet
  LoRa.beginPacket();
  for(int i=0; i<3; i++){
   LoRa.print(digitalRead(inputs[i])); 
   Serial.print(digitalRead(inputs[i]));
  }
  LoRa.endPacket();
}
