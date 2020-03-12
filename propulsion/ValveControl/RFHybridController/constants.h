#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

namespace rkt {
    const uint8_t ADDR_GS    = 0b0000;
    const uint8_t ADDR_RKT   = 0b0001;
    const uint8_t ADDR_RB    = 0b0010;
    const uint8_t ADDR_BCAST = 0b1111;

    const uint8_t PKT_NOOP          =   0;
    const uint8_t PKT_GS_STATE_SYNC =   1;
    const uint8_t PKT_HEARTBEAT     =   2;
    const uint8_t PKT_ACK           = 100;
    const uint8_t PKT_NACK          = 101;

    const uint8_t LED_SYS_ERR     = 31;
    const uint8_t LED_RKT_ACK_ERR = 33;
    const uint8_t LED_IGC_ACK_ERR = 35;
    const uint8_t LED_COM         = 37;
    const uint8_t LED_LINK        = 39;

    const uint8_t      HB_MAX_MISSED =   12;
    const unsigned int HB_INT        = 2500;

    const uint8_t      CS_MAX_RETRIES    =   3;
    const unsigned int CS_BLINK_INT      = 100;
    const unsigned int CS_RESEND_INT     = 800;
   
    // standard valve commands
    const uint8_t CMD_V_NOOP  = 0;
    const uint8_t CMD_V_CLOSE = 1;
    const uint8_t CMD_V_OPEN  = 2;
}

#endif
