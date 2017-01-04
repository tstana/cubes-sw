/*
 * blinky.c
 *
 *  Created on: Dec 7, 2016
 *      Author: Thedi
 */

#include <xil_types.h>

#include <xparameters.h>
#include <xgpiops.h>

#include <xscugic.h>
#include <xil_exception.h>
#include <xttcps.h>

#include <sleep.h>

int main(void)
{
	int stat;
	int i;
	int count = 0;
	int val = 0;

	XGpioPs_Config *p_gpio_cfg;
	XGpioPs gpio_instance;

	p_gpio_cfg = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	stat = XGpioPs_CfgInitialize(&gpio_instance, p_gpio_cfg, p_gpio_cfg->BaseAddr);

	while (stat != XST_SUCCESS)
		;

	/* Init GPIO */
	XGpioPs_SetDirectionPin(&gpio_instance, 7, 1);
	XGpioPs_SetOutputEnablePin(&gpio_instance, 7, 1);
	XGpioPs_WritePin(&gpio_instance, 7, 0);

	XGpioPs_SetDirection(&gpio_instance, 2, 0x3);
	XGpioPs_SetOutputEnable(&gpio_instance, 2, 0x3);
	XGpioPs_Write(&gpio_instance, 2, 0);

	while(1) {
		++count;
		count %= 4;
		XGpioPs_Write(&gpio_instance, 2, count);

		val ^= 1;
		XGpioPs_WritePin(&gpio_instance, 7, val);

		usleep(250000);
	}
}
