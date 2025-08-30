/*
 * gpio_example.c
 *
 *  Created on: Jun 19, 2025
 *      Author: dawoo
 */

#include "DAL_stm32g4xx_gpio.h"

void delay(void)
{
	for(uint32_t i=0;i < 50000;i++);
}

uint8_t valuereg=0;
int main(void)
{
//	GPIO_handle_t LedPin =
//	{
//		.pGPIOx = GPIOA,
//		.pGPIO_Pinconfig =
//		{
//			.Pin_number =  DAL_GPIO_PIN_5, //getting error here
//			.Pin_OP_type = DAL_GPIO_OP_MODE_PP,
//			.Pin_PuPd =    DAL_GPIO_NO_PuPd,
//			.Pin_mode =    DAL_GPIO_MODE_OUTPUT
//		}
//	};
	GPIO_handle_t LedPin;
	LedPin.pGPIOx = GPIOA;
	LedPin.pGPIO_Pinconfig.Pin_number =  DAL_GPIO_PIN_5, //getting error here
	LedPin.pGPIO_Pinconfig.Pin_mode =    DAL_GPIO_MODE_OUTPUT;
	LedPin.pGPIO_Pinconfig.Pin_OP_type = DAL_GPIO_OP_MODE_PP;
	LedPin.pGPIO_Pinconfig.Pin_PuPd =    DAL_GPIO_PULL_UP;
//	DAL_GPIO_Peri_CLK(LedPin.pGPIOx, DAL_ENABLE);
	DAL_GPIO_Init(&LedPin);

//	DAL_GPIO_Write_Pin(&LedPin, DAL_GPIO_SET);
	valuereg = DAL_GPIO_Read_Pin(&LedPin);
	while(1)
	{

	}
}
