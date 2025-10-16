/*
 * DAL_stm32g4xx_i2c.c
 *
 *  Created on: Oct 16, 2025
 *      Author: dawoo
 */

#include "DAL_stm32g4xx_i2c.h"

uint8_t DAL_I2C_PCLK(I2C_Handle_t* I2C_Handle, uint8_t EnorDi)
{
	if(EnorDi == DAL_ENABLE)
	{
		if(I2C_Handle->pI2Cx == I2C1)
		{
			I2C1_PCLK_EN();
		}
		else if(I2C_Handle->pI2Cx == I2C2)
		{
			I2C2_PCLK_EN();
		}
		else if(I2C_Handle->pI2Cx == I2C3)
		{
			I2C3_PCLK_EN();
		}
		else
		{
			return DAL_ERROR;
		}
	}
	else
	{
	if(I2C_Handle->pI2Cx == I2C1)
		{
			I2C1_PCLK_DI();
		}
		else if(I2C_Handle->pI2Cx == I2C2)
		{
			I2C2_PCLK_DI();
		}
		else if(I2C_Handle->pI2Cx == I2C3)
		{
			I2C3_PCLK_DI();
		}
		else
		{
			return DAL_ERROR;
		}
	}
	return DAL_OK;
}

