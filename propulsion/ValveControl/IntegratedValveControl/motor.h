#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "const-ivc.h"

class DCMotorController {
private:
        int target_pulses;
        int active_pin;
        int low_pin;
        bool req_state;

public:
    // true closed; false open
    volatile bool state;
    volatile bool valve_moving;
    volatile int current_pulses;
    int set_pulses;

    DCMotorController(unsigned int pulses): set_pulses{pulses},
        current_pulses{0}, state{false}, valve_moving{false}, active_pin{rkt::PIN_MOTOR_FWD},
        low_pin{rkt::PIN_MOTOR_REV} {};
    
    void handlePulseEvent() {
        if(valve_moving == true) {
            ++current_pulses;
            if(current_pulses >= target_pulses) {
                stop_motor();
                state = req_state;

            }
        }
    }

    void handleLimitSwitch() {
        if(req_state == false && valve_moving == true){
            stop_motor();
            state = false;
        }
    }
    
    void stop_motor() {
        digitalWrite(active_pin, LOW);
        digitalWrite(low_pin, LOW);
        current_pulses = 0;
        valve_moving = false;
    }

    void actuate(uint8_t dir) {
        // If the motor is on, turn it off first.
        // || state == (bool) (dir - 1)
        if (valve_moving || dir == 0 ) {
            return;
        }
        state? Serial.println("open") : Serial.print("closed");
        digitalWrite(active_pin, LOW);
        digitalWrite(low_pin, LOW);
        target_pulses = 0;
        switch (dir){
            case 2:
                active_pin = rkt::PIN_MOTOR_REV;
                low_pin = rkt::PIN_MOTOR_FWD;
                Serial.println("BV OPEN");
                target_pulses = set_pulses;
                req_state = true;
                break;
            case 1:
                active_pin = rkt::PIN_MOTOR_FWD;
                low_pin = rkt::PIN_MOTOR_REV;
                target_pulses = set_pulses*1.2;
                req_state = false;
                Serial.println("BV CLOSE");
                break;
        }
        Serial.print("target pulses: ");
        Serial.println(target_pulses);
        digitalWrite(active_pin, HIGH);
        digitalWrite(low_pin, LOW);
        // Set valve moving state and pin
        valve_moving = true;
    }
};

#endif
