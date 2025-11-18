/*
 * DAL_GPIO.h
 *
 *  Created on: Jun 9, 2025
 *      Author: dawoo
 */

#ifndef INC_DAL_STM32G4XX_H_
#define INC_DAL_STM32G4XX_H_

#include <stdint.h>
#include <stddef.h>

/**
 * @def Processor specific registers
 * @brief
 *
 */

/**
 * @def NVIC base address
 * @brief
 *
 */
#define NVIC_ICTR_REG_ADDR ((volatile uint32_t*)0xE000E004)

#define NVIC_ISER0_REG_ADDR ((volatile uint32_t*)0xE000E100)
#define NVIC_ISER1_REG_ADDR ((volatile uint32_t*)0xE000E104)
#define NVIC_ISER2_REG_ADDR ((volatile uint32_t*)0xE000E108)

#define NVIC_ICER0_REG_ADDR ((volatile uint32_t*)0xE000E180)
#define NVIC_ICER1_REG_ADDR ((volatile uint32_t*)0xE000E184)
#define NVIC_ICER2_REG_ADDR ((volatile uint32_t*)0xE000E188)

#define NVIC_IPR_REG_ADDR  ((volatile uint32_t*)0xE000E400)

#define NO_PR_BITS_IMPLEMENTED 4
/**
 * Memory base address
 */
#define DAL_FLASH_BASEADDR 0x08000000
#define DAL_SRAM1_BASEADDR 0x20000000
#define DAL_SRAM2_BASEADDR 0x20004000
#define DAL_ROM_BASEADDR   0x1FFF0000 //system memory

/**
 *  Bus domain
 */
#define DAL_AHB2_BASEADDR 0x48000000
#define DAL_AHB1_BASEADDR 0x40020000
#define DAL_APB2_BASEADDR 0x40010000
#define DAL_APB1_BASEADDR 0x40000000

/**
 * Peripheral base addressesl
 */
//GPIO
#define DAL_GPIOA_BASEADDR DAL_AHB2_BASEADDR
#define DAL_GPIOB_BASEADDR (DAL_AHB2_BASEADDR + 0x0400)
#define DAL_GPIOC_BASEADDR (DAL_AHB2_BASEADDR + 0x0800)
#define DAL_GPIOD_BASEADDR (DAL_AHB2_BASEADDR + 0x0C00)
#define DAL_GPIOE_BASEADDR (DAL_AHB2_BASEADDR + 0x1000)
#define DAL_GPIOF_BASEADDR (DAL_AHB2_BASEADDR + 0x1400)
#define DAL_GPIOG_BASEADDR (DAL_AHB2_BASEADDR + 0x1800)
//#define DAL_GPIOG_BASEADDR (DAL_AHB2_BASEADDR + 0x1C00)

//I2C
#define DAL_I2C1_BASEAADDR (DAL_APB1_BASEADDR + 0x5400)
#define DAL_I2C2_BASEAADDR (DAL_APB1_BASEADDR + 0x5800)
#define DAL_I2C3_BASEAADDR (DAL_APB1_BASEADDR + 0x7800)
#define DAL_I2C4_BASEAADDR (DAL_APB1_BASEADDR + 0x8400)
//SPI
#define DAL_SPI1_BASEAADDR (DAL_APB2_BASEADDR + 0x3000)
#define DAL_SPI2_BASEAADDR (DAL_APB1_BASEADDR + 0x3800)
#define DAL_SPI3_BASEAADDR (DAL_APB1_BASEADDR + 0x3C00)
#define DAL_SPI4_BASEAADDR (DAL_APB2_BASEADDR + 0x3C00)
//UART
#define DAL_USART1_BASEADDR (DAL_APB2_BASEADDR + 0x3800)
#define DAL_USART2_BASEADDR (DAL_APB1_BASEADDR + 0x4400)
#define DAL_USART3_BASEADDR (DAL_APB1_BASEADDR + 0x4800)
#define DAL_UART4_BASEADDR  (DAL_APB1_BASEADDR + 0x4C00)
#define DAL_UART5_BASEADDR  (DAL_APB1_BASEADDR + 0x5000)

#define DAL_EXTI_BASEADDR   (DAL_APB2_BASEADDR + 0x0400)
#define DAL_SYSCONFIG_BASEADDR   DAL_APB2_BASEADDR
#define DAL_RCC_BASEADDR   (DAL_AHB1_BASEADDR + 0x1000)

/**
 * GPIO Port register definitions
 */
typedef struct
{
   volatile uint32_t 	MODER_reg;
   volatile uint32_t 	OTYPER_reg;
   volatile uint32_t 	OSPEEDR_reg;
   volatile uint32_t	PUPDR_reg;
   volatile uint32_t	IDR_reg;
   volatile uint32_t	ODR_reg;
   volatile uint32_t	BSSR_reg;
   volatile uint32_t	LCKR_reg;
   volatile uint32_t	AFRL_reg;
   volatile uint32_t	AFRH_reg;
   volatile uint32_t	BRR_reg;
}GPIO_Regdef_t;

/**
 * Gpio base address
 */
#define GPIOA ((GPIO_Regdef_t*)DAL_GPIOA_BASEADDR)
#define GPIOB ((GPIO_Regdef_t*)DAL_GPIOB_BASEADDR)
#define GPIOC ((GPIO_Regdef_t*)DAL_GPIOC_BASEADDR)
#define GPIOD ((GPIO_Regdef_t*)DAL_GPIOD_BASEADDR)
#define GPIOE ((GPIO_Regdef_t*)DAL_GPIOE_BASEADDR)
#define GPIOF ((GPIO_Regdef_t*)DAL_GPIOF_BASEADDR)
#define GPIOG ((GPIO_Regdef_t*)DAL_GPIOG_BASEADDR)

/**
 * RCC reg def
 */
typedef struct
{
	volatile uint32_t CR_reg;
	volatile uint32_t ICSCR_reg;
	volatile uint32_t CFGR_reg;
	volatile uint32_t PLLCFGR_reg;
	volatile uint32_t DUMMY1;
	volatile uint32_t DUMMY2;
	volatile uint32_t DUMMY3;
//	volatile uint32_t DUMMY;
	volatile uint32_t CIER_reg;
	volatile uint32_t CIFR_reg;
	volatile uint32_t CICR_reg;
	volatile uint32_t DUMMY4;
	volatile uint32_t AHB1RSTR_reg;
	volatile uint32_t AHB2RSTR_reg;
	volatile uint32_t AHB3RSTR_reg;
//	volatile uint32_t DUMMY5;
	volatile uint32_t APB1RSTR1_reg;
	volatile uint32_t APB1RSTR2_reg;
	volatile uint32_t APB2RSTR_reg;
	volatile uint32_t DUMMY6;
	volatile uint32_t AHB1ENR_reg;
	volatile uint32_t AHB2ENR_reg;
	volatile uint32_t AHB3ENR_reg;
//	volatile uint32_t DUMMY7;
	volatile uint32_t APB1ENR1_reg;
	volatile uint32_t APB1ENR2_reg;
	volatile uint32_t APB2ENR1_reg;
	volatile uint32_t DUMMY8;
	volatile uint32_t AHB1SMENR_reg;
	volatile uint32_t AHB2SMENR_reg;
	volatile uint32_t AHB3SMENR_reg;
	volatile uint32_t DUMMY9;
	volatile uint32_t APB1SMENR1_reg;
	volatile uint32_t APB1SMENR2_reg;
	volatile uint32_t APB2SMENR_reg;
	volatile uint32_t DUMMY10;
	volatile uint32_t CCIPR_reg;
	volatile uint32_t DUMMY11;
	volatile uint32_t BDCR_reg;
	volatile uint32_t CSR_reg;
	volatile uint32_t CRRCR_reg;
	volatile uint32_t CCIPR2_reg;
}RCC_Regdef_t;

/**
 * RCC base address
 */
#define RCC ((RCC_Regdef_t*)DAL_RCC_BASEADDR)

typedef struct
{
	volatile uint32_t EXTI_IMR1;
	volatile uint32_t EXTI_EMR1;
	volatile uint32_t EXTI_RSTR1;
	volatile uint32_t EXTI_FTSR1;
	volatile uint32_t EXTI_SWIER1;
	volatile uint32_t EXTI_PR1;
	volatile uint32_t EXTI_IMR2;
	volatile uint32_t EXTI_EMR2;
	volatile uint32_t EXTI_RSTR2;
	volatile uint32_t EXTI_FTSR2;
	volatile uint32_t EXTI_SWIER2;
	volatile uint32_t EXTI_PR2;
}EXTI_Regdef_t;

#define EXTI ((EXTI_Regdef_t*)DAL_EXTI_BASEADDR)

typedef struct
{
	volatile uint32_t SYSCFG_MEMRMP;
	volatile uint32_t SYSCFG_CFGR1;
	volatile uint32_t SYSCFG_EXTICR1;
	volatile uint32_t SYSCFG_EXTICR2;
	volatile uint32_t SYSCFG_EXTICR3;
	volatile uint32_t SYSCFG_EXTICR4;
	volatile uint32_t SYSCFG_SCSR;
	volatile uint32_t SYSCFG_CFGR2;
	volatile uint32_t SYSCONFIG_SWPR;
	volatile uint32_t SYSCONFIG_SKR;
}SYSCONFIG_Regdef_t;

#define SYSCONFIG ((SYSCONFIG_Regdef_t*)DAL_SYSCONFIG_BASEADDR)

typedef struct {
	volatile uint32_t SPI_CR1;
	volatile uint32_t SPI_CR2;
	volatile uint32_t SPI_SR;
	volatile uint32_t SPI_DR;
	volatile uint32_t SPI_CRCPR;
	volatile uint32_t SPI_RXCRCR;
	volatile uint32_t SPI_TXCRCR;
	volatile uint32_t SPI_I2SCFGR;
	volatile uint32_t SPI_I2SPR;
}SPI_Regdef_t;

#define SPI1 ((SPI_Regdef_t *)DAL_SPI1_BASEAADDR)
#define SPI2 ((SPI_Regdef_t *)DAL_SPI2_BASEAADDR)
#define SPI3 ((SPI_Regdef_t *)DAL_SPI3_BASEAADDR)

#define SPI1_PCLK_EN()   (RCC->APB2ENR1_reg |= (1<<12))
#define SPI2_PCLK_EN()   (RCC->APB1ENR1_reg |= (1<<14))
#define SPI3_PCLK_EN()   (RCC->APB1ENR1_reg |= (1<<15))

#define SPI1_PCLK_DI()   (RCC->APB2ENR1_reg &= ~(1<<12))
#define SPI2_PCLK_DI()   (RCC->APB1ENR1_reg &= ~(1<<14))
#define SPI3_PCLK_DI()   (RCC->APB1ENR1_reg &= ~(1<<15))

#define SPI1_RESET() do{ (RCC->APB2RSTR_reg |= (1<<12)); (RCC->APB2RSTR_reg &= ~(1<<12));}while(0)
#define SPI2_RESET() do{ (RCC->APB1RSTR_reg |= (1<<14)); (RCC->APB1RSTR_reg &= ~(1<<14));}while(0)
#define SPI3_RESET() do{ (RCC->APB1RSTR_reg |= (1<<15)); (RCC->APB1RSTR_reg &= ~(1<<15));}while(0)

#define SPI_PCLK_EN()   (RCC->APB2ENR1_reg |= (1<<14))

typedef struct {
	uint32_t I2C_CR1;
	uint32_t I2C_CR2;
	uint32_t I2C_OAR1;
	uint32_t I2C_OAR2;
	uint32_t I2C_TIMINGR;
	uint32_t I2C_TIMEOUTR;
	uint32_t I2C_ISR;
	uint32_t I2C_ICR;
	uint32_t I2C_PECR;
	uint32_t I2C_RXDR;
	uint32_t I2C_TXDR;
}I2C_Regdef_t;

/* Correct pointer cast macros */
#undef I2C1
#undef I2C2
#undef I2C3
#define I2C1	((I2C_Regdef_t*)DAL_I2C1_BASEAADDR)
#define I2C2	((I2C_Regdef_t*)DAL_I2C2_BASEAADDR)
#define I2C3	((I2C_Regdef_t*)DAL_I2C3_BASEAADDR)

/* Ensure clock macros use unsigned shifts */
#undef I2C1_PCLK_EN
#undef I2C2_PCLK_EN
#undef I2C3_PCLK_EN
#undef I2C1_PCLK_DI
#undef I2C2_PCLK_DI
#undef I2C3_PCLK_DI
#undef I2C1_PCLK_RESET
#undef I2C2_PCLK_RESET
#undef I2C3_PCLK_RESET

#define I2C1_PCLK_EN() (RCC->APB1ENR1_reg |= (1U<<21))
#define I2C2_PCLK_EN() (RCC->APB1ENR1_reg |= (1U<<22))
#define I2C3_PCLK_EN() (RCC->APB1ENR1_reg |= (1U<<30))

#define I2C1_PCLK_DI() (RCC->APB1ENR1_reg &= ~(1U<<21))
#define I2C2_PCLK_DI() (RCC->APB1ENR1_reg &= ~(1U<<22))
#define I2C3_PCLK_DI() (RCC->APB1ENR1_reg &= ~(1U<<30))

#define I2C1_PCLK_RESET() (RCC->APB1RSTR1_reg |= (1U<<21))
#define I2C2_PCLK_RESET() (RCC->APB1RSTR1_reg |= (1U<<22))
#define I2C3_PCLK_RESET() (RCC->APB1RSTR1_reg |= (1U<<30))

//bit definitons
#define I2C_CR1_PE 			0
#define I2C_CR1_TXIE 		1
#define I2C_CR1_RXIE 		2
#define I2C_CR1_ADDRIE 		3
#define I2C_CR1_NACKIE 		4
#define I2C_CR1_STOPIE 		5
#define I2C_CR1_TCIE 		6
#define I2C_CR1_EERIE 		7
#define I2C_CR1_DNF1 		8
#define I2C_CR1_DNF2 		9
#define I2C_CR1_DNF3 		10
#define I2C_CR1_DNF4 		11
#define I2C_CR1_ANFOFF 		12
#define I2C_CR1_TXDMAEN 	14
#define I2C_CR1_RXDMAEN 	15
#define I2C_CR1_SBC 		16
#define I2C_CR1_NOSTRETCH 	17
#define I2C_CR1_WUPEN   	18
#define I2C_CR1_GCEN   		19
#define I2C_CR1_SMBHEN  	20
#define I2C_CR1_SMBDEN  	21
#define I2C_CR1_ALERTEN 	22
#define I2C_CR1_PECEN   	23

#define I2C_CR2_SADD 	0
#define I2C_CR2_RD_WRN 	10
#define I2C_CR2_ADD10 	11
#define I2C_CR2_HEAD10R 12
#define I2C_CR2_START 	13
#define I2C_CR2_STOP 	14
#define I2C_CR2_NACK 	15
#define I2C_CR2_NBYTEST 16
#define I2C_CR2_RELOAD 	24
#define I2C_CR2_AUTOEND 25
#define I2C_CR2_PECBYTE 26
//OAR1 REG
#define I2C_OAR1_OA1      0
#define I2C_OAR1_OA1MODE  10
#define I2C_OAR1_OA1EN    15
//ISR REG
#define I2C_ISR_TXE		0
#define I2C_ISR_TXIS	1
#define I2C_ISR_RXNE	2
#define I2C_ISR_ADDR	3
#define I2C_ISR_NACKF	4
#define I2C_ISR_STOPF	5
#define I2C_ISR_TC		6
#define I2C_ISR_TCR		7
#define I2C_ISR_BERR	8
#define I2C_ISR_ARLO	9
#define I2C_ISR_OVR		10
#define I2C_ISR_PECERR	11
#define I2C_ISR_TIMEOUT	12
#define I2C_ISR_ALERT	13
#define I2C_ISR_BUSY	15
#define I2C_ISR_DIR		16
#define I2C_ISR_ADDCODE 17


#define I2C_ICR_ADDRCF 		3
#define I2C_ICR_NACKCF 		4
#define I2C_ICR_STOPCF 		5
#define I2C_ICR_BERRCF 		8
#define I2C_ICR_ARLOCF 		9
#define I2C_ICR_OVRCF 		10
#define I2C_ICR_PECCF 		11
#define I2C_ICR_TIMEOUTCF 	12
#define I2C_ICR_ALERTCF 	13

#define I2C_PE 0

#define I2C_READY     	0
#define I2C_BUSY_IN_TX 	1
#define I2C_BUSY_IN_RX 	2

// I2C application events
#define I2C_EV_TX_CMPLT 	0
#define I2C_EV_RX_CMPLT 	1
#define I2C_EV_STOP     	2
#define I2C_EV_TC	   		3
#define I2C_EV_ADDR_MATCH 	4
#define I2C_EV_NACK_ERR   	5
// I2C application errors
#define I2C_ERROR_BERR 		0 // Bus error
#define I2C_ERROR_ARLO 		1 // Arbitration lost
#define I2C_ERROR_OVR  		2 // Overrun/Underrun
#define I2C_ERROR_TIMEOUT 	3 // Timeout error
#define I2C_PECERR 			4 // PEC error
/**
 * GPIO CLK EN macro
 * GPIO CLK EN macro
 */
#define GPIOA_PCLK_EN() (RCC->AHB2ENR_reg |= (1<<0))
#define GPIOB_PCLK_EN() (RCC->AHB2ENR_reg |= (1<<1))
#define GPIOC_PCLK_EN() (RCC->AHB2ENR_reg |= (1<<2))
#define GPIOD_PCLK_EN() (RCC->AHB2ENR_reg |= (1<<3))
#define GPIOE_PCLK_EN() (RCC->AHB2ENR_reg |= (1<<4))
#define GPIOF_PCLK_EN() (RCC->AHB2ENR_reg |= (1<<5))
#define GPIOG_PCLK_EN() (RCC->AHB2ENR_reg |= (1<<6))

#define GPIOA_PCLK_DI() (RCC->AHB2ENR_reg &= ~(1<<0))
#define GPIOB_PCLK_DI() (RCC->AHB2ENR_reg &= ~(1<<1))
#define GPIOC_PCLK_DI() (RCC->AHB2ENR_reg &= ~(1<<2))
#define GPIOD_PCLK_DI() (RCC->AHB2ENR_reg &= ~(1<<3))
#define GPIOE_PCLK_DI() (RCC->AHB2ENR_reg &= ~(1<<4))
#define GPIOF_PCLK_DI() (RCC->AHB2ENR_reg &= ~(1<<5))
#define GPIOG_PCLK_DI() (RCC->AHB2ENR_reg &= ~(1<<6))

/**
 * GPIO Pin RESET
 */
#define GPIOA_RESET() do{ (RCC->AHB2RSTR_reg |= (1<<0)); (RCC->AHB2RSTR_reg &= ~(1<<0));}while(0)
#define GPIOB_RESET() do{ (RCC->AHB2RSTR_reg |= (1<<1)); (RCC->AHB2RSTR_reg &= ~(1<<1));}while(0)
#define GPIOC_RESET() do{ (RCC->AHB2RSTR_reg |= (1<<2)); (RCC->AHB2RSTR_reg &= ~(1<<2));}while(0)
#define GPIOD_RESET() do{ (RCC->AHB2RSTR_reg |= (1<<3)); (RCC->AHB2RSTR_reg &= ~(1<<3));}while(0)
#define GPIOE_RESET() do{ (RCC->AHB2RSTR_reg |= (1<<4)); (RCC->AHB2RSTR_reg &= ~(1<<4));}while(0)
#define GPIOF_RESET() do{ (RCC->AHB2RSTR_reg |= (1<<5)); (RCC->AHB2RSTR_reg &= ~(1<<5));}while(0)
#define GPIOG_RESET() do{ (RCC->AHB2RSTR_reg |= (1<<6)); (RCC->AHB2RSTR_reg &= ~(1<<6));}while(0)


#define SYSCONFIG_PCLK_EN() RCC->APB2ENR_reg |= ~(1<<0)
#define EXTI_PCLK_EN()      RCC->APB2ENR_reg |= ~(1<<0)

/**
 * Other useful macros
 */
#define DAL_ENABLE     1
#define DAL_DISABLE    0
#define DAL_GPIO_SET   DAL_ENABLE
#define DAL_GPIO_RESET DAL_DISABLE

#define DAL_OK         0
#define DAL_ERROR 	   1
#define DAL_BUSY 	   2
#define DAL_BUSY_IN_TX

/**
 * SPI Bit Definitions
 */
#define SPI_CR1_CPHA		0
#define SPI_CR1_CPOL		1
#define SPI_CR1_MSTR		2
#define SPI_CR1_BRR0		3
#define SPI_CR1_BRR1		4
#define SPI_CR1_BRR2		5
#define SPI_CR1_SPE			6
#define SPI_CR1_LSBFIRST	7
#define SPI_CR1_SSI			8
#define SPI_CR1_SSM			9
#define SPI_CR1_RXONLY		10
#define SPI_CR1_CRCL 		11
#define SPI_CR1_CRCNEXT		12
#define SPI_CR1_CRCEN		13
#define SPI_CR1_BIDIOE		14
#define SPI_CR1_BIDIMODE	15

#define SPI_CR2_RXDMAEN		0
#define SPI_CR2_TXDMAEN		1
#define SPI_CR2_SSOE		2
#define SPI_CR2_NSSP		3
#define SPI_CR2_FRF			4
#define SPI_CR2_ERRIE		5
#define SPI_CR2_RXNEIE		6
#define SPI_CR2_TXEIE		7
#define SPI_CR2_DS0			8
#define SPI_CR2_DS1			9
#define SPI_CR2_DS2			10
#define SPI_CR2_DS3			11
#define SPI_CR2_FRXTH		12
#define SPI_CR2_LDMA_RX		13

#define SPI_SR_RXNE		0
#define SPI_SR_TXE		0x1
#define SPI_SR_CHSIDE	0x2
#define SPI_SR_UDR		0x3
#define SPI_SR_CRCERR	0x4
#define SPI_SR_MODF		0x5
#define SPI_SR_OVR		0x6
#define SPI_SR_BSY		0x7
#define SPI_SR_FRE		0x8
#define SPI_SR_FRVL0	0x9
#define SPI_SR_FRVL1	0x10
#define SPI_SR_FTLVL0	0x11
#define SPI_SR_FTLVL1	0x12

#define SPI_READY		0
#define SPI_BUSY_IN_TX	1
#define SPI_BUSY_IN_RX	2

#define SPI_EVENT_TX_CMPLT 1
#define SPI_EVENT_RX_CMPLT 2
#define SPI_EVENT_OVR_ERR 3

#endif /* INC_DAL_STM32G4XX_H_ */
