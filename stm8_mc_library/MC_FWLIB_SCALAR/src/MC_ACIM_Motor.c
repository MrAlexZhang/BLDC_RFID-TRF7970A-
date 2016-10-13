/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_ACIM_Motor.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : ACIM motor implementation module
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
/* Includes ------------------------------------------------------------------*/
#include "MC_ACIM_conf.h"
#include "MC_ACIM_Motor.h"
#include "MC_ACIM_Motor_Param.h"
#include "MC_ACIM_Drive_Param.h"
#include "MC_tacho_param.h"
#include "MC_pid_regulators.h"
#include "MC_PowerStage_Param.h"
#include "MC_ControlStage_param.h"

/* Private define ------------------------------------------------------------*/
//specific settings for stm8s

#if (STM8_FREQ_MHZ == 24)
  #if (PWM_FREQUENCY > 23437)
    #define TIM1_PRESCALER 0        
  #elif (PWM_FREQUENCY > 11718)
    #define TIM1_PRESCALER 1
  #elif (PWM_FREQUENCY > 7812)
    #define TIM1_PRESCALER 2    
  #elif (PWM_FREQUENCY > 5859)
    #define TIM1_PRESCALER 3    
  #endif
#elif (STM8_FREQ_MHZ == 16)
  #if (PWM_FREQUENCY > 15626)
    #define TIM1_PRESCALER 0    
  #elif (PWM_FREQUENCY > 7812)
    #define TIM1_PRESCALER 1      
  #elif (PWM_FREQUENCY > 5209)
    #define TIM1_PRESCALER 2     
  #elif (PWM_FREQUENCY > 3907)
    #define TIM1_PRESCALER 3    
  #endif
#endif

#define VOUT_RESOLUTION 256
#define VBUS_RESOLUTION 256

#define TIM1_COUNTS (u16)(((STM8_FREQ_MHZ*1000000.0)/(TIM1_PRESCALER+1))/PWM_FREQUENCY)
#define TIM1_ARR (u16)(TIM1_COUNTS/2)
#define TIM1_MMI (u8)(TIM1_COUNTS/4)

#define DEAD_TIME1 ((DEAD_TIME_NS * 1.0 * STM8_FREQ_MHZ)/1000)
#define DEAD_TIME2 ((DEAD_TIME_NS * 1.0 * STM8_FREQ_MHZ)/(2*1000)+0x80-64)
#define DEAD_TIME3 ((DEAD_TIME_NS * 1.0 * STM8_FREQ_MHZ)/(8*1000)+0xC0-32)
#define DEAD_TIME4 ((DEAD_TIME_NS * 1.0 * STM8_FREQ_MHZ)/(16*1000)+0xE0-32)

#define DEAD_TIME DEAD_TIME1
#if (DEAD_TIME >= 0x80)
#undef DEAD_TIME
#define DEAD_TIME DEAD_TIME2
#endif
#if (DEAD_TIME >= 0xC0)
#undef DEAD_TIME
#define DEAD_TIME DEAD_TIME3
#endif
#if (DEAD_TIME >= 0xE0)
#undef DEAD_TIME
#define DEAD_TIME DEAD_TIME4
#endif

#if (DEAD_TIME < 2)
#warning "Dead time resolution may be too low"
#endif

#define V_CONSTANT ((1.0*VOUT_RESOLUTION*VBUS_RESOLUTION*BUS2PHASECONST)/(1.0*BUSV_CONVERSION))

#define HZ_TO_DPP_CONSTANT ((65536.0*256*PWM_REFRESH_RATE)/(10.0*PWM_FREQUENCY))

#define VOUT_AMPL_FACT 2.0
#define VOUT_CONSTANT_A ((1.0*VOUT_RESOLUTION*VBUS_RESOLUTION*BUS2PHASECONST*VOUT_AMPL_FACT)/(10.0*BUSV_CONVERSION))
#define VOUT_CONSTANT_B (1.0*V_F_RATIO*256)
#define V_F_CONSTANT ((1.0*VOUT_CONSTANT_A*VOUT_CONSTANT_B)/(1.0*VOUT_AMPL_FACT))

#define VOUT_CONSTANT_C (1.0*STARTUP_V_F_RATIO*256)
#define STARTUP_V_F_CONSTANT ((1.0*VOUT_CONSTANT_A*VOUT_CONSTANT_C)/(1.0*VOUT_AMPL_FACT))

#define ACCELERATION_SLOPE_CONSTANT ((1.0*OPEN_LOOP_ACCELERATION_SLOPE*MOTOR_POLE_PAIRS*10.0*CONTROL_LOOP_PERIOD*1024)/(60.0*1000))

/**** Private define **********************************************************/
#ifdef SPEED_CLOSED_LOOP
/************ Speed closed-loop ACIM structures initialization ****************/
#define _ACIM_VAR_INSTANCE                              		\
	static ACIM_Var_t sACIM_Var =					\
	{				          			\
		(speed_control_mode_t)(CLOSEDLOOP_CONTROLMODE),         \
                (speed_control_mode_t)(CLOSEDLOOP_CONTROLMODE),         \
		(s16)(TARGET_ROTOR_SPEED), /*Target_rotor_speed*/	\
                (s16)((((s32)TARGET_ROTOR_SPEED)*RPM_TO_HZ_CONV)/RPM_TO_HZ_AMPL), /*Target rotor speed Hz mec */                \
                (s8)(0), /* direction */  \
                (s16)(0), /* measured rotor speed RPM*/		        \
                (s16)(0), /* applied stator voltage amplitude*/	        \
		(s16)(0), /* applied stator voltage frequency*/	        \
                (s16)(MTPA_SLIP), /* MTPA optimum slip      */	        \
                (u16)(V_F_CONSTANT), \
                (u16)(STARTUP_V_F_CONSTANT),\
                (s16)(STARTUP_SLIP), \
                (u16)(0), /*Bus Voltage Amplitude */                    \
                (u8)(0), /*Heatsink Temp */                             \
                (u16)(0)                                                \
	};

#define _ACIM_CONST_INSTANCE																\
_PID_INSTANCE(VF)							\
_PID_INSTANCE(MTPA)                                                     \
	static ACIM_Const_t sACIM_Const =                               \
	{								\
		(u16)(PWM_FREQUENCY),					\
                (u8)(PWM_REFRESH_RATE),                                 \
                (u16)(TIM1_PRESCALER),                                  \
                (u16)(TIM1_ARR),                                        \
                (u8)(TIM1_MMI),                                         \
                (u8)(DEAD_TIME),                                        \
                (u16)(V_CONSTANT),                                      \
                (u16)(HZ_TO_DPP_CONSTANT),                              \
		(u8)(MOTOR_POLE_PAIRS),					\
                (u8)(RPM_TO_HZ_CONV),                                   \
                (u16)(RPM_TO_HZ_AMPL),                                  \
                (u16)(BUSV_CONVERSION),                                 \
                (u8)(TEMP_SENS_ALPHA),                                  \
                (u8)(TEMP_SENS_BETA),                                   \
                (u8)(STARTUP_V0),                                       \
		(u16)(MAX_SPEED_RPM),					\
		(u16)(MIN_RUN_SPEED/6),                         	\
                (u16)(STALL_SPEED/6),                                   \
                (u16)(STARTUP_VAL_SPEED/6),                             \
                (u16)(STARTUP_DURATION),                                \
                (s16)((((s32)STARTUP_VAL_SPEED)*RPM_TO_HZ_CONV)/RPM_TO_HZ_AMPL), \
                (u8)(CONTROL_LOOP_PERIOD),				\
		&sPID_VF,						\
		&sPID_MTPA                                              \
	};

#elif defined SPEED_OPEN_LOOP
/************** Speed open-loop ACIM structures initialization ****************/
#define _ACIM_VAR_INSTANCE                              		\
	static ACIM_Var_t sACIM_Var =					\
	{				          			\
		(speed_control_mode_t)(OPENLOOP_CONTROLMODE),           \
                (s16)(TARGET_ROTOR_SPEED), /*Target_rotor_speed*/	\
                (s16)((((s32)TARGET_ROTOR_SPEED)*RPM_TO_HZ_CONV)/RPM_TO_HZ_AMPL), /*Target rotor speed Hz mec */                \
                (s8)(0), /* direction */  \
                (s16)(0), /* measured rotor speed RPM*/		        \
                (s16)(0), /* actual rotor speed HzEl */                 \
                (s16)(0), /* applied stator voltage amplitude*/	        \
		(s16)(0), /* applied stator voltage frequency*/	        \
                (u16)(ACCELERATION_SLOPE_CONSTANT), /* acc.slope */     \
                (s16)(OPENLOOP_SLIP), /* applied slip */    \
                (u16)(V_F_CONSTANT), \
                (u16)(STARTUP_V_F_CONSTANT),\
                (s16)(STARTUP_SLIP), \
                (u16)(0), /*Bus Voltage Amplitude */                    \
                (u8)(0), /*Heatsink Temp */                             \
                (u16)(0) /*user adc*/                                   \
	};

#define _ACIM_CONST_INSTANCE						\
	static ACIM_Const_t sACIM_Const =                               \
	{								\
		(u16)(PWM_FREQUENCY),					\
                (u8)(PWM_REFRESH_RATE),                                 \
                (u16)(TIM1_PRESCALER),                                  \
                (u16)(TIM1_ARR),                                        \
                (u8)(TIM1_MMI),                                         \
                (u8)(DEAD_TIME),                                        \
                (u16)(V_CONSTANT),                                      \
                ANGC,                                                   \
                OFST,                                                   \
                (s16)(SEGDIV),                                          \
                (u16)(HZ_TO_DPP_CONSTANT),                              \
		(u8)(MOTOR_POLE_PAIRS),					\
                (u8)(RPM_TO_HZ_CONV),                                   \
                (u16)(RPM_TO_HZ_AMPL),                                  \
                (u16)(BUSV_CONVERSION),                                 \
                (u8)(TEMP_SENS_ALPHA),                                  \
                (u8)(TEMP_SENS_BETA),                                   \
                (u8)(STARTUP_V0),                                       \
		(u16)(MAX_SPEED_RPM),					\
		(u16)(MIN_RUN_SPEED/6),                         	\
                (u16)(STALL_SPEED/6),                                   \
                (u16)(STARTUP_VAL_SPEED/6),                             \
                (u16)(STARTUP_DURATION),                                \
                (s16)((((s32)STARTUP_VAL_SPEED)*RPM_TO_HZ_CONV)/RPM_TO_HZ_AMPL), \
                (u8)(CONTROL_LOOP_PERIOD) 				\
	};
#endif

#define _ACIM_INSTANCE							\
	/* SubFields Instances */                 			\
	_ACIM_VAR_INSTANCE						\
	_ACIM_CONST_INSTANCE						\
									\
	/* Instance of the motor 					\
		 using default values */				\
	static ACIM_Struct_t sACIM_Struct=				\
	{								\
	&sACIM_Var, /*pACIM_Var*/					\
	&sACIM_Const /*pACIM_Const*/					\
	};					        		\
									\
	static const PACIM_Struct_t pACIM_Struct = &sACIM_Struct;

/**** Private instances *******************************************************/
_ACIM_INSTANCE  // Instances the drive+motor params struct

static u16 hVF_Ratio = (u16)(V_F_RATIO*1000);
static u16 hStartUp_VF_Ratio = (u16)(STARTUP_V_F_RATIO*1000);

PACIM_Struct_t Get_ACIM_Struct(void)
{
	return pACIM_Struct;
}

s16 ACIM_GetTargetRotorSpeed_RPM(void)
{
	return sACIM_Struct.pACIM_Var->hTarget_rotor_speed_RPM;
}

void ACIM_SetTargetRotorSpeed_RPM_HzEl(s16 val)
{
  if (val > sACIM_Struct.pACIM_Const->hMax_Speed)
  {}
  else if ( (-val) > sACIM_Struct.pACIM_Const->hMax_Speed)
  {}
  else
  {
    sACIM_Struct.pACIM_Var->hTarget_rotor_speed_RPM = val;
    sACIM_Struct.pACIM_Var->hTarget_rotor_speed_HzEl =
                  (s16)(((s32)val * sACIM_Struct.pACIM_Const->bRPM_to_Hz_Conv)/
                  sACIM_Struct.pACIM_Const->hRPM_to_Hz_Ampl);
  }
}

s16 ACIM_GetTargetRotorSpeed_HzEl(void)
{
  s16 htemp;
  htemp = sACIM_Struct.pACIM_Var->hTarget_rotor_speed_HzEl;
  if (htemp < 0)
  {
    htemp = -htemp;
  }
  return htemp;
}

s16 ACIM_GetMeasuredRotorRpeed_RPM(void)
{
	return sACIM_Struct.pACIM_Var->hMeasured_rotor_speed_RPM;
}

void ACIM_SetMeasuredRotorSpeed_RPM(s16 val)
{
	sACIM_Struct.pACIM_Var->hMeasured_rotor_speed_RPM = val;
}

s16  ACIM_Get_Startup_Slip(void)
{
	return sACIM_Struct.pACIM_Var->hStartUpSlip;
}

void  ACIM_Set_Startup_Slip(s16 val)
{
	sACIM_Struct.pACIM_Var->hStartUpSlip = val;
}

s16 ACIM_Get_Bus_Voltage(void)
{
        return (s16)(sACIM_Struct.pACIM_Var->hBusVoltage);
}

void ACIM_Set_Bus_Voltage(s16 BusVoltage)
{
  sACIM_Struct.pACIM_Var->hBusVoltage = BusVoltage;
}

void ACIM_Set_HeatsinkTemp(u8 bTemperature)
{
  sACIM_Struct.pACIM_Var->bHeatsinkTemp = bTemperature;
}

u8 ACIM_Get_HeatsinkTemp(void)
{
  return (u8)(sACIM_Struct.pACIM_Var->bHeatsinkTemp);
}

void ACIM_Set_Vf_Ratio(s16 val)
{
  u16 htemp;
  hVF_Ratio = val;  
  
  htemp = (u16)(((s32)val * 256)/1000);
  
  sACIM_Struct.pACIM_Var->hVFConstant = (u16)((((u32)VOUT_CONSTANT_A)*htemp)/2);
}

s16 ACIM_Get_Vf_Ratio(void)
{
  return (hVF_Ratio);
}

void ACIM_Set_Startup_Vf_Ratio(s16 val)
{
  u16 htemp;
  hStartUp_VF_Ratio = val;  
  
  htemp = (u16)(((s32)val * 256)/1000);
  
  sACIM_Struct.pACIM_Var->hStartUpVFConstant = 
    (u16)((((u32)VOUT_CONSTANT_A)*htemp)/2);
}

s16 ACIM_Get_Startup_Vf_Ratio(void)
{
  return (hStartUp_VF_Ratio);
}

#ifdef SPEED_CLOSED_LOOP
s16  ACIM_Get_Speed_VF_KP(void)
{
	return sACIM_Struct.pACIM_Const->pPID_VF_Struct->pPID_Var->hKp_Gain;
}

void  ACIM_Set_Speed_VF_KP(s16 val)
{
	sACIM_Struct.pACIM_Const->pPID_VF_Struct->pPID_Var->hKp_Gain = val;
}

s16  ACIM_Get_Speed_VF_KI(void)
{
	return sACIM_Struct.pACIM_Const->pPID_VF_Struct->pPID_Var->hKi_Gain;
}

void  ACIM_Set_Speed_VF_KI(s16 val)
{
	sACIM_Struct.pACIM_Const->pPID_VF_Struct->pPID_Var->hKi_Gain = val;
}

s16  ACIM_Get_Speed_MTPA_KP(void)
{
	return sACIM_Struct.pACIM_Const->pPID_MTPA_Struct->pPID_Var->hKp_Gain;
}

void  ACIM_Set_Speed_MTPA_KP(s16 val)
{
	sACIM_Struct.pACIM_Const->pPID_MTPA_Struct->pPID_Var->hKp_Gain = val;
}

s16  ACIM_Get_Speed_MTPA_KI(void)
{
	return sACIM_Struct.pACIM_Const->pPID_MTPA_Struct->pPID_Var->hKi_Gain;
}

void  ACIM_Set_Speed_MTPA_KI(s16 val)
{
	sACIM_Struct.pACIM_Const->pPID_MTPA_Struct->pPID_Var->hKi_Gain = val;
}

s16  ACIM_Get_MTPA_Slip(void)
{
	return sACIM_Struct.pACIM_Var->hMTPAslip;
}

void  ACIM_Set_MTPA_Slip(s16 val)
{
	sACIM_Struct.pACIM_Var->hMTPAslip = val;
}

void ACIM_Set_MTPA(u8 val)
{
  if (val == 0)
  {
    sACIM_Struct.pACIM_Var->Control_Mode = SPEED_CLOSEDLOOP_VF;
  }
  else
  {
    sACIM_Struct.pACIM_Var->Control_Mode = SPEED_CLOSEDLOOP_MTA;
  }
}

u8 ACIM_Get_MTPA_Mode(void)
{
  u8 btemp;
  control_mode_t Control_Mode = sACIM_Struct.pACIM_Var->Control_Mode;
  if (Control_Mode == SPEED_CLOSEDLOOP_VF)
  {
    btemp = 0;
  }
  else
  {
    btemp = 1;
  }
  return btemp;
}    
    
u8 ACIM_Get_Actual_MTPA_Mode(void)
{
  control_mode_t Control_Mode;
  u8 btemp;
  Control_Mode = sACIM_Struct.pACIM_Var->Actual_Control_Mode;
  if (Control_Mode == SPEED_CLOSEDLOOP_VF)
  {
    btemp = 0;
  }
  else
  {
    btemp = 1;
  }
  return btemp;
}
#elif (defined SPEED_OPEN_LOOP)
s16 ACIM_GetActualRotorRpeed_RPM(void)
{
  return ((sACIM_Struct.pACIM_Var->hActual_rotor_speed_HzEl*6)/
          sACIM_Struct.pACIM_Const->bMotor_pole_Pairs);
}

void  ACIM_Set_Slip(s16 val)
{
	sACIM_Struct.pACIM_Var->hSlip = val;
}

s16  ACIM_Get_Slip(void)
{
	return sACIM_Struct.pACIM_Var->hSlip;
}

#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
