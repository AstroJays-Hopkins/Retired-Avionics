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

const int packet_size = 4;

/* *** MOTOR RELAY PINS for BV VALVES*** */
// Signal pin to relay that is currently powering motor.  If no relay is
// currently on (running a motor), this variable will be 0 .
int Active_Relay_Pin = 0;
int Low_Relay_Pin = 0;
const int PIN_EMATCH_CONT = 52;

// Signal pins to relays.
// Program assumes that one relay spins the motor forwad and the other relay
// is connected to the motor backwards to spin it in reverse.
const int MOTOR_FORWARD_RELAY_PIN = 29;
const int MOTOR_REVERSE_RELAY_PIN = 27;
// Forward opens; reverse closes

// States for the above pins
bool Valve_moving = false;  // If HIGH -> valve is moving; LOW -> not moving
bool req_valve_state = false;
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
byte commands[3];

//BV valve timer variables
long target_BV_stop_t;
const long BV_move_duration = 1000; // set to 3000ms for now, to be changed based on the actual run time of ball valve.

void turn_motor_off () {
  Serial.println("motor_turned_off");
  digitalWrite(Active_Relay_Pin, LOW);
  digitalWrite(Low_Relay_Pin, LOW);
  Valve_moving = LOW;
  Valve_state = req_valve_state;
  Active_Relay_Pin = 0;
  target_BV_stop_t = 0;
}

void turn_motor_on(char dir) {
  // If the motor is on, turn it off first.
  if (Active_Relay_Pin) {
    turn_motor_off();
  }
  switch (dir){
    case 1:
      Active_Relay_Pin = MOTOR_FORWARD_RELAY_PIN;
      Low_Relay_Pin = MOTOR_REVERSE_RELAY_PIN;
      req_valve_state = true;
      Serial.println("BV FORWARD");
      break;
    case 0:
      Active_Relay_Pin = MOTOR_REVERSE_RELAY_PIN;
      Low_Relay_Pin = MOTOR_FORWARD_RELAY_PIN;
      req_valve_state = false;
      Serial.println("BV BACKWARD");
      break;
  }
  target_BV_stop_t = millis() + BV_move_duration;
  Serial.println(target_BV_stop_t);
  digitalWrite(Active_Relay_Pin, HIGH);
  digitalWrite(Low_Relay_Pin, LOW);
  // Set valve moving state and pin
  Valve_moving = HIGH;
}


bool ematch_continuity = false; // Check if ematch is burnt through.

void check_BV_time() {
  /* Checks to see if motor has turned for the desired interval, 10 ms margin set in case the exact is missed for any reason.*/
  if (abs(millis()-target_BV_stop_t) < 5 && Valve_moving == HIGH) {
    turn_motor_off();
    Serial.println("TURNING BV OFF!!!");
  }
}

void switch_solenoid_valve(int valve_pin, int state){
  digitalWrite(valve_pin, state);
}

void ignition() {
  turn_motor_on(1);
}

void emergency() {
    // fill with emergency code
}

//Every time data is requested, send data back.
void writeI2C() {
  uint8_t ecstate = (int) Vent_state ; //vent
  ecstate += (int) Valve_moving << 1; //ball valve Moving
  ecstate += (int) Valve_state << 2; //ball valve
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

class SignalPacket{
  private:
  byte self_id;
  byte cur_trans_id;
  byte cur_packet_type;
  byte cur_sequence_code;
  byte* cur_recvd_packet;
  byte* cur_recvd_command;
  byte* cur_ack_command;
  int full_packet_size = packet_size+2;

  public: 
  SignalPacket(byte self_id): self_id{self_id} {};

  void set_self_id(byte cur_id){
    self_id = cur_id;
  }

  bool is_command()
  {
    return cur_packet_type == 1;
  }
  
  void refresh_cur_packet(byte* p_cur_packet)
  {
    cur_recvd_packet = p_cur_packet;
    cur_trans_id = cur_recvd_packet[0] >> 4;
    cur_packet_type = cur_recvd_packet[1];
    cur_sequence_code = cur_recvd_packet[2];
    if (is_command()){
      cur_recvd_command = cur_recvd_packet + 3;
      update_cur_global_command();
    }
  }

  void update_cur_global_command(){ // update the global variable command
    if (is_command()){
      for (int i=0; i<3; i++){
        commands[i] = cur_recvd_command[i];
      }
    }
  }
  
  void get_ack_command(byte* buf){ 
    buf[0] = cur_trans_id + (self_id<<4);
    buf[1] = 100;
    buf[2] = cur_sequence_code;
  }
};

class RfTr{
  private:
  byte self_id;
  SignalPacket cur_packet;
  char raw_data_packet[packet_size+2];
   
  public:
  RfTr(byte raw_self_id): cur_packet(raw_self_id), self_id{raw_self_id} {
  }
  
  byte check_id(byte * raw_cur_packet){
    byte recv_id = raw_cur_packet[0] & 0b1111;
    Serial.print("recv  id: ");
    Serial.println(recv_id);
    if(recv_id == self_id || recv_id == 15) {
      Serial.println("id match");
      return true;
    }  else {
      Serial.println("id mismatch");
      return false;
    }
  }

  void check_receive_packet(){
    if (get_packet_size()){
      receive_data();
    }
  }

  int get_packet_size()
  {
    int packet_size = LoRa.parsePacket();
    return packet_size;
  }
  
  void receive_data() {
    int i = 0;
    while(LoRa.available()){
      byte command = LoRa.read();
      raw_data_packet[i] = command;
      ++i;
    }
    Serial.println((byte) raw_data_packet[0], BIN);
    if (check_id(raw_data_packet)){
      Serial.print("Recieving data: {");
      for (int j = 0; j<i; ++j) {
        Serial.print((byte) raw_data_packet[j]);
        Serial.print(", ");
      }
      Serial.println("}");

      cur_packet.refresh_cur_packet(raw_data_packet);
      send_ack();
    }
  }

  void send_ack() {
    Serial.println("sending ack");
    byte buf[3];
    LoRa.beginPacket();
    cur_packet.get_ack_command(buf);
    LoRa.write(buf, 3);
    LoRa.endPacket();

  }
};

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
  pinMode(MOTOR_FORWARD_RELAY_PIN, OUTPUT);
  pinMode(MOTOR_REVERSE_RELAY_PIN, OUTPUT);
  
  //set relay pins to output voltage
  pinMode(ventRelay,OUTPUT);
  pinMode(fuelRelay, OUTPUT);

  //set default relay states to LOW (default)
  digitalWrite(ventRelay, HIGH);
  digitalWrite(fuelRelay, LOW);
  digitalWrite(MOTOR_FORWARD_RELAY_PIN, LOW);
  digitalWrite(MOTOR_REVERSE_RELAY_PIN, LOW);
  
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
  if (RECVD_IG_CMD && (ematch_continuity = true) ){
    burn_time += 1;
    if (burn_time >= burn_duration){
      RECVD_IG_CMD = 0;
      turn_motor_on(0);
    }
  }
}

void checkEMatchCont() {
  ematch_continuity = digitalRead(PIN_EMATCH_CONT);
}

void loop() {
  // Put code in to activate relays upon request.
  rf_tr.check_receive_packet();
  checkEMatchCont();
    
  int8_t BV_Command = commands[0]; //check ball valve desired state
  switch (BV_Command) { //set desired ball valve state
  case 49: 
    turn_motor_on(1);
    commands[2] = 2;
    break;
  case 50:
    turn_motor_on(0);
    commands[2] = 2;
    break;
  case 255: // TODO check char types
    Serial.println("RECVD IGNITION CMD...");
    ignition();
    RECVD_IG_CMD = 1;
    commands[0] = 2;
    break;
  case 48:
    // Serial.println("HOLDING");
    break;
  }

  int8_t Fuel_Command = commands[2]; //Check fuel valve desired state
  switch (Fuel_Command) { //set desired fuel valve
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
      switch_solenoid_valve(ventRelay, 0); //Vent valve is normally open.
      commands[0] = 2;
      break;
    case 1:
      Vent_state = false;
      switch_solenoid_valve(ventRelay, 1);
      commands[0] = 2;
      break;
  }
  check_BV_time();
  //Serial.println("------------------------------");
}
