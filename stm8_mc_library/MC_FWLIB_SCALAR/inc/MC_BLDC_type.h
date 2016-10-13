/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : BLDC_type.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : BLDC types
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
#ifndef __BLDC_TYPE_H
#define __BLDC_TYPE_H

#include "MC_type.h"

#define VOLTAGE_MODE 0
#define CURRENT_MODE 1

typedef u8 current_control_mode_t;

typedef struct
{
	speed_control_mode_t bSpeed_Control_Mode;
	current_control_mode_t bCurrent_Control_Mode;
	s16 hMeasured_rotor_speed;
	s16 hTarget_rotor_speed;
	u16 hDuty_cycle;
	u16 hCurrent_reference;
	u16 hCurrent_measured;
	u8 bFalling_Delay;
	u8 bRising_Delay;
	u8 bDemag_Time;
	u16 hMinimumOffTime;
	u16 hBusVoltage;
	u8	bHeatsinkTemp;
	u8  bFastDemag;
	u8  bToggleMode;
	u8  bAutoDelay;
} BLDC_Var_t, *PBLDC_Var_t;

typedef const struct
{
	u8 bMotor_Pole_Pairs;
	u16 hMax_Speed;
	u16 hPWM_Frequency;
	u8 bSpeed_PID_sampling_time;
	PPID_Struct_t pPID_Speed;
	u16 hCurrent_Limitation;
	u16 hDeadTime;
} BLDC_Const_t, *PBLDC_Const_t;

typedef const struct
{
	PBLDC_Var_t pBLDC_Var;
	PBLDC_Const_t pBLDC_Const;
} BLDC_Struct_t, *PBLDC_Struct_t;

#endif /* __BLDC_TYPE_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
