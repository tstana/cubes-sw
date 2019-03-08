/* A generated file from setup.py */

#include "Arduino.h"
#include "obcsim_configuration.hpp"
#include "obcsim_transactions.hpp"

static unsigned char data_SEND_PARAMS[] = {0x50, 0x00, 0x10, 0x22};
static unsigned char data_SEND_MESSAGE[] = "default \'message\' data";

void sequence_init(msp_link_t *lnk)
{
	delay(5000);
	Serial.println("-------- Invoking ACTIVE --------");
	invoke_syscommand(lnk, MSP_OP_ACTIVE);
	Serial.println("---------------------------------\n");
}

void sequence_loop(msp_link_t *lnk)
{
	delay(5000);
	Serial.println("-------- Invoking REQ_PAYLOAD --------");
	invoke_request(lnk, MSP_OP_REQ_PAYLOAD, BYTES);
	Serial.println("--------------------------------------\n");
}

