#include <TimerOne.h>
#include<Wire.h>

bool valveState = false;
volatile uint8_t actuate = 0;
uint8_t state = 1;

/**
 *  moves the stepper motor  certain number of steps
 *  true is open (CW) and false is close (CCW)
 *  DQ542MA DIP Config: {1,3,4,6,7,8} ON, REST OFF
 */
void move(int deg, bool direction) {
  deg *= 55.55;
  // This is a tenary operator . If the condition is true, the first expression is
  // executed, otherwise the second expression in executed.
  // LOW is CCW and CLOSES the valve
  // HIGH is CW and OPENS the valve
  direction ? digitalWrite(2, HIGH) : digitalWrite(2, LOW);
  Serial.println("moving");
  for(size_t i = 0; i < deg; ++i) {
    digitalWrite(3, HIGH);
    delayMicroseconds(300);
    digitalWrite(3, LOW);
    delayMicroseconds(300);
  }
  noInterrupts();
  actuate = 0;
  interrupts();
}

void handleData() {
  actuate = Wire.read();
  // clear buffer if the master oversent
  while(Wire.available()){
    Wire.read();
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(0x11);
  Wire.onReceive(handleData);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
}

void loop() {
  if(actuate != 0 && actuate != state) {
    Serial.println("Actuating!");
    switch(actuate) {
        case 1:
          move(94, false);
          state = 1;
          break;
        case 2:
          move(94, true);
          state = 2;
          break;   
    }
    Serial.println("Done!");
  }
}
