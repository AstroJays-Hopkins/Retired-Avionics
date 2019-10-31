#include <SPI.h>
#include <LoRa.h>
#include <MemoryFree.h>

#include "const-common.h"
#include "const-gs.h"
#include "util.h"

//declare pins for fueling, venting, and disconnect/reset switches
SwInput SV_R1_Ena(rkt::PIN_IN_SV_R1_ENA);
SwInput fuelOn = SwInput(rkt::PIN_IN_MV_G1_OPEN);
SwInput fuelOff = SwInput(rkt::PIN_IN_MV_G1_CLOSE);
SwInput ignition = SwInput(rkt::PIN_IN_IGNITION);
SwInput bvFwd = SwInput(rkt::PIN_IN_MV_R1_OPEN);
SwInput bvRev = SwInput(rkt::PIN_IN_MV_R1_CLOSE);
SwInput vent = SwInput(rkt::PIN_IN_VENT);

HeartBeat hb = HeartBeat();
CommandSender cs = CommandSender();
uint8_t seq = 0;
boolean ignited = false;

void setup() {
    //begin RF communiation
    LoRa.begin(915E6);
    LoRa.setTxPower(2);
    Serial.begin(115200);
    // Set pullups on switch pins (see schematic)
    pinMode(rkt::LED_SYS_ERR, OUTPUT);
    pinMode(rkt::LED_RKT_ACK_ERR, OUTPUT);
    pinMode(rkt::LED_IGC_ACK_ERR, OUTPUT);
    pinMode(rkt::LED_COM, OUTPUT);
    pinMode(rkt::LED_LINK, OUTPUT);
    pinMode(SV_R1_Ena.pin, INPUT_PULLUP);
    pinMode(fuelOn.pin, INPUT_PULLUP);
    pinMode(fuelOff.pin, INPUT_PULLUP);
    pinMode(bvFwd.pin, INPUT_PULLUP);
    pinMode(bvRev.pin, INPUT_PULLUP);
    pinMode(vent.pin, INPUT_PULLUP);
    // Ignition is different due to LED and should not be pulled up
    pinMode(ignition.pin, INPUT);
}

void ventCom() {
    if(getButton(&SV_R1_Ena) == HIGH && cs.MV_S1 != rkt::CMD_V_CLOSE) {
        Serial.println("Vent open");
        cs.MV_S1 = rkt::CMD_V_CLOSE;
        cs.newCommand = true;
    }
    
    if(getButton(&SV_R1_Ena) == LOW) {
        if(getButton(&vent) == LOW && cs.MV_S1 != rkt::CMD_V_CLOSE) {
            Serial.println("Vent close");
            cs.MV_S1 = rkt::CMD_V_CLOSE;
            cs.newCommand = true;
        } else if(getButton(&vent) == HIGH && cs.MV_S1 != rkt::CMD_V_OPEN) {
            Serial.println("Vent open");
            cs.MV_S1 = rkt::CMD_V_OPEN;
            cs.newCommand = true;
        }
    }
}

void fuelCom() {
    if(getButton(&fuelOn) == LOW && cs.MV_G1 == rkt::CMD_V_NOOP) {
        Serial.println("Fuel open");
        cs.MV_G1 = rkt::CMD_V_OPEN;
        cs.newCommand = true;
    } else if(getButton(&fuelOff) == LOW && cs.MV_G1 == rkt::CMD_V_NOOP) {
        Serial.println("Fuel closed");
        cs.MV_G1 = rkt::CMD_V_CLOSE;
        cs.newCommand = true;
    } else if(getButton(&fuelOn) == HIGH && getButton(&fuelOff) == HIGH && cs.MV_G1 != rkt::CMD_V_NOOP) {
        Serial.println("Fuel reset");
        cs.MV_G1= rkt::CMD_V_NOOP;
        cs.newCommand = true;
    }
}

void BV() {
    if(getButton(&bvFwd) == LOW && cs.MV_R1 == rkt::CMD_V_NOOP) {
        cs.MV_R1 = rkt::CMD_V_OPEN;
        cs.newCommand = true;
    } else if(getButton(&bvRev) == LOW && cs.MV_R1 == rkt::CMD_V_NOOP) {
        cs.MV_R1 = rkt::CMD_V_CLOSE;
        cs.newCommand = true;
    } else if(getButton(&bvFwd) == HIGH && getButton(&bvRev) == HIGH && cs.MV_R1 != rkt::CMD_V_NOOP) {
        cs.MV_R1 = rkt::CMD_V_NOOP;
        cs.newCommand = true;
    }
}

void ignitionCom() {
    // fill meeeeeeee
    if(getButton(&ignition) == HIGH && !ignited) {
        Serial.println("Ignition engaged");
        cs.MV_S1 = rkt::CMD_V_IGNITE;
        cs.newCommand = true;
    }
}

void getCom() { //check states, send commands to open/close valves to rocket
    ventCom();
    fuelCom();
    BV();
    ignitionCom();
}

void loop() {
    unsigned long now = millis();
  
    // check for acks
    procRadio(&cs, &hb);
    
    // collect commands
    ventCom();
    fuelCom();
    BV();
//    ignitionCom();

    // Process heartbeat sends
    hb.process(now, &seq);
    // Process command sends
    cs.process(now, &seq);
//    
}
