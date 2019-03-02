/* 
 *  Ball valve control code.  Reads Hall effect detector
 *  built into the AndyMark RS-775 motor to measure rotation.
 *  
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

/* *** MOTOR RELAY PINS, CONTROL FUNCTIONS *** */

// Signal pin to relay that is currently powering motor.  If no relay is
// currently on (running a motor), this variable will be 0 .
int Active_Relay_Pin = 0;
// Signal pins to relays.
// Program assumes that one relay spins the motor forwad and the other relay
// is connected to the motor backwards to spin it in reverse.
const int MOTOR_FORWARD_RELAY_PIN = 5;
const int MOTOR_REVERSE_RELAY_PIN = 6;

void turn_off_motor () {
  digitalWrite(Active_Relay_Pin, LOW);
  Active_Relay_Pin = 0;
}

void turn_motor_on_forward () {
  // If the motor is on, turn it off first.
  if (!Active_Relay_Pin) {
    turn_motor_off();
  }
  digitalWrite(MOTOR_FORWARD_RELAY_PIN, HIGH);
  Active_Relay_Pin = MOTOR_FORWARD_RELAY_PIN;
}  

void turn_motor_on_reverse () {
  // If the motor is on, turn it off first.
  if (!Active_Relay_Pin) {
    turn_motor_off();
  }
  digitalWrite(MOTOR_REVERSE_RELAY_PIN, HIGH);
  Active_Relay_Pin = MOTOR_REVERSE_RELAY_PIN;
}

/* *** MOTOR HALL EFFECT ENCODER *** */

int Encoder_Pulse_Count[2] = {0, 0};
                // Channels: {A, B}

void reset_pulse_count () { 
  Encoder_Pulse_Count = {0, 0};
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

/*
 * Gearbox between motor and ball valve has a 188:1 reduction ratio.  
 * To turn the ball valve \pi/2 radians, the motor has to spin 47 times.
 * The encoder produces 7 pulses per revolution.  
 * To spin the ball valve \pi/2 radians, we will need to count 329 pulses.
 */
void check_rotation_and_stop_if_needed () {
  /* Checks to see if motor has spun 90 degrees */
  if ((Encoder_Pulse_Count[0] >= 329) && (Encoder_Pulse_Count[1] >= 329)) {
    /* Shut off motor if that's the case. */
    turn_off_motor();
  }
}

void setup() {
  /* Initialize Hall effect pulse counter interrupts */
  attachInterrupt(digitalPinToInterrupt(2), increment_channel_a, RISING);
  attachInterrupt(digitalPinToInterrupt(3), increment_channel_b, RISING);
  /* Initialize relay signal pins */
  pinMode(MOTOR_FORWARD_RELAY_PIN, OUTPUT);
  pinMode(MOTOR_REVERSE_RELAY_PIN, OUTPUT);
  // For testing:
  Serial.begin(9600);
}

void loop() {
  // Put code in to activate relays upon request.
  if (Serial.available()) {
    char command_char = Serial.read();
    switch (command_char) {
    case 'F': 
      turn_motor_on_forward();
      break;
    case 'R':
      turn_motor_on_reverse();
      break;
    }
  }
}
