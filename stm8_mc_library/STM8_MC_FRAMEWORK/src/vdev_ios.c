/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : vdev_ios.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : virtual ios c file
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

#include "vdev_ios.h"
#include "vdev.h"
#include "stm8s.h"//"stm8s_lib.h"

#include "MC_ControlStage_param.h"

#ifdef DISPLAY
	#include "mc_dev_display.h"
#endif

/******************************************************************************/
errorcode vdev_fninp8(u32 addr, pu8 pportvalue)
{
	errorcode ret = VDEV_ERROR_NONE;

	switch (addr)
	{
		case VDEV_INP8_USER_INPUT:
			*pportvalue = 0;
			#ifdef JOYSTICK
				*pportvalue |= (u8)((u8)(!GPIO_ReadInputPin(KEY_RIGHT_PORT, KEY_RIGHT_BIT)) << USER_INPUT_RIGHT);
				*pportvalue |= (u8)((u8)(!GPIO_ReadInputPin(KEY_LEFT_PORT, KEY_LEFT_BIT)) << USER_INPUT_LEFT);
				*pportvalue |= (u8)((u8)(!GPIO_ReadInputPin(KEY_UP_PORT, KEY_UP_BIT)) << USER_INPUT_UP);
				*pportvalue |= (u8)((u8)(!GPIO_ReadInputPin(KEY_DOWN_PORT, KEY_DOWN_BIT)) << USER_INPUT_DOWN);
				*pportvalue |= (u8)((u8)(!GPIO_ReadInputPin(KEY_SEL_PORT, KEY_SEL_BIT)) << USER_INPUT_SEL);
			#endif
			#ifndef AUTO_START_UP
				*pportvalue |= (u8)((u8)(!GPIO_ReadInputPin(USER_BUTTON_PORT, USER_BUTTON_BIT)) << USER_INPUT_KEY);
			#endif
		break;
	}
	return ret;
}

errorcode vdev_fninp16(u32 addr, pu16 pportvalue)
{
	errorcode ret = VDEV_ERROR_NONE;

	return ret;
}

errorcode vdev_fninp32(u32 addr, pu32 pportvalue)
{
	errorcode ret = VDEV_ERROR_NONE;

	return ret;
}

errorcode vdev_fnout8(u32 addr, u8 pportvalue)
{
	errorcode ret = VDEV_ERROR_NONE;
	
	switch (addr)
	{
		case VDEV_OUT8_DISPLAY_FLUSH:
			#ifdef DISPLAY
				// Display flush
				dev_displayFlush();
			#endif
		break;
		
		case VDEV_OUT8_DISPLAY_PRINTCH:
			#ifdef DISPLAY
				// Display printch
				dev_displayPrintch();
			#endif
		break;

		case VDEV_OUT8_LED_1:
			switch (pportvalue)
			{
				case LED_ON:
					DEBUG0_PORT->ODR |= DEBUG0_PIN;
				break;
				case LED_OFF:
					DEBUG0_PORT->ODR &= (u8)(~DEBUG0_PIN);
				break;
				case LED_TOGGLE:
					DEBUG0_PORT->ODR ^= DEBUG0_PIN;
				break;
			}
		break;

		case VDEV_OUT8_LED_2:
			switch (pportvalue)
			{
				case LED_ON:
					DEBUG1_PORT->ODR |= DEBUG1_PIN;
				break;
				case LED_OFF:
					DEBUG1_PORT->ODR &= (u8)(~DEBUG1_PIN);
				break;
				case LED_TOGGLE:
					DEBUG1_PORT->ODR ^= DEBUG1_PIN;
				break;
			}
		break;

		case VDEV_OUT8_LED_3:
			switch (pportvalue)
			{
				case LED_ON:
					DEBUG2_PORT->ODR |= DEBUG2_PIN;
				break;
				case LED_OFF:
					DEBUG2_PORT->ODR &= (u8)(~DEBUG2_PIN);
				break;
				case LED_TOGGLE:
					DEBUG2_PORT->ODR ^= DEBUG2_PIN;
				break;
			}

		break;

		case VDEV_OUT8_LED_4:
			switch (pportvalue)
			{
				case LED_ON:
					DEBUG3_PORT->ODR |= DEBUG3_PIN;
				break;
				case LED_OFF:
					DEBUG3_PORT->ODR &= (u8)(~DEBUG3_PIN);
				break;
				case LED_TOGGLE:
					DEBUG3_PORT->ODR ^= DEBUG3_PIN;
				break;
			}

		break;
	}
	return ret;
}

errorcode vdev_fnout16(u32 addr, u16 pportvalue)
{
	errorcode ret = VDEV_ERROR_NONE;

	return ret;
}

errorcode vdev_fnout32(u32 addr, u32 pportvalue)
{
	errorcode ret = VDEV_ERROR_NONE;

	return ret;
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
