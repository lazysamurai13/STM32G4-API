/*
 * spi_example.c
 *
 *  Created on: Aug 18, 2025
 *      Author: dawoo
 */

#include "DAL_stm32g4xx.h"
#include "DAL_stm32g4xx_gpio.h"
#include "DAL_stm32g4xx_spi.h"
#include <string.h>

/**
 * clk -  PB13
 * miso - PA10
 * mosi - PA11
 * nss  - PB12
 */


SPI_Handle_t spi2;

void SPI_ApplicationEventCallback(SPI_Handle_t *pSPI_Handle , uint8_t AppEv)
{
	if(AppEv == SPI_EVENT_TX_CMPLT)
	{
		//1. Check SPI BSY flag
		while (DAL_SPI_FlagStatus(pSPI_Handle , SPI_SR_BSY) == DAL_BUSY);
		//2. Disable SPI peripheral
		DAL_SPI_PeripheralEnDi(pSPI_Handle , DAL_DISABLE);
	}
}

void SPI_GPIO_Init(void)
{
	GPIO_handle_t spi_pins;
	spi_pins.pGPIOx = GPIOA;
	spi_pins.pGPIO_Pinconfig.Pin_Alt_fn  = DAL_GPIO_AF5;
	spi_pins.pGPIO_Pinconfig.Pin_OP_type = DAL_GPIO_OP_MODE_PP;
	spi_pins.pGPIO_Pinconfig.Pin_PuPd    = DAL_GPIO_NO_PuPd;
	spi_pins.pGPIO_Pinconfig.Pin_mode    = DAL_GPIO_MODE_ALT_FN;
	spi_pins.pGPIO_Pinconfig.Pin_speed   = DAL_GPIO_HIGH_SPEED; // doesnt matter
	//miso
	spi_pins.pGPIO_Pinconfig.Pin_number  = DAL_GPIO_PIN_10;
	DAL_GPIO_Init(&spi_pins);
	//mosi
	spi_pins.pGPIO_Pinconfig.Pin_number  = DAL_GPIO_PIN_11;
	DAL_GPIO_Init(&spi_pins);

	spi_pins.pGPIOx = GPIOB;
	spi_pins.pGPIO_Pinconfig.Pin_Alt_fn  = DAL_GPIO_AF5;
	spi_pins.pGPIO_Pinconfig.Pin_OP_type = DAL_GPIO_OP_MODE_PP;
	spi_pins.pGPIO_Pinconfig.Pin_PuPd    = DAL_GPIO_NO_PuPd;
	spi_pins.pGPIO_Pinconfig.Pin_mode    = DAL_GPIO_MODE_ALT_FN;
	spi_pins.pGPIO_Pinconfig.Pin_speed   = DAL_GPIO_HIGH_SPEED;
	//NSS
	spi_pins.pGPIO_Pinconfig.Pin_number  = DAL_GPIO_PIN_12;
	DAL_GPIO_Init(&spi_pins);
	//CLK
	spi_pins.pGPIO_Pinconfig.Pin_number  = DAL_GPIO_PIN_13;
	DAL_GPIO_Init(&spi_pins);
};

int main(void)
{
	char Txbuffer[] = "Hello\0";
	spi2.pSPIx = SPI2;
	spi2.spi_config_t.SPI_BusConfig = SPI_FD;
	spi2.spi_config_t.SPI_CPOL = SPI_CPOL0;
	spi2.spi_config_t.SPI_CPHA = SPI_CPHA0;
	spi2.spi_config_t.SPI_DFF = SPI_DFF8;
	spi2.spi_config_t.SPI_Mode = SPI_MASTER;
	spi2.spi_config_t.SPI_SSM = SPI_SSM_DI;
	spi2.spi_config_t.SPI_Speed = SPI_SPEED_FPCLKBY128;
	spi2.TxState = SPI_READY;
	spi2.RxState = SPI_READY;
	//1. Init GPIO pins
	SPI_GPIO_Init();
	//2. Init SPI
	DAL_SPI_Init(&spi2);
	DAL_SPI_ConfigIT(36 , DAL_ENABLE); //36 is IRQ number of SPI2
  	//3. Enable SPI
	DAL_SPI_PeripheralEnDi(&spi2 , DAL_ENABLE);
	//4. Send data
	DAL_SPI_SendDataIT(&spi2 , (uint8_t*)&Txbuffer, strlen(Txbuffer));
	//5. wait for tx to complete
	while(1);
	return 0;
}

void SPI2_IRQHandler(void)
{
	DAL_SPI_IRQHandling(&spi2);
}

