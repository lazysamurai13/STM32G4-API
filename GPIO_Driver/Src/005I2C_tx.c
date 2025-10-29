/*
 * spi_example.c
 *
 *  Created on: Aug 18, 2025
 *      Author: dawoo
 */

#include "DAL_stm32g4xx.h"
#include "DAL_stm32g4xx_gpio.h"
#include "DAL_stm32g4xx_i2c.h"
#include <string.h>

/**
 * scl - PA9
 * sda - PA8
 */

#define SLAVE_ADDR_I2C 0x31

I2C_Handle_t i2c2;


void I2C_GPIO_Init(void)
{
	GPIO_handle_t i2c_pins;
	i2c_pins.pGPIOx = GPIOA;
	i2c_pins.pGPIO_Pinconfig.Pin_Alt_fn  = DAL_GPIO_AF4;
	i2c_pins.pGPIO_Pinconfig.Pin_OP_type = DAL_GPIO_OP_MODE_OD;
	i2c_pins.pGPIO_Pinconfig.Pin_PuPd    = DAL_GPIO_PULL_UP;
	i2c_pins.pGPIO_Pinconfig.Pin_mode    = DAL_GPIO_MODE_ALT_FN;
	i2c_pins.pGPIO_Pinconfig.Pin_speed   = DAL_GPIO_HIGH_SPEED; // doesnt matter
	//miso
	i2c_pins.pGPIO_Pinconfig.Pin_number  = DAL_GPIO_PIN_8;
	DAL_GPIO_Init(&i2c_pins);
	//mosi
	i2c_pins.pGPIO_Pinconfig.Pin_number  = DAL_GPIO_PIN_9;
	DAL_GPIO_Init(&i2c_pins);
};

int main(void)
{
	uint8_t Txbuffer[] = "Hello\0";

	//1. Init GPIO pins
	I2C_GPIO_Init();
	//2. Init SPI
	i2c2.pI2Cx = I2C2;
	i2c2.i2c_state = I2C_READY;
	i2c2.i2c_config.i2c_ack = I2C_ACK_EN; // only applicable in f4 series not here
	i2c2.i2c_config.i2c_fm = I2C_FM_DUTY_2; //only applicable in f4 series not here
	i2c2.i2c_config.i2c_mode = I2C_MODE_MASTER;
	i2c2.i2c_config.i2c_speed = I2C_MODE_STANDARD;
	DAL_I2C_Init(&i2c2);
	//3. Send data
	DAL_I2C_Master_Transmit(&i2c2, SLAVE_ADDR_I2C, Txbuffer, strlen((char*)Txbuffer));
	while(1);
	return 0;
}

