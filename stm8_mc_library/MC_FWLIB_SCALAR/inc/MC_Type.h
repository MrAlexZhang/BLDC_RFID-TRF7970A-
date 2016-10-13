/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_Type.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Motor Control Common Type
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
#ifndef __MC_TYPE_H
#define __MC_TYPE_H

#include "dev_type.h"

typedef enum
{
	OPEN_LOOP, CLOSED_LOOP
} speed_control_mode_t;

typedef enum
{FUNCTION_RUNNING,FUNCTION_ENDED,FUNCTION_ERROR} MC_FuncRetVal_t;

typedef const struct
{
	s16 (*pFUNC)(s16,s16,void*);
	u16 hKp_Divisor;
	u16 hKi_Divisor;
	u16 hKd_Divisor;
  s16 hLower_Limit_Output;     //Lower Limit for Output limitation
  s16 hUpper_Limit_Output;     //Lower Limit for Output limitation
  s32 wLower_Limit_Integral;   //Lower Limit for Integral term limitation
  s32 wUpper_Limit_Integral;   //Lower Limit for Integral term limitation
} PID_Const_t, *PPID_Const_t;

typedef struct
{
	s16 hKp_Gain;
  s16 hKi_Gain;
	s16 hKd_Gain;
  s32 wIntegral;  
  s32 wPreviousError;
} PID_Var_t, *PPID_Var_t;

typedef const struct
{
	PPID_Var_t pPID_Var;
	PPID_Const_t pPID_Const;
} PID_Struct_t, *PPID_Struct_t;

#endif /* __MC_TYPE_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
