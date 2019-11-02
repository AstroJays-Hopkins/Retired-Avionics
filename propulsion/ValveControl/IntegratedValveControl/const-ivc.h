#ifndef __CONST_IVC_H__
#define __CONST_IVC_H__
namespace rkt {
    // Signal pins to relays.
    // Program assumes that one relay spins the motor forwad and the other relay
    // is connected to the motor backwards to spin it in reverse.
    const int PIN_MOTOR_FWD = 29;
    const int PIN_MOTOR_REV = 27;
    // Forward opens; reverse closes
    const int PIN_INTR_ENCODER = 18;
    const int PIN_INTR_LIMIT   = 19;
}
#endif
