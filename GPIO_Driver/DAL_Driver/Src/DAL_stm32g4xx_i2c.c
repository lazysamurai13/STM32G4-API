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
static void DAL_I2C_ADDRIT_Handling(I2C_Handle_t* I2C_Handle, uint32_t isr_reg , uint32_t cr1_reg);
static void DAL_I2C_TCR_Handling(I2C_Handle_t* I2C_Handle);
static void DAL_I2C_TC_Handling(I2C_Handle_t* I2C_Handle);
static void DAL_I2C_NACKF_Handling(I2C_Handle_t* I2C_Handle);
static void DAL_I2C_STOPF_Handling(I2C_Handle_t* I2C_Handle);

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
			volatile uint32_t *ptemp = (uint32_t *) 0x40021058U;
			*ptemp |= (1 << 22);
			(void)ptemp;
//			I2C2_PCLK_EN();
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
	uint32_t slave_addr = (I2C_Handle->i2c_config.i2c_device_slave_addr << 1U);
	if(slave_addr != 0U)
	{
		// Disable own addressing before reconfiguring
		I2C_Handle->pI2Cx->I2C_OAR1 &= ~(1U << I2C_OAR1_OA1EN);	//disable own addressing
		//Set slave address
		temp |= slave_addr;
		//Configure 7-bit or 10-bit addressing for slave
		if(I2C_Handle->i2c_config.i2c_slave_addr_mode == I2C_SLAVE_7BIT)
		{
			temp &= ~(1U << I2C_OAR1_OA1MODE);//7 bit addressing in slave mode
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
            timingr_val = 0x30420F13U;
//            timingr_val = 0x00313F57U;
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
            timingr_val = 0x30420F13U;
            break;
    }
    return timingr_val;
}

uint8_t DAL_I2C_CheckFlag(I2C_Handle_t* I2C_Handle , uint32_t Flagname)
{
	uint32_t mask = (1U << Flagname);
	if(I2C_Handle->pI2Cx->I2C_ISR & mask)
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

I2C_Status_t DAL_I2C_Master_Transmit(I2C_Handle_t* I2C_Handle, uint8_t slave_address, uint8_t* data, uint32_t size)
{
    volatile uint32_t timeout;

    // --- 1. Wait until bus is not busy ---
    timeout = I2C_TIMEOUT;
    while(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_BUSY) == DAL_OK)
    {
    	if (--timeout == 0)
    		{
    			return I2C_TIMEOUT_ERROR;
    		}
    }
    // --- 2. Configure the transfer ---
    // Clear CR2 register
    I2C_Handle->pI2Cx->I2C_CR2 = 0;

    // Set slave address (7-bit address)
    I2C_Handle->pI2Cx->I2C_CR2 |= (uint32_t)(slave_address << 1);

    // Set transfer direction to WRITE (0)
    I2C_Handle->pI2Cx->I2C_CR2 &= ~(1U << I2C_CR2_RD_WRN);

    // Set number of bytes to transfer
    I2C_Handle->pI2Cx->I2C_CR2 &= ~(0xFFU << I2C_CR2_NBYTEST); //clear NBYTES bits
    I2C_Handle->pI2Cx->I2C_CR2 |= (size << I2C_CR2_NBYTEST); //set NBYTES
#ifdef I2C_AUTOEND_MODE_En

    // Set AUTOEND mode: automatically send STOP after NBYTES
    I2C_Handle->pI2Cx->I2C_CR2 |= I2C_CR2_AUTOEND;
#endif
    // Generate START condition
    I2C_Handle->pI2Cx->I2C_CR2 |= (1U<<I2C_CR2_START);

    // --- 3. Transmit data bytes ---
    if(size > 1) // for single byte transmission
    {
        for (uint32_t i = 0; i < size; i++)
        {
            // Wait for Transmit Interrupt Status (TXIS) flag
            // This means the transmit data register (TXDR) is empty
            timeout = I2C_TIMEOUT;
            while(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_TXIS) != DAL_OK)
            {
                // Check for NACK (Not Acknowledge)
            	if (DAL_I2C_CheckFlag(I2C_Handle, I2C_ISR_NACKF) == DAL_OK)
            	{
            	    I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_NACKCF; // Clear NACK flag
    				return I2C_NACK_ERROR;
            	}
                if (--timeout == 0)
                	{
                		return I2C_TIMEOUT_ERROR;
                	}
            }

            // Write data to TXDR
            I2C_Handle->pI2Cx->I2C_TXDR = data[i];
        }
		// --- 4. Wait for transfer complete (TC) flag ---
		while (DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_TC) != DAL_OK)
		{
			if (--timeout == 0)
				{
					return I2C_TIMEOUT_ERROR;
				}
		}

    }
    else
    {
		// Wait for Transmit Interrupt Status (TXIS) flag
		// This means the transmit data register (TXDR) is empty
		timeout = I2C_TIMEOUT;
		while(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_TXIS) != DAL_OK)
		{
            if (--timeout == 0)
            {
            	return I2C_TIMEOUT_ERROR;
            }
		}
        // Write data to TXDR
        I2C_Handle->pI2Cx->I2C_TXDR = data[0];
		if (DAL_I2C_CheckFlag(I2C_Handle, I2C_ISR_NACKF) == DAL_OK)
		{
		    I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_NACKCF; // Clear NACK flag
			return I2C_NACK_ERROR;
		}
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

    // --- 5. Generate STOP condition ---
    I2C_Handle->pI2Cx->I2C_CR2 |= (1<< I2C_CR2_STOP);

    timeout = I2C_TIMEOUT;
    //--- 6. Wait for STOP flag ---
    while (DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_STOPF) != DAL_OK)
    {
        // 7. Check if any NACK occurred during transmission -- can be checked during transmission as well
        if (DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_NACKF) == DAL_OK)
        {
            I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_NACKCF; // Clear NACK flag
            return I2C_NACK_ERROR;
        }
        if (--timeout == 0)
        	{
        		return I2C_TIMEOUT_ERROR;
        	}
    }
#endif

    // --- 8. Clear STOP flag ---
    I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_STOPCF;
    return I2C_OK;
}

I2C_Status_t DAL_I2C_Master_Receive(I2C_Handle_t* I2C_Handle ,uint8_t slave_address, uint8_t* pdata, uint32_t size)
{
    volatile uint32_t timeout;

    // --- 1. Wait until bus is not busy ---
    timeout = I2C_TIMEOUT;
    while(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_BUSY) == DAL_OK)
    {
    	if (--timeout == 0)
		{
			return I2C_TIMEOUT_ERROR;
		}
    }
    // --- 2. Configure the transfer ---
    // Clear CR2 register
    I2C_Handle->pI2Cx->I2C_CR2 = 0;
    // Clear all flags
    I2C_Handle->pI2Cx->I2C_ICR = 0xFFFFFFFFU;
    // Set slave address (7-bit address)
    I2C_Handle->pI2Cx->I2C_CR2 |= (uint32_t)(slave_address << 1);
    // Set transfer direction to READ (1)
    I2C_Handle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_RD_WRN);
    // Set number of bytes to read
//    I2C_Handle->pI2Cx->I2C_CR2 &= ~(0xFFU << I2C_CR2_NBYTEST); //clear NBYTES bits
//    I2C_Handle->pI2Cx->I2C_CR2 |= (size << I2C_CR2_NBYTEST); //set NBYTES

    I2C_Handle->pI2Cx->I2C_CR2 &= ~(0xFFU << I2C_CR2_NBYTEST);
    I2C_Handle->pI2Cx->I2C_CR2 |= ((size & 0xFFU) << I2C_CR2_NBYTEST);

#ifdef I2C_AUTOEND_MODE_En

    // Set AUTOEND mode: automatically send STOP after NBYTES
    I2C_Handle->pI2Cx->I2C_CR2 |= I2C_CR2_AUTOEND;
#endif
    // Generate START condition
    I2C_Handle->pI2Cx->I2C_CR2 |= (1U<<I2C_CR2_START);

    if(size == 1)
	{
        timeout = I2C_TIMEOUT;
        while(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_RXNE) != DAL_OK)
        {
            if (--timeout == 0)
			{
				return I2C_TIMEOUT_ERROR;
			}
        }
#ifndef I2C_AUTOEND_MODE_En
        // --- 5. Generate STOP condition ---
	   I2C_Handle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_STOP);
        // Read data from RXDR
        *pdata =  I2C_Handle->pI2Cx->I2C_RXDR;
#endif
	}
    else
    {
		// --- 3. Read data bytes ---
		for (uint32_t i = 0; i < size; i++)
		{
			// Wait for Ready Interrupt Status (RXNE) flag
			// This means the transmit data register RXNE is not empty
			timeout = I2C_TIMEOUT;
			while(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_RXNE) != DAL_OK)
			{
				// Check for NACK (Not Acknowledge)
				if (DAL_I2C_CheckFlag(I2C_Handle, I2C_ISR_NACKF) == DAL_OK)
				{
					I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_NACKCF; // Clear NACK flag
					return I2C_NACK_ERROR;
				}
				if (--timeout == 0) return I2C_TIMEOUT_ERROR;
			}

			// Read data from RXDR
			pdata[i] = I2C_Handle->pI2Cx->I2C_RXDR;
		}
#ifndef I2C_AUTOEND_MODE_En
	    // --- 4. Wait for transfer complete (TC) flag ---
	    while (DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_TC) != DAL_OK)
	    {
	        if (--timeout == 0)
	        	{
	        		return I2C_TIMEOUT_ERROR;
	        	}
	    }
	    // --- 5. Generate STOP condition ---
	    I2C_Handle->pI2Cx->I2C_CR2 |= (1U <<I2C_CR2_STOP);
#endif
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

    //--- 6. Wait for STOP flag ---
    while (DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_STOPF) != DAL_OK)
    {
        // 7. Check if any NACK occurred during transmission -- can be checked during transmission as well
        if (DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_NACKF) == DAL_OK)
        {
            I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_NACKCF; // Clear NACK flag
            return I2C_NACK_ERROR;
        }
        if (--timeout == 0)
		{
			return I2C_TIMEOUT_ERROR;
		}
    }
#endif
    // --- 8. Clear STOP flag ---
    I2C_Handle->pI2Cx->I2C_ICR |= (1U <<I2C_ICR_STOPCF);
    return I2C_OK;
}

I2C_Status_t I2C_Master_Transmit_IT(I2C_Handle_t* I2C_Handle ,uint8_t slave_address, uint8_t* pdata , uint32_t size, uint8_t Sr)
{
    volatile uint32_t timeout;
    uint32_t tsize=0;
    // --- 1. Wait until bus is not busy ---
    timeout = I2C_TIMEOUT;
    while(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_BUSY) == DAL_OK)
    {
    	if (--timeout == 0)
    	{
    		return I2C_TIMEOUT_ERROR;
    	}
    }
    if((size == 0U) || (pdata == NULL))
    {
    	return I2C_CONFIG_ERROR;
    }
    //2. ---- 2. copy data to structure ---
    I2C_Handle->pTxBuffer = pdata;
	I2C_Handle->TxLen   = size;
	I2C_Handle->TxCount   = size;
    I2C_Handle->i2c_state = I2C_BUSY_IN_TX; // assign busy state
    I2C_Handle->SlaveAddress = (slave_address << 1U);
	I2C_Handle->Sr =  Sr; // Repeated Start
    if(size > 255U )
    {
    	//enable RELOAD
    	I2C_Handle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_RELOAD);
    	tsize = 255U;
    }
    else
    {
    	tsize = size;
    }
    //3. --setup CR2 - slave address , nbytea , R/W
    uint32_t temp_reg = 0U;
    temp_reg |= (uint32_t) (slave_address << 1U);
    temp_reg |= ((tsize&0xFFU) << I2C_CR2_NBYTEST);
    temp_reg &= ~(1U << I2C_CR2_RD_WRN);
    I2C_Handle->pI2Cx->I2C_CR2 = temp_reg;
    //4. --setup CR2 - enable interrupts
    I2C_Handle->pI2Cx->I2C_CR1 |= ((1U << I2C_CR1_EERIE) | (1U << I2C_CR1_NACKIE) | (1U << I2C_CR1_STOPIE) |(1U << I2C_CR1_TXIE) | (1U << I2C_CR1_TCIE));
    //5. --generate start
	I2C_Handle->pI2Cx->I2C_CR2 |= (1U<<I2C_CR2_START);
	return I2C_OK;
}

I2C_Status_t I2C_Master_Receive_IT(I2C_Handle_t* I2C_Handle ,uint8_t slave_address, uint8_t* pdata , uint32_t size, uint8_t Sr)
{
	volatile uint32_t timeout;
    uint32_t tsize=0;
	// --- 1. Wait until bus is not busy ---
	timeout = I2C_TIMEOUT;
	while(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_BUSY) == DAL_OK)
	{
		if (--timeout == 0)
		{
			return I2C_TIMEOUT_ERROR;
		}
	}
    if(size == 0U)
    {
    	return I2C_CONFIG_ERROR;
    }
	//2 ---- 2. copy data to structure ---
	I2C_Handle->RxLen   = size;
	I2C_Handle->RxCount = size;
	I2C_Handle->pRxBuffer = pdata;
	I2C_Handle->i2c_state = I2C_BUSY_IN_RX; // assign busy state
	I2C_Handle->SlaveAddress = slave_address;
	I2C_Handle->Sr =  Sr; // Repeated Start
    if(size > 255U )
    {
    	//enable RELOAD
    	I2C_Handle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_RELOAD);
    	tsize = 255U;
    }
    else
    {
    	tsize = size;
    }
	//3. ---- 3. Configure CR2 register - slave address , read/write , number of bytes
	uint32_t temp_reg = 0;
	temp_reg = 0;
	temp_reg |= (uint32_t)(slave_address << 1);
	temp_reg |= (1U << I2C_CR2_RD_WRN);
	temp_reg |= (tsize & 0xFFU) << I2C_CR2_NBYTEST;
	I2C_Handle->pI2Cx->I2C_CR2 = temp_reg;
	// 3. Enable Interrupts
	// We need RXIE (to get data), NACKIE (fail), STOPIE (complete), ERRIE (errors), ADDRIE (address matched)
	// Do NOT enable TXIE here.
    I2C_Handle->pI2Cx->I2C_CR1 |=((1U << I2C_CR1_EERIE) | (1U << I2C_CR1_ADDRIE)  | (1U << I2C_CR1_NACKIE) | (1U << I2C_CR1_RXIE) | (1U << I2C_CR1_STOPIE));
	//4. --- 4,Start I2C communication by generating START condition
	I2C_Handle->pI2Cx->I2C_CR2 |= (1U<<I2C_CR2_START);
	return I2C_OK;
}
/**
 * @fn void DAL_I2C_ADDRIT_Handling(void)
 * @brief Handles irq routine when address is matched in slave mode
 *
 * @pre
 * @post
 */
static void DAL_I2C_ADDRIT_Handling(I2C_Handle_t* I2C_Handle, uint32_t isr_reg , uint32_t cr1_reg )
{
	uint32_t size =0;
	if(I2C_Handle->i2c_state ==I2C_BUSY_IN_RX)
	{
		size = I2C_Handle->RxLen;
	}
	else if(I2C_Handle->i2c_state ==I2C_BUSY_IN_TX)
	{
		size = I2C_Handle->TxLen;
	}
	if(((isr_reg >> I2C_ISR_ADDR)&0x1) && ((cr1_reg >> I2C_CR1_ADDRIE)&0x1))
	{
		//clear ADDR flag is done in IRQ handling function
		I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_ADDRCF;
		//read the R/W bit
		uint8_t dir = (uint8_t) ((I2C_Handle->pI2Cx->I2C_ISR >> I2C_ISR_DIR) & 0x1);
		if(dir == I2C_ISR_DIR_WRITE)
		{
			//setup cr2 with nbytes , direction
			I2C_Handle->pI2Cx->I2C_CR2 &= ~(0xFFU << I2C_CR2_NBYTEST);
			I2C_Handle->pI2Cx->I2C_CR2 |= (size << I2C_CR2_NBYTEST);
			I2C_Handle->pI2Cx->I2C_CR2 &= ~(1U << I2C_CR2_RD_WRN);
			//enable interrupts
			I2C_Handle->i2c_state = I2C_BUSY_IN_TX;
			// Enable TXIE (to send data) and disable RXIE
			I2C_Handle->pI2Cx->I2C_CR1 |= (1U << I2C_CR1_TXIE);
			I2C_Handle->pI2Cx->I2C_CR1 &= ~(1U << I2C_CR1_RXIE);
		}
		else
		{
			//setup cr2 with nbytes , direction
			I2C_Handle->pI2Cx->I2C_CR2 &= ~(0xFFU << I2C_CR2_NBYTEST);
			I2C_Handle->pI2Cx->I2C_CR2 |= (size << I2C_CR2_NBYTEST);
			I2C_Handle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_RD_WRN);
			//enable interrupts
			I2C_Handle->i2c_state = I2C_BUSY_IN_RX;
			// Enable RXIE (to send data) and disable TXIE
			I2C_Handle->pI2Cx->I2C_CR1 |= (1U << I2C_CR1_RXIE);
			I2C_Handle->pI2Cx->I2C_CR1 &= ~(1U << I2C_CR1_TXIE);
		}
		//NOTIFY Application
		DAL_I2C_EV_ApplicationEventCallback(I2C_Handle , I2C_EV_ADDR_MATCH);
	}
}
static void DAL_I2C_TCR_Handling(I2C_Handle_t* I2C_Handle)
{
	if((I2C_Handle->TxCount > 255U ) && (I2C_Handle->i2c_state == I2C_BUSY_IN_TX))
	{
		uint8_t size = (uint8_t) I2C_Handle->TxLen;
		if(size < 255)
		{
			//disable RELOAD
			I2C_Handle->pI2Cx->I2C_CR2 &=~(1U << I2C_CR2_RELOAD);
			//load NBYTES
				I2C_Handle->pI2Cx->I2C_CR2 &= ~(0xFFU << I2C_CR2_NBYTEST);
			I2C_Handle->pI2Cx->I2C_CR2 |= (size << I2C_CR2_NBYTEST);
		}
		else
		{
			size = 255U;
			//load NBYTES
			I2C_Handle->pI2Cx->I2C_CR2 &= ~(0xFFU << I2C_CR2_NBYTEST);
			I2C_Handle->pI2Cx->I2C_CR2 |= (size << I2C_CR2_NBYTEST);
		}
	}
	else if((I2C_Handle->RxCount > 255U ) && (I2C_Handle->i2c_state == I2C_BUSY_IN_TX))
	{
		uint8_t size = (uint8_t) (I2C_Handle->RxLen);
		if(size < 255)
		{
			//disable RELOAD
			I2C_Handle->pI2Cx->I2C_CR2 &=~(1U << I2C_CR2_RELOAD);
			//load NBYTES
			size = I2C_Handle->TxLen;
			I2C_Handle->pI2Cx->I2C_CR2 |= (size << I2C_CR2_NBYTEST);
		}
		else
		{
			size = 255U;
			//load NBYTES
			I2C_Handle->pI2Cx->I2C_CR2 |= (size << I2C_CR2_NBYTEST);
		}
	}
}

static void DAL_I2C_TC_Handling(I2C_Handle_t* I2C_Handle)
{
	if(I2C_Handle->Sr == REPEATED_START_EN)
	{
		//notify application
		DAL_I2C_EV_ApplicationEventCallback(I2C_Handle , I2C_EV_TC);
	}
	else
	{
		//send stop signal which clears the flag
		I2C_Handle->pI2Cx->I2C_CR2 |= (1U<<I2C_CR2_STOP);
		//notify application
		DAL_I2C_EV_ApplicationEventCallback(I2C_Handle , I2C_EV_TC);
	}
}

static void DAL_I2C_NACKF_Handling(I2C_Handle_t* I2C_Handle)
{
	//clear NACKF flag
	I2C_Handle->pI2Cx->I2C_ICR |= (1U<<I2C_ICR_NACKCF);
	//generate stop condition
	I2C_Handle->pI2Cx->I2C_CR2 |= (1U << I2C_CR2_STOP);
	//disable interrupts
	I2C_Handle->pI2Cx->I2C_CR1 &= ~((1U << I2C_CR1_NACKIE) | (1U << I2C_CR1_RXIE) | (1U << I2C_CR1_TXIE)| (1U << I2C_CR1_STOPIE) | (1U << I2C_CR1_TCIE));
	//reset structure data
	I2C_Handle->ErrorCode = I2C_ERROR_NACK;
	I2C_Handle->RxCount = 0;
	I2C_Handle->TxCount = 0;
	I2C_Handle->TxLen = 0;
	I2C_Handle->RxLen = 0;
	I2C_Handle->i2c_state = I2C_READY;
	I2C_Handle->pRxBuffer = NULL;
	I2C_Handle->pTxBuffer = NULL;
}

static void DAL_I2C_STOPF_Handling(I2C_Handle_t* I2C_Handle)
{
	//clear STOPF flag
	I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_STOPCF;
	//disable interrupts
	I2C_Handle->pI2Cx->I2C_CR1 &= ~((1U << I2C_CR1_NACKIE) | (1U << I2C_CR1_RXIE) | (1U << I2C_CR1_TXIE)| (1U << I2C_CR1_STOPIE) | (1U << I2C_CR1_TCIE));
	//reset structure data
	I2C_Handle->ErrorCode = 0;
	I2C_Handle->RxCount = 0;
	I2C_Handle->TxCount = 0;
	I2C_Handle->TxLen = 0;
	I2C_Handle->RxLen = 0;
	I2C_Handle->i2c_state = I2C_READY;
	I2C_Handle->pRxBuffer = NULL;
	I2C_Handle->pTxBuffer = NULL;
}

void I2C_EV_IRQHandling(I2C_Handle_t* I2C_Handle)
{
	uint32_t isr_reg = I2C_Handle->pI2Cx->I2C_ISR;
	uint32_t cr1_reg = I2C_Handle->pI2Cx->I2C_CR1;
	//1. Handle ADDR event - addrie is enabled and addrcf is triggerd
	if(I2C_Handle->i2c_state == I2C_READY) // IN Slave mode
	{
		DAL_I2C_ADDRIT_Handling(I2C_Handle,isr_reg,cr1_reg);
	}
	else if(((I2C_Handle->i2c_state == I2C_BUSY_IN_RX)) || ((I2C_Handle->i2c_state == I2C_BUSY_IN_TX)))
	{
		//Handle TXE event
		if(((isr_reg >> I2C_ISR_TXIS)&0x1) && ((cr1_reg >> I2C_CR1_TXIE)&0x1))
		{
			if(I2C_Handle->TxLen !=0)
			{
				I2C_Handle->pI2Cx->I2C_TXDR =  (uint32_t) *(I2C_Handle->pTxBuffer);
				I2C_Handle->pTxBuffer++;
				I2C_Handle->TxLen--;
			}
		}
		//Handle RXE event
		if(((isr_reg >> I2C_ISR_RXNE)&0x1) && ((cr1_reg >> I2C_CR1_RXIE)&0x1))
		{
			if(I2C_Handle->RxLen !=0)
			{
				*(I2C_Handle->pRxBuffer) = (uint8_t) (I2C_Handle->pI2Cx->I2C_RXDR);
				I2C_Handle->pRxBuffer++;
				I2C_Handle->RxLen--;
			}
		}
		//4. Handle TC event
		if(((isr_reg >> I2C_ISR_TCR)&0x1) && ((cr1_reg >> I2C_CR1_TCIE)&0x1) && ((I2C_Handle->pI2Cx->I2C_CR2 >> I2C_CR2_RELOAD)))
		{
			DAL_I2C_TCR_Handling(I2C_Handle);
		}
		else if(((isr_reg >> I2C_ISR_TC)&0x1) && ((cr1_reg >> I2C_CR1_TCIE)&0x1))
		{
			DAL_I2C_TC_Handling(I2C_Handle);
		}
	}

	//2. Handle NACKF event
	if(((isr_reg >> I2C_ISR_NACKF)&0x1) && ((cr1_reg >> I2C_CR1_NACKIE)&0x1))
	{
		DAL_I2C_NACKF_Handling(I2C_Handle);
		//notify application
		DAL_I2C_EV_ApplicationEventCallback(I2C_Handle , I2C_EV_NACK_ERR);
	}

	//3. Handle STOPF event
	if(((isr_reg >> I2C_ISR_STOPF)&0x1) && ((cr1_reg >> I2C_CR1_STOPIE)&0x1))
	{
		DAL_I2C_STOPF_Handling(I2C_Handle);
		//notify application
		DAL_I2C_EV_ApplicationEventCallback(I2C_Handle , I2C_EV_STOP);
	}
}

void I2C_ERR_IRQHandling(I2C_Handle_t* I2C_Handle)
{
	//1. Handle BERR event
	if(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_BERR) == DAL_OK)
	{
		//clear BERR flag
		I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_BERRCF;
		//notify application
		DAL_I2C_ER_ApplicationEventCallback(I2C_Handle , I2C_ERROR_BERR);
	}
	//2. Handle CRC event
	if(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_PECERR) == DAL_OK)
	{
		//clear BERR flag
		I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_PECCF;
		//notify application
		DAL_I2C_ER_ApplicationEventCallback(I2C_Handle , I2C_ERROR_PECC);
	}
	//3. Handle ARLO event
	if(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_ARLO) == DAL_OK)
	{
		//clear ARLO flag
		I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_ARLOCF;
		//notify application
		DAL_I2C_ER_ApplicationEventCallback(I2C_Handle , I2C_ERROR_ARLO);
	}
	//4. Handle OVR event
	if(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_OVR) == DAL_OK)
	{
		//clear OVR flag
		I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_OVRCF;
		//notify application
		DAL_I2C_ER_ApplicationEventCallback(I2C_Handle , I2C_ERROR_OVR);
	}
	//5. Handle TIMEOUT event
	if(DAL_I2C_CheckFlag(I2C_Handle , I2C_ISR_TIMEOUT) == DAL_OK)
	{
		//clear TIMEOUT flag
		I2C_Handle->pI2Cx->I2C_ICR |= I2C_ICR_TIMEOUTCF;
		//notify application
		DAL_I2C_ER_ApplicationEventCallback(I2C_Handle , I2C_ERROR_TIMEOUT);
	}
}
