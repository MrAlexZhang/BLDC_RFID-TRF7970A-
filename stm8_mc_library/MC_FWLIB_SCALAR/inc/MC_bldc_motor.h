/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_BLDC_Motor.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : BLDC motor interface
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
#ifndef __MC_BLDC_MOTOR_H
#define __MC_BLDC_MOTOR_H

#include "MC_BLDC_type.h"

#ifndef MC_BLDC_VER
#define MC_BLDC_EXT extern 
#else 
#define MC_BLDC_EXT  
#endif

MC_BLDC_EXT PBLDC_Struct_t Get_BLDC_Struct(void);
MC_BLDC_EXT PBLDC_Var_t Get_BLDC_Var(void);
MC_BLDC_EXT PBLDC_Const_t Get_BLDC_Const(void);

MC_BLDC_EXT s16 BLDC_Get_Target_rotor_speed(void);
MC_BLDC_EXT void BLDC_Set_Target_rotor_speed(s16 val);

MC_BLDC_EXT s16 BLDC_Get_Measured_rotor_speed(void);

MC_BLDC_EXT u8  BLDC_Get_Duty_cycle(void);
MC_BLDC_EXT void  BLDC_Set_Duty_cycle(u8 val);

MC_BLDC_EXT u16  BLDC_Get_Duty_cycle_cnt(void);
MC_BLDC_EXT void  BLDC_Set_Duty_cycle_cnt(u16 val);

MC_BLDC_EXT u16 BLDC_Get_Current_reference(void);
MC_BLDC_EXT void  BLDC_Set_Current_reference(u16 val);

MC_BLDC_EXT u16 BLDC_Get_Current_measured(void);
MC_BLDC_EXT void  BLDC_Set_Current_measured(u16 val);

MC_BLDC_EXT u8  BLDC_Get_Falling_Delay(void);
MC_BLDC_EXT void  BLDC_Set_Falling_Delay(u8 val);

MC_BLDC_EXT u8 	BLDC_Get_Rising_Delay(void);
MC_BLDC_EXT void 	BLDC_Set_Rising_Delay(u8 val);

MC_BLDC_EXT u8 BLDC_Get_Demag_Time(void);
MC_BLDC_EXT void BLDC_Set_Demag_Time(u8 val);

MC_BLDC_EXT u16 	BLDC_Get_MinimumOffTime(void);
MC_BLDC_EXT void 	BLDC_Set_MinimumOffTime(u16 val);

MC_BLDC_EXT s16  BLDC_Get_Speed_KP(void);
MC_BLDC_EXT void  BLDC_Set_Speed_KP(s16 val);

MC_BLDC_EXT s16  BLDC_Get_Speed_KI(void);
MC_BLDC_EXT void  BLDC_Set_Speed_KI(s16 val);

MC_BLDC_EXT s16  BLDC_Get_Speed_KD(void);
MC_BLDC_EXT void  BLDC_Set_Speed_KD(s16 val);

MC_BLDC_EXT s16 BLDC_Get_Bus_Voltage(void);
MC_BLDC_EXT void  BLDC_Set_Bus_Voltage(s16 BusVoltage);

MC_BLDC_EXT u8 BLDC_Get_Heatsink_Temperature(void);
MC_BLDC_EXT void BLDC_Set_Heatsink_Temperature(u8 temp);

MC_BLDC_EXT u8 BLDC_Get_FastDemag(void);
MC_BLDC_EXT void BLDC_Set_FastDemag(u8 temp);

MC_BLDC_EXT u8 BLDC_Get_ToggleMode(void);
MC_BLDC_EXT void BLDC_Set_ToggleMode(u8 temp);

MC_BLDC_EXT u8 BLDC_Get_AutoDelay(void);
MC_BLDC_EXT void BLDC_Set_AutoDelay(u8 temp);

#endif /* __MC_BLDC_MOTOR_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
