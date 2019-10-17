#ifndef __UTIL_H__
#define __UTIL_H__

/* Time in milliseconds to wait before deciding that the switch input has
 * changed
 */
#define DEBOUNCE_TIME 50

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
        Serial.println("state changed");
        if(t_delta > DEBOUNCE_TIME) {
            input->lastState = state;
            input->state = state;
        }
    }
    return state;
}

#endif
