/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_stm8s_port_param.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : 
********************************************************************************
* History:
* mm/dd/yyyy ver. x.y.z
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/******************************************************************************/
#ifndef __MC_PORT_PARAM_H
#define __MC_PORT_PARAM_H

#include "MC_ControlStage_param.h"
#include "MC_PowerStage_param.h"

#define MCO1_PORT GPIOC
#define MCO1_PIN	GPIO_PIN_1
#define MCO3_PORT GPIOC
#define MCO3_PIN	GPIO_PIN_2
#define MCO5_PORT GPIOC
#define MCO5_PIN	GPIO_PIN_3
#define MCO6_PORT GPIOE
#define MCO6_PIN	GPIO_PIN_3

#ifndef TIM1_CHxN_REMAP
	#define MCO0_PORT GPIOH
	#define MCO0_PIN	GPIO_PIN_7
	#define MCO2_PORT GPIOH
	#define MCO2_PIN	GPIO_PIN_6
	#define MCO4_PORT GPIOH
	#define MCO4_PIN	GPIO_PIN_5
#else
	#define MCO0_PORT GPIOB
	#define MCO0_PIN	GPIO_PIN_0
	#define MCO2_PORT GPIOB
	#define MCO2_PIN	GPIO_PIN_1
	#define MCO4_PORT GPIOB
	#define MCO4_PIN	GPIO_PIN_2
#endif

#define DEBUG0_PORT GPIOH
#define DEBUG0_PIN GPIO_PIN_0
#define DEBUG1_PORT GPIOH
#define DEBUG1_PIN GPIO_PIN_1
#define DEBUG2_PORT GPIOH
#define DEBUG2_PIN GPIO_PIN_2
#define DEBUG3_PORT GPIOH
#define DEBUG3_PIN GPIO_PIN_3

// User interface configuration port/pin
#define KEY_UP_PORT GPIOB
#define KEY_UP_BIT  GPIO_PIN_6

#define KEY_DOWN_PORT GPIOB
#define KEY_DOWN_BIT  GPIO_PIN_7

#define KEY_RIGHT_PORT GPIOB
#define KEY_RIGHT_BIT  GPIO_PIN_5

#define KEY_LEFT_PORT  GPIOB
#define KEY_LEFT_BIT   GPIO_PIN_4

#define KEY_SEL_PORT   GPIOD
#define KEY_SEL_BIT    GPIO_PIN_7

#define USER_BUTTON_PORT GPIOC
#define USER_BUTTON_BIT  GPIO_PIN_0

/* LCD Chip Select I/O definition */
#define LCD_CS_PORT (GPIOF)
#define LCD_CS_PIN  (GPIO_PIN_0)

// Brake command
#define DISSIPATIVE_BRAKE_PORT GPIOD
#define DISSIPATIVE_BRAKE_BIT GPIO_PIN_0

#endif /* __MC_PORT_PARAM_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/