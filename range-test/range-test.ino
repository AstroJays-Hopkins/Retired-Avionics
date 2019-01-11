/* THIS SOFTWARE IS PROVIDED "AS IS", WITH NO WARRANTY TO THE EXTENT 
 * PERMITTED BY APPLICABLE LAW.  ALL WARRANTIES OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING WARRANTIES OF MERCHANTIBILITY OR FITNESS OF PURPOSE, 
 * ARE EXPRESSLY DISCLAIMED. */

#include <SPI.h>
#include <RF24_config.h>
#include <RF24.h>
#include <nRF24L01.h>
/* ^^ Hrm, which of these are necessary? ... */

/* ***** RADIO CONSTANTS, VARIABLES  ***** */

/* A little bit on selecting good addresses, for reference: 
 *   https://maniacalbits.blogspot.com/2013/04/rf24-addressing-nrf24l01-radios-require.html
 * Avoid addresses with one or fewer logic-level shifts such as
 *    -> 0xFFFFFFFFFF     or     -> 0x0000000000     or     -> 0x0000FFFFFF
 */

// Pipe addresses have been stolen from the RF24 GettingStarted example
const uint64_t pipe_addresses[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

/* Pin read by program to distinguish between the two different radios 
 * used in range testing.  Prior to uploading code to Arduino or 
 * pressing the reset button, connect the id_pin to ground for one of 
 * the radios; leave the pin on the other radio unconnected.
 * FIXME: See if we can use Nordic's nRF24 node system instead. */
const int id_pin = 7;

/* Used to store the number of bytes read from the serial line
 * Also the maximum payload size for the radio; see documentation for 
 * setPayloadSize() in RF24  */
const int BUF_LEN = 32;  
/* Buffer used for {reading, writing} data {from, to} the {radio, serial line}
 * ^^ Apologies; I do not know regexes  */
char buf[BUF_LEN];

RF24 radio(9,10);     // Create radio on SPI bus and pins 9 and 10.


/* ***** CONTROL PACKET CONSTANTS, VARIABLES ***** */

/* These constants have been hand-selected to provide significant 
 * binary-level differences between the constants.  If you want to 
 * change them, make sure there are at least two level shifts in 
 * each byte and that runs are kept to a minimum.  Better numbers are welcome.
 * Zero IS NOT a valid control byte.  */
const unsigned int CTRL_PKT_HDR_BYTE_1 =    0b10101010;   // 170
const unsigned int CTRL_PKT_HDR_BYTE_2 =    0b10011001;   // 153
const unsigned int CTRL_PKT_NOT_PING =      0b00100101;   // 37
const unsigned int CTRL_PKT_PING_TRANSMIT = 0b01100011;   // 99
const unsigned int CTRL_PKT_PING_ECHO =     0b10010110;   // 150

/* Structure of a control packet:
 *   {CTRL_PKT_HDR_BYTE_1, CTRL_PKT_HDR_BYTE_2, <arbitrary control byte>} 
 * The 'arbitrary control byte' determines what action the receiving device 
 * will take; see handle_incoming_radio_communications() for details.
 
 * Data is not encapsulated in packets yet but immediately follows a 
 * control packet.  
 */

unsigned int control_byte;   // used to store the control byte for global access
unsigned int control_packet[3] = {CTRL_PKT_HDR_BYTE_1, CTRL_PKT_HDR_BYTE_2, 0};   // Generic control packet with unspecified control byte.
const unsigned int CTRL_PKT_LEN = 3;


/* ***** PING VARIABLES, CONSTANTS ***** */

unsigned long int last_sent_ping_time;     // Time sent to other radio as new ping
unsigned long int last_echoed_ping_time;   // Time received from other radio and echoed back
unsigned long int last_returned_ping_time; // Time echoed back by other radio
const unsigned long int PING_INTERVAL = 1000;  // send a ping every second

/* ***** MISCELLANEOUS VARIABLES ***** */

int number_bytes_read;   // Number of byte read from serial; for handling text messages


/* ***** FUNCTIONS ***** */

/* void send_control_packet (unsigned int control_byte) { */
/*   radio.write(&CTRL_PKT_HDR_BYTE_1, sizeof(CTRL_PKT_HDR_BYTE_1)); */
/*   radio.write(&CTRL_PKT_HDR_BYTE_2, sizeof(CTRL_PKT_HDR_BYTE_2)); */
/*   radio.write(&control_byte, sizeof(control_byte)); */
/* } */

void serial_newline () {
  Serial.print("\n");
}

void send_control_packet (unsigned int control_byte) {
  // set last byte of control packet to control byte
  control_packet[CTRL_PKT_LEN - 1] = control_byte;
  // Transmit; declare errors
  if (!radio.write(&control_packet, CTRL_PKT_LEN)) {
    Serial.print("[ERROR] Unable to send control packet:");
    for (int i; i < CTRL_PKT_LEN; i++) {
      Serial.print(" ");
      Serial.print(control_packet[i]);
    }
    serial_newline();
  }
}

/* Determines if an incoming control packet's headers are correct */
bool read_and_verify_headers () {
  unsigned int header1;
  unsigned int header2;
  radio.read(&header1, sizeof(header1));
  if (header1 != CTRL_PKT_HDR_BYTE_1) {
    return false;
  } else {
    radio.read(&header2, sizeof(header2));
    if (header2 != CTRL_PKT_HDR_BYTE_2) {
      return false;
    } else {
      return true;
    }
  }
}

void send_ping () {
  last_sent_ping_time = millis();
  // Stop listening for incoming packets so we can holler.  Must call
  // radio.stopListening() prior to calling radio.write() .
  radio.stopListening();
  // declare to receiver that transmission is a ping transmit
  send_control_packet(CTRL_PKT_PING_TRANSMIT);
  // Send ping and declare if transmission was sucessful or not
  if (radio.write(&last_sent_ping_time, sizeof(last_sent_ping_time))) {
    // Start listening for incoming packets again
    Serial.print("Ping sent at ");
    Serial.println(last_sent_ping_time);
  } else {
    Serial.print("[ERROR] Ping at ");
    Serial.print(last_sent_ping_time);
    Serial.println(" not sucessfully transmitted.");
  }
  // Resume listening
  radio.startListening();
}

void echo_ping (unsigned long received_ping_time) {
  radio.stopListening();
  // declare to receiver that transmission is a ping echo
  send_control_packet(CTRL_PKT_PING_ECHO);
  // Echo back ping and declare if transmission was sucessful
  if (radio.write(&received_ping_time, sizeof(received_ping_time))) {
    Serial.print("Received ping with timestamp ");
    Serial.print(received_ping_time);
    Serial.println(" echoed back sucessfully.");
  } else {
    Serial.print("[ERROR] Received ping with timestamp ");
    Serial.print(received_ping_time);
    Serial.println(" not sucessfully echoed.");
  }
  radio.startListening();
}  

/* Read data from serial and transmit over the radio */
void read_and_transmit_all_from_serial () {
  radio.stopListening();
  // Tell other radio that we are sending a message, not a ping:
  send_control_packet(CTRL_PKT_NOT_PING);
  // Copy bytes from serial buffer to program buffer; transmit
  while (Serial.available()) {
    number_bytes_read = Serial.readBytes(buf, BUF_LEN);
    // Transmit buffer; if transmission unsucessful, declare prescence of error
    if (!radio.write(buf, number_bytes_read)) {
      Serial.println("[ERROR] Unable to transmit text message: ");
      // Dump contents of buffer to serial
      for (int i = 0; i < BUF_LEN; i++) {
        Serial.write(buf[i]);
      }
      serial_newline();
    }
    radio.startListening();
  }
}

/* Read incoming packets from radio and write to the serial monitor.  */
/* FIXME: Determine number of bytes available for reading in radio to 
 * avoid printing out gibberish after the end of a message.           */
void read_and_display_all_from_radio () {
  while(radio.available()) {
    // Copy bytes from radio to program buffer, then write to serial line (monitor)
    radio.read(buf, BUF_LEN);
    Serial.write(buf, BUF_LEN);
  }
}

void handle_incoming_radio_communications () {
  if (read_and_verify_headers()) {
    radio.read(&control_byte, sizeof(control_byte));
    switch (control_byte) {
    case CTRL_PKT_PING_TRANSMIT:
      // Have received a new ping from the other radio;
      // read time and echo it back.
      radio.read(&last_echoed_ping_time, sizeof(last_echoed_ping_time));
      echo_ping(last_echoed_ping_time);
      break;
    case CTRL_PKT_PING_ECHO:
      // Other radio has returned a ping that this radio sent.
      // Read time, compare, and announce return of ping over serial line.
      radio.read(&last_returned_ping_time, sizeof(last_returned_ping_time));
      Serial.print("Ping at time ");
      Serial.print(last_returned_ping_time);
      Serial.print(" returned.  Delay: ");
      Serial.println(millis() - last_returned_ping_time);
    case CTRL_PKT_NOT_PING:
      // Ooh, a text!
      delay(50);         // Delay a bit to receive most of text
      read_and_display_all_from_radio();
    }
  }
}

void check_for_other_modules () {
  Serial.println("--------------- RADIO MODULE LISTENING TESTS ---------------");
  Serial.println("Attempting to detect other radio modules ...");
  Serial.print("radio.testCarrier() returned ");
  Serial.print(radio.testCarrier());
  serial_newline();
  Serial.print("radio.testRPD() returned     ");
  Serial.print(radio.testRPD());
  serial_newline();
}

void setup() {
  // Configure id_pin as input with pullup resistor activated
  pinMode(id_pin, INPUT_PULLUP);
  
  Serial.begin(115200); // We're the rocketry team...

  radio.begin();        // Initialize the radio
  // ^^ the transmission power level should be high by default.
  // vv Increase delay between retries and number of retries
  radio.setRetries(15,15);
  
  /* Set up pipes */
  if(digitalRead(id_pin)) {  // Use state of id_pin to determine identity of radio
    radio.openWritingPipe(pipe_addresses[0]);
    radio.openReadingPipe(1,pipe_addresses[1]);  // use reading pipe number 1; assign it the second pipe_address .
  } else {   // if we are the other radio, swap the reading and writing pipes
    radio.openWritingPipe(pipe_addresses[1]);
    radio.openReadingPipe(1,pipe_addresses[0]);
  }

  Serial.println("nRF24 Range Test Software v0.0.2.  Started when 2018 was still a thing.");
  Serial.println("--------------- RADIO MODULE STATUS AND DEBUGGING INFORMATION ---------------");
  radio.printDetails();

  Serial.println("For Esperanto, please press one.");
  serial_newline();

  Serial.println("Should you like to send a message to the other radio participating in the range test,");
  Serial.println("type your message in the serial monitor input and strike RETURN.");
  Serial.println("If you would like to print the radio module's configuration and status information,");
  Serial.println("Please type the letter Z and strike RETURN.");
  serial_newline();

  Serial.println("In a moment, the radio will begin broadcasting ping signals.");
  delay(1000);
  radio.startListening();
}


void loop() {
  if (radio.available()) {
    handle_incoming_radio_communications();
  }

  if (Serial.available()) {
    // Check to see if asking to print details
    char c = toupper(Serial.peek());
    if (c = 'Z') {
      Serial.println("--------------- RADIO MODULE STATUS AND DEBUGGING INFORMATION ---------------");
      radio.printDetails();
      check_for_other_modules();
    } else {  // Otherwise read and transmit message over radio
      read_and_transmit_all_from_serial();
    }
  }

  // time to send another ping?
  if (millis() > (last_sent_ping_time + PING_INTERVAL)) {
    send_ping();
  }
}

/* "Beware of bugs in the above code; I have only proved it correct, 
 *  not tried it."        - Donald E. Knuth        */
