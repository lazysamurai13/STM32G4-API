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

void SPI_GPIO_Init(void)
{
	GPIO_handle_t spi_pins;
	spi_pins.pGPIOx = GPIOA;
	spi_pins.pGPIO_Pinconfig.Pin_Alt_fn  = DAL_GPIO_AF5;
	spi_pins.pGPIO_Pinconfig.Pin_OP_type = DAL_GPIO_OP_MODE_PP;
	spi_pins.pGPIO_Pinconfig.Pin_PuPd    = DAL_GPIO_NO_PuPd;
	spi_pins.pGPIO_Pinconfig.Pin_mode    = DAL_GPIO_MODE_ALT_FN;
	spi_pins.pGPIO_Pinconfig.Pin_number  = DAL_GPIO_PIN_0;
	spi_pins.pGPIO_Pinconfig.Pin_speed   = DAL_GPIO_HIGH_SPEED; /// doesnt matter

	DAL_GPIO_Init(&spi_pins);
};

int main(void)
{
	char Txbuffer[] = "Hello\0";
	SPI_Handle_t spi2;
	spi2.pSPIx = SPI2;
	spi2.spi_config_t.SPI_BusConfig = SPI_FD;
	spi2.spi_config_t.SPI_CPOL = SPI_CPOL0;
	spi2.spi_config_t.SPI_CPHA = SPI_CPHA0;
	spi2.spi_config_t.SPI_DFF = SPI_DFF8;
	spi2.spi_config_t.SPI_Mode = SPI_MASTER;
	spi2.spi_config_t.SPI_SSM = SPI_SSM_DI;
	spi2.spi_config_t.SPI_Speed = SPI_SPEED_FPCLKBY2;
	//1. Init GPIO pins
	SPI_GPIO_Init();
	//2. Init SPI
	DAL_SPI_Init(&spi2);
	//3. Init SSI -  no need for hardware nss
//	DAL_SPI_SSI(&spi1, DAL_ENABLE);
  	//4. Enable SPI
	DAL_SPI_PeripheralEnDi(&spi2 , DAL_ENABLE);
	//5. Send data
	DAL_SPI_SendData(&spi2 , (uint8_t*)&Txbuffer, strlen(Txbuffer));
	//5. Check SPI BSY Flag for spi completion
	while (DAL_SPI_FlagStatus(&spi2 , SPI_SR_BSY) == DAL_BUSY);
	//7. Disable SPI
	DAL_SPI_PeripheralEnDi(&spi2 , DAL_DISABLE);
	while(1);
	return 0;
}
