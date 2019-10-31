#include <LoRa.h>
#include "const-common.h"

const int EMATCH_RELAY_PIN = 2;  // Connect to signal line of relay that switches firing current; change as needed.
const int IGNITE_MAX_TIME = 2000; // Maximum time, in milliseconds, that relay is to be kept closed while attempting to light the ematch.
unsigned long IgnitingStart = 0;  // Time relay to ignite ematch was closed

bool ignite = false;
unsigned long lastHeartbeat = 0;

void setup() {
    /* Initialize radio */
    LoRa.begin(915E6);
    // For testing:
    Serial.begin(115200);
    pinMode(EMATCH_RELAY_PIN, OUTPUT);
    digitalWrite(EMATCH_RELAY_PIN, LOW);
}

void loop() {
     unsigned long now = millis();
    if(checkHeartbeat(now)) {
        parseRadio(now);
        // Fire ematch if asked to do so
        if (ignite) {
            digitalWrite(EMATCH_RELAY_PIN, HIGH);
            IgnitingStart = now;
        }
      
        // Open ematch-firing relay if max ignition time is up
        if (IgnitingStart && (now - IgnitingStart) > IGNITE_MAX_TIME) {
            digitalWrite(EMATCH_RELAY_PIN, LOW);
            IgnitingStart = 0;
            ignite = false;
        }
    }
}

void sendAck(uint8_t dst, uint8_t seq) {
    delay(50);
    Serial.println("sending ack");
    LoRa.beginPacket();
    LoRa.write(dst + (rkt::ADDR_RB << 4));
    LoRa.write(rkt::PKT_ACK);
    LoRa.write(seq);
    LoRa.endPacket();
}

void emptyBuffer() {
    while(LoRa.available()) {
        LoRa.read();
    }
}

bool checkHeartbeat(unsigned long now) {
    if((now - lastHeartbeat) > (rkt::HB_INT * rkt::HB_MAX_MISSED)) {
        return false;
    } else {
        return true;
    }
}

void parseRadio(unsigned long now) {
      int packetSize = LoRa.parsePacket();
      if(packetSize) {
          Serial.println("recieved");
          uint8_t recv_addr = LoRa.read();
          uint8_t send_addr = recv_addr >> 4;
          recv_addr &= 0b1111;
          if(recv_addr == rkt::ADDR_RB || recv_addr == rkt::ADDR_BCAST) {
                uint8_t pkt_type = LoRa.read();
                uint8_t seq = LoRa.read();
                switch(pkt_type) {
                    case rkt::PKT_GS_STATE_SYNC:
                        if(LoRa.read() == 255) {
                          ignite = true;
                        }
                        break;
                    case rkt::PKT_HEARTBEAT:
                        lastHeartbeat = now;
                        break;
                }
              sendAck(send_addr,seq); 
          }
      }
}
