/*
 * DAL_stm32g4xx_rcc.c
 *
 *  Created on: Oct 22, 2025
 *      Author: dawoo
 */

#include "DAL_stm32g4xx_rcc.h"

uint32_t ahb_prescaler[8] = {2,4,8,16,64,128,256,512};
uint32_t apb_prescaler[4] = {2,4,8,16};

uint32_t DAL_RCC_GET_PCLK(void)
{
	uint32_t pclk,temp,apbclk_prescaler,sysclk,ahbclk_prescaler;
	temp = (RCC->CFGR_reg >> 2) & (0x3);
	if(temp == 1)
	{
		//HSI
		sysclk = 16000000;//MHZ
	}
	else if(temp == 2)
	{
		//HSE
		sysclk = 8000000;
	}
	else
	{
		//pll
	}
	temp = (RCC->CFGR_reg >> 4) & (0xF);
	if(temp<8)
	{
		ahbclk_prescaler = 1;
	}
	else
	{
		ahbclk_prescaler = ahb_prescaler[8-temp];
	}

	temp = (RCC->CFGR_reg >> 8) & (0x2);
	if(temp<8)
	{
		apbclk_prescaler = 1;
	}
	else
	{
		apbclk_prescaler = apb_prescaler[4-temp];
	}
	pclk = (sysclk / ahbclk_prescaler) / apbclk_prescaler;
	return pclk;
}
