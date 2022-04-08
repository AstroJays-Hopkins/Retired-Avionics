#include <Wire.h>
#include "IntersemaBaro.h"    // pressure sensor
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BNO055.h"  // IMU
#include "utility/imumaths.h"
#include "music/music.h"
#include "music/pitches.h"
#include "music-scores.h"

// WIRING

// A2: accelerometer z
// A1: accelerometer y
// A0: accelerometer x
// 15: uSD SCK
// 14: uSD DO
// 16: uSD DI
// 10: uSD CS

// 2: i2c SCL
// 3: i2c SDA
// 4: buzzer pin
// 8: separation (main??)
// 9: drogue

// i2c goes to IMU (BNO55) and pressure sensor

int Flight_Log[10] = {0,0,0,0,0,0,0,0,0};

int loop_counter;

//%%%%%%%%%%%%%%%%%%%%%%%% MUSIC SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

Music music(4);

//%%%%%%%%%%%%%%%%%%%%%%%% DATALOGGER SETUP %%%%%%%%%%%%%%%%%%%%%%%%//
int chipSelect = 10;

//%%%%%%%%%%%%%%%%%%%%%%%% ALTIMETER SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

Intersema::BaroPressure_MS5607B baro(true);

// Altitude variables
float avg_alt;
float alt0;
float altitude;
float old_Alt;
float new_Alt;
unsigned long T0;
unsigned long T1;
unsigned long T;

//%%%%%%%%%%%%%%%%%%%%%%%% IMU SETUP %%%%%%%%%%%%%%%%%%%%%%%%//
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055(55);
int ang[3] = {0,0,0};

//%%%%%%%%%%%%%%%%%%%%%%%% ACCELEROMETER SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

int xRawMin = 0;
int xRawMax = 512;
 
int yRawMin = 0;
int yRawMax = 512;
 
int zRawMin = 0;
int zRawMax = 512;

int acc[3] = {0,0,0}; //accelerometer readout array; x, y, z axes respectively

//create state machine for deployment stages
enum states{
  None,
  Setup,
  Launchpad,
  Thrust,
  Descent,
  Deployment
};

//set initial state
states flight = None;
states timer = None;
int stages[5] = {1,2,3,4,5};
int currentStage = 1;


//%%%%%%%%%%%%%%%%%%%%%%%% RECOVERY PIN SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

const int SEP = 8; // separation
const int R = 9; // drogue

void setup() { 
  Serial.begin(9600); 
  Serial.println("initializing");
  
  //initialize pins
  pinMode(A0, INPUT); //X
  pinMode(A1, INPUT); //Y
  pinMode(A2, INPUT); //Z
  
  //recovery pins
  pinMode(R, OUTPUT);
  pinMode(SEP, OUTPUT);
  digitalWrite(R, HIGH); // potentially change based on relays
  digitalWrite(SEP, HIGH); // 
  
  //IMU
  bno.begin();
  bno.setExtCrystalUse(true);
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  Serial.println("Sensors Initialized");

  //altimeter
  baro.init();

  //set starting altitude
  alt0 = 0;
  altitude = 0;
  avg_alt = 0;
  int num_points = 50;
  for (int i=0; i<num_points; i++)
    {
      alt0 += baro.getHeightCentiMeters()/30.48;
      delay(10);
    }
  alt0 /= num_points;            //normalize to the number of samples collected

  //set initial states
  flight = Setup;
  timer = None;
  currentStage = stages[0];
 
  //initialize datalogging
  if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
    }
    else{
      File dataFile = SD.open("Flight.txt", FILE_WRITE);

      if (dataFile){
          dataFile.println("Beginning New Flight");
          dataFile.close();
          Serial.println("New Datalog Created");
        }
      else{
          Serial.println("error opening file Flight.txt");
          return;
        }
      Serial.println("Initialization Complete");
    }
    
  // Music initialization
  music.is_playing = true;
}

//create a loop to calculate average acceleration values
int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  for (int i = 0; i < 10; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading/10;
}

void AutoCalibrate(int xRaw, int yRaw, int zRaw)
{
  //Serial.println("Calibrate");
  if (xRaw < xRawMin)
  {
    xRawMin = xRaw;
  }
  if (xRaw > xRawMax)
  {
    xRawMax = xRaw;
  }
  
  if (yRaw < yRawMin)
  {
    yRawMin = yRaw;
  }
  if (yRaw > yRawMax)
  {
    yRawMax = yRaw;
  }
 
  if (zRaw < zRawMin)
  {
    zRawMin = zRaw;
  }
  if (zRaw > zRawMax)
  {
    zRawMax = zRaw;
  }
}

void loop(){

  ////////// Accelerometer //////////
  int xRaw = ReadAxis(A0); //take accelerometer voltages in X,Y,Z directions
  int yRaw = ReadAxis(A1);
  int zRaw = ReadAxis(A2);

  int xScaled = map(xRaw, xRawMin, xRawMax, -1000, 1000); //convert acceleration signals from voltage to numerical values
  int yScaled = map(yRaw, yRawMin, yRawMax, -1000, 1000);
  int zScaled = map(zRaw, zRawMin, zRawMax, -1000, 1000);

  int AX = {xScaled / 1000.0}; //scale acceleration vectors
  int AY = {yScaled / 1000.0};
  int AZ = {zScaled / 1000.0};

  acc[1] = AX; //write acceleration vector values to array
  acc[2] = AY;
  acc[3] = AZ;
  
  ////////// IMU -- using as gyroscope? also don't use anywhere //////////
  sensors_event_t event;
  bno.getEvent(&event);
  ang[1] = {(int)event.orientation.x}; //write orientation angles to array
  ang[2] = {(int)event.orientation.y};
  ang[3] = {(int)event.orientation.z};
  delay(BNO055_SAMPLERATE_DELAY_MS);

  //ALTIMETER BASED DEPLOYMENT LOOP//
  altitude = baro.getHeightCentiMeters()/30.48 - alt0;
  avg_alt += (altitude - avg_alt)/5;
  Serial.println(avg_alt);
  switch(flight) {
    case Setup:
      currentStage = stages[0];
      Serial.println("SETUP");
      
      //%%%SAFETY CONDITION%%%
      /*while (digitalRead(SAFETY) == LOW){
        Serial.println('Please activate safety switch to continue');
      delay(1000);
       }
      */
      music.music_array = ARMED_SIGNAL;
        if(avg_alt > 10){
          flight = Launchpad;
        }
      break;
      
    case Launchpad:
      currentStage = stages[1];
      Serial.println("LAUNCHPAD");
      AutoCalibrate(xRaw,yRaw,zRaw);
      old_Alt = avg_alt;
      if(old_Alt > 100){
        T0 = millis();
        flight = Thrust;
        timer = Thrust;
      }
      break;
    
    case Thrust:
      currentStage = stages[2];
      Serial.println("THRUST");
      music.music_array = THRUST_SIGNAL;
      new_Alt = avg_alt;
      T = millis();
      if ((new_Alt - old_Alt)/(millis() - T) < 0){
        digitalWrite(SEP,LOW);
        flight = Descent;
      }
      old_Alt = new_Alt;
      break;

    case Descent:
      currentStage = stages[3];
      music.music_array = DESCENT_SIGNAL;
      Serial.println("DESCENT");
      if (avg_alt < 1000){
        digitalWrite(R, LOW);
        currentStage = stages[4];
        music.music_array = RECOVERY_SIGNAL;
        Serial.println("RECOVER");
      }
  }

  ////////// link data to arrays //////////
  Flight_Log[0] = {currentStage};
  Flight_Log[1] = {avg_alt};
  Flight_Log[2] = {acc[1]};
  Flight_Log[3] = {acc[2]};
  Flight_Log[4] = {acc[3]};
  Flight_Log[5] = {ang[1]};
  Flight_Log[6] = {ang[2]};
  Flight_Log[7] = {ang[3]};

 String data = "";

 for (int i = 0;i < 10;i++){
    data += Flight_Log[i];
    if (i < 10) {
      data += " ";
    }
 }
 Serial.println("Data acquired");
 File dataFile = SD.open("Flight.txt", FILE_WRITE);

 if (dataFile){
  dataFile.println(data);
  dataFile.close();
 }
 else{
  Serial.println("error opening file Flight.txt");
 }
 
  //Debug//

   if (loop_counter % 20 == 0) {
    Serial.print("Stage ");
    Serial.println(Flight_Log[0]);
    Serial.println("Altitude:");
    Serial.println(Flight_Log[1]);
    Serial.println("Acceleration:");
    Serial.println(Flight_Log[2]);
    Serial.println(Flight_Log[3]); 
    Serial.println(Flight_Log[4]);
    Serial.println("Angular orientation:");
    Serial.println(Flight_Log[5]);
    Serial.println(Flight_Log[6]); 
    Serial.println(Flight_Log[7]);

  }
 
 loop_counter++;
 music.update();
}
