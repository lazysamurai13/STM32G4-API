/*
 * DAL_stm32g4xx_i2c.c
 *
 *  Created on: Oct 16, 2025
 *      Author: dawoo
 */

#include "DAL_stm32g4xx_i2c.h"
#include "DAL_stm32g4xx_rcc.h"

static void DAL_I2C_Configure_Slave_Addr(I2C_Handle_t* I2C_Handle);
static uint32_t I2C_Calculate_TIMINGR(uint32_t i2c_clk_freq, uint8_t mode);

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

void DAL_I2C_PeripheralEnorDi(I2C_Handle_t* I2C_Handle, uint8_t EnorDi)
{
	if(EnorDi == DAL_ENABLE)
	{
		I2C_Handle->pI2Cx->I2C_CR1 |= (1 << I2C_CR1_PE);
	}
	else if(EnorDi == DAL_DISABLE)
	{
		I2C_Handle->pI2Cx->I2C_CR1 &= ~(1 << I2C_CR1_PE);
	}
}

static void DAL_I2C_Configure_Slave_Addr(I2C_Handle_t* I2C_Handle)
{
	uint32_t temp=0;
	uint32_t slave_addr = I2C_Handle->i2c_config.i2c_slave_addr;
	if(slave_addr != 0)
	{
		I2C_Handle->pI2Cx->I2C_OAR1 &= ~(1 << I2C_OAR1_OA1EN);	//disable own addressing
		if(I2C_Handle->i2c_config.i2c_slave_addr_mode == I2C_SLAVE_7BIT)
		{
			I2C_Handle->pI2Cx->I2C_CR2 &= ~(1 << I2C_CR2_ADD10); //7 bit in controller mode
//			temp &= ~(1 << I2C_OAR1_OA1MODE);//7 bit addressing in slave mode
		}
		else
		{
			I2C_Handle->pI2Cx->I2C_CR2 |= (1 << I2C_CR2_ADD10);	//10 bit in controller mode
//			temp |= (1 << I2C_OAR1_OA1MODE);//10 bit addressing in slave mode
		}
		temp |= slave_addr;
		temp |= (1 << I2C_OAR1_OA1EN);	//enable own addressing
		I2C_Handle->pI2Cx->I2C_OAR1 = temp;
	}
}

static 	void DAL_I2C_Configure_Mode(I2C_Handle_t* I2C_Handle)
{
	uint32_t pclk = DAL_RCC_GET_PCLK();
	uint32_t timingr_val = I2C_Calculate_TIMINGR(pclk ,I2C_Handle->i2c_config.i2c_speed);
	I2C_Handle->pI2Cx->I2C_TIMINGR = timingr_val;
}

static uint32_t I2C_Calculate_TIMINGR(uint32_t i2c_clk_freq, uint8_t mode)
{
    uint32_t timingr_val = 0;

    switch (mode) {
        case I2C_MODE_STANDARD:
            // Example value for 100 kHz (SCL: 7.7uS Low, 2.3uS High)
            // PRESC=1, SCLDEL=7, SDADEL=0, SCLH=7C, SCLL=0C -> 0x10707C0C
            timingr_val = 0x00A03D53;
            break;

        case I2C_MODE_FAST:
            // Example value for 400 kHz (SCL: 1.3uS Low, 1.2uS High)
            // PRESC=1, SCLDEL=4, SDADEL=0, SCLH=16, SCLL=32 -> 0x10401632
            timingr_val = 0x00300F38;
            break;

        case I2C_MODE_FAST_PLUS:
            // Example value for 1 MHz (SCL: 0.5uS Low, 0.4uS High)
            // PRESC=0, SCLDEL=1, SDADEL=0, SCLH=09, SCLL=10 -> 0x00100910
            timingr_val = 0x0010030D;
            break;

        default:
            // Fallback to Standard Mode or return an error code
            timingr_val = 0x00A03D53;
            break;
    }
    return timingr_val;
}

uint8_t DAL_I2C_Init(I2C_Handle_t* I2C_Handle)
{
	//1. Enable PCLK
	DAL_I2C_PCLK(I2C_Handle , DAL_ENABLE);
	//2. Disable peripheral
	DAL_I2C_PeripheralEnorDi(I2C_Handle, DAL_DISABLE);
	//3. Enable ACK
	I2C_Handle->pI2Cx->I2C_CR2 &= ~(1 << I2C_CR2_NACK);
	//4. Add slave addr if in slave mode
	DAL_I2C_Configure_Slave_Addr(I2C_Handle);
	//5. Configure i2c mode ,duty cycle,slew rate
	DAL_I2C_Configure_Mode(I2C_Handle);
	//6. Renable i2c perheral
	DAL_I2C_PeripheralEnorDi(I2C_Handle , DAL_ENABLE);

	return 0;
}
