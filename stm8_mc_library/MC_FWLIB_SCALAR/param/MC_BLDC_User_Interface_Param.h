/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_BLDC_User_Interface_Param.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : User Interface parameters for BLDC drive
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
#ifndef __MC_BLDC_USER_INTERFACE_PARAM_H
#define __MC_BLDC_USER_INTERFACE_PARAM_H

#include "MC_ControlStage_param.h"
#include "MC_BLDC_Motor.h"
#include "MC_BLDC_Motor_Param.h"
#include "MC_BLDC_Conf.h"
#include "MC_Type.h"
#include "MC_BLDC_Drive_Param.h"

#define FIELD_NUM 2
#define FIELD_N(n) \
{ \
TAB_##n##_FIELD_0, \
TAB_##n##_FIELD_1  \
}

#define TAB(n) {FIELD_NUM,sField_##n}
#define FIELD_INSTANCE(n) Field_t sField_##n [FIELD_NUM]=FIELD_N(n)
#define _EEXTRAFIELD_INSTANCE(n) EditExtraField_t sEditExtraField_##n = n##_EXTRA

// Section modificable

#define UI_DEFAULT_TAB 3

#define TAB_NUM 12
#define TAB_N \
{ 						\
		TAB(0),		\
		TAB(1),		\
		TAB(2),		\
		TAB(3),		\
		TAB(4),		\
		TAB(5),		\
		TAB(6),		\
		TAB(7),		\
		TAB(8),		\
		TAB(9),		\
		TAB(10),	\
		TAB(11)		\
}
#define _FIELD_INSTANCE_N 	\
FIELD_INSTANCE(0);				\
FIELD_INSTANCE(1);				\
FIELD_INSTANCE(2);				\
FIELD_INSTANCE(3);				\
FIELD_INSTANCE(4);				\
FIELD_INSTANCE(5);				\
FIELD_INSTANCE(6);				\
FIELD_INSTANCE(7);				\
FIELD_INSTANCE(8);				\
FIELD_INSTANCE(9);				\
FIELD_INSTANCE(10);				\
FIELD_INSTANCE(11);	

#define _EEXTRAFIELD_INSTANCE_N 				\
_EEXTRAFIELD_INSTANCE(TAB_3_FIELD_0);		\
_EEXTRAFIELD_INSTANCE(TAB_4_FIELD_0);		\
_EEXTRAFIELD_INSTANCE(TAB_4_FIELD_1);		\
_EEXTRAFIELD_INSTANCE(TAB_5_FIELD_0);		\
_EEXTRAFIELD_INSTANCE(TAB_5_FIELD_1);		\
_EEXTRAFIELD_INSTANCE(TAB_6_FIELD_0);		\
_EEXTRAFIELD_INSTANCE(TAB_6_FIELD_1);		\
_EEXTRAFIELD_INSTANCE(TAB_7_FIELD_0);		\
_EEXTRAFIELD_INSTANCE(TAB_7_FIELD_1);		\
_EEXTRAFIELD_INSTANCE(TAB_10_FIELD_0);		\
_EEXTRAFIELD_INSTANCE(TAB_10_FIELD_1);		\
_EEXTRAFIELD_INSTANCE(TAB_11_FIELD_0);		\

#define WELCOME_MSG_LINE1 " STM8 – MC Kit "
#define WELCOME_MSG_LINE2 " BLDC  ver 1.0 "

#define TAB_0_FIELD_0 {"\x1b\x1a change tab",'',READ_ONLY,TYPE_TXT,0,0}
#define TAB_0_FIELD_1 {"\x18\x19 change field",'',READ_ONLY,TYPE_TXT,0,0}

#define TAB_1_FIELD_0 {"JoySel \x1a select",'',READ_ONLY,TYPE_TXT,0,0}
#define TAB_1_FIELD_1 {"\x18\x19 change value",'',READ_ONLY,TYPE_TXT,0,0}

#define TAB_2_FIELD_0 {"Key \x1a run motor",'',READ_ONLY,TYPE_TXT,0,0}
#define TAB_2_FIELD_1 {"              \x10",'',READ_ONLY,TYPE_TXT,0,0}

#if (!defined(SET_TARGET_SPEED_BY_POTENTIOMETER) && (defined(JOYSTICK)))
	#define SET_TARGET_SPEED_UI_TYPE EDIT
#else
	#define SET_TARGET_SPEED_UI_TYPE READ_ONLY
#endif

#define TAB_3_FIELD_0_EXTRA {&BLDC_Set_Target_rotor_speed,MAX_SPEED_RPM,-MAX_SPEED_RPM,50}
#define TAB_3_FIELD_0 {"Targ.rpm",'',SET_TARGET_SPEED_UI_TYPE,TYPE_S16_NU,&BLDC_Get_Target_rotor_speed,&sEditExtraField_TAB_3_FIELD_0}
#define TAB_3_FIELD_1 {"Meas.rpm",'',READ_ONLY,TYPE_S16_NU,&BLDC_Get_Measured_rotor_speed,0}

#define TAB_4_FIELD_0_EXTRA {&BLDC_Set_Falling_Delay,255,0,10}
#define TAB_4_FIELD_0 {"Fall Dly",' ',EDIT,TYPE_U8,&BLDC_Get_Falling_Delay,&sEditExtraField_TAB_4_FIELD_0}

#define TAB_4_FIELD_1_EXTRA {&BLDC_Set_Rising_Delay,255,0,10}
#define TAB_4_FIELD_1 {"Rise Dly",' ',EDIT,TYPE_U8,&BLDC_Get_Rising_Delay,&sEditExtraField_TAB_4_FIELD_1}

#define TAB_5_FIELD_0_EXTRA {&BLDC_Set_Current_reference,2000,0,1}
#define TAB_5_FIELD_1_EXTRA {&BLDC_Set_Duty_cycle,100,0,2}
//#define TAB_5_FIELD_1_EXTRA {&BLDC_Set_Duty_cycle_cnt,888,0,1}
//#define TAB_5_FIELD_1 {"Duty cyc",' ',EDIT,TYPE_S16,&BLDC_Get_Duty_cycle_cnt,&sEditExtraField_TAB_5_FIELD_1}
#define TAB_5_FIELD_0 {"Curr ref",'A',EDIT,TYPE_F_2_1,&BLDC_Get_Current_reference,&sEditExtraField_TAB_5_FIELD_0}
#define TAB_5_FIELD_1 {"Duty cyc",'%',EDIT,TYPE_U8,&BLDC_Get_Duty_cycle,&sEditExtraField_TAB_5_FIELD_1}

#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
	#define SPEED_PID_VARS_TYPE EDIT
#else
	#define SPEED_PID_VARS_TYPE READ_ONLY
#endif

#define TAB_6_FIELD_0_EXTRA {&BLDC_Set_Speed_KP,255,0,1}
#define TAB_6_FIELD_0 {"Speed KP",'',SPEED_PID_VARS_TYPE,TYPE_S16,&BLDC_Get_Speed_KP,&sEditExtraField_TAB_6_FIELD_0}

#define TAB_6_FIELD_1_EXTRA {&BLDC_Set_Speed_KI,255,0,1}
#define TAB_6_FIELD_1 {"Speed KI",'',SPEED_PID_VARS_TYPE,TYPE_S16,&BLDC_Get_Speed_KI,&sEditExtraField_TAB_6_FIELD_1}

#define TAB_7_FIELD_0_EXTRA {&BLDC_Set_Speed_KD,255,0,1}
#define TAB_7_FIELD_1_EXTRA {&BLDC_Set_Demag_Time,50,5,1}
#define TAB_7_FIELD_0 {"Speed KD",'',SPEED_PID_VARS_TYPE,TYPE_S16,&BLDC_Get_Speed_KD,&sEditExtraField_TAB_7_FIELD_0}
#define TAB_7_FIELD_1 {"DemagTim",'%',EDIT,TYPE_U8,&BLDC_Get_Demag_Time,&sEditExtraField_TAB_7_FIELD_1}

#define TAB_8_FIELD_0 {"Curr ref",'A',READ_ONLY,TYPE_F_2_1,&BLDC_Get_Current_reference,&sEditExtraField_TAB_7_FIELD_0}
#define TAB_8_FIELD_1 {"Curr mes",'A',READ_ONLY,TYPE_F_2_1,&BLDC_Get_Current_measured,0}

#define TAB_9_FIELD_0 {"Bus DC  ",'V',READ_ONLY,TYPE_S16,&BLDC_Get_Bus_Voltage,0}
#define TAB_9_FIELD_1 {"Heatsink",'C',READ_ONLY,TYPE_U8,&BLDC_Get_Heatsink_Temperature,0}

#define TAB_10_FIELD_0_EXTRA {&BLDC_Set_FastDemag,1,0,1}
#define TAB_10_FIELD_0 {"FastDemg",'',EDIT,TYPE_BOOL,&BLDC_Get_FastDemag,&sEditExtraField_TAB_10_FIELD_0}
#define TAB_10_FIELD_1_EXTRA {&BLDC_Set_ToggleMode,1,0,1}
#define TAB_10_FIELD_1 {"Toggle  ",'',EDIT,TYPE_BOOL,&BLDC_Get_ToggleMode,&sEditExtraField_TAB_10_FIELD_1}


#define TAB_11_FIELD_0_EXTRA {&BLDC_Set_AutoDelay,1,0,1}
#ifdef SENSORLESS
	#define TAB_11_FIELD_0 {"Auto Dly",'',EDIT,TYPE_BOOL,&BLDC_Get_AutoDelay,&sEditExtraField_TAB_11_FIELD_0}
	#define TAB_11_FIELD_1 {"",'',READ_ONLY,TYPE_TXT,0,0}
#endif
#ifdef HALL
	#define TAB_11_FIELD_0 {"Auto Dly",'',READ_ONLY,TYPE_BOOL,&BLDC_Get_AutoDelay,0}
	#define TAB_11_FIELD_1 {"",'',READ_ONLY,TYPE_TXT,0,0}
#endif

#endif /* __MC_BLDC_USER_INTERFACE_PARAM_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
