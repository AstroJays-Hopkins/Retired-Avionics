void setup() {
  //begin serial communication for debugging/to Pi
  Serial.begin(9600);

  //begin communication with load cells
  Serial1.begin(115200);
  Serial1.write('c');
  delay(1);
  Serial2.begin(115200);
  Serial2.write('c');
  delay(1);
  Serial3.begin(115200);
  Serial3.write('c');
  delay(1);
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
  String dataLC; //create variable for load cell data
  dataLC += readWeight1(); //Take data from load cells
  dataLC += " ";
  dataLC += readWeight2();
  dataLC += " ";
  dataLC += readWeight3();
  Serial.println(dataLC); //Send data to Pi
}

