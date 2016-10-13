/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : ACIM_Motor.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : ACIM motor interface
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
#ifndef __ACIM_MOTOR_H
#define __ACIM_MOTOR_H

#include "MC_ACIM_type.h"

PACIM_Struct_t Get_ACIM_Struct(void);

void ACIM_SetTargetRotorSpeed_RPM_HzEl(s16 val);
s16 ACIM_GetTargetRotorSpeed_RPM(void);
s16 ACIM_GetTargetRotorSpeed_HzEl(void);
s16 ACIM_GetMeasuredRotorRpeed_RPM(void);
void ACIM_SetMeasuredRotorSpeed_RPM(s16 val);
s16  ACIM_Get_Startup_Slip(void);
void  ACIM_Set_Startup_Slip(s16 val);
s16 ACIM_Get_Bus_Voltage(void);
void  ACIM_Set_Bus_Voltage(s16 BusVoltage);
void ACIM_Set_HeatsinkTemp(u8 bTemperature);
u8 ACIM_Get_HeatsinkTemp(void);
void ACIM_Set_Vf_Ratio(s16 val);
s16 ACIM_Get_Vf_Ratio(void);
void ACIM_Set_Startup_Vf_Ratio(s16 val);
s16 ACIM_Get_Startup_Vf_Ratio(void);

#ifdef SPEED_CLOSED_LOOP
s16  ACIM_Get_Speed_VF_KP(void);
void  ACIM_Set_Speed_VF_KP(s16 val);
s16  ACIM_Get_Speed_VF_KI(void);
void  ACIM_Set_Speed_VF_KI(s16 val);
s16  ACIM_Get_Speed_MTPA_KP(void);
void  ACIM_Set_Speed_MTPA_KP(s16 val);
s16  ACIM_Get_Speed_MTPA_KI(void);
void  ACIM_Set_Speed_MTPA_KI(s16 val);
void ACIM_Set_MTPA(u8 val);
u8 ACIM_Get_MTPA_Mode(void);
u8 ACIM_Get_Actual_MTPA_Mode(void);
s16  ACIM_Get_MTPA_Slip(void);
void  ACIM_Set_MTPA_Slip(s16 val);

#elif (defined SPEED_OPEN_LOOP)
s16 ACIM_GetActualRotorRpeed_RPM(void);
void  ACIM_Set_Slip(s16 val);
s16  ACIM_Get_Slip(void);
#endif

#endif /* __ACIM_MOTOR_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
