#include <TimerOne.h>
#include<Wire.h>

bool valveState = false;
int actuate = 0;
int state = 1;

/**
 *  moves the stepper motor  certain number of steps
 *  true is forwards and false is backwards
 */
void move(int deg, bool direction) {
  deg *= 55.55;
  // This is a tenary operator . If the condition is true, the first expression is
  // executed, otherwise the second expression in executed.
  direction ? digitalWrite(2, LOW) : digitalWrite(2, HIGH);
  for(size_t i = 0; i < deg; ++i) {
    digitalWrite(3, HIGH);
    delayMicroseconds(300);
    digitalWrite(3, LOW);
    delayMicroseconds(300);
  }
  actuate = 0;
}

void handleData() {
  actuate = Wire.read();
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
  switch(actuate) {
    case 1:
      move(90, false);
      state = 1;
    case 2:
      move(90, true);
      state = 1;
  }
}
