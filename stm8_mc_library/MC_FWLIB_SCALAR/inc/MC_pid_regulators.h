/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_pid_regulators.h
* Author             : IMS Systems Lab
* Date First Issued  : mm/dd/yyy
* Description        : Prototype definition for MC_pid_regulators.c
* Software package   : 
********************************************************************************
* History:
* 
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MC_PID_REGULATORS_H
#define __MC_PID_REGULATORS_H

#include "MC_type.h"

#define PI PI_Regulator
#define PID PID_Regulator

#define PID_REG(a,b,c) (c->pPID_Const->pFUNC)(a,b,(void *)c)

#define _PID_VAR_INSTANCE(Name)														\
	static PID_Var_t sPID_##Name##_Var =											\
	{																												\
		(s16)(##Name##_KP),																			\
		(s16)(##Name##_KI),																			\
		(s16)(##Name##_KD),																			\
		(s32)(0),	/*integral sum*/														\
		(s32)(0)	/*previous error*/													\
	};
#define _PID_CONST_INSTANCE(Name)													\
	static PID_Const_t sPID_##Name##_Const =									\
	{																												\
		&##Name##_PID_TYPE,																			\
		(u16)(##Name##_KP_DIVISOR),															\
		(u16)(##Name##_KI_DIVISOR),															\
		(u16)(##Name##_KD_DIVISOR),															\
		(s16)(##Name##_OUT_MIN),																\
		(s16)(##Name##_OUT_MAX),																\
		(s32)(##Name##_INTERM_MIN),																				\
		(s32)(##Name##_INTERM_MAX)																				\
	};
#define _PID_INSTANCE(Name)																\
_PID_VAR_INSTANCE(Name)   															\
_PID_CONST_INSTANCE(Name)																\
	static PID_Struct_t sPID_##Name =												\
	{																												\
		&sPID_##Name##_Var,																			\
		&sPID_##Name##_Const																		\
	};

s16 PI_Regulator(s16 hReference, s16 hPresentFeedback, void* PID_Struct);
s16 PID_Regulator(s16 hReference, s16 hPresentFeedback, void* PID_Struct);

#endif //__MC_PID_REGULATORS_H

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
