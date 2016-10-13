/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : BLDC_timers.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : BLDC timers
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
#ifndef __BLDC_TIMERS_H
#define __BLDC_TIMERS_H

// Common
#define VTIM_KEY						VTIM0
#define VTIM_DISPLAY_BLINK				VTIM1
#define VTIM_DISPLAY_REFRESH			VTIM2
#define VTIM_USER_INTERFACE_REFRESH		VTIM3
#define BLDC_CONTROL_TIMER 				VTIM4
#define DEV_DUTY_UPDATE_TIMER			VTIM5
#define MTC_ALIGN_RAMP_TIMER 			VTIM6
#define MTC_ALIGN_TIMER					VTIM7
#define ADC_SAMPLE_TIMER				VTIM8
#define HALL_CAPT_TIMEOUT_TIMER			VTIM9

#endif //__BLDC_TIMERS_H

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/