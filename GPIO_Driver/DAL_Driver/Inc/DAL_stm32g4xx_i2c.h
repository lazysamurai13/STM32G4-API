/*
 * DAL_stm32g4xx_i2c.h
 *
 *  Created on: Oct 16, 2025
 *      Author: dawoo
 */

#ifndef INC_DAL_STM32G4XX_I2C_H_
#define INC_DAL_STM32G4XX_I2C_H_

#include "DAL_stm32g4xx.h"

typedef struct{
	uint8_t i2c_speed;
	uint8_t i2c_ack;
	uint8_t i2c_fm;
	uint8_t i2c_slave_addr;
	uint8_t i2c_slave_addr_mode;
}I2C_Config_t;

typedef struct{
	I2C_Config_t i2c_config;
	I2C_Regdef_t* pI2Cx;
	uint8_t* TxBuffer;
	uint8_t* RxBuffer;
	uint32_t TxLen;
	uint32_t RxLen;
	uint8_t  i2c_state;
}I2C_Handle_t;


#define I2C_ACK_EN 1
#define I2C_ACK_DI 0

#define I2C_MODE_STANDARD  0
#define I2C_MODE_FAST      1
#define I2C_MODE_FAST_PLUS 2

#define I2C_FM_DUTY_2 	 0
#define I2C_FM_DUTY_16_9 1

#define I2C_SLAVE_7BIT  0
#define I2C_SLAVE_10BIT 0

#define I2C_ENABLE  1
#define I2C_DISABLE 0

#define I2C_READY       0
#define I2C_BUSY_IN_TX  1
#define I2C_BUSY_IN_RX  2

#define I2C_EV_TX_CMPLT 0
#define I2C_EV_RX_CMPLT 1

uint8_t DAL_I2C_Init(I2C_Handle_t*);
uint8_t DAL_I2C_DeInit(I2C_Handle_t*);
uint8_t DAL_I2C_PCLK(I2C_Handle_t* , uint8_t EnorDi);
void DAL_I2C_PeripheralEnorDi(I2C_Handle_t* , uint8_t EnorDi);
void DAL_I2C_Config(I2C_Handle_t*);
void DAL_I2C_Config_IT(uint8_t IRQNumber , uint8_t EnorDi);
void DAL_I2C_Config_IT_Priority(uint8_t IRQNumber , uint8_t Priority);
uint8_t DAL_I2C_Send(I2C_Handle_t* , uint8_t* pdata , uint32_t len);
uint8_t DAL_I2C_Send_IT(I2C_Handle_t*);
uint8_t DAL_I2C_Receive(I2C_Handle_t*,uint8_t* pdata , uint32_t len);
uint8_t DAL_I2C_Receive_IT(I2C_Handle_t*);
uint8_t DAL_I2C_FlagStatus(I2C_Handle_t* , uint8_t FlagName);

#endif /* INC_DAL_STM32G4XX_I2C_H_ */
