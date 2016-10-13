/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_BLDC_Drive.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : BLDC drive implementation module
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
#include "MC_drive.h"
#include "MC_dev_drive.h"  // Include low level drive function
#include "MC_BLDC_motor.h" // Include motor & drive param
#include "MC_BLDC_conf.h"  // Include sensor configuration

#include "MC_BLDC_drive_param.h"  
#include "MC_ControlStage_param.h"
#include "MC_PowerStage_Param.h"		 

#include "MC_BLDC_timers.h"

#include "MC_pid_regulators.h"
#include "MC_vtimer.h"

#include "Globe.h"

#ifdef DAC_FUNCTIONALITY
#include "MC_dev_DAC.h"
#endif

static u16 speed_measure_start_cnt = 0;
static bool start_flag = FALSE;
static u16 bldc_new_target_speed = 0;

/**** Private typedef *********************************************************/
typedef enum 
{DRIVE_RESET,DRIVE_IDLE,DRIVE_STARTINIT,DRIVE_START,DRIVE_RUN,DRIVE_STOP,DRIVE_WAIT,DRIVE_FAULT} DriveState_t;

/**** Private define **********************************************************/

#define alpha_Rising_1      (s32)( ((s32)((s16)Rising_F_1-(s16)Rising_Fmin)*1024) / (s32)(F_1-Freq_Min)  )   // *1024 to keep good accuracy
#define alpha_Falling_1     (s32)( ((s32)((s16)Falling_F_1-(s16)Falling_Fmin)*1024) / (s32)(F_1-Freq_Min) )

#define alpha_Rising_2      (s32)( ((s32)((s16)Rising_F_2-(s16)Rising_F_1)*1024) / (s32)(F_2-F_1) )
#define alpha_Falling_2     (s32)( ((s32)((s16)Falling_F_2-(s16)Falling_F_1)*1024) / (s32)(F_2-F_1) )

#define alpha_Rising_3      (s32)( ((s32)((s16)Rising_Fmax-(s16)Rising_F_2)*1024) / (s32)(Freq_Max-F_2) )
#define alpha_Falling_3     (s32)( ((s32)((s16)Falling_Fmax-(s16)Falling_F_2)*1024) / (s32)(Freq_Max-F_2) )

/**** Private instances *******************************************************/
static PBLDC_Var_t g_pMotorVar;
u8 bHztoRPM;
PPID_Struct_t g_pPID_Speed;
u8 bSpeed_PID_sampling_time;
static const u16 hArrPwmVal = ((u16)((STM8_FREQ_MHZ * (u32)1000000)/PWM_FREQUENCY));
static const u16 CurrentLimit = CURRENT_LIMITATION;
static pvdev_device_t g_pdevice;
static pu16 pcounter_reg;
static pu16 pDutyCycleCounts_reg;

static DriveState_t DriveState = DRIVE_RESET;
static MC_FuncRetVal_t DriveStatus;

static u8 bValidatedMeasuredSpeed = 0;

#ifdef HALL
u8 bHallError;
#endif

/**** Private Functions *******************************************************/
void BLDC_Drive(void);
u16 GetSpeed_01HZ(void);
void BLDCDelayCoefComputation(u16 Motor_Frequency);

/**** Private define **********************************************************/

// Get rotor speed express in 0.1 Hz
u16 GetSpeed_01HZ(void)
{
  u16 speedHz10;
  u16 hTemp;
  u32 wTemp;
  if (*pcounter_reg == 0) 
    return 0;
  hTemp = PWM_FREQUENCY;
  wTemp = (u32)(hTemp) * 10;
  wTemp /= (*pcounter_reg);
  speedHz10 = (u16)(wTemp);
  return speedHz10;
}

void driveInit(pvdev_device_t pdevice)
{
  PBLDC_Struct_t pBLDC_Struct;
  
  dev_driveInit(pdevice);
  
//  EEPROM_INIT();
//  bldc_new_target_speed = _Merge_16Bit(FLASH_ReadByte(BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address1),FLASH_ReadByte(BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address2));
//  if(bldc_new_target_speed ==0)
//    bldc_new_target_speed = 2000;
//  
//  FLASH_Lock(FLASH_MEMTYPE_DATA);
  
  pBLDC_Struct = Get_BLDC_Struct();
  g_pMotorVar = pBLDC_Struct->pBLDC_Var;
  bHztoRPM = (u8)(60 / pBLDC_Struct->pBLDC_Const->bMotor_Pole_Pairs);
  g_pPID_Speed = pBLDC_Struct->pBLDC_Const->pPID_Speed;
  bSpeed_PID_sampling_time = pBLDC_Struct->pBLDC_Const->bSpeed_PID_sampling_time;
  
  g_pdevice = pdevice;
  
#ifdef SENSORLESS
  pcounter_reg = &(pdevice->regs.r16[VDEV_REG16_BEMF_COUNTS]);
#endif
#ifdef HALL
  pcounter_reg = &(pdevice->regs.r16[VDEV_REG16_HALL_COUNTS]);
#endif
  
  pDutyCycleCounts_reg = &(pdevice->regs.r16[VDEV_REG16_BLDC_DUTY_CYCLE_COUNTS]);
  
  vtimer_SetTimer(BLDC_CONTROL_TIMER,bSpeed_PID_sampling_time,(void *)&BLDC_Drive);
}

void driveIdle(void)
{
  DriveState = DRIVE_IDLE;
  dev_driveIdle();
}

MC_FuncRetVal_t driveStartUpInit(void)
{
  DriveState = DRIVE_STARTINIT;
  bValidatedMeasuredSpeed = 0;
#ifdef HALL
  bHallError = 0;
#endif
  DriveStatus = FUNCTION_RUNNING;
  
#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
  g_pPID_Speed->pPID_Var->wIntegral = 0;
#endif
  
  return dev_driveStartUpInit();
}

MC_FuncRetVal_t driveStartUp(void)
{
  DriveState = DRIVE_START;
  return dev_driveStartUp();
}

MC_FuncRetVal_t driveRun(void)
{
  DriveState = DRIVE_RUN;
  
  if (DriveStatus == FUNCTION_ERROR)
  {
    return FUNCTION_ERROR;
  }
  else
  {
    return dev_driveRun();
  }
}

MC_FuncRetVal_t driveStop(void)
{
  DriveState = DRIVE_STOP;
  speed_measure_start_cnt = 0;
  start_flag = FALSE;
  
  return dev_driveStop();
}

MC_FuncRetVal_t driveWait(void)
{
  DriveState = DRIVE_WAIT;
  return dev_driveWait();
}

MC_FuncRetVal_t driveFault(void)
{
  return FUNCTION_ENDED;
}

void BLDC_Drive(void)
{    
  u16 hSpeed_01HZ;
#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
  u16 outPid;
#endif
  s16 hTargetSpeed;
  u16 hMeasuredSpeed;
  
  vtimer_SetTimer(BLDC_CONTROL_TIMER,bSpeed_PID_sampling_time,(void *)&BLDC_Drive);
  
//  if(speed_measure_start_cnt >= 400)
//  {
  // Update measured speed
  hSpeed_01HZ = GetSpeed_01HZ();
  
#ifdef HALL
  if (hSpeed_01HZ > HALL_MAX_SPEED_01HZ)
  {
    hSpeed_01HZ = HALL_MAX_SPEED_01HZ;
  }
#if (HALL_MIN_SPEED_01HZ > 0)
  if (hSpeed_01HZ < HALL_MIN_SPEED_01HZ)//((hSpeed_01HZ < HALL_MIN_SPEED_01HZ) && (bValidatedMeasuredSpeed == 1))
  {
    bHallError++;
    if (bHallError >= HALL_MAX_ERROR_NUMBER)
    {
      DriveStatus = FUNCTION_ERROR;
      bValidatedMeasuredSpeed = 0;
    }
  }
  if (hSpeed_01HZ > HALL_MIN_SPEED_01HZ)
  {
    bValidatedMeasuredSpeed = 1;
    bHallError = 0;
  }
   
#else
//  bValidatedMeasuredSpeed = 1;
  if (hSpeed_01HZ > MIN_SPEED_01HZ)
  {
    bValidatedMeasuredSpeed = 1;
  }
#endif
  
#endif
  
#ifdef SENSORLESS
  if (hSpeed_01HZ > MIN_SPEED_01HZ)
  {
    bValidatedMeasuredSpeed = 1;
  }
#endif
  
  hMeasuredSpeed = (u16)(((u32)bHztoRPM * hSpeed_01HZ)/10);
  hTargetSpeed = g_pMotorVar->hTarget_rotor_speed;
  
  if (hTargetSpeed < 0)
  {
    hTargetSpeed = -hTargetSpeed; // hTargetSpeed is the absolute value
    g_pMotorVar->hMeasured_rotor_speed = -hMeasuredSpeed; // Visualize negative speed
  }
  else
  {
    g_pMotorVar->hMeasured_rotor_speed = hMeasuredSpeed; // Visualize positive speed
  }

//  }
//  else speed_measure_start_cnt++;
  
  if (bValidatedMeasuredSpeed == 1)
  {
    // Run time Delay Coeff Computation
    if (g_pMotorVar->bAutoDelay)
    {
      BLDCDelayCoefComputation(hMeasuredSpeed);
    }
    
#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
#if (CURRENT_CONTROL_MODE == VOLTAGE_MODE)
    outPid = PID_REG(hTargetSpeed,hMeasuredSpeed,g_pPID_Speed);
#endif
#if (CURRENT_CONTROL_MODE == CURRENT_MODE)
    outPid = PID_REG(hTargetSpeed,hMeasuredSpeed,g_pPID_Speed);
    if (outPid > CurrentLimit)
    {
      outPid = CurrentLimit;
    }
#endif
#endif
  }
  else
  {
    // Run in open loop untill minimum valid speed is measured
#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
#if (CURRENT_CONTROL_MODE == VOLTAGE_MODE)
    outPid = g_pMotorVar->hDuty_cycle;
#endif
#if (CURRENT_CONTROL_MODE == CURRENT_MODE)
    outPid = g_pMotorVar->hCurrent_reference;
#endif
#endif
  }
  
  if (DriveState == DRIVE_RUN)
  {
#if (SPEED_CONTROL_MODE == OPEN_LOOP)
#if (CURRENT_CONTROL_MODE == VOLTAGE_MODE)
    *pDutyCycleCounts_reg = g_pMotorVar->hDuty_cycle;
#endif
#if (CURRENT_CONTROL_MODE == CURRENT_MODE)
    *pDutyCycleCounts_reg = g_pMotorVar->hCurrent_reference;
#endif
#endif
#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
#if (CURRENT_CONTROL_MODE == VOLTAGE_MODE)
    *pDutyCycleCounts_reg = outPid;
#endif
#if (CURRENT_CONTROL_MODE == CURRENT_MODE)
    *pDutyCycleCounts_reg = outPid;
#endif
#endif
  }
  
#ifdef DAC_FUNCTIONALITY
  dev_DACUpdateValues(DAC_CH_1,(u8)(g_pMotorVar->hMeasured_rotor_speed>>6));
#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
  dev_DACUpdateValues(DAC_CH_2,(u8)(outPid>>4));
#endif
#endif
   
}

void BLDCDelayCoefComputation(u16 Motor_Frequency)
{
  u8 BEMF_Rising_Factor,BEMF_Falling_Factor;
  if (Motor_Frequency <= Freq_Min) 
  {
    BEMF_Rising_Factor = Rising_Fmin;
    BEMF_Falling_Factor = Falling_Fmin;
  } 
  else if (Motor_Frequency <= F_1) 
  {    
    BEMF_Rising_Factor = (u8)(Rising_Fmin + (s32)(alpha_Rising_1*(Motor_Frequency-Freq_Min)/1024)); 
    BEMF_Falling_Factor = (u8)(Falling_Fmin + (s32)(alpha_Falling_1*(Motor_Frequency-Freq_Min)/1024));
  } 
  else if (Motor_Frequency <= F_2) 
  { 
    BEMF_Rising_Factor = (u8)(Rising_F_1 + (s16)(alpha_Rising_2*(Motor_Frequency-F_1)/1024)); 
    BEMF_Falling_Factor = (u8)(Falling_F_1 + (s16)(alpha_Falling_2*(Motor_Frequency-F_1)/1024));
  } 
  else if (Motor_Frequency <= Freq_Max) 
  {    
    BEMF_Rising_Factor = (u8)(Rising_F_2 + (u16)(alpha_Rising_3*(Motor_Frequency-F_2)/1024)); 
    BEMF_Falling_Factor = (u8)(Falling_F_2 + (u16)(alpha_Falling_3*(Motor_Frequency-F_2)/1024));
  } 
  else 
  { 
    BEMF_Rising_Factor = Rising_Fmax ;
    BEMF_Falling_Factor = Falling_Fmax;
  }
  
  g_pMotorVar->bRising_Delay = BEMF_Rising_Factor;
  g_pMotorVar->bFalling_Delay = BEMF_Falling_Factor;
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
