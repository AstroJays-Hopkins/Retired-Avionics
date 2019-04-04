#include "HX711.h"
#include <Wire.h> 
#include <time.h> 

#define DOUT  2
#define CLK   3

#define DEC_POINT  2
#define STABLE  1

float offset = 0;
float zero_factor = 545; //545 for cantilever, 58461 for button
float calibration_factor = 13198.00; //13198.00 for cantilever, 1956 for button
float real_weight = -11.35;//kg //negative for cantilever, positive for button


//Iteration 1
//Zero factor: 58459
//Calibration factor: 11842


//Iteration 2
//Zero factor: 58461
//Calibration factor: 1956

HX711 scale(DOUT, CLK);

unsigned char state=0;
long  FindZeroFactor();
float get_units_kg();
void  ReadWeight();
void  FindCalibrationFactor();
//unsigned long time;

void setup()
{
  scale.tare();
  Serial.begin(115200);
  Serial.println();
  Serial.println("Auto Calibrate Program");
  Serial.println("Send 'a' to Find Zero Factor (Please Remove all weight from scale)");
  Serial.println("Send 'b' to Find Calibration Factor (Please insert know the weight on the scales)");
  Serial.println("Send 'c' Show weight on the scales");
}
void loop() 
{
  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp=='a')
      state=1;
    if(temp=='b')
      state=2;   
    if(temp=='c')
      state=3;
  }

  switch(state)
  {
    case 0:
    break;
    case 1:
      FindZeroFactor();
   //   ReadWeight();
      state=0;
    break;
    case 2:
     FindCalibrationFactor();
     state=0;
    break;
    case 3:
      ReadWeight();
      delay(5);
    break;
    case 4:

    break;

  }
}

long FindZeroFactor()
{
     Serial.println("Find Zero Factor");
     Serial.println("Please wait .....");
     scale.set_scale();
     scale.tare(); 
     long zero_factor = scale.read_average(20); 
     Serial.print("Zero factor: ");
     Serial.println(zero_factor);
     return(zero_factor);
}

void FindCalibrationFactor()
{
  unsigned char flag_stable=0;
  unsigned int decpoint=1;
  for(unsigned char i=0;i<DEC_POINT+1;i++ )
    decpoint = decpoint*10;
  while(1)
  {
      scale.set_scale(calibration_factor); //Adjust to this calibration factor
      Serial.print("Reading: ");
      float read_weight = get_units_kg();
      String data = String(read_weight, DEC_POINT);
      Serial.print(data);
      Serial.print(" kg"); 
      Serial.print(" calibration_factor: ");
      Serial.print(calibration_factor);
      Serial.println();
      long r_weight      = (real_weight*decpoint);
      long int_read_weight = read_weight*decpoint;
      Serial.print(r_weight);
      Serial.print(" , ");
      Serial.println(int_read_weight);
      long x;
      if(r_weight == int_read_weight)
      {
        flag_stable++;
        if(flag_stable>=STABLE)
        {
          Serial.print("Calibration Factor is = ");
          Serial.println(calibration_factor);
          break;
         }
        }
       if(r_weight < int_read_weight)
          {
            x = r_weight - int_read_weight;
            if(x > 100)
              calibration_factor -= 100; 
            else if(x > 100)
              calibration_factor -= 10;
            else
              calibration_factor -= 1;
            flag_stable=0;
          }
          if(r_weight > int_read_weight)
          {
            x =  int_read_weight-r_weight;
            if(x > 100)
              calibration_factor += 90; 
            else if(x > 10)
              calibration_factor += 10;
            else
              calibration_factor += 1; 
            flag_stable=0; 
           }  
  }
}

float get_units_kg()
{
  return(scale.get_units()*0.453592);
}
void ReadWeight()
{
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  String data = String((get_units_kg()+offset) * -2.20462, DEC_POINT); // negative for cantilever
  Serial.print(data);
  Serial.print("\n");
}
