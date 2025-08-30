/*
 * DAL_stm32g4xx_spi.h
 *
 *  Created on: Aug 16, 2025
 *      Author: dawoo
 */

#ifndef INC_DAL_STM32G4XX_SPI_H_
#define INC_DAL_STM32G4XX_SPI_H_

#include "DAL_stm32g4xx.h"

#define SPI_MASTER 1
#define SPI_SLAVE  0

#define SPI_FD      0
#define SPI_HD      1
#define SPI_RXONLY  2
#define SPI_TXONLY  3

#define SPI_SPEED_FPCLKBY2		0
#define SPI_SPEED_FPCLKBY4 		0x1
#define SPI_SPEED_FPCLKBY8 		0x2
#define SPI_SPEED_FPCLKBY16 	0x3
#define SPI_SPEED_FPCLKBY32		0x4
#define SPI_SPEED_FPCLKBY64		0x5
#define SPI_SPEED_FPCLKBY128	0x6
#define SPI_SPEED_FPCLKBY256	0x7

#define SPI_SSM_EN 1
#define SPI_SSM_DI 0

#define SPI_CPOL0 0
#define SPI_CPOL1 1

#define SPI_CPHA0 0
#define SPI_CPHA1 1

#define SPI_DFF4	0
#define SPI_DFF5	0x1
#define SPI_DFF6	0x2
#define SPI_DFF7 	0x3
#define SPI_DFF8 	0x4
#define SPI_DFF9 	0x5
#define SPI_DFF10 	0x6
#define SPI_DFF11	0x7
#define SPI_DFF12	0x8
#define SPI_DFF13	0x9
#define SPI_DFF14	0x10
#define SPI_DFF15	0x11
#define SPI_DFF16	0x12

#define SPI_ENABLE  1
#define SPI_DISABLE 0

#define SPI_SSI_EN  1
#define SPI_SSI_DI  0

typedef struct
{
	uint8_t SPI_Mode; // Master or slave
	uint8_t SPI_BusConfig; // bus vonfiguration - full duplex,half duplex.simplex
	uint8_t SPI_DFF; // data frame format
	uint8_t SPI_CPOL; // clk polarity
	uint8_t SPI_CPHA; // clk phase
	uint8_t SPI_Speed; // pclk speed
	uint8_t SPI_SSM; // software slave managment
}SPI_Config_t;

typedef struct
{
	SPI_Regdef_t *pSPIx;
	SPI_Config_t spi_config_t;
}SPI_Handle_t;

#define SPI_TX_Flag 0
#define SPI_RX_Flag 1

void DAL_SPI_Peri_CLK(SPI_Regdef_t *pSPIx, uint8_t EnOrDi);
uint8_t DAL_SPI_FlagStatus(SPI_Handle_t *pSPI_Handle , uint8_t Flagname);
void DAL_SPI_Init(SPI_Handle_t *pSPI_Handle);
void DAL_SPI_PeripheralEnDi(SPI_Handle_t *pSPI_Handle ,uint8_t ENorDi);
uint8_t DAL_SPI_SendData(SPI_Handle_t *pSPI_Handle , uint8_t* pdata , uint32_t len);

#endif /* INC_DAL_STM32G4XX_SPI_H_ */
