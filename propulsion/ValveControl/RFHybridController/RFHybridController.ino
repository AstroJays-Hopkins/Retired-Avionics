#include <SPI.h>
#include <LoRa.h>
#include <MemoryFree.h>

#include "constants.h"
#include "util.h"

//declare pins for fueling, venting, and disconnect/reset switches
SwInput fuelOn = SwInput(51);
SwInput fuelOff = SwInput(53);
SwInput ignition = SwInput(47);
SwInput bvFwd = SwInput(43);
SwInput bvRev = SwInput(45);
SwInput vent = SwInput(41);

HeartBeat hb = HeartBeat();
CommandSender cs = CommandSender();
uint8_t seq = 0;

void setup() {
    //begin RF communiation
    LoRa.begin(915E6);
    LoRa.setTxPower(2);
    Serial.begin(115200);
    // Set pullups on switch pins (see schematic)
    pinMode(bvFwd.pin, INPUT_PULLUP);
    pinMode(bvRev.pin, INPUT_PULLUP);
    pinMode(vent.pin, INPUT_PULLUP);
    // Ignition is different due to LED and should not be pulled up
    pinMode(ignition.pin, INPUT);
}

void ventCom(){
    if(getButton(&vent) == LOW && cs.MV_S1 != rkt::CMD_V_OPEN) {
        Serial.println("open");
        cs.MV_S1 = rkt::CMD_V_OPEN;
        cs.newCommand = true;
    } else if(getButton(&vent) == HIGH && cs.MV_S1 != rkt::CMD_V_CLOSE) {
        Serial.println("close");
        cs.MV_S1 = rkt::CMD_V_CLOSE;
        cs.newCommand = true;
    }
}

void fuelCom() {
    if(getButton(&fuelOn) == LOW && cs.MV_G1 == rkt::CMD_V_NOOP) {
        cs.MV_G1 = rkt::CMD_V_OPEN;
        cs.newCommand = true;
    } else if(getButton(&fuelOff) == LOW && cs.MV_G1 == rkt::CMD_V_NOOP) {
        cs.MV_G1 = rkt::CMD_V_CLOSE;
        cs.newCommand = true;
    } else if(getButton(&fuelOn) == HIGH && getButton(&fuelOff) == HIGH && cs.MV_G1 != rkt::CMD_V_NOOP) {
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
}

void getCom() { //check states, send commands to open/close valves to rocket
    ventCom();
    fuelCom();
    BV();
    ignitionCom();
}

void loop() {
    unsigned long now = millis();
  //Serial.println(now);
  
    // check for acks
    procRadio(&cs, &hb);
    
    // collect commands
    ventCom();
    fuelCom();
    BV();
//    ignitionCom();

    // Process heartbeat sends
    hb.process(now, &seq);
    delay(100);
    // Process command sends
    cs.process(now, &seq);
//    
}
