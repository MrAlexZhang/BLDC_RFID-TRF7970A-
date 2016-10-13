/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_BLDC_Motor.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : BLDC motor implementation module
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
#define MC_BLDC_VER 

#include "MC_BLDC_Motor.h"
#include "MC_BLDC_Motor_Param.h"
#include "MC_BLDC_Drive_Param.h"
#include "MC_pid_regulators.h"
#include "MC_stm8s_clk_param.h"
#include "MC_Powerstage_Param.h"

/**** Private define **********************************************************/
#define ARR_CNT ((STM8_FREQ_MHZ * (u32)1000000)/PWM_FREQUENCY)
#define DUTY_CYCLE_CNT (ARR_CNT * DUTY_CYCLE)/100

#define _BLDC_VAR_INSTANCE 	\
	static BLDC_Var_t sBLDC_Var =			 												\
	{																													\
		(speed_control_mode_t)(SPEED_CONTROL_MODE), /* bSpeed_Control_Mode */\
		(current_control_mode_t)(CURRENT_CONTROL_MODE), /* bCurrent_Control_Mode */\
		(s16)(MEASURED_ROTOR_SPEED), /* hMeasured_rotor_speed */\
		(s16)(TARGET_ROTOR_SPEED), /*Target_rotor_speed*/\
		(u16)(DUTY_CYCLE_CNT), /* Duty_cycle */\
		(u16)(CURRENT_REFERENCE), /* Current_reference */\
		0, /* hCurrent_measured */\
		(u8)(FALLING_DELAY), /* Falling_Delay */\
		(u8)(RISING_DELAY), /* Rising_Delay */\
		(u8)(DEMAG_TIME), /* Demag_Time */											\
		(u16)(MINIMUM_OFF_TIME), /* hMinimumOffTime */\
		0, /* hBusVoltage */\
		0, /* bHeatsinkTemp */\
		FAST_DEMAG, /* bFastDemag */\
		TOGGLE_MODE, /* bToggleMode */\
		AUTO_DELAY /* bAutoDelay */\
	};

#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
#define _BLDC_CONST_INSTANCE																\
_PID_INSTANCE(SPEED)																				\
	static BLDC_Const_t sBLDC_Const =													\
	{																													\
		(u8)(MOTOR_POLE_PAIRS), /* Motor_Pole_Pairs */					\
		(u16)(MAX_SPEED_RPM), /* Max_Speed */										\
		(u16)(PWM_FREQUENCY), /* PWM_Frequency */								\
		(u8)(SPEED_PID_SAMPLING_TIME), /* Speed_PID_sampling_time */\
		&sPID_SPEED,/*  pPID_Speed */ 													\
		(u16)(CURRENT_LIMITATION), /* Current_Limitation */			\
		(u16)(0) /* hDeadTime */												\
	};
#else
	#define _BLDC_CONST_INSTANCE																\
	static BLDC_Const_t sBLDC_Const =													\
	{																													\
		(u8)(MOTOR_POLE_PAIRS), /* Motor_Pole_Pairs */					\
		(u16)(MAX_SPEED_RPM), /* Max_Speed */										\
		(u16)(PWM_FREQUENCY), /* PWM_Frequency */								\
		(u8)(SPEED_PID_SAMPLING_TIME), /* Speed_PID_sampling_time */\
		0, /* pPID_Speed */ 													\
		(u16)(CURRENT_LIMITATION), /* Current_Limitation */			\
		(u16)(0) /* hDeadTime */												\
	};
#endif

#define _BLDC_INSTANCE 																\
	/* SubFields Instances */														\
	_BLDC_VAR_INSTANCE																	\
	_BLDC_CONST_INSTANCE																\
																											\
	/* Instance of the motor 														\
		 using default values */													\
	static BLDC_Struct_t sBLDC_Struct=									\
	{																										\
	&sBLDC_Var, /*pBLDC_Var*/														\
	&sBLDC_Const /*pBLDC_Const*/												\
	};																									\
																											\
	static PBLDC_Struct_t const pBLDC_Struct = &sBLDC_Struct;	\

/**** Private instances *******************************************************/
_BLDC_INSTANCE  // Instances the drive+motor params struct

// Auto reload value used to compute duty cycle percentage
static const u16 hArrPwmVal = ((u16)((STM8_FREQ_MHZ * (u32)1000000)/PWM_FREQUENCY));

PBLDC_Struct_t Get_BLDC_Struct(void)
{
	return pBLDC_Struct;
}

PBLDC_Var_t Get_BLDC_Var(void)
{
	return sBLDC_Struct.pBLDC_Var;
}

PBLDC_Const_t Get_BLDC_Const(void)
{
	return sBLDC_Struct.pBLDC_Const;
}

s16 BLDC_Get_Target_rotor_speed(void)
{
	return sBLDC_Struct.pBLDC_Var->hTarget_rotor_speed;
}

void BLDC_Set_Target_rotor_speed(s16 val)
{
	sBLDC_Struct.pBLDC_Var->hTarget_rotor_speed = val;
}

s16 BLDC_Get_Measured_rotor_speed(void)
{
	return sBLDC_Struct.pBLDC_Var->hMeasured_rotor_speed;
}

u8  BLDC_Get_Duty_cycle(void)
{	
	return (u8)((100*(u32)(sBLDC_Struct.pBLDC_Var->hDuty_cycle))/hArrPwmVal);
}

u16  BLDC_Get_Duty_cycle_cnt(void)
{	
	return sBLDC_Struct.pBLDC_Var->hDuty_cycle;
}

void  BLDC_Set_Duty_cycle(u8 val)
{
	sBLDC_Struct.pBLDC_Var->hDuty_cycle = (u16)(((u32)(hArrPwmVal)*val)/100);
}

void  BLDC_Set_Duty_cycle_cnt(u16 val)
{
	sBLDC_Struct.pBLDC_Var->hDuty_cycle = (u16)val;
}

u16 BLDC_Get_Current_reference(void)
{
	return sBLDC_Struct.pBLDC_Var->hCurrent_reference/100;
}

void  BLDC_Set_Current_reference(u16 val)
{
	sBLDC_Struct.pBLDC_Var->hCurrent_reference = val * 100;
}

u16 BLDC_Get_Current_measured(void)
{
	return sBLDC_Struct.pBLDC_Var->hCurrent_measured;
}

void  BLDC_Set_Current_measured(u16 val)
{
	sBLDC_Struct.pBLDC_Var->hCurrent_measured = val;
}

u8  BLDC_Get_Falling_Delay(void)
{
	return sBLDC_Struct.pBLDC_Var->bFalling_Delay;
}

void  BLDC_Set_Falling_Delay(u8 val)
{
	#ifdef RISE_FALL_DELAY_LINK
		sBLDC_Struct.pBLDC_Var->bRising_Delay = val;
	#endif
	sBLDC_Struct.pBLDC_Var->bFalling_Delay = val;
}

u8 	BLDC_Get_Rising_Delay(void)
{
	return sBLDC_Struct.pBLDC_Var->bRising_Delay;
}

void 	BLDC_Set_Rising_Delay(u8 val)
{
	#ifdef RISE_FALL_DELAY_LINK
		sBLDC_Struct.pBLDC_Var->bFalling_Delay = val;
	#endif
	sBLDC_Struct.pBLDC_Var->bRising_Delay = val;
}

u8 BLDC_Get_Demag_Time(void)
{
	return sBLDC_Struct.pBLDC_Var->bDemag_Time;
}

void BLDC_Set_Demag_Time(u8 val)
{
	sBLDC_Struct.pBLDC_Var->bDemag_Time = val;
}

u16 	BLDC_Get_MinimumOffTime(void)
{
	return sBLDC_Struct.pBLDC_Var->hMinimumOffTime;
}

void 	BLDC_Set_MinimumOffTime(u16 val)
{
	sBLDC_Struct.pBLDC_Var->hMinimumOffTime = val;
}

s16  BLDC_Get_Speed_KP(void)
{
	#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
		return sBLDC_Struct.pBLDC_Const->pPID_Speed->pPID_Var->hKp_Gain;
	#else
		return 0;
	#endif
}

void  BLDC_Set_Speed_KP(s16 val)
{
	sBLDC_Struct.pBLDC_Const->pPID_Speed->pPID_Var->hKp_Gain = val;
}

s16  BLDC_Get_Speed_KI(void)
{
	#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
		return sBLDC_Struct.pBLDC_Const->pPID_Speed->pPID_Var->hKi_Gain;
	#else
		return 0;
	#endif
}

void  BLDC_Set_Speed_KI(s16 val)
{
	sBLDC_Struct.pBLDC_Const->pPID_Speed->pPID_Var->hKi_Gain = val;
}

s16  BLDC_Get_Speed_KD(void)
{
	#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
		return sBLDC_Struct.pBLDC_Const->pPID_Speed->pPID_Var->hKd_Gain;
	#else
		return 0;
	#endif
}

void  BLDC_Set_Speed_KD(s16 val)
{
	sBLDC_Struct.pBLDC_Const->pPID_Speed->pPID_Var->hKd_Gain = val;
}

s16 BLDC_Get_Bus_Voltage(void)
{
        return (s16)(sBLDC_Struct.pBLDC_Var->hBusVoltage);
}

void  BLDC_Set_Bus_Voltage(s16 BusVoltage)
{
	sBLDC_Struct.pBLDC_Var->hBusVoltage = BusVoltage;
}

u8 BLDC_Get_Heatsink_Temperature(void)
{
	return (u8)(sBLDC_Struct.pBLDC_Var->bHeatsinkTemp);
}

void BLDC_Set_Heatsink_Temperature(u8 temp)
{
	sBLDC_Struct.pBLDC_Var->bHeatsinkTemp = temp;
}

u8 BLDC_Get_FastDemag(void)
{
	return (u8)(sBLDC_Struct.pBLDC_Var->bFastDemag);
}

void BLDC_Set_FastDemag(u8 temp)
{
	#ifdef PWM_LOWSIDE_OUTPUT_ENABLE
		sBLDC_Struct.pBLDC_Var->bFastDemag = temp;
	#else
		sBLDC_Struct.pBLDC_Var->bFastDemag = 0;
	#endif
}

u8 BLDC_Get_ToggleMode(void)
{
	return (u8)(sBLDC_Struct.pBLDC_Var->bToggleMode);
}

void BLDC_Set_ToggleMode(u8 temp)
{
	sBLDC_Struct.pBLDC_Var->bToggleMode = temp;
}

u8 BLDC_Get_AutoDelay(void)
{
	return (u8)(sBLDC_Struct.pBLDC_Var->bAutoDelay);
}

void BLDC_Set_AutoDelay(u8 temp)
{
	sBLDC_Struct.pBLDC_Var->bAutoDelay = temp;
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
