#include <SPI.h>
#include <LoRa.h>
#include <SD.h>
const int chipSelect = 9;
char Data;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // read packet
    while (LoRa.available()) {
      Data = (char)LoRa.read();
      File dataFile = SD.open("datalog.txt", FILE_WRITE);

      // if the file is available, write to it:
      if (dataFile) {
        dataFile.println(Data);
        dataFile.close();
        // print to the serial port too:
        Serial.println(Data);
      }
    // if the file isn't open, pop up an error:
        else {
          Serial.println("error opening datalog.txt");
        }
      }
    }
    else{Serial.println("LOST CONNECTION");
  }
  delay(500);
}
