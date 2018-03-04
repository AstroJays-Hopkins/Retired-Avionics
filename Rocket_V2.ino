//#include <RH_RF95.h>
//#include <Adafruit_GPS.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "IntersemaBaro.h"

float Flight_Log[7] = {0,0,0,0,0,0,0};
//%%%%%%%%%%%%%%%%%%%%%%%% GPS SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

float latitude,longitude;
char NS,EW;

// what's the name of the hardware serial port?
//SoftwareSerial GPSSerial(1,0);


// Connect to the GPS on the hardware port
//Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
//#define GPSECHO  true

//%%%%%%%%%%%%%%%%%%%%%%%% ALTIMETER SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

Intersema::BaroPressure_MS5607B baro(true);

#define _DRA  8

// Altitude differential array
float alt_diff[_DRA] = {0,0,0,0,0,0,0,0};
// Differential index
float avg;
float sum=0;

float alt0;
float altitude = (baro.getHeightCentiMeters()/30.48) - alt0;
float old_Alt;
float new_Alt;
unsigned long t0;
unsigned long T;

//create state machine for deployment stages
enum states{
  Setup,
  Launchpad,
  Thrust,
  Descent,
  Deployment
};

//set initial state
states flight = Setup;
//states timer = Setup;


//%%%%%%%%%%%%%%%%%%%%%%%% IMU SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

Adafruit_BNO055 bno = Adafruit_BNO055(55);
float ang[3] = {0,0,0};
#define BNO055_SAMPLERATE_DELAY_MS (100)


//%%%%%%%%%%%%%%%%%%%%%%%% ACCELEROMETER SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

float xRawMin = 512;
float xRawMax = 512;
 
float yRawMin = 512;
float yRawMax = 512;
 
float zRawMin = 512;
float zRawMax = 512;

float acc[3] = {0,0,0}; //accelerometer readout array; x, y, z axes respectively

//%%%%%%%%%%%%%%%%%%%%%%%% RECOVERY PIN SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

int REC[2] = {3,4}; //recovery pins

//%%%%%%%%%%%%%%%%%%%%%%%% PAYLOAD PIN SETUP %%%%%%%%%%%%%%%%%%%%%%%%//

int PL = 5;


void setup() {
Serial.begin(115200);
//initialize pins
//analogReference(EXTERNAL);
pinMode(A1, INPUT);
pinMode(A2, INPUT);
pinMode(A3, INPUT);
pinMode(REC, OUTPUT);
pinMode(PL, OUTPUT);
digitalWrite(REC, LOW);
digitalWrite(PL, LOW);


//initialize sensors
baro.init();
bno.begin();


//set starting altitude
alt0 = 0;
int num_points = 50;
for (int i=0; i<num_points; i++)
    {
      alt0 += baro.getHeightCentiMeters()/30.48;
      delay(10);
    }
alt0 /= num_points;            //normalize to the number of samples collected
}

//create a loop to calculate average acceleration values
float ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  unsigned long t0 = millis();
  delay(5) ;
  //take 10 acceleration values in each direction 5 milliseconds apart
  for (int i = 0; i < 10; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading/10;
}

//create a loop to calculate average altitudes
float avg_alt(){
  unsigned long T0 = millis();
  for (int i=0; i < _DRA; i++){
    alt_diff[i] = altitude; //write an altitude value to each element with 5 milliseconds in between iterations
    delay(5);
  }
  for (int i=0; i < _DRA; i++){ //take the average of the altitude values
    sum = sum+alt_diff[i];
  }
  return sum/_DRA - alt0;
}

void loop() {
  
  ////////// Accelerometer //////////
  float xRaw = ReadAxis(A1); //take accelerometer voltages in X,Y,Z directions
  float yRaw = ReadAxis(A2);
  float zRaw = ReadAxis(A3);

  float xScaled = map(xRaw, xRawMin, xRawMax, -1000, 1000); //convert acceleration signals from voltage to numerical values
  float yScaled = map(yRaw, yRawMin, yRawMax, -1000, 1000);
  float zScaled = map(zRaw, zRawMin, zRawMax, -1000, 1000);

  float Ax = {xScaled / 1000.0}; //scale acceleration vectors
  float Ay = {yScaled / 1000.0};
  float Az = {zScaled / 1000.0};

  acc[1] = Ax; //write acceleration vector values to array
  acc[2] = Ay;
  acc[3] = Az;
  
  ////////// IMU //////////
  sensors_event_t event;
  bno.getEvent(&event);
  ang[1] = {(float)event.orientation.x}; //write orientation angles to array
  ang[2] = {(float)event.orientation.y};
  ang[3] = {(float)event.orientation.z};
  delay(BNO055_SAMPLERATE_DELAY_MS);


  ////////// Altimeter //////////
  switch(flight) {
    case Setup:
      t0 = millis();
      if(millis() > t0 + 10000){
        if(avg_alt() > alt0 + 3){
          old_Alt = altitude;
          flight = Launchpad;
          break;
        }
        break;
      }
      
    case Launchpad:
      if(avg_alt() > alt0 + 100){
        old_Alt = avg_alt();
        flight = Thrust;
        break;
      }
      break;
    
    case Thrust:
      new_Alt = avg_alt();
      T = millis();
      if ((new_Alt - old_Alt)/(millis() - T) < 0){
        digitalWrite(PL, HIGH);
        flight = Descent;
        break;
      }
      old_Alt = new_Alt;
      break;

    case Descent:
      if (avg_alt() < alt0 + 400){
        digitalWrite(REC, HIGH);
      }
  }
  
  ////////// GPS //////////
/*  latitude = GPS.latitude;
  longitude = GPS.latitude;
  NS = GPS.lat;
  EW = GPS.lon;
*/
  ////////// link data to arrays //////////

  Flight_Log[1] = {altitude};
  Flight_Log[2] = {acc[1]};
  Flight_Log[3] = {acc[2]};
  Flight_Log[4] = {acc[3]};
  Flight_Log[5] = {ang[1]};
  Flight_Log[6] = {ang[2]};
  Flight_Log[7] = {ang[3]};

  Serial.println("Altitude:");
  Serial.println(altitude);
  Serial.println("Acceleration:");
  Serial.println(acc[1]);
  Serial.println(acc[2]); 
  Serial.println(acc[3]);
  Serial.println("Angular orientation:");
  Serial.println((ang[1],4));
  Serial.println((ang[2],4)); 
  Serial.println((ang[3],4));

  
  }

  

