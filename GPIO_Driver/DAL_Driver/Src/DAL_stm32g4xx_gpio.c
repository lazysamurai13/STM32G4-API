/**
 * Dal_stm32g4xx.c
 *
 *  Created on: Jun 17, 2025
 *      Author: dawood
 */

#include "DAL_stm32g4xx_gpio.h"

/**
 * Function definitions
 */

/**
 * @fn void DAL_GPIO_Peri_CLK(GPIO_Regdef_t*, uint8_t)
 * @brief To initialize peripheral clk
 *
 * @pre
 * @post
 * @param pGPIOx
 * @param EnOrDi
 */
void DAL_GPIO_Peri_CLK(GPIO_Regdef_t *pGPIOx, uint8_t EnOrDi)
{
	if(EnOrDi == DAL_ENABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_EN();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_EN();
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_EN();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_EN();
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_EN();
		}
		else
		{
			GPIOG_PCLK_EN();
		}
	}
	else
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_DI();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_DI();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_DI();
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_DI();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_DI();
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_DI();
		}
		else
		{
			GPIOG_PCLK_DI();
		}
	}
}

/**
 * @fn void DAL_GPIO_Init(GPIO_handle_t*)
 * @brief Initialize GPIO Pin
 *
 * @pre
 * @post
 * @param pGPIO_Handle
 */
void DAL_GPIO_Init(GPIO_handle_t *pGPIO_Handle)
{
	uint32_t temp=0;
	//1. Enable CLock
	DAL_GPIO_Peri_CLK(pGPIO_Handle->pGPIOx, DAL_ENABLE);
	//2. Set pin mode
	if(pGPIO_Handle->pGPIO_Pinconfig.Pin_mode < DAL_GPIO_MODE_IT_FE)
	{
		//clear the bits
		temp = 0x3 << (2 * pGPIO_Handle->pGPIO_Pinconfig.Pin_number);
		pGPIO_Handle->pGPIOx->MODER_reg &= ~temp;
		temp =0;
		//set the bits
		temp = pGPIO_Handle->pGPIO_Pinconfig.Pin_mode << ( 2 * pGPIO_Handle->pGPIO_Pinconfig.Pin_number);
//		pGPIO_Handle->pGPIOx->MODER_reg |= (pGPIO_Handle->pGPIO_Pinconfig.Pin_mode << ( 2 * pGPIO_Handle->pGPIO_Pinconfig.Pin_number));
		pGPIO_Handle->pGPIOx->MODER_reg |= temp;
		temp=0;
	}
	else
	{
		//TODO
		//1.put GPIO in input mode

		if(pGPIO_Handle->pGPIO_Pinconfig.Pin_mode == DAL_GPIO_MODE_IT_FE)
		{
			//1. Set interrupt in SYSCONFIG

			//2. Set interrupt in NVIC
		}
		else if (pGPIO_Handle->pGPIO_Pinconfig.Pin_mode == DAL_GPIO_MODE_IT_RE)
		{

		}
		else
		{

		}
	}
	//3. Set pin OP type
	temp = 0x3 << (2 * pGPIO_Handle->pGPIO_Pinconfig.Pin_number);
	pGPIO_Handle->pGPIOx->OTYPER_reg &= ~temp;
	temp = 0;
	temp = pGPIO_Handle->pGPIO_Pinconfig.Pin_OP_type << (pGPIO_Handle->pGPIO_Pinconfig.Pin_number);
	pGPIO_Handle->pGPIOx->OTYPER_reg |=temp;
	temp = 0;
	//4. Set Speed
	temp = 0x3 << (2 * pGPIO_Handle->pGPIO_Pinconfig.Pin_number);
	pGPIO_Handle->pGPIOx->OSPEEDR_reg &= ~temp;
	temp=0;
	temp = pGPIO_Handle->pGPIO_Pinconfig.Pin_speed << (2 * pGPIO_Handle->pGPIO_Pinconfig.Pin_number);
	pGPIO_Handle->pGPIOx->OSPEEDR_reg |=temp;
	temp=0;
	//5. Set PuPd
	temp = 0x3 << (2 * pGPIO_Handle->pGPIO_Pinconfig.Pin_number);
	pGPIO_Handle->pGPIOx->PUPDR_reg &= ~temp;
	temp=0;
	temp = pGPIO_Handle->pGPIO_Pinconfig.Pin_PuPd << (2 * pGPIO_Handle->pGPIO_Pinconfig.Pin_number);
	pGPIO_Handle->pGPIOx->PUPDR_reg |=temp;
	temp=0;

	//6. Set Alt fn if needed
	if(pGPIO_Handle->pGPIO_Pinconfig.Pin_mode == DAL_GPIO_MODE_ALT_FN)
	{
		uint8_t temp2 = pGPIO_Handle->pGPIO_Pinconfig.Pin_Alt_fn % 8;
		if(temp2 > 1)
		{
			temp = 0xF << (4 * temp2);
			pGPIO_Handle->pGPIOx->AFRL_reg &= ~temp;
			temp=0;
			temp = pGPIO_Handle->pGPIO_Pinconfig.Pin_speed << (4 * temp2);
			pGPIO_Handle->pGPIOx->AFRL_reg |=temp;
			temp=0;
		}
		else
		{
			temp = 0xF << (4 * temp2);
			pGPIO_Handle->pGPIOx->AFRH_reg &= ~temp;
			temp=0;
			temp = pGPIO_Handle->pGPIO_Pinconfig.Pin_speed << (4 * temp2);
			pGPIO_Handle->pGPIOx->AFRH_reg |=temp;
			temp=0;
		}
	}
}

/**
 * @fn void DAL_GPIO_Deinit(GPIO_Regdef_t*)
 * @brief
 *
 * @pre
 * @post
 * @param pGPIOx
 */
void DAL_GPIO_Deinit(GPIO_Regdef_t *pGPIOx)
{
	if(pGPIOx == GPIOA)
	{
		GPIOA_RESET();
	}
	else if(pGPIOx == GPIOB)
	{
		GPIOB_RESET();
	}
	else if(pGPIOx == GPIOC)
	{
		GPIOC_RESET();
	}
	else if(pGPIOx == GPIOD)
	{
		GPIOD_RESET();
	}
	else if(pGPIOx == GPIOE)
	{
		GPIOE_RESET();
	}
	else if(pGPIOx == GPIOF)
	{
		GPIOF_RESET();
	}
	else
	{
		GPIOG_RESET();
	}
}

/**
 * @fn uint8_t DAL_GPIO_Read_Pin(GPIO_handle_t*)
 * @brief
 * @pre
 * @post
 * @param pGPIO_handle
 * @return
 */
uint8_t DAL_GPIO_Read_Pin(GPIO_handle_t *pGPIO_handle)
{
	uint8_t temp=0;
	return temp = (pGPIO_handle->pGPIOx->IDR_reg >> pGPIO_handle->pGPIO_Pinconfig.Pin_number);
}

/**
 * @fn uint16_t DAL_GPIO_Read_Port(GPIO_Regdef_t*)
 * @brief
 *
 * @pre
 * @post
 * @param pGPIOx
 * @return
 */
uint16_t DAL_GPIO_Read_Port(GPIO_Regdef_t *pGPIOx)
{
	uint16_t temp=0;
	return temp = pGPIOx->IDR_reg;
}

/**
 * @fn void DAL_GPIO_Write_Port(GPIO_Regdef_t*, uint16_t)
 * @brief
 *
 * @pre
 * @post
 * @param pGPIOx
 * @param value
 */
void DAL_GPIO_Write_Port(GPIO_Regdef_t *pGPIOx, uint16_t value)
{
	pGPIOx->ODR_reg |= value;
}

/**
 * @fn void DAL_GPIO_Write_Pin(GPIO_handle_t*, uint8_t)
 * @brief
 *
 * @pre
 * @post
 * @param pGPIO_Handle
 * @param value
 */
void DAL_GPIO_Write_Pin(GPIO_handle_t *pGPIO_Handle, uint8_t value)
{
	pGPIO_Handle->pGPIOx->ODR_reg |= (value << pGPIO_Handle->pGPIO_Pinconfig.Pin_number);
}

/**
 * @fn void DAL_GPIO_Toggle_Pin(GPIO_handle_t*)
 * @brief
 *
 * @pre
 * @post
 * @param pGPIO_Handle
 */
void DAL_GPIO_Toggle_Pin(GPIO_handle_t *pGPIO_Handle)
{
	pGPIO_Handle->pGPIOx->ODR_reg ^= (1 << pGPIO_Handle->pGPIO_Pinconfig.Pin_number);
}

