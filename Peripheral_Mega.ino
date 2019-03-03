#include <Wire.h>

//Solenoid pins
const int QDS = 10;
const int VENT = 11;

//Timer variables
long ventBegin;
long ventTime;

//Quick disconnect condition
boolean CLOSED;

//Packet to send
String Data;

void setup() {
  //begin communication for debugging
  Serial.begin(115200);
  
  //begin communication with load cells
  //LC1
  Serial1.begin(115200);
  if(!Serial1.available()){
    Serial.println("NO CONNECTION LC1");
  }else{
    Serial1.write('c');
    delay(1);
  }

  //LC2
  Serial2.begin(115200);
  if(!Serial2.available()){
    Serial.println("NO CONNECTION LC2");
  }else{
    Serial2.write('c');
    delay(1);
  }

  //LC3
  Serial3.begin(115200);
  if(!Serial3.available()){
    Serial.println("NO CONNECTION LC3");
  }else{
    Serial3.write('c');
    delay(1);
  }
  
  //Set up I2C line between ground station and peripherals board
  Wire.begin(9);
  Wire.onRequest(requestEvent);
  
  //Set pins to read for button presses
  pinMode(QDS,INPUT); //quick disconnect solenoid button
  pinMode(VENT,INPUT); //venting solenoid button
  ventBegin = millis();
  
  Serial.println("PERIPHERALS READY");
  delay(1000);
}

float readWeight1()
{
    int timeOut = 2000; //Number of miliseconds before giving up
    int counter;
    
    //Clear out any trash in the buffer
    while(Serial1.available()) Serial1.read();
    
    //Send any character to trigger a read
    Serial1.print('.');
    
    //Now we need to spin to the first comma after the time stamp
    counter = 0;
    while(Serial1.read() != ',')
    {
        if(counter++ == timeOut) return(0); //Error
        delay(1);
    }
    //Now we read the weight
    counter = 0;
    String weightStr;
    while(1)  {
      if(Serial1.available()){
        char incoming = Serial1.read();
        if(incoming == ','){                
          return(weightStr.toFloat());
        }else{
          weightStr.concat(String(incoming));
        }
      }
        
      if(counter++ == timeOut) return(0); //Error
      delay(1);
  }
} 

float readWeight2()
{
    int timeOut = 2000; //Number of miliseconds before giving up
    int counter;
    
    //Clear out any trash in the buffer
    while(Serial2.available()) Serial2.read();
    
    //Send any character to trigger a read
    Serial2.print('.');
    
    //Now we need to spin to the first comma after the time stamp
    counter = 0;
    while(Serial2.read() != ',')
    {
        if(counter++ == timeOut) return(0); //Error
        delay(1);
    }
    //Now we read the weight
    counter = 0;
    String weightStr;
    while(1)  {
      if(Serial2.available()){
        char incoming = Serial2.read();
        if(incoming == ','){                
          return(weightStr.toFloat());
        }else{
          weightStr.concat(String(incoming));
        }
      }
        
      if(counter++ == timeOut) return(0); //Error
      delay(1);
  }
} 

float readWeight3()
{
    int timeOut = 2000; //Number of miliseconds before giving up
    int counter;
    
    //Clear out any trash in the buffer
    while(Serial3.available()) Serial3.read();
    
    //Send any character to trigger a read
    Serial3.print('.');
    
    //Now we need to spin to the first comma after the time stamp
    counter = 0;
    while(Serial3.read() != ',')
    {
        if(counter++ == timeOut) return(0); //Error
        delay(1);
    }
    //Now we read the weight
    counter = 0;
    String weightStr;
    while(1)  {
      if(Serial3.available()){
        char incoming = Serial3.read();
        if(incoming == ','){                
          return(weightStr.toFloat());
        }else{
          weightStr.concat(String(incoming));
        }
      }
        
      if(counter++ == timeOut) return(0); //Error
      delay(1);
  }
} 

void loop() {
  ////Calculate venting time////
  if(digitalRead(VENT == LOW)){
    ventBegin = millis();
    ventTime = 0;
  }else if(digitalRead(VENT == HIGH)){
    ventTime = millis()-ventBegin;
  }

  ///Determine quick disconnect solenoid condition///
  if(digitalRead(QDS == LOW)){
    CLOSED = true;
  }else if(digitalRead(QDS == HIGH)){
    CLOSED = false;
  }

  //append load cell data
  Data+= readWeight1();
  Data+= " ";
  Data+= readWeight2();
  Data+= " ";
  Data+= readWeight3();
  Data+= " ";

  //append vent time
  Data+= ventTime;
  Data+= " ";

  //append quick disconnect condition
  if(CLOSED){
    Data+= "CLOSED";
  }else if (!CLOSED){
    Data+= "OPEN";
  }
  Data+= " ";

  //append terminating character
  Data+= "x";
}


void requestEvent(){
  Wire.write(Data.c_str());
}

