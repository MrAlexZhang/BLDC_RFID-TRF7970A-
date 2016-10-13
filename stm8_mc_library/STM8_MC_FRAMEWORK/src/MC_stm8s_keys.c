/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_stm8s_keys.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Low level management of Joystick and Push button
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
#include "MC_dev_keys.h"

#include "stm8s.h"//"stm8s_lib.h"

#include "MC_ControlStage_param.h"
#include "MC_stm8s_port_param.h"


void dev_keysInit(void)
{
	#ifdef JOYSTICK
		/* Joystick GPIOs configuration*/
		GPIO_Init(KEY_UP_PORT, KEY_UP_BIT, GPIO_MODE_IN_FL_NO_IT);
		GPIO_Init(KEY_DOWN_PORT, KEY_DOWN_BIT, GPIO_MODE_IN_FL_NO_IT);
		GPIO_Init(KEY_RIGHT_PORT, KEY_RIGHT_BIT, GPIO_MODE_IN_FL_NO_IT);
		GPIO_Init(KEY_LEFT_PORT, KEY_LEFT_BIT, GPIO_MODE_IN_FL_NO_IT);
		GPIO_Init(KEY_SEL_PORT, KEY_SEL_BIT, GPIO_MODE_IN_FL_NO_IT);
	#endif

	#ifndef AUTO_START_UP
		/* User button GPIO configuration */
		GPIO_Init(USER_BUTTON_PORT, USER_BUTTON_BIT, GPIO_MODE_IN_FL_NO_IT);
	#endif
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
