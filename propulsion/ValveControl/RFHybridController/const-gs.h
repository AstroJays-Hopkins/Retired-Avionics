#ifndef __CONST_GS_H__
#define __CONST_GS_H__

namespace rkt {
    const uint8_t PIN_IN_SV_R1_ENA   = 49;
    const uint8_t PIN_IN_MV_G1_OPEN  = 48;
    const uint8_t PIN_IN_IGNITION    = 47;
    const uint8_t PIN_IN_MV_G1_CLOSE = 46;
    const uint8_t PIN_IN_MV_R1_CLOSE = 45;
    const uint8_t PIN_IN_MV_R1_OPEN  = 43;
    const uint8_t PIN_IN_VENT        = 41;

    const uint8_t LED_SYS_ERR     = 39;
    const uint8_t LED_RKT_ACK_ERR = 37;
    const uint8_t LED_IGC_ACK_ERR = 35;
    const uint8_t LED_COM         = 33;
    const uint8_t LED_LINK        = 31;
}
#endif
