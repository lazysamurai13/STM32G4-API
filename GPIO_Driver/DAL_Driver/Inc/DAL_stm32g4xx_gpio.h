/*
 * DAL_stm32g4xx_gpio.h
 *
 *  Created on: Jun 17, 2025
 *      Author: dawoo
 */

#ifndef INC_DAL_STM32G4XX_GPIO_H_
#define INC_DAL_STM32G4XX_GPIO_H_

#include "Dal_stm32g4xx.h"

/**
 * GPIO pin config for user
 */
typedef struct
{
	uint8_t Pin_number;
	uint8_t Pin_mode;
	uint8_t Pin_speed;
	uint8_t Pin_PuPd;
	uint8_t Pin_OP_type;
	uint8_t Pin_Alt_fn;
}GPIO_Pinconfig_t;

/**
 * GPIO hanfle for user
 */
typedef struct
{
	GPIO_Regdef_t *pGPIOx; // to hold the base address of gpio port
	GPIO_Pinconfig_t pGPIO_Pinconfig; //to hold pin config
}GPIO_handle_t;

/**
 * Function prototypes
 */
void DAL_GPIO_Init(GPIO_handle_t *pGPIO_Handle); //initialise the port and the pin
void DAL_GPIO_Deinit(GPIO_Regdef_t *pGPIOx); //reset the port
uint8_t DAL_GPIO_Read_Pin(GPIO_handle_t *pGPIO_Handle);
uint16_t DAL_GPIO_Read_Port(GPIO_Regdef_t *pGPIOx);
void DAL_GPIO_Write_Pin(GPIO_handle_t *pGPIO_Handle, uint8_t value);
void DAL_GPIO_Write_Port(GPIO_Regdef_t *pGPIOx, uint16_t value);
void DAL_GPIO_Peri_CLK(GPIO_Regdef_t *pGPIOx, uint8_t EnOrDi);
void DAL_GPIO_Toggle_Pin (GPIO_handle_t *pGPIO_Handle);
void DAL_Interrupt_Handle(GPIO_handle_t *pGPIO_Handle);
void DAL_Interrupt_Congif(GPIO_handle_t *pGPIO_Handle);

/**
 * GPIO mode
 */
#define DAL_GPIO_MODE_INPUT		0
#define DAL_GPIO_MODE_OUTPUT	1
#define DAL_GPIO_MODE_ALT_FN	2
#define DAL_GPIO_MODE_IT_RE		3
#define DAL_GPIO_MODE_IT_FE		4
#define DAL_GPIO_MODE_IT_RE_FE	5
/**
 * GPIO OP type
 */
#define DAL_GPIO_OP_MODE_OD     0
#define DAL_GPIO_OP_MODE_PP     1
/**
 * GPIO PuPd
 */
#define DAL_GPIO_NO_PuPd		0
#define DAL_GPIO_PULL_UP        1
#define DAL_GPIO_PULL_DOWN      2
/**
 * GPIO Speed
 */
#define DAL_GPIO_LOW_SPEED		 0
#define DAL_GPIO_MEDIUM_SPEED	 1
#define DAL_GPIO_HIGH_SPEED		 2
#define DAL_GPIO_VERY_HIGH_SPEED 3

/**
 * GPIO PIN DEF
 */
#define DAL_GPIO_PIN_0 0
#define DAL_GPIO_PIN_1 1
#define DAL_GPIO_PIN_2 2
#define DAL_GPIO_PIN_3 3
#define DAL_GPIO_PIN_4 4
#define DAL_GPIO_PIN_5 5
#define DAL_GPIO_PIN_6 6
#define DAL_GPIO_PIN_7 7
#define DAL_GPIO_PIN_8 8
#define DAL_GPIO_PIN_9 9
#define DAL_GPIO_PIN_10 10
#define DAL_GPIO_PIN_11 11
#define DAL_GPIO_PIN_12 12
#define DAL_GPIO_PIN_13 13
#define DAL_GPIO_PIN_14 14

#define DAL_GPIO_AF0	0
#define DAL_GPIO_AF1	1
#define DAL_GPIO_AF2	2
#define DAL_GPIO_AF3	3
#define DAL_GPIO_AF4	4
#define DAL_GPIO_AF5	5
#define DAL_GPIO_AF6	6
#define DAL_GPIO_AF7	7
#define DAL_GPIO_AF8	8
#define DAL_GPIO_AF9	9
#define DAL_GPIO_AF10	10
#define DAL_GPIO_AF11	11
#define DAL_GPIO_AF12	12
#define DAL_GPIO_AF13	13
#define DAL_GPIO_AF14	14
#define DAL_GPIO_AF15	15

#endif /* INC_DAL_STM32G4XX_GPIO_H_ */
