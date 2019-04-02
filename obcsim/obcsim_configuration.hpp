/* A generated header file from setup.py */

#ifndef OBCSIM_CONFIGURATION_H
#define OBCSIM_CONFIGURATION_H

#include "msp_obc.h"

#define EXP_ADDR 0x35
#define EXP_MTU 507
#define REQUEST_BUFFER_SIZE 4096
#define MSP_ERROR_THRESHOLD 5

#ifndef MSP_OP_REQ_PAYLOAD
#define MSP_OP_REQ_PAYLOAD 0x20
#endif
#ifndef MSP_OP_ACTIVE
#define MSP_OP_ACTIVE 0x10
#endif
#ifndef MSP_OP_SEND_HVPS_CONF
#define MSP_OP_SEND_HVPS_CONF 0x71
#endif
#ifndef MSP_OP_REQ_PARAMS
#define MSP_OP_REQ_PARAMS 0x60
#endif
#ifndef MSP_OP_POWER_OFF
#define MSP_OP_POWER_OFF 0x12
#endif
#ifndef MSP_OP_SEND_PARAMS
#define MSP_OP_SEND_PARAMS 0x70
#endif
#ifndef MSP_OP_REQ_HK
#define MSP_OP_REQ_HK 0x21
#endif
#ifndef MSP_OP_SEND_CITI_CONF
#define MSP_OP_SEND_CITI_CONF 0x72
#endif
#ifndef MSP_OP_SEND_PROB_CONF
#define MSP_OP_SEND_PROB_CONF 0x73
#endif
#ifndef MSP_OP_REQ_PUS
#define MSP_OP_REQ_PUS 0x22
#endif
#ifndef MSP_OP_SEND_TIME
#define MSP_OP_SEND_TIME 0x30
#endif
#ifndef MSP_OP_SEND_PUS
#define MSP_OP_SEND_PUS 0x31
#endif
#ifndef MSP_OP_SLEEP
#define MSP_OP_SLEEP 0x11
#endif

void sequence_init(msp_link_t *lnk);
void sequence_loop(msp_link_t *lnk);

#endif /* OBCSIM_CONFIGURATION_H */
