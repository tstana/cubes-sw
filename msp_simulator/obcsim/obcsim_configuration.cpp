/* A generated file from setup.py */

#include "Arduino.h"
#include "obcsim_configuration.hpp"
#include "obcsim_transactions.hpp"

static unsigned char data_SEND_VOLT[] = "7691";
static unsigned char data_SEND_PARAMS[] = {0x50, 0x00, 0x10, 0x22};
static unsigned char data_seqloop0[] = "6102";
static unsigned char data_seqloop4[] = "6BC9";

void sequence_init(msp_link_t *lnk)
{
	delay(5000);
	Serial.println("-------- Invoking ACTIVE --------");
	invoke_syscommand(lnk, MSP_OP_ACTIVE);
	Serial.println("---------------------------------\n");
	delay(2000);
}

void sequence_loop(msp_link_t *lnk)
{
	Serial.println("-------- Invoking SEND_VOLT --------");
	invoke_send(lnk, MSP_OP_SEND_VOLT, data_seqloop0, sizeof(data_seqloop0), STRING);
	Serial.println("------------------------------------\n");
	delay(1000);
	Serial.println("-------- Invoking REQ_HK --------");
	invoke_request(lnk, MSP_OP_REQ_HK, BYTES);
	Serial.println("---------------------------------\n");
	delay(4000);
	Serial.println("-------- Invoking SEND_VOLT --------");
	invoke_send(lnk, MSP_OP_SEND_VOLT, data_seqloop4, sizeof(data_seqloop4), STRING);
	Serial.println("------------------------------------\n");
	delay(4000);
	Serial.println("-------- Invoking REQ_HK --------");
	invoke_request(lnk, MSP_OP_REQ_HK, BYTES);
	Serial.println("---------------------------------\n");
	delay(1000);
	Serial.println("-------- Invoking SLEEP --------");
	invoke_syscommand(lnk, MSP_OP_SLEEP);
	Serial.println("--------------------------------\n");
	delay(10000);
	Serial.println("-------- Invoking ACTIVE --------");
	invoke_syscommand(lnk, MSP_OP_ACTIVE);
	Serial.println("---------------------------------\n");
	delay(1000);
	Serial.println("-------- Invoking REQ_HK --------");
	invoke_request(lnk, MSP_OP_REQ_HK, BYTES);
	Serial.println("---------------------------------\n");
	delay(4000);
	Serial.println("-------- Invoking SEND_VOLT --------");
	invoke_send(lnk, MSP_OP_SEND_VOLT, data_SEND_VOLT, sizeof(data_SEND_VOLT), STRING);
	Serial.println("------------------------------------\n");
	delay(1000);
	Serial.println("-------- Invoking REQ_HK --------");
	invoke_request(lnk, MSP_OP_REQ_HK, BYTES);
	Serial.println("---------------------------------\n");
	delay(4000);
}

