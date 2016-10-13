/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_ACIM_type.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : AC IND motor types and interface
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
#ifndef __MC_ACIM_TYPE_H
#define __MC_ACIM_TYPE_H

#include "MC_Type.h"
#include "MC_ACIM_conf.h"
#include "MC_ACIM_Drive_Param.h"
#include "MC_ACIM_Motor_Param.h"

typedef enum
{
  SPEED_OPENLOOP,SPEED_OPENLOOP_LOAD_COMPENSATION,SPEED_CLOSEDLOOP_VF,SPEED_CLOSEDLOOP_MTA
} control_mode_t;

#ifdef SPEED_CLOSED_LOOP
/************************ Speed closed-loop ACIM types ************************/
typedef struct
{
	control_mode_t Control_Mode;
        control_mode_t Actual_Control_Mode;
	s16 hTarget_rotor_speed_RPM;
        s16 hTarget_rotor_speed_HzEl;
        s8 bDirection;
	s16 hMeasured_rotor_speed_RPM;
	s16 hStator_voltage_ampl;
	s16 hStator_voltage_freq;
        s16 hMTPAslip;
        u16 hVFConstant;
        u16 hStartUpVFConstant;        
        s16 hStartUpSlip;
        u16 hBusVoltage;
        u8 bHeatsinkTemp;
        u16 hUserADC;
} ACIM_Var_t, *PACIM_Var_t;

typedef const struct
{
	u16 hPWM_Frequency;
        u8 bPWM_RefreshRate;
        u16 hPWM_Prescaler;
        u16 hPWM_Timer_ARR;
        u8 bPWM_Timer_MMI;
        u8 bPWM_DeadTime;
        u16 hV_Constant;
        u16 hHz_to_DPP_Conv;
	u8 bMotor_pole_Pairs;
        u8 bRPM_to_Hz_Conv;
        u16 hRPM_to_Hz_Ampl;
        u16 hDigit_to_BusV_Conv;
        u8 bNTC_alpha;
        u8 bNTC_beta;
        u8 bStartup_V0;
	s16 hMax_Speed;
	u16 hMin_run_speed;
        u16 hStall_speed;
        u16 hStartup_val_speed;
        u16 hStartup_duration;
        s16 hStartUpFinalSpeed_HzEl;
	u8 bControlLoop_Period_ms;
	PPID_Struct_t pPID_VF_Struct;
        PPID_Struct_t pPID_MTPA_Struct;
} ACIM_Const_t, *PACIM_Const_t;
#elif defined SPEED_OPEN_LOOP
/************************* Speed open-loop ACIM types ************************/
typedef struct
{
	control_mode_t Control_Mode;
	s16 hTarget_rotor_speed_RPM;
        s16 hTarget_rotor_speed_HzEl;
        s8 bDirection;
	s16 hMeasured_rotor_speed_RPM;
        s16 hActual_rotor_speed_HzEl;        
	s16 hStator_voltage_ampl;
	s16 hStator_voltage_freq;
        u16 hAccelerationSlope;
        s16 hSlip;    //open_loop_comp tuning - visualization
        u16 hVFConstant;
        u16 hStartUpVFConstant;
        s16 hStartUpSlip;
        u16 hBusVoltage;
        u8 bHeatsinkTemp;
        u16 hUserADC;
} ACIM_Var_t, *PACIM_Var_t;

typedef const struct
{
	u16 hPWM_Frequency;
        u8 bPWM_RefreshRate;
        u16 hPWM_Prescaler;
        u16 hPWM_Timer_ARR;
        u8 bPWM_Timer_MMI;
        u8 bPWM_DeadTime;
        u16 hV_Constant;
        s32 wangc[SEGMNUM+1];
        s32 wofst[SEGMNUM+1];
        s16 hSegdiv;
        u16 hHz_to_DPP_Conv;
	u8 bMotor_pole_Pairs;
        u8 bRPM_to_Hz_Conv;
        u16 hRPM_to_Hz_Ampl;
        u16 hDigit_to_BusV_Conv;
        u8 bNTC_alpha;
        u8 bNTC_beta;
        u8 bStartup_V0;
	s16 hMax_Speed;
	u16 hMin_run_speed;
        u16 hStall_speed;
        u16 hStartup_val_speed;
        u16 hStartup_duration;
        s16 hStartUpFinalSpeed_HzEl;        
	u8 bControlLoop_Period_ms;
} ACIM_Const_t, *PACIM_Const_t;
#endif

typedef const struct
{
	PACIM_Var_t pACIM_Var;
	PACIM_Const_t pACIM_Const;
} ACIM_Struct_t, *PACIM_Struct_t;

#endif /* __MC_ACIM_TYPE_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
