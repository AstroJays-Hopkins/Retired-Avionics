#include <Wire.h>
#include <SPI.h>
#include <SD.h>

String packet1;
String packet2;
String Data;
int end1;
int end2;

const int chipSelect = 10;

void setup() {
  Wire.begin();        // join i2c bus
  Serial.begin(115200);  // start serial for output to GUI
  
  // see if the card is present and can be initialized:
  SD.begin(chipSelect);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }else{
    Serial.println("card initialized.");
  }
}

void loop() {
  //request data from sensing, then peripheral devices
  Wire.requestFrom(8, 30);    // request 30 bytes from sensing device
  while (Wire.available()) { // slave may send less than requested
    packet1 = Wire.readString(); // receive a byte as a string
  }
  Wire.requestFrom(9,30);// request 30 bytes from peripheral device
  while (Wire.available()){ // slave may send less than requested
    packet2 = Wire.readString(); // receive a byte as a string
  }

  //determine position of terminating character of each packet
  end1 = packet1.indexOf("x");
  end2 = packet2.indexOf("x");

  //remove all noise caused by extra bytes
  packet1 = packet1.substring(end1);
  packet2 = packet2.substring(end2);

  //combine packets from both devices
  Data = packet1 + packet2;
  
  File dataFile = SD.open("COLDFLOW.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(Data);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening COLDFLOW.txt");
  }
  Serial.println(Data);
}
