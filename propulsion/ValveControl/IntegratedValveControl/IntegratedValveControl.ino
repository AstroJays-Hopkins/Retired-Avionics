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

/*
 * Connections: 
 *   Ch. A --> Digital 2
 *   Ch. B --> Digital 3
 */

//Commands will be sent via 915MHz radio communication
#include <SPI.h>
#include <LoRa.h>

/* *** MOTOR RELAY PINS, CONTROL FUNCTIONS *** */

// Signal pin to relay that is currently powering motor.  If no relay is
// currently on (running a motor), this variable will be 0 .
int Active_Relay_Pin = 0;
// Signal pins to relays.
// Program assumes that one relay spins the motor forwad and the other relay
// is connected to the motor backwards to spin it in reverse.
const int MOTOR_FORWARD_RELAY_PIN = 5;
const int MOTOR_REVERSE_RELAY_PIN = 6;
// Forward opens; reverse closes
// Signal pins to Pi
const int VALVE_MOVING_INDICATOR_PIN = 7;
const int VALVE_STATE_INDICATOR_PIN = 8;
const int ematch_pin = 9;
// States for the above pins
bool Valve_moving = true;  // If HIGH -> valve is moving; LOW -> not moving
bool Valve_state = false;   // HIGH -> open; LOW -> closed 

bool RECVD_IG_CMD = 0;
const int burn_duration = 6500; // The duration for the ignition burn
long burn_time = 0;

//Signal pins to relays for solenoid valves
const int fuelRelay = 4;
const int ventRelay = 5;
const int dcRelay = 6;
const int resetRelay = 7;

void turn_motor_off () {
  digitalWrite(Active_Relay_Pin, LOW);
  Valve_moving = LOW;
  digitalWrite(VALVE_MOVING_INDICATOR_PIN, LOW);
  Active_Relay_Pin = 0;
}

void turn_motor_on_forward () {
  // If the motor is on, turn it off first.
  if (Active_Relay_Pin) {
    turn_motor_off();
  }
  digitalWrite(MOTOR_FORWARD_RELAY_PIN, HIGH);
  Active_Relay_Pin = MOTOR_FORWARD_RELAY_PIN;
  // Set valve moving state and pin
  Valve_moving = HIGH;
  digitalWrite(VALVE_MOVING_INDICATOR_PIN, HIGH);
}  

void turn_motor_on_reverse () {
  // If the motor is on, turn it off first.
  if (Active_Relay_Pin) {
    turn_motor_off();
  }
  digitalWrite(MOTOR_REVERSE_RELAY_PIN, HIGH);
  Active_Relay_Pin = MOTOR_REVERSE_RELAY_PIN;
  // Set valve state and pin
  Valve_moving = HIGH;
  digitalWrite(VALVE_MOVING_INDICATOR_PIN, HIGH);
}

/* *** MOTOR HALL EFFECT ENCODER *** */

int Encoder_Pulse_Count[2] = {0, 0};
                // Channels: {A, B}

void reset_pulse_count () { 
  Encoder_Pulse_Count[0] = 0;
  Encoder_Pulse_Count[1] = 0;
}

// Increment pulse count for channel A
void increment_channel_a () { 
  Encoder_Pulse_Count[0] += 1;
  check_rotation_and_stop_if_needed();
}

// Increment pulse count for channel B
void increment_channel_b () { 
  Encoder_Pulse_Count[1] += 1;
  check_rotation_and_stop_if_needed();
}

bool ematch_continuity() { 
  return digitalRead(ematch_pin); 
}

/*
 * Gearbox between motor and ball valve has a 188:1 reduction ratio.  
 * To turn the ball valve \pi/2 radians, the motor has to spin 47 times.
 * The encoder produces 7 pulses per revolution.  
 * To spin the ball valve \pi/2 radians, we will need to count 329 pulses.
 */
void check_rotation_and_stop_if_needed () {
  /* Checks to see if motor has spun 90 degrees */
  if ((Encoder_Pulse_Count[0] >= 329) || (Encoder_Pulse_Count[1] >= 329)) {
    /* Shut off motor if that's the case. */
    turn_motor_off();
    reset_pulse_count();
    if (Active_Relay_Pin == MOTOR_REVERSE_RELAY_PIN) { 
      // TODO What is the purpose of the incomplete control flow?
    }
  }
}

void open_vent() { //open venting solenoid
  digitalWrite(ventRelay,LOW);
  Serial.println("VENTING");
}

void close_vent() { // close venting solenoid
  digitalWrite(ventRelay,HIGH);
  Serial.println("NOT VENTING");
}

void open_fuel() { //open fueling valve
  digitalWrite(fuelRelay,HIGH);
  Serial.println("FUELING VALVE OPEN");
}

void close_fuel() { //close fueling valve
  digitalWrite(fuelRelay,LOW);
  Serial.println("FUELING VALVE CLOSED");
}

void close_all() { //close all valves
  digitalWrite(fuelRelay,LOW);
  // digitalWrite(resetRelay,LOW);
  digitalWrite(ventRelay,LOW);
  Serial.println("ALL VALVES CLOSED");
}

void ignition() {
  turn_motor_on_forward();
}

void setup() {
  /* Initialize radio communication protocol */
  LoRa.begin(915E6);
  Serial.begin(9600);
  
  /* Initialize Hall effect pulse counter interrupts */
  attachInterrupt(digitalPinToInterrupt(2), increment_channel_a, RISING);
  attachInterrupt(digitalPinToInterrupt(3), increment_channel_b, RISING);
  
  /* Initialize relay signal pins */
  pinMode(MOTOR_FORWARD_RELAY_PIN, OUTPUT);
  pinMode(MOTOR_REVERSE_RELAY_PIN, OUTPUT);
  pinMode(ematch_pin, INPUT);
  
  //set relay pins to output voltage
  pinMode(ventRelay,OUTPUT);
  // pinMode(dcRelay,OUTPUT);
  pinMode(resetRelay,OUTPUT);

  //set default relay states to LOW (default)
  digitalWrite(ventRelay,LOW);
  digitalWrite(dcRelay,LOW);
  digitalWrite(resetRelay,LOW);
  
  //setup timer2 interrupt for ignition procedure. 1kHz -> 64 prescalar + 249 compare interrupt
  // The control of the rocket will be disabled during ignition if delay() is used, which is blocking.
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  OCR2A = 249;
  
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS22);
  TIMSK2 |= (1 << OCIE2A);
  
  sei();
}

ISR(TIMER2_COMPA_vect){
  if (RECVD_IG_CMD && ematch_continuity()){
    burn_time += 1;
    if (burn_time >= burn_duration){
      RECVD_IG_CMD = 0;
      turn_motor_on_reverse();
    }
  }
}

void loop() {
  // Put code in to activate relays upon request.
  int8_t command[3];
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    int8_t i = 0;
    while (LoRa.available()) { //get full RF command
      command[i] = LoRa.read();
      i = (i+1) % 3; // just to ensure that the index is always less than 3. 
      //In case under mysterious reasons, 1 packet contains multiple data.
    }
    // Serial.println(command);
    for(int i=0; i<3; i++){ //DEBUG
      Serial.print(command[i]);
    }
  }
  
  int8_t BV_Command = command[2]; //check ball valve desired state
  switch (BV_Command) { //set desired ball valve state
  case 1: 
    Serial.println("FORWARD");
    turn_motor_on_forward();
    break;
  case 0:
    Serial.println("REVERSE");
    turn_motor_on_reverse();
    break;
  case 127: 
    Serial.println("RECVD IGNITION CMD...");
    turn_motor_on_forward();
    RECVD_IG_CMD = 1;
    break;
  case -1:
    Serial.println("HOLDING");
    break;
  }
  
  int8_t Vent_Command = command[0]; //Check vent valve desired state
  switch (Vent_Command) { //set desired vent valve state
    case 1:
      open_vent();
    case 0:
      close_vent();
  }
  
  int8_t Fuel_Command = command[1]; //Check fuel valve desired state
  switch (Fuel_Command) { //set desired fuel valve
    case 1:
      open_fuel();
    case 0:
      close_fuel();
  }
}
