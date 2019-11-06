/* 
 *  Control code for ball valve and solenoid valves.  
 *  Reads Hall effect detector
 *  built into the AndyMark RS-775 motor to measure rotation.
 *  
 *  The ball valve section of the code is:
 *  Code adapted and pilfered from Jeffrey La Favre:
 *    http://www.lafavre.us/robotics/encoder_code.pdf
 *  An explanation of Hall effect encoders, also 
 *  by La Favre: 
 *    http://www.lafavre.us/robotics/Hall_Encoders.pdf
 *  Do take the technical explanation located above with a grain of salt.
 *  
 *  BEFORE EDITING THIS PROGRAM, please read the
 *  "Notes and Warnings" on interrupts here: 
 *    https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 *
 */

//Commands will be sent via 915MHz radio communication
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>

#include "motor.h"
#include "radio.h"

/* *** MOTOR RELAY PINS for BV VALVES*** */
// Signal pin to relay that is currently powering motor.  If no relay is
// currently on (running a motor), this variable will be 0 .
int Active_Relay_Pin = 0;
int Low_Relay_Pin = 0;
const int PIN_EMATCH_CONT = 52;


// States for the above pins
bool Valve_state = false;   // HIGH -> open; LOW -> closed 


/* *** RELAY PINS for Solenoid Valves */
const int fuelRelay = 6;
const int ventRelay = 23;
//States for the above pins
bool Fuel_state = false;
bool Vent_state = false;

/* *** IGNITION PARAMETER *** */
bool RECVD_IG_CMD = 0;
const int burn_duration = 6500; // The duration for the ignition burn
long burn_time = 0;

bool ematch_continuity = false; // Check if ematch is burnt through.

DCMotorController MV_R1(320*1.4);

void MV_R1_pulse() {
  MV_R1.handlePulseEvent();
}

void MV_R1_limit() {
  MV_R1.handleLimitSwitch();
}


void switch_solenoid_valve(int valve_pin, int state){
  digitalWrite(valve_pin, state);
}

void ignition() {
  MV_R1.actuate(1);
}

void emergency() {
    // fill with emergency code
}

//Every time data is requested, send data back.
void writeI2C() {
  uint8_t ecstate = (int) Vent_state; //vent
  ecstate += (int) MV_R1.valve_moving << 1; //ball valve Moving
  ecstate += (int) MV_R1.state << 2; //ball valve
  ecstate += (int) ematch_continuity << 3; //e-match
  ecstate += (int) Fuel_state << 4; //fuel
  Wire.write(ecstate);
}

void readI2C() {
  byte command = Wire.read();
  // read off vent command
  commands[2] = (command & (0b11 << 1)) >> 1;
  // emergency shutdown command
  if(command & 0b1) emergency();
}

RfTr rf_tr(0b0001);

void setup() {
  /* Initialize radio communication protocol */
  LoRa.begin(915E6);
  LoRa.setTxPower(2);
  Serial.begin(115200);
  Wire.begin(0x10);
  Wire.onReceive(readI2C);
  Wire.onRequest(writeI2C);

  /* Initialize relay signal pins */
  pinMode(rkt::PIN_MOTOR_FWD, OUTPUT);
  pinMode(rkt::PIN_MOTOR_REV, OUTPUT);
  
  //set relay pins to output voltage
  pinMode(ventRelay,OUTPUT);
  pinMode(fuelRelay, OUTPUT);
  pinMode(rkt::PIN_INTR_ENCODER, INPUT);
  pinMode(rkt::PIN_INTR_LIMIT, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(rkt::PIN_INTR_ENCODER), MV_R1_pulse, RISING);
  attachInterrupt(digitalPinToInterrupt(rkt::PIN_INTR_LIMIT), MV_R1_limit, FALLING);

  //set default relay states to LOW (default)
  digitalWrite(ventRelay, HIGH);
  digitalWrite(fuelRelay, LOW);
  digitalWrite(rkt::PIN_MOTOR_FWD, LOW);
  digitalWrite(rkt::PIN_MOTOR_REV, LOW);

  //setup timer2 interrupt for ignition procedure. 1kHz -> 64 prescalar + 249 compare interrupt
  // The control of the rocket will be disabled during ignition if delay() is used, which is blocking.
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  OCR2A = 249;
  
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS22);
  TIMSK2 |= (1 << OCIE2A);
  Serial.print("Set Pulses: ");
  Serial.println(MV_R1.set_pulses);
  sei();
}

ISR(TIMER2_COMPA_vect){
  if (RECVD_IG_CMD && (ematch_continuity = true) ){
    burn_time += 1;
    if (burn_time >= burn_duration){
      RECVD_IG_CMD = 0;
      MV_R1.actuate(2);
    }
  }
}

void checkEMatchCont() {
  ematch_continuity = digitalRead(PIN_EMATCH_CONT);
}

int curPulse = 0;

void loop() {
//  if(curPulse != MV_R1.current_pulses) {
//    curPulse = MV_R1.current_pulses;
//    Serial.print("PULSE CHANGED: ");
//    Serial.println(curPulse);
//  }
  // Put code in to activate relays upon request.
  rf_tr.check_receive_packet();
  checkEMatchCont();
    
  int8_t BV_Command = commands[0]; //check ball valve desired state
  switch (BV_Command) { //set desired ball valve state
  case 2:
    Serial.println("Valve Open");
    MV_R1.actuate(2);
    commands[2] = 0;
    break;
  case 1:
    Serial.println("valve close");
    MV_R1.actuate(1);
    commands[2] = 0;
    break;
  case 255: // TODO check char types
    Serial.println("RECVD IGNITION CMD...");
    ignition();
    RECVD_IG_CMD = 1;
    commands[0] = 2;
    break;
  case 0:
    // Serial.println("HOLDING");
    break;
  }

  int8_t Fuel_Command = commands[2]; //Check fuel valve desired state
  switch (Fuel_Command) { //set desired fuel valve state
    case 2:
      Fuel_state = true;
      Serial.println("FUEL OPEN");
      break;
    case 1:
      Fuel_state = false;
      Serial.println("FUEL CLOSE");
      break;
  }
  
  int8_t Vent_Command = commands[1]; //Check vent valve desired state
  switch (Vent_Command) { //set desired vent valve state
    case 2:
      Vent_state = true;
      switch_solenoid_valve(ventRelay, 1); //Vent valve is normally open.
      commands[0] = 2;
      break;
    case 1:
      Vent_state = false;
      switch_solenoid_valve(ventRelay, 0);
      commands[0] = 1;
      break;
  }
  commands[0] = 0;
  commands[1] = 0;
  commands[2] = 0;
  //Serial.println("------------------------------");
}
