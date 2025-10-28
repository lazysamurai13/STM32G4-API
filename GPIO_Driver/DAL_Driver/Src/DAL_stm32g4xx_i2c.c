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
	uint32_t temp = 0U;
	uint32_t slave_addr = (I2C_Handle->i2c_config.i2c_slave_addr << 1U);
	if(slave_addr != 0U)
	{
		// Disable own addressing before reconfiguring
		I2C_Handle->pI2Cx->I2C_OAR1 &= ~(1U << I2C_OAR1_OA1EN);	//disable own addressing
		//Set slave address
		temp |= slave_addr;
		//Configure 7-bit or 10-bit addressing for slave
		if(I2C_Handle->i2c_config.i2c_slave_addr_mode == I2C_SLAVE_7BIT)
		{
			temp &= ~(1 << I2C_OAR1_OA1MODE);//7 bit addressing in slave mode
		}
		//Enable own addressing
		temp |= (1U << I2C_OAR1_OA1EN);
		I2C_Handle->pI2Cx->I2C_OAR1 = temp;
	}
}

static 	void DAL_I2C_Configure_Mode(I2C_Handle_t* I2C_Handle)
{
	//set Timingr reg - clk properties - rise time , fall time , delay
	uint32_t pclk = DAL_RCC_GET_PCLK();
	uint32_t timingr_val = I2C_Calculate_TIMINGR(pclk ,I2C_Handle->i2c_config.i2c_speed);
	I2C_Handle->pI2Cx->I2C_TIMINGR = timingr_val;
}

static uint32_t I2C_Calculate_TIMINGR(uint32_t i2c_clk_freq, uint8_t mode)
{
    uint32_t timingr_val = 0U;

    switch (mode) {
        case I2C_MODE_STANDARD:
            // Example value for 100 kHz (SCL: 7.7uS Low, 2.3uS High)
            // PRESC=1, SCLDEL=7, SDADEL=0, SCLH=7C, SCLL=0C -> 0x10707C0C
            timingr_val = 0x00A03D53U;
            break;

        case I2C_MODE_FAST:
            // Example value for 400 kHz (SCL: 1.3uS Low, 1.2uS High)
            // PRESC=1, SCLDEL=4, SDADEL=0, SCLH=16, SCLL=32 -> 0x10401632
            timingr_val = 0x00300F38U;
            break;

        case I2C_MODE_FAST_PLUS:
            // Example value for 1 MHz (SCL: 0.5uS Low, 0.4uS High)
            // PRESC=0, SCLDEL=1, SDADEL=0, SCLH=09, SCLL=10 -> 0x00100910
            timingr_val = 0x0010030DU;
            break;

        default:
            // Fallback to Standard Mode or return an error code
            timingr_val = 0x00A03D53U;
            break;
    }
    return timingr_val;
}

uint8_t DAL_I2C_CheckFlag(I2C_Handle_t* I2C_Handle , uint8_t Flagname)
{
	if(I2C_Handle->pI2Cx->I2C_ISR & Flagname)
	{
		return DAL_OK;
	}
	return DAL_ERROR;
}

uint8_t DAL_I2C_Init(I2C_Handle_t* I2C_Handle)
{
	//1. Enable PCLK
	DAL_I2C_PCLK(I2C_Handle , DAL_ENABLE);
	//2. Disable peripheral
	DAL_I2C_PeripheralEnorDi(I2C_Handle, DAL_DISABLE);
	//3. Enable ACK
//	I2C_Handle->pI2Cx->I2C_CR2 &= ~(1 << I2C_CR2_NACK); -> only in older f4 series
	//4. Add slave addr if in slave mode
	if(I2C_Handle->i2c_config.i2c_mode == I2C_MODE_SLAVE)
	{
		DAL_I2C_Configure_Slave_Addr(I2C_Handle);
	}
	//5. Configure i2c mode ,duty cycle,slew rate
	DAL_I2C_Configure_Mode(I2C_Handle);
	//6. Clear all flags
	I2C_Handle->pI2Cx->I2C_ICR = 0xFFFFFFFFU; // Clear all flags
	//7. Renable i2c perheral
	DAL_I2C_PeripheralEnorDi(I2C_Handle , DAL_ENABLE);
	return 0;
}

I2C_Status_t I2C_Master_Transmit(I2C_Handle_t* I2C_Handle, uint8_t slave_address, uint8_t* data, uint32_t size)
{
    volatile uint32_t timeout;

    // --- 1. Wait until bus is not busy ---
    timeout = I2C_TIMEOUT;
    while(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_BUSY) != DAL_OK)
    {
    	if (--timeout == 0) return I2C_TIMEOUT_ERROR;
    }
    // --- 2. Configure the transfer ---
    // Clear CR2 register
    I2C_Handle->pI2Cx->I2C_CR2 = 0;

    // Set slave address (7-bit address)
    I2C_Handle->pI2Cx->I2C_CR2 |= (uint32_t)(slave_address << 1);

    // Set transfer direction to WRITE (0)
    I2C_Handle->pI2Cx->I2C_CR2 &= ~(1<< I2C_CR2_RD_WRN);

#ifdef I2C_AUTOEND_MODE_En
    // Set number of bytes to transfer
    I2C_Handle->pI2Cx->I2C_CR2 |= (size << I2C_CR2_NBYTEST);

    // Set AUTOEND mode: automatically send STOP after NBYTES
    I2C_Handle->pI2Cx->I2C_CR2 |= I2C_CR2_AUTOEND;
#endif
    // Generate START condition
    I2C_Handle->pI2Cx->I2C_CR2 |= (1<<I2C_CR2_START);

    // --- 3. Transmit data bytes ---
    for (uint32_t i = 0; i < size; i++)
    {
        // Wait for Transmit Interrupt Status (TXIS) flag
        // This means the transmit data register (TXDR) is empty
        timeout = I2C_TIMEOUT;
        while(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_TXIS) != DAL_OK)
        {
            // Check for NACK (Not Acknowledge)
            if (DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_NACKF) != DAL_OK)
            {
                I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_NACKCF; // Clear NACK flag
                return I2C_NACK_ERROR;
            }
            if (--timeout == 0) return I2C_TIMEOUT_ERROR;
        }

        // Write data to TXDR
        I2C_Handle->pI2Cx->I2C_TXDR = data[i];
    }
#ifdef I2C_AUTOEND_MODE_En
    // --- 4. Wait for transfer to complete ---
    // With AUTOEND set, we just need to wait for the STOPF flag.
    timeout = I2C_TIMEOUT;
    while (!(I2C_Handle->pI2Cx->I2C_ISR & I2C_ISR_STOPF)) {
        // We still check for NACK here, as the last byte could be NACK'd
        if (I2C_Handle->pI2Cx->I2C_ISR & I2C_ISR_NACKF) {
            I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_NACKCF; // Clear NACK flag
            return I2C_NACK_ERROR;
        }
        if (--timeout == 0) return I2C_TIMEOUT_ERROR;
    }
#else
    // --- 4. Wait for transfer complete (TC) flag ---
    while (DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_TC) != DAL_OK)
    {
        if (--timeout == 0) return I2C_TIMEOUT_ERROR;
    }

    // --- 5. Generate STOP condition ---
    I2C_Handle->pI2Cx->I2C_CR2 |= I2C_CR2_STOP;

    //--- 6. Wait for STOP flag ---
    while (DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_STOPF) != DAL_OK)
    {
        // 7. Check if any NACK occurred during transmission -- can be checked during transmission as well
        if (DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_NACKF) != DAL_OK)
        {
            I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_NACKCF; // Clear NACK flag
            return I2C_NACK_ERROR;
        }
        if (--timeout == 0) return I2C_TIMEOUT_ERROR;
    }
#endif

    // --- 8. Clear STOP flag ---
    I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_STOPCF;
    return I2C_OK;
}
