/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_ACIM_User_Interface_Param.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : User Interface parameters for ACIM drive
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
#ifndef __MC_ACIM_USER_INTERFACE_PARAM_H
#define __MC_ACIM_USER_INTERFACE_PARAM_H

#include "MC_ACIM_conf.h"
#include "MC_ACIM_Motor.h"
#include "MC_ACIM_Drive_Param.h"

#define FIELD_NUM 2
#define FIELD_N(n) \
{ \
TAB_##n##_FIELD_0, \
TAB_##n##_FIELD_1  \
}

#define TAB(n) {FIELD_NUM,sField_##n}
#define FIELD_INSTANCE(n) Field_t sField_##n [FIELD_NUM]=FIELD_N(n)
#define _EEXTRAFIELD_INSTANCE(n) EditExtraField_t sEditExtraField_##n = n##_EXTRA

#if (!defined(SET_TARGET_SPEED_BY_POTENTIOMETER) && (defined(JOYSTICK)))
	#define SET_TARGET_SPEED_UI_TYPE EDIT
#else
	#define SET_TARGET_SPEED_UI_TYPE READ_ONLY
#endif

// Section modificable
//#define LED_UI
/************************** Speed closed-loop UI ******************************/
#ifdef SPEED_CLOSED_LOOP
#ifdef CLOSEDLOOP_TUNING

#define UI_DEFAULT_TAB 1

#define TAB_NUM 8
#define TAB_N \
{ 						\
		TAB(0),		\
		TAB(1),		\
		TAB(2),		\
		TAB(3),		\
                TAB(4),         \
                TAB(5),         \
                TAB(6),         \
                TAB(7)         \
}

#define _FIELD_INSTANCE_N 	\
FIELD_INSTANCE(0);				\
FIELD_INSTANCE(1);				\
FIELD_INSTANCE(2);				\
FIELD_INSTANCE(3);                              \
FIELD_INSTANCE(4);                              \
FIELD_INSTANCE(5);                              \
FIELD_INSTANCE(6);                              \
FIELD_INSTANCE(7);

#define _EEXTRAFIELD_INSTANCE_N 				\
_EEXTRAFIELD_INSTANCE(TAB_1_FIELD_0);		\
_EEXTRAFIELD_INSTANCE(TAB_2_FIELD_0);		\
_EEXTRAFIELD_INSTANCE(TAB_2_FIELD_1);           \
_EEXTRAFIELD_INSTANCE(TAB_3_FIELD_0);		\
_EEXTRAFIELD_INSTANCE(TAB_3_FIELD_1);           \
_EEXTRAFIELD_INSTANCE(TAB_4_FIELD_0);           \
_EEXTRAFIELD_INSTANCE(TAB_4_FIELD_1);           \
_EEXTRAFIELD_INSTANCE(TAB_5_FIELD_0);           \
_EEXTRAFIELD_INSTANCE(TAB_6_FIELD_0);           \
_EEXTRAFIELD_INSTANCE(TAB_6_FIELD_1);

#define WELCOME_MSG_LINE1 " STM8 – MC Kit "
#define WELCOME_MSG_LINE2 " ACIM  ver 1.0 "

#define TAB_0_FIELD_0 {"<  > change tab",'',READ_ONLY,TYPE_TXT,0,0}
#define TAB_0_FIELD_1 {"^ v chan. focus",'',READ_ONLY,TYPE_TXT,0,0}

#define TAB_1_FIELD_0_EXTRA {&ACIM_SetTargetRotorSpeed_RPM_HzEl,15000,-15000,100}
#define TAB_1_FIELD_0 {"Targ.rpm",' ',SET_TARGET_SPEED_UI_TYPE,TYPE_S16_NU,&ACIM_GetTargetRotorSpeed_RPM,&sEditExtraField_TAB_1_FIELD_0}
#define TAB_1_FIELD_1 {"Meas.rpm",' ',READ_ONLY,TYPE_S16_NU,&ACIM_GetMeasuredRotorRpeed_RPM,0}

#define TAB_2_FIELD_0_EXTRA {&ACIM_Set_Speed_VF_KP,255,0,1}
#define TAB_2_FIELD_0 {"V/f   KP",'',EDIT,TYPE_S16,&ACIM_Get_Speed_VF_KP,&sEditExtraField_TAB_2_FIELD_0}
#define TAB_2_FIELD_1_EXTRA {&ACIM_Set_Speed_VF_KI,255,0,1}
#define TAB_2_FIELD_1 {"V/f   KI",'',EDIT,TYPE_S16,&ACIM_Get_Speed_VF_KI,&sEditExtraField_TAB_2_FIELD_1}

#define TAB_3_FIELD_0_EXTRA {&ACIM_Set_Speed_MTPA_KP,255,0,1}
#define TAB_3_FIELD_0 {"MTPA  KP",'',EDIT,TYPE_S16,&ACIM_Get_Speed_MTPA_KP,&sEditExtraField_TAB_3_FIELD_0}
#define TAB_3_FIELD_1_EXTRA {&ACIM_Set_Speed_MTPA_KI,255,0,1}
#define TAB_3_FIELD_1 {"MTPA  KI",'',EDIT,TYPE_S16,&ACIM_Get_Speed_MTPA_KI,&sEditExtraField_TAB_3_FIELD_1}

#define TAB_4_FIELD_0_EXTRA {&ACIM_Set_MTPA_Slip,255,0,1}
#define TAB_4_FIELD_0 {"MTPAslip",'',EDIT,TYPE_F_2_1,&ACIM_Get_MTPA_Slip,&sEditExtraField_TAB_4_FIELD_0}
#define TAB_4_FIELD_1_EXTRA {&ACIM_Set_Vf_Ratio,9999,0,50}
#define TAB_4_FIELD_1 {"Vf ratio",'',EDIT,TYPE_S16,&ACIM_Get_Vf_Ratio,&sEditExtraField_TAB_4_FIELD_1}

#define TAB_5_FIELD_0_EXTRA {&ACIM_Set_MTPA,1,0,1}
#define TAB_5_FIELD_0 {"MTPAMode",'',EDIT,TYPE_BOOL,&ACIM_Get_MTPA_Mode,&sEditExtraField_TAB_5_FIELD_0}
#define TAB_5_FIELD_1 {"MTPA    ",'',READ_ONLY,TYPE_BOOL,&ACIM_Get_Actual_MTPA_Mode,0}

#define TAB_6_FIELD_0_EXTRA {&ACIM_Set_Startup_Vf_Ratio,9999,0,50}
#define TAB_6_FIELD_0 {"StUpVf R",'',EDIT,TYPE_S16,&ACIM_Get_Startup_Vf_Ratio,&sEditExtraField_TAB_6_FIELD_0}
#define TAB_6_FIELD_1_EXTRA {&ACIM_Set_Startup_Slip,255,0,1}
#define TAB_6_FIELD_1 {"StUpSlip",'',EDIT,TYPE_F_2_1,&ACIM_Get_Startup_Slip,&sEditExtraField_TAB_6_FIELD_1}

#define TAB_7_FIELD_0 {"Bus DC  ",'V',READ_ONLY,TYPE_S16,&ACIM_Get_Bus_Voltage,0}
#define TAB_7_FIELD_1 {"Heatsink",'C',READ_ONLY,TYPE_U8,&ACIM_Get_HeatsinkTemp,0}

#else /*************** not defined CLOSEDLOOP_TUNING **************************/
#define UI_DEFAULT_TAB 1

#define TAB_NUM 3
#define TAB_N \
{ 						\
		TAB(0),		\
		TAB(1),		\
		TAB(2)		\
}

#define _FIELD_INSTANCE_N 	\
FIELD_INSTANCE(0);				\
FIELD_INSTANCE(1);				\
FIELD_INSTANCE(2);

#define _EEXTRAFIELD_INSTANCE_N 				\
_EEXTRAFIELD_INSTANCE(TAB_1_FIELD_0);

#define WELCOME_MSG_LINE1 " STM8 – MC Kit "
#define WELCOME_MSG_LINE2 " ACIM  ver 1.0 "

#define TAB_0_FIELD_0 {"<  > change tab",'',READ_ONLY,TYPE_TXT,0,0}
#define TAB_0_FIELD_1 {"^ v chan. focus",'',READ_ONLY,TYPE_TXT,0,0}

#define TAB_1_FIELD_0_EXTRA {&ACIM_SetTargetRotorSpeed_RPM_HzEl,15000,-15000,100}
#define TAB_1_FIELD_0 {"Targ.rpm",' ',SET_TARGET_SPEED_UI_TYPE,TYPE_S16_NU,&ACIM_GetTargetRotorSpeed_RPM,&sEditExtraField_TAB_1_FIELD_0}
#define TAB_1_FIELD_1 {"Meas.rpm",' ',READ_ONLY,TYPE_S16_NU,&ACIM_GetMeasuredRotorRpeed_RPM,0}

#define TAB_2_FIELD_0 {"Bus DC  ",'V',READ_ONLY,TYPE_S16,&ACIM_Get_Bus_Voltage,0}
#define TAB_2_FIELD_1 {"Heatsink",'C',READ_ONLY,TYPE_U8,&ACIM_Get_HeatsinkTemp,0}
#endif

#elif (defined SPEED_OPEN_LOOP)
/************************** Speed open-loop UI ********************************/
//#if (OPENLOOP_CONTROLMODE == SPEED_OPENLOOP_LOAD_COMPENSATION)
#define UI_DEFAULT_TAB 1

#ifdef SPEED_OPEN_LOOP_TACHO_SENSING
#define TAB_NUM 6
#define TAB_N \
{ 						\
                TAB(0),		\
		TAB(1),		\
                TAB(2),		\
                TAB(3),		\
                TAB(4),		\
                TAB(5)		\
}

#define _FIELD_INSTANCE_N 	\
FIELD_INSTANCE(0);				\
FIELD_INSTANCE(1);				\
FIELD_INSTANCE(2);                              \
FIELD_INSTANCE(3);                              \
FIELD_INSTANCE(4);                              \
FIELD_INSTANCE(5);
#else
#define TAB_NUM 5
#define TAB_N \
{ 						\
                TAB(0),		\
		TAB(1),		\
                TAB(2),		\
                TAB(3),		\
                TAB(4)		\
}

#define _FIELD_INSTANCE_N 	\
FIELD_INSTANCE(0);				\
FIELD_INSTANCE(1);				\
FIELD_INSTANCE(2);                              \
FIELD_INSTANCE(3);                              \
FIELD_INSTANCE(4);
#endif

#if (OPENLOOP_CONTROLMODE == SPEED_OPENLOOP_LOAD_COMPENSATION)
#define _EEXTRAFIELD_INSTANCE_N 				\
_EEXTRAFIELD_INSTANCE(TAB_1_FIELD_0);                           \
_EEXTRAFIELD_INSTANCE(TAB_2_FIELD_0);                           \
_EEXTRAFIELD_INSTANCE(TAB_2_FIELD_1);

#define TAB_3_FIELD_0 {"Vf ratio",'',READ_ONLY,TYPE_S16,&ACIM_Get_Vf_Ratio,0}
#define TAB_3_FIELD_1 {"Slip    ",'',READ_ONLY,TYPE_F_2_1,&ACIM_Get_Slip,0}

#else
#define _EEXTRAFIELD_INSTANCE_N 				\
_EEXTRAFIELD_INSTANCE(TAB_1_FIELD_0);                           \
_EEXTRAFIELD_INSTANCE(TAB_2_FIELD_0);                           \
_EEXTRAFIELD_INSTANCE(TAB_2_FIELD_1);                           \
_EEXTRAFIELD_INSTANCE(TAB_3_FIELD_0);                           \
_EEXTRAFIELD_INSTANCE(TAB_3_FIELD_1);

#define TAB_3_FIELD_0_EXTRA {&ACIM_Set_Vf_Ratio,9999,0,50}
#define TAB_3_FIELD_0 {"Vf ratio",'',EDIT,TYPE_S16,&ACIM_Get_Vf_Ratio,&sEditExtraField_TAB_3_FIELD_0}
#define TAB_3_FIELD_1_EXTRA {&ACIM_Set_Slip,255,0,1}
#define TAB_3_FIELD_1 {"Slip    ",'',EDIT,TYPE_F_2_1,&ACIM_Get_Slip,&sEditExtraField_TAB_3_FIELD_1}

#endif

#define WELCOME_MSG_LINE1 " STM8 – MC Kit "
#define WELCOME_MSG_LINE2 " ACIM  ver 1.0 "

#define TAB_0_FIELD_0 {"<  > change tab",'',READ_ONLY,TYPE_TXT,0,0}
#define TAB_0_FIELD_1 {"^ v chan. focus",'',READ_ONLY,TYPE_TXT,0,0}

#define TAB_1_FIELD_0_EXTRA {&ACIM_SetTargetRotorSpeed_RPM_HzEl,15000,-15000,200}
#define TAB_1_FIELD_0 {"Targ.rpm",' ',SET_TARGET_SPEED_UI_TYPE,TYPE_S16_NU,&ACIM_GetTargetRotorSpeed_RPM,&sEditExtraField_TAB_1_FIELD_0}
#define TAB_1_FIELD_1 {"Exp. rpm",' ',READ_ONLY,TYPE_S16_NU,&ACIM_GetActualRotorRpeed_RPM,0}

#define TAB_2_FIELD_0_EXTRA {&ACIM_Set_Startup_Vf_Ratio,9999,0,50}
#define TAB_2_FIELD_0 {"StUpVf R",'',EDIT,TYPE_S16,&ACIM_Get_Startup_Vf_Ratio,&sEditExtraField_TAB_2_FIELD_0}
#define TAB_2_FIELD_1_EXTRA {&ACIM_Set_Startup_Slip,255,0,1}
#define TAB_2_FIELD_1 {"StUpSlip",'',EDIT,TYPE_F_2_1,&ACIM_Get_Startup_Slip,&sEditExtraField_TAB_2_FIELD_1}

#define TAB_4_FIELD_0 {"Bus DC  ",'V',READ_ONLY,TYPE_S16,&ACIM_Get_Bus_Voltage,0}
#define TAB_4_FIELD_1 {"Heatsink",'C',READ_ONLY,TYPE_U8,&ACIM_Get_HeatsinkTemp,0}

#ifdef SPEED_OPEN_LOOP_TACHO_SENSING
#define TAB_5_FIELD_0 {"Exp. rpm",' ',READ_ONLY,TYPE_S16_NU,&ACIM_GetActualRotorRpeed_RPM,0}
#define TAB_5_FIELD_1 {"Meas.rpm",' ',READ_ONLY,TYPE_S16_NU,&ACIM_GetMeasuredRotorRpeed_RPM,0}
#endif

//#endif
#endif

#endif /* __MC_ACIM_USER_INTERFACE_PARAM_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
