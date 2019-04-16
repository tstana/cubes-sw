/* A generated file from setup.py */

#include "Arduino.h"
#include "SDcard.hpp"
#include "obcsim_configuration.hpp"
#include "obcsim_transactions.hpp"

static unsigned char data_SEND_HVPS_CONF[] = "0000000000000000746900C8";
static unsigned char data_SEND_PARAMS[] = {0x50, 0x00, 0x10, 0x22};
static unsigned char data_SEND_CITI_CONF[143];
static unsigned char data_SEND_PROB_CONF[32];
static unsigned char data_seqinit2[] = {0xAA, 0x10, 0x50, 0x30};
static char conf[13] = "asic.cfg";
static char prob[13] = "probec00.cfg";

static char cfg_sent = 0;

void sequence_init(msp_link_t *lnk)
{
	delay(5000);
	Serial.println("-------- Invoking ACTIVE --------");
	invoke_syscommand(lnk, MSP_OP_ACTIVE);
  initSD();
  readSD(data_SEND_CITI_CONF, conf);
  readSD(data_SEND_PROB_CONF, prob);
	Serial.println("---------------------------------\n");
	Serial.println("-------- Invoking SEND_TIME --------");
	invoke_send(lnk, MSP_OP_SEND_TIME, data_seqinit2, sizeof(data_seqinit2), BYTES);
	Serial.println("------------------------------------\n");
	delay(2000);
}

void sequence_loop(msp_link_t *lnk)
{
	Serial.println("-------- Invoking SEND_CITI_CONF --------");
	invoke_send(lnk, MSP_OP_SEND_CITI_CONF, data_SEND_CITI_CONF, sizeof(data_SEND_CITI_CONF), BYTES);
	Serial.println("-----------------------------------------\n");

  delay(5000);

  Serial.println("-------- Invoking SEND_PROB_CONF --------");
  invoke_send(lnk, MSP_OP_SEND_PROB_CONF, data_SEND_PROB_CONF, sizeof(data_SEND_PROB_CONF), BYTES);
  Serial.println("-----------------------------------------\n");

  Serial.println("-----------------------------------------");
  Serial.println("End of OBC simulator operation.");
  Serial.println("-----------------------------------------\n");
  
  for (;;)
    ;
}
