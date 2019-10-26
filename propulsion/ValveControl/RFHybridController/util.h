#include <LoRa.h>

#ifndef __UTIL_H__
#define __UTIL_H__
/* 
 *  Time in milliseconds to wait before deciding that the switch input has
 *  changed
 */
#define DEBOUNCE_TIME 50
/* 
 *  Convinience macro. src should be a four bit binary addres (without the 0b and
 *  dst should also be a four bit binary address
 */
#define MAKE_ADDR(src, dst) 0b##src##dst

struct SwInput{
    int pin;
    bool state;
    bool lastState;
    unsigned long lastTime;

    SwInput(int pin) {
        this->pin = pin;
        state = false;
        lastState = false;
        lastTime  = 0;
    }
};

/**
 * Get and debounce the state of the push button at `input`
 */
bool getButton(SwInput* input) {
    bool state = digitalRead(input->pin);
    unsigned long now = millis();
    unsigned long t_delta = now - input->lastTime;
    if(state != input->state) {
        input->lastTime = now;
        //Serial.println(input->pin);
        if(t_delta > DEBOUNCE_TIME) {
            input->lastState = state;
            input->state = state;
        }
    }
    return state;
}

inline void beginPacket(int8_t src, int8_t dst, int8_t com_type) {
    LoRa.beginPacket();
    // as defiend, this shifts the src address into the high bits and places
    // dst into the low bits see spec for more details
    LoRa.write((src << 4) + dst);
    LoRa.write(com_type);
}

struct ackState {
    uint8_t missed;
    bool acked;
    int err_led;
};

class HeartBeat {
    private:
        ackState EC;
        ackState RB;
        uint8_t lastSeq;
        unsigned long lastSend;
        // inlined
        void processAcked(ackState* node) {
            if(! node->acked) {
                digitalWrite(node->err_led, HIGH);
                digitalWrite(rkt::LED_LINK, LOW);
                ++node->missed;
                if(node->missed > rkt::HB_MAX_MISSED) {
                    digitalWrite(rkt::LED_SYS_ERR, HIGH);
                }
            }
            node->acked = false;
        }
        void reset() {
            EC.acked = false;
            EC.missed = 0;
            RB.acked = false;
            RB.missed = 0;
        }

    public:
        HeartBeat();
        int process(unsigned long now, uint8_t* seq);
        void ack(int node, uint8_t seq);
};

HeartBeat::HeartBeat() {
    reset();
    EC.err_led = rkt::LED_RKT_ACK_ERR;
    RB.err_led = rkt::LED_IGC_ACK_ERR;
    lastSend = millis();
}

int HeartBeat::process(unsigned long now, uint8_t* seq) {
    if((now - lastSend) >= rkt::HB_INT) {
        Serial.println("Heartbeat sent....");
        beginPacket(rkt::ADDR_GS, rkt::ADDR_BCAST, rkt::PKT_HEARTBEAT);
        LoRa.write(*seq);
        LoRa.endPacket();
        lastSeq = *seq;
        processAcked(&EC);
        processAcked(&RB);
        ++(*seq);
        lastSend = now;
    }
    return 0;
}

void HeartBeat::ack(int node, uint8_t seq) {
    if(seq == lastSeq) {
        Serial.println("HB Acked");
        switch(node) {
            case 1:
                EC.acked = true;
                EC.missed = 0;
                digitalWrite(EC.err_led, LOW);
                break;
            case 2:
                RB.acked = true;
                RB.missed = 0;
                digitalWrite(RB.err_led, LOW);
                break;
        }
    }
    if(EC.acked && RB.acked) {
        digitalWrite(rkt::LED_LINK, HIGH);
    }
}

struct CState {
    uint8_t addr;
    uint8_t retries;
    bool acked;
};

class CommandSender {
    private:
        CState EC;
        CState RB;
        unsigned long lastSend;
        unsigned long lastBlink;
        uint8_t lastSeq;
        bool stopRetry;

        void resetSendState();
        void sendPacket(uint8_t dest, uint8_t seq);
        void resend(CState* node, uint8_t seq);

    public:
        uint8_t MV_R1;
        uint8_t MV_G1;
        uint8_t MV_S1;
        bool newCommand;

        CommandSender();
        void process(unsigned long now, uint8_t* seq);
        void ack(uint8_t node, uint8_t seq);
};

CommandSender::CommandSender() {
    EC.addr = rkt::ADDR_RKT;
    RB.addr = rkt::ADDR_RB;
    MV_R1 = 0;
    MV_G1 = 0;
    MV_S1 = 0;
    stopRetry = true;
}

void CommandSender::sendPacket(uint8_t dest, uint8_t seq) {
    beginPacket(rkt::ADDR_GS, dest, rkt::PKT_GS_STATE_SYNC);
    LoRa.write(seq);
    LoRa.write(MV_R1);
    LoRa.write(MV_S1);
    LoRa.write(MV_G1);
    LoRa.endPacket();
}

void CommandSender::resend(CState* node, uint8_t seq) {
    sendPacket(node->addr, seq);
    ++node->retries;
    if(node->retries > rkt::CS_MAX_RETRIES) {
       digitalWrite(rkt::LED_COM, HIGH);
       stopRetry = true;
    }
}

void CommandSender::resetSendState() {
    EC.acked = false;
    EC.retries = 0;
    RB.acked = false;
    RB.retries = 0;
    newCommand = false;
}

void CommandSender::process(unsigned long now, uint8_t* seq) {
    if(newCommand) {
        Serial.print("sent command: {");
        Serial.print(MV_R1);
        Serial.print(", ");
        Serial.print(MV_S1);
        Serial.print(", ");
        Serial.print(MV_G1);
        Serial.print("; seq = ");
        Serial.print(*seq);
        Serial.println("}");
        sendPacket(rkt::ADDR_BCAST, *seq);
        resetSendState();
        lastSend = now;
        lastSeq = *seq;
        stopRetry = false;
        ++(*seq);
    } else if(!stopRetry) {
        if((now - lastBlink) >= rkt::CS_BLINK_INT) {
            digitalWrite(rkt::LED_COM, !digitalRead(rkt::LED_COM));
            lastBlink = now;
        }
        if((now - lastSend) >= rkt::CS_RESEND_INT) {
            Serial.println("Retrying...");
            resend(&EC, *seq);
            //resend(&RB, *seq);
            lastSend = now;
            lastSeq = *seq;
            ++(*seq);
        }
    }
}

void CommandSender::ack(uint8_t node, uint8_t seq) {
    if(seq == lastSeq) {
        Serial.println("Command Acked");
        switch(node) {
            case rkt::ADDR_RKT:
                EC.retries = 0;
                EC.acked = true;
                break;
            case rkt::ADDR_RB:
                RB.retries = 0;
                RB.acked = true;
        }
        if(EC.acked && RB.acked) {
            resetSendState();
            stopRetry = true;
            digitalWrite(rkt::LED_COM, LOW);
        }
    }
}

void clearPacket() {
    while(LoRa.available()) {
        LoRa.read();
    }
}

void processAckPacket(CommandSender* cs, HeartBeat* hb, uint8_t send_id) {
    uint8_t seq = LoRa.read();
    cs->ack(send_id, seq);
    hb->ack(send_id, seq);
}

void procRadio(CommandSender* cs, HeartBeat* hb) {
    int bytes = LoRa.parsePacket();
    if(bytes) {
        uint8_t address = LoRa.read();
        uint8_t recv_id = address & 0b1111;
        if(recv_id == rkt::ADDR_GS) {
            uint8_t send_id = address >> 4;
            switch(LoRa.read()) {
                case rkt::PKT_ACK:
                    processAckPacket(cs, hb, send_id);
                    break;
                default:
                    clearPacket();
                    break;
            }
        } else {
            clearPacket();
        }
    }
}
#endif
