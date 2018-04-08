//#include <RH_RF95.h>
//#include <Adafruit_GPS.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "IntersemaBaro.h"

const int CS = 4;

float Flight_Log[9] = {0,0,0,0,0,0,0,0,0};
//%%%%%%%%%%%%%%%%%%%%%%%% GPS SETUP %%%%%%%%%%%%%%%%%%%%%%%%//
/*
float latitude,longitude;

//what's the name of the hardware serial port?
SoftwareSerial GPSSerial(1,0);


//Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

//Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
//Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  true
*/
//%%%%%%%%%%%%%%%%%%%%%%%% ALTIMETER SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

Intersema::BaroPressure_MS5607B baro(true);

// Altitude variables
int avg_alt;
int alt0;
int altitude;
int old_Alt;
int new_Alt;
unsigned long T0;
unsigned long T1;
unsigned long T;

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




//%%%%%%%%%%%%%%%%%%%%%%%% IMU SETUP %%%%%%%%%%%%%%%%%%%%%%%%//
#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055(55);
int ang[3] = {0,0,0};


//%%%%%%%%%%%%%%%%%%%%%%%% ACCELEROMETER SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

float xRawMin = 0;
float xRawMax = 512;
 
float yRawMin = 0;
float yRawMax = 512;
 
float zRawMin = 0;
float zRawMax = 512;

int acc[3] = {0,0,0}; //accelerometer readout array; x, y, z axes respectively

//%%%%%%%%%%%%%%%%%%%%%%%% RECOVERY PIN SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

int R = 5;
//%%%%%%%%%%%%%%%%%%%%%%%% PAYLOAD PIN SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

int PL = 6;

//%%%%%%%%%%%%%%%%%%%%%%%% RADIO SETUP %%%%%%%%%%%%%%%%%%%%%%%%//
int loop_counter = 0;
//RH_RF95 rf95;
//#define RF95_FREQ 915.0

void setup() {
//begin communication
Serial.begin(9600); //set to 115200 when using GPS
//GPS.begin(9600);

if (!SD.begin(CS)) {
    Serial.println("Card failed, or not present");
    return;
}
//initialize pins
pinMode(A1, INPUT);
pinMode(A2, INPUT);
pinMode(A3, INPUT);
pinMode(R, OUTPUT);
pinMode(PL, OUTPUT);
digitalWrite(R, LOW);
digitalWrite(PL, LOW);

Serial.println("Booted up");
//initialize sensors
baro.init();
bno.begin();
bno.setExtCrystalUse(true);
uint8_t system, gyro, accel, mag;
system = gyro = accel = mag = 0;
bno.getCalibration(&system, &gyro, &accel, &mag);
Serial.println("Initialized");

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
}


//create a loop to calculate average acceleration values
float ReadAxis(int axisPin)
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


void loop() {

  ////////// Altimeter //////////
  altitude = baro.getHeightCentiMeters()/30.48 - alt0;
  avg_alt += (altitude - avg_alt)/5;
  
////////// Accelerometer //////////
  float xRaw = ReadAxis(A1); //take accelerometer voltages in X,Y,Z directions
  float yRaw = ReadAxis(A2);
  float zRaw = ReadAxis(A3);

  long xScaled = map(xRaw, xRawMin, xRawMax, -1000, 1000); //convert acceleration signals from voltage to numerical values
  long yScaled = map(yRaw, yRawMin, yRawMax, -1000, 1000);
  long zScaled = map(zRaw, zRawMin, zRawMax, -1000, 1000);

  float AX = {xScaled / 1000.0}; //scale acceleration vectors
  float AY = {yScaled / 1000.0};
  float AZ = {zScaled / 1000.0};

  acc[1] = AX; //write acceleration vector values to array
  acc[2] = AY;
  acc[3] = AZ;

  
  ////////// IMU //////////
  sensors_event_t event;
  bno.getEvent(&event);
  ang[1] = {(int)event.orientation.x}; //write orientation angles to array
  ang[2] = {(int)event.orientation.y};
  ang[3] = {(int)event.orientation.z};
  delay(BNO055_SAMPLERATE_DELAY_MS);
  
  
  ////////// Recovery and Payload Activation //////////
  
  //ALTMETER BASED DEPLOYMENT LOOP//
  
  switch(flight) {
    case Setup:
      //Serial.println("SETUP");
        if(avg_alt > 10){
          flight = Launchpad;
        }
      break;
      
      
    case Launchpad:
      //Serial.println("LAUNCHPAD");
      old_Alt = avg_alt;
      AutoCalibrate(xRaw,yRaw,zRaw);
      if(old_Alt > 100){
        T0 = millis();
        flight = Thrust;
        timer = Thrust;
      }
      break;
    
    case Thrust:
     // Serial.println("THRUST");
      new_Alt = avg_alt;
      T = millis();
      if ((new_Alt - old_Alt)/(millis() - T) < 0){
        digitalWrite(PL, HIGH);
        flight = Descent;
      }
      old_Alt = new_Alt;
      break;

    case Descent:
      //Serial.println("DESCENT");
      if (avg_alt < 400){
        digitalWrite(R, HIGH);
        //Serial.println("RECOVER");
      }
  }


  //TIMER BASED DEPLOYMENT LOOP//

  switch(timer){
    case Thrust:
      T1 = millis();
      if(T1 > T0 + 20000){
        digitalWrite(PL, HIGH);
        T0 = millis();
        timer = Descent;
      }
      break;
      
    case Descent:
      T1 = millis();
      if (T1 > T0 + 60000){
        digitalWrite(R, HIGH);
      }
  }

  
  ////////// GPS //////////
// latitude = GPS.latitudeDegrees;
// longitude = GPS.latitudeDegrees;


  ////////// link data to arrays //////////

  Flight_Log[1] = {avg_alt};
  Flight_Log[2] = {acc[1]};
  Flight_Log[3] = {acc[2]};
  Flight_Log[4] = {acc[3]};
  Flight_Log[5] = {ang[1]};
  Flight_Log[6] = {ang[2]};
  Flight_Log[7] = {ang[3]};
//  Flight_Log[8] = {latitude};
//  Flight_Log[9] = {longitude};


 if (loop_counter % 20 == 0) {
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

 String data = "";

 for (int i = 0; i < 7; i++){
  data += String(Flight_Log[i]);
  if (i<6){
    data += ",";
  }
 }

 File dataFile = SD.open("Flight_Log.txt", FILE_WRITE);

 if (dataFile){
  dataFile.println(data);
  dataFile.close();
  Serial.println(data);
 }
 else{
  Serial.println("error opening Flight_Log.txt");
 }
 /*
 if (!rf95.available()){
         Serial.println("NO RADIO");
  }
  else{
    
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      RH_RF95::printBuffer("request: ", buf, len);
      //Serial.print("got request: ");
      //Serial.println((char*)buf);
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);
      
      // Send a reply
      uint8_t data[] = {Flight_Log};
      rf95.send(data,sizeof(data));
      rf95.waitPacketSent();
    }
  }
  */
 }


