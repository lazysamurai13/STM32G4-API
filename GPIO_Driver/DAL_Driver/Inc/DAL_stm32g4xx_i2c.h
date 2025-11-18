/*
 * DAL_stm32g4xx_i2c.h
 *
 *  Created on: Oct 16, 2025
 *      Author: dawoo
 */

#ifndef INC_DAL_STM32G4XX_I2C_H_
#define INC_DAL_STM32G4XX_I2C_H_

#include "DAL_stm32g4xx.h"

#define DAL_CHECK_BIT(REG,MASK) (((REG) & (MASK)) == (MASK)) ? 1U : 0)
#define DAL_READ_BIT(REG,BIT_POS,MASK) (((REG) >> (BIT)) & (MASK))
typedef struct{
	uint8_t i2c_speed;
	uint8_t i2c_ack;
	uint8_t i2c_fm;
	uint8_t i2c_slave_addr;
	uint8_t i2c_slave_addr_mode;
	uint8_t i2c_mode;
}I2C_Config_t;

typedef struct{
	I2C_Config_t i2c_config;
	I2C_Regdef_t* pI2Cx;
	uint8_t* pTxBuffer;
	uint8_t* pRxBuffer;
	uint32_t TxCount;
	uint32_t TxLen;
	uint32_t RxLen;
	uint32_t RxCount;
	uint8_t ErrorCode;
	uint8_t  i2c_state;
}I2C_Handle_t;

/**
 * @brief I2C operation status
 */
typedef enum {
    I2C_OK = 0,
    I2C_NACK_ERROR = 1,
    I2C_TIMEOUT_ERROR = 2
} I2C_Status_t;

// Define a reasonable timeout value for polling loops
#define I2C_TIMEOUT 1000000U

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

#define I2C_FLAG_TXE 0
#define I2C_FLAG_RXNE 2
#define I2C_FLAG_ADDRCF 3

#define I2C_MODE_SLAVE 0
#define I2C_MODE_MASTER 1

#define I2C_EV_TX_CMPLT 0
#define I2C_EV_RX_CMPLT 1

uint8_t DAL_I2C_Init(I2C_Handle_t*);
uint8_t DAL_I2C_DeInit(I2C_Handle_t*);
uint8_t DAL_I2C_PCLK(I2C_Handle_t* , uint8_t EnorDi);
void DAL_I2C_PeripheralEnorDi(I2C_Handle_t* , uint8_t EnorDi);
void DAL_I2C_Config(I2C_Handle_t*);
void DAL_I2C_Config_IT(uint8_t IRQNumber , uint8_t EnorDi);
void DAL_I2C_Config_IT_Priority(uint8_t IRQNumber , uint8_t Priority);
I2C_Status_t DAL_I2C_Master_Transmit(I2C_Handle_t* I2C_Handle, uint8_t slave_address, uint8_t* data, uint32_t size);
I2C_Status_t DAL_I2C_Master_Receive(I2C_Handle_t* I2C_Handle ,uint8_t slave_address, uint8_t* pdata, uint32_t size);
uint8_t DAL_I2C_FlagStatus(I2C_Handle_t* , uint8_t FlagName);
uint8_t DAL_I2C_CheckFlag(I2C_Handle_t* I2C_Handle , uint32_t Flagname);
#endif /* INC_DAL_STM32G4XX_I2C_H_ */
