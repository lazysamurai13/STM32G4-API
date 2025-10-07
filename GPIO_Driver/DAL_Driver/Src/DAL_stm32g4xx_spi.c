/*
 * DAL_stm32g4xx_spi.c
 *
 *  Created on: Aug 16, 2025
 *      Author: dawood
 */

#include "DAL_stm32g4xx_spi.h"
#include <stdio.h>

/**
 * @fn void DAL_SPI_Peri_CLK(SPI_Regdef_t*, uint8_t)
 * @brief Enable or disable peripheral clk
 *
 * @pre
 * @post
 * @param pSPIx
 * @param EnOrDi
 */
void DAL_SPI_Peri_CLK(SPI_Regdef_t *pSPIx, uint8_t EnOrDi)
{
	if(EnOrDi == DAL_ENABLE)
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_EN();
			volatile uint32_t* temp_addr = (0x40021000 + 0x58);
			*temp_addr |= (1<<14);
//			RCC->APB1ENR1_reg |= (1<<18);
		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_EN();
		}
	}
	else
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_DI();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_DI();
		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_DI();
		}
	}
}

void DAL_SPI_Denit(SPI_Regdef_t *pSPIx)
{

}

/**
 * @fn void DAL_SPI_PeripheralEnDi(SPI_Handle_t*, uint8_t)
 * @brief Enable or disable SPI peripheral
 *
 * @pre
 * @post
 * @param pSPI_Handle
 * @param ENorDi
 */
void DAL_SPI_PeripheralEnDi(SPI_Handle_t *pSPI_Handle ,uint8_t ENorDi)
{
	if(ENorDi == DAL_ENABLE){
	//Enable SPI Peripheral
		pSPI_Handle->pSPIx->SPI_CR1 |= (SPI_ENABLE << SPI_CR1_SPE);
	}
	else{
	//Disable SPI Peripheral
		pSPI_Handle->pSPIx->SPI_CR1 |= (SPI_DISABLE << SPI_CR1_SPE);
	}
}

/**
 * @fn void DAL_SPI_Init(SPI_Handle_t*)
 * @brief Initialize SPI peripheral
 *
 * @pre
 * @post
 * @param pSPI_Handle
 */
void DAL_SPI_Init(SPI_Handle_t *pSPI_Handle)
{
	volatile uint32_t temp=0;
	//1. Enable peripheral clk
	DAL_SPI_Peri_CLK(pSPI_Handle->pSPIx, DAL_ENABLE);
	//2. Set SPI mode
	temp |= (pSPI_Handle->spi_config_t.SPI_Mode << SPI_CR1_MSTR);
	//3. Set SPI Bus Config
	if(pSPI_Handle->spi_config_t.SPI_BusConfig == SPI_FD)
	{
		temp |= (SPI_FD << SPI_CR1_BIDIMODE);
	}
	else if(pSPI_Handle->spi_config_t.SPI_BusConfig == SPI_HD)
	{
		temp |= (SPI_HD << SPI_CR1_BIDIMODE);
	}
	else if(pSPI_Handle->spi_config_t.SPI_BusConfig == SPI_RXONLY)
	{
		temp |= (SPI_HD << SPI_CR1_BIDIMODE); // put in biderectional mode
		temp |= (0 << SPI_CR1_BIDIOE); // rx only
	}
	else
	{
		temp |= (SPI_HD << SPI_CR1_BIDIMODE); // put in biderectional mode
		temp |= (1 << SPI_CR1_BIDIOE); // tx only
	}
	//4. Set SPI DFF
	pSPI_Handle->pSPIx->SPI_CR2 &= ~(1 << SPI_CR2_DS0); //clear bit
	pSPI_Handle->pSPIx->SPI_CR2 &= ~(1 << SPI_CR2_DS1); //clear bit
	pSPI_Handle->pSPIx->SPI_CR2 &= ~(1 << SPI_CR2_DS2); //clear bit
	pSPI_Handle->pSPIx->SPI_CR2 |= (pSPI_Handle->spi_config_t.SPI_BusConfig << SPI_CR2_DS0);
	//5. Set SPI CPOL
	temp |= (pSPI_Handle->spi_config_t.SPI_CPOL << SPI_CR1_CPOL);
	//6. Set SPI CPHA
	temp |= (pSPI_Handle->spi_config_t.SPI_CPHA << SPI_CR1_CPHA);
	//7. Set SPI SSM
	temp |= (pSPI_Handle->spi_config_t.SPI_SSM << SPI_CR1_SSM);
	if(pSPI_Handle->spi_config_t.SPI_SSM == SPI_SSM_DI) // hardware nss management
	{
		//enable ssoe
		pSPI_Handle->pSPIx->SPI_CR2 |= (1 << SPI_CR2_SSOE);
	}
	//8. assign temp value to reg
	pSPI_Handle->pSPIx->SPI_CR1 = temp;
}

/**
 * @fn void DAL_SPI_SSI(SPI_Handle_t*, uint8_t)
 * @brief Enable or disable SSI - used for software nss management
 *
 * @pre
 * @post
 * @param pSPI_Handle
 * @param Flagname
 */
void DAL_SPI_SSI(SPI_Handle_t *pSPI_Handle , uint8_t Flagname)
{
	if(Flagname == DAL_ENABLE)
	{
		pSPI_Handle->pSPIx->SPI_CR1 |= (SPI_SSI_EN << SPI_CR1_SSI);
	}
	else
	{
		pSPI_Handle->pSPIx->SPI_CR1 |= (SPI_SSI_DI << SPI_CR1_SSI);
	}
}
/**
 * @fn uint8_t DAL_SPI_SendData(SPI_Handle_t*, uint8_t*, uint32_t)
 * @brief Send data through SPI peripheral
 *
 * @pre
 * @post
 * @param pSPI_Handle
 * @param pdata
 * @param len
 * @return uint8_t
 */
uint8_t DAL_SPI_SendData(SPI_Handle_t *pSPI_Handle , uint8_t* pdata , uint32_t len)
{
	//1. check wheather data is null or length is 0
	if(pdata == NULL || len == 0)
	{
		return DAL_ERROR;
	}
	//2. until length is 0
	while(len > 0)
	{
	//3. check tx buffer is empty by reading status in SR reg
		if(DAL_SPI_FlagStatus(pSPI_Handle , SPI_SR_TXE) == DAL_OK)
		{
			//4. load data into dr respective of dff
//			if(pSPI_Handle->pSPIx->SPI_CR2 &=  (1 << SPI_CR2_DS3))
//			{
				//5. load data
				pSPI_Handle->pSPIx->SPI_DR = *pdata;
				//6. increment data pointer
				pdata++;
				//7. decrement length
				len--;
//			}
//			else
//			{
//				pSPI_Handle->pSPIx->SPI_DR = *((uint16_t*)pdata);
//				(uint16_t*)pdata++;
//				len--;
//				len--;
//			}
		}
	}
	return DAL_OK;
}

uint8_t DAL_SPI_ReceiveData(SPI_Handle_t *pSPI_Handle , uint8_t* pdata , uint32_t len)
{
	//1. check wheather data is null or length is 0
	if(len == 0)
	{
		return DAL_ERROR;
	}
	//2. until length is 0
	while(len > 0)
	{
	//3. check tx buffer is empty by reading status in SR reg
		if(DAL_SPI_FlagStatus(pSPI_Handle , SPI_SR_RXNE) == DAL_OK)
		{
			//4. load data into dr respective of dff
//			if(pSPI_Handle->pSPIx->SPI_CR2 &=  (1 << SPI_CR2_DS3))
//			{
				//5. load data
				*pdata = pSPI_Handle->pSPIx->SPI_DR;
				//6. increment data pointer
				pdata++;
				//7. decrement length
				len--;
//			}
//			else
//			{
//				pSPI_Handle->pSPIx->SPI_DR = *((uint16_t*)pdata);
//				(uint16_t*)pdata++;
//				len--;
//				len--;
//			}
		}
	}
	return DAL_OK;
}

/**
 * @fn uint8_t DAL_SPI_FlagStatus(SPI_Handle_t*, uint8_t)
 * @brief Check status of flag in SR reg
 *
 * @pre
 * @post
 * @param pSPI_Handle
 * @param Flagname
 * @return uint8_t
 */
uint8_t DAL_SPI_FlagStatus(SPI_Handle_t *pSPI_Handle , uint8_t Flagname)
{
	uint32_t reg_value = pSPI_Handle->pSPIx->SPI_SR;  // Read full register
	// Check if bit n is set (1) or cleared (0)
	uint8_t bit_value = (reg_value >> Flagname) & 0x1;
	if(bit_value)
	{
		return DAL_OK;
	}
	return DAL_ERROR;
}

void DAL_SPI_ConfigIT(uint8_t IRQNumber, uint8_t EnOrDi)
{
	if(EnOrDi == DAL_ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//program ISER0 reg
			*NVIC_ISER0_REG_ADDR |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber < 64)
		{
			//program ISER1 reg
			*NVIC_ISER1_REG_ADDR |= (1 << (IRQNumber % 32));
		}
		else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			//program ISER2 reg
			*NVIC_ISER2_REG_ADDR |= (1 << (IRQNumber % 64));
		}
	}
	else
	{
		if(IRQNumber <= 31)
		{
			//program ICER0 reg
			*NVIC_ICER0_REG_ADDR |= (1 << IRQNumber);
		}
		else if(IRQNumber > 31 && IRQNumber < 64)
		{
			//program ICER1 reg
			*NVIC_ICER1_REG_ADDR |= (1 << (IRQNumber % 32));
		}
		else if(IRQNumber >= 64 && IRQNumber < 96)
		{
			//program ICER2 reg
			*NVIC_ICER2_REG_ADDR |= (1 << (IRQNumber % 64));
		}
	}
}

void DAL_SPI_ConfigIRQPriority(uint8_t IRQNumber, uint32_t IRQPriority)
{
	if(IRQNumber < 240)
	{
		uint8_t iprx = IRQNumber / 4;
		uint8_t iprx_section = IRQNumber % 4;
		uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
		*(NVIC_IPR_REG_ADDR + iprx) |= (IRQPriority << shift_amount);
	}
}
