/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_ACIM_Drive.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : ACIM drive implementation module
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
#include "stm8s_lib.h"
#include "MC_Drive.h"
#include "MC_dev_drive.h"
#include "MC_ACIM_Motor.h"
#include "MC_ACIM_conf.h"
#include "MC_pid_regulators.h"
#include "MC_vtimer.h"
#include "MC_dev_DAC.h"
#include "MC_ACIM_User_Interface_Param.h"

/* Private typedef -----------------------------------------------------------*/
//ACIMDrive state machine status
typedef enum 
{DRIVE_IDLE,DRIVE_STARTUP,DRIVE_RUN,DRIVE_FAULT} DriveState_t;

/* Private define ------------------------------------------------------------*/
//Virtual timers definitions
#define V_TIM_ACIMDRIVE VTIM4
#define V_TIM_ACIMSTARTUP VTIM5
#if ((defined DISPLAY)||(defined DAC_FUNCTIONALITY))
#define V_TIM_ACIMUPDATEINFO VTIM6
#define V_TIM_ACIMUPDATEINFO_PERIOD 5
#endif
#define V_TIM_ACIMSTARTUPINIT VTIM7
#define V_TIM_ACIMSTARTUPINIT_PERIOD 5

#define MAX_SPEED_FEEDBACK_HZMEC (s16)(MAX_SPEED_FEEDBACK/6)

/* Private functions ---------------------------------------------------------*/
void ACIM_Drive(void);
void ACIM_StartupEnded(void);
#if ((defined DISPLAY)||(defined DAC_FUNCTIONALITY))
void ACIM_UpdateInfo(void);
#endif

#ifdef SPEED_CLOSED_LOOP
void ACIM_MTPA_Control(void);
void ACIM_VF_Control(void);
void ACIM_StartUp_ClosedLoop(void);
#endif
#if ((defined SPEED_OPEN_LOOP)||(defined SPEED_OPEN_LOOP_TACHO_SENSING))
void ACIM_StartUp_OpenLoop(void);
void ACIM_OpenLoop(void);
#endif
#if (((defined SPEED_OPEN_LOOP)||(defined SPEED_OPEN_LOOP_TACHO_SENSING))&&\
    (OPENLOOP_CONTROLMODE==SPEED_OPENLOOP_LOAD_COMPENSATION))
s16 ACIM_LoadCompensation(s16 hTargetSpeedHzEl);
#endif

/* Private variable ----------------------------------------------------------*/
//pointers to virtual registers
static pu8 pModulationIndex;
static pu8 pHeatsinkTemp;
static pu16 pFreq;
static pu16 pBusVoltage;

//pointer to motorstruct
static PACIM_Struct_t ACIMmotor;

static DriveState_t DriveState;
MC_FuncRetVal_t DriveStatus;
static control_mode_t OperationControlMode;

static u16 vout;
static u16 freqout;
static s16 hSlip;
static u16 hVFConstant;
static s16 actspeed_HzEl;
static s16 actspeed_HzMec;
static s16 targetspeed_HzEl;
static u8 bV0;

#if ((defined SPEED_OPEN_LOOP)||(defined SPEED_OPEN_LOOP_TACHO_SENSING))
static volatile s32 wAccelerationSteps;
static volatile s16 hDeltaSpeedStep;
static volatile s32 wDeltaSpeedIncr;
static volatile s16 hSpeedReference;
static volatile s16 hInitialSpeed;
static volatile s16 act_targetspeed_HzEl;
#endif

/*******************************************************************************
* Function Name : driveInit
* Description   : It's called by the relative state machine function.
*                 This function initializes local pointers to the virtual
*                 registers and drive structure. It initializes the low level
*                 drive module by calling the function dev_driveInit.
*                 It also starts the ACIMDRIVE virtual timer which calls the 
*                 ACIM_Drive function with a periodicity settled by parameter
*                 CONTROL_LOOP_PERIOD (MC_ACIM_Drive_Param.h)
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void driveInit(pvdev_device_t pdevice)
{	
  dev_driveInit(pdevice);
#ifdef TACHO  
  Tacho_Init(pdevice);
#endif
  pModulationIndex = pdevice->regs.r8+VDEV_REG8_ACIM_MODULATION_INDEX;
  pHeatsinkTemp = pdevice->regs.r8+VDEV_REG8_HEATSINK_TEMPERATURE;
  pFreq = pdevice->regs.r16+VDEV_REG16_ACIM_FREQUENCY;
  pBusVoltage =  pdevice->regs.r16 + VDEV_REG16_BOARD_BUS_VOLTAGE;  
  
  ACIMmotor = Get_ACIM_Struct();
	
  DriveState = DRIVE_IDLE;
  DriveStatus = FUNCTION_RUNNING;

  if (vtimer_TimerElapsed(V_TIM_ACIMDRIVE))
  {
    vtimer_SetTimer(V_TIM_ACIMDRIVE,ACIMmotor->pACIM_Const->bControlLoop_Period_ms,
                    (void*)(&ACIM_Drive)); 
  }  
}

/*******************************************************************************
* Function Name : driveIdle
* Description   : It's called by the relative state machine function.
*                 It updates the local (MC_ACIM_Drive.c) state machine to IDLE
*                 state. It updates the info towards LCD and DAC and LED
*                 (if defined) and reset the ACIMSTARTUPINIT timer
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void driveIdle(void)
{
#ifdef LED_UI
  pvdev_device_t pdevice = vdev_get();
  pdevice->ios.out8(VDEV_OUT8_LED_4,LED_ON);
  pdevice->ios.out8(VDEV_OUT8_LED_3,LED_OFF);  
  pdevice->ios.out8(VDEV_OUT8_LED_2,LED_OFF);
  pdevice->ios.out8(VDEV_OUT8_LED_1,LED_OFF);  
#endif
  DriveState = DRIVE_IDLE;
  vtimer_KillTimer(V_TIM_ACIMSTARTUPINIT);
#if ((defined DISPLAY)||(defined DAC_FUNCTIONALITY))  
  ACIM_UpdateInfo();
#endif
}

/*******************************************************************************
* Function Name : driveStartUpInit
* Description   : It's called by the relative state machine function.
*                 This routine handles these phases of the startup procedure:
*                 1.(speed sensor enabled) Check the motor speed: if it's higher
*                 then the threshold STALL_SPEED (MC_ACIM_Drive_Param.h) the
*                 startup is aborted and an error message "MOTOR IS RUNNING"
*                 is issued;
*                 2.(speed closed loop): checks the operation mode to be applied
*                 (V/f and slip control or MTPA)(operation mode won't be changed
*                 any longer during START or RUN states), initializes the
*                 integral terms of the PID controller(s);
*                 3. reads the desired speed direction (speed direction won't
*                 be reversible any longer during START or RUN states);
*                 4. initializes control variables to zero;
*                 5. calls the relative low level function (that enables the PWM
*                 outputs);
*                 6. Initializes the virtual timer ACIMSTARTUPINIT, used to
*                 charge the high side drivers' bootstrap capacitors.
* Input         : None.
* Output        : None.
* Return        : FUNCTION_ENDED.
*******************************************************************************/
MC_FuncRetVal_t driveStartUpInit(void)
{
  MC_FuncRetVal_t tempRetVal;

#ifdef TACHO
  
  //speed check, the motor should be at standstill before startup proc init  
  actspeed_HzMec = Tacho_GetSpeed_HzMec();
  if (actspeed_HzMec > ACIMmotor->pACIM_Const->hStall_speed)
  {
    tempRetVal = FUNCTION_ERROR;        
  }  
  else
#endif
  {
    s16 targetspeed_RPM;
    
#ifdef SPEED_CLOSED_LOOP
    
    //checks the operation mode to be applied: V/f or MTPA    
    OperationControlMode = ACIMmotor->pACIM_Var->Control_Mode;
    ACIMmotor->pACIM_Var->Actual_Control_Mode = OperationControlMode;
        
    //initializes to zero the integral terms of the PID controller(s)
    ACIMmotor->pACIM_Const->pPID_VF_Struct->pPID_Var->wIntegral = 0;
    ACIMmotor->pACIM_Const->pPID_MTPA_Struct->pPID_Var->wIntegral = 0;      
#endif
    
    //Speed direction reading    
    targetspeed_RPM = ACIM_GetTargetRotorSpeed_RPM();
    
    if (targetspeed_RPM >= 0)
    {
      ACIMmotor->pACIM_Var->bDirection = +1;
    }
    else
    {
      ACIMmotor->pACIM_Var->bDirection = -1;
    }

    //output initialization (through virtual registers)
    *pModulationIndex = 0;
    *pFreq = 0;
    vout = 0;
    freqout = 0;
    
    dev_driveStartUpInit();
    
    //bootstrap capacitors charging phase is started 
    vtimer_SetTimer(V_TIM_ACIMSTARTUPINIT,V_TIM_ACIMSTARTUPINIT_PERIOD,0);
    
    tempRetVal = FUNCTION_ENDED;
  }
  
  return tempRetVal;
}

/*******************************************************************************
* Function Name : driveStartUp
* Description   : It's called by the relative state machine function.
*                 This routine handles these phases of the startup procedure:
*                 1.it ends the bootstrap capacitor charging as soon as virtual
*                 timer ACIMSTARTUPINIT is elapsed. At that point, it updates
*                 the local state machine to DRIVE_STARTUP state and settles the
*                 virtual timer ACIMSTARTUP to the value allowed for the startup
*                 process (parameter STARTUP_DURATION, MC_ACIM_Drive_Param.h)
*                 2.Updates the info towards LCD, DAC and LED;
*                 3.Returns the value defined by variable DriveStatus; 
*                 4.If DriveStatus is FUNCTION_ENDED, it's reset to
*                 FUNCTION_RUNNING
* Input         : None.
* Output        : None.
* Return        : DriveStatus.
*******************************************************************************/
MC_FuncRetVal_t driveStartUp(void)
{
  MC_FuncRetVal_t tempRetVal;
  
#ifdef LED_UI
  pvdev_device_t pdevice = vdev_get();
  pdevice->ios.out8(VDEV_OUT8_LED_4,LED_OFF);
  pdevice->ios.out8(VDEV_OUT8_LED_3,LED_ON);
#endif

  if (vtimer_TimerElapsed(V_TIM_ACIMSTARTUPINIT))
  {
    //bootstrap capacitor charging phase is ended
    vtimer_SetTimer(V_TIM_ACIMSTARTUPINIT,65535,0);
    vtimer_KillTimer(V_TIM_ACIMSTARTUP);
    
    //start-up procedure enabled
    DriveState = DRIVE_STARTUP;
  }
  
  tempRetVal = DriveStatus;
  
  //check drive status  
  if (DriveStatus == FUNCTION_ENDED)
  {
    //startup procedure ended, DriveStatus reset to FUNCTION_RUNNING
    DriveStatus = FUNCTION_RUNNING;
  }
  
#if ((defined DISPLAY)||(defined DAC_FUNCTIONALITY))   
  ACIM_UpdateInfo();
#endif

  return tempRetVal;
}

/*******************************************************************************
* Function Name : driveRun
* Description   : It's called by the relative state machine function.
*                 1.It updates or confirms the local state machine to DRIVE_RUN
*                 state; updates the info towards LCD, DAC and LED;
*                 2.Returns the value defined by variable DriveStatus; 
*                 3.If DriveStatus is FUNCTION_ENDED, it's reset to
*                 FUNCTION_RUNNING
* Input         : None.
* Output        : None.
* Return        : DriveStatus.
*******************************************************************************/
MC_FuncRetVal_t driveRun(void)
{
  MC_FuncRetVal_t tempRetVal;
  
#ifdef LED_UI
  pvdev_device_t pdevice = vdev_get();
  pdevice->ios.out8(VDEV_OUT8_LED_3,LED_OFF);
  pdevice->ios.out8(VDEV_OUT8_LED_2,LED_ON);
#endif    
  
  DriveState = DRIVE_RUN;
  
  tempRetVal = DriveStatus;
  
  //check drive status  
  if (DriveStatus == FUNCTION_ENDED)
  {
    DriveStatus = FUNCTION_RUNNING;
  }

#if ((defined DISPLAY)||(defined DAC_FUNCTIONALITY)) 
  ACIM_UpdateInfo();
#endif

  return tempRetVal;
}

/*******************************************************************************
* Function Name : driveWait
* Description   : It's called by the relative state machine function.
*                 1.it updates the info towards LCD, DAC;
* Input         : None.
* Output        : None.
* Return        : FUNCTION_ENDED.
*******************************************************************************/
MC_FuncRetVal_t driveWait(void)
{
#if ((defined DISPLAY)||(defined DAC_FUNCTIONALITY)) 
  ACIM_UpdateInfo();
#endif
  return FUNCTION_ENDED;
}

/*******************************************************************************
* Function Name : driveStop
* Description   : It's called by the relative state machine function.
*                 1. puts the control variables to zero;
*                 2. calls the relative low level function (that disables the PWM
*                 outputs);
* Input         : None.
* Output        : None.
* Return        : FUNCTION_ENDED.
*******************************************************************************/
MC_FuncRetVal_t driveStop(void)
{
  //output control variables set to zero (through virtual registers)
  dev_driveStop();
  *pModulationIndex = 0;
  *pFreq = 0;
  
  //local state machine is set to IDLE state
  DriveState = DRIVE_IDLE;

  return FUNCTION_ENDED;
}

/*******************************************************************************
* Function Name : driveFault
* Description   : It's called by the relative state machine function.
*                 It updates the info towards LCD, DAC, LED;
* Input         : None.
* Output        : None.
* Return        : FUNCTION_ENDED.
*******************************************************************************/
MC_FuncRetVal_t driveFault(void)
{
#ifdef LED_UI
  pvdev_device_t pdevice = vdev_get();
  pdevice->ios.out8(VDEV_OUT8_LED_4,LED_OFF);
  pdevice->ios.out8(VDEV_OUT8_LED_3,LED_OFF);
  pdevice->ios.out8(VDEV_OUT8_LED_2,LED_OFF);  
  pdevice->ios.out8(VDEV_OUT8_LED_1,LED_ON);
#endif    
  return FUNCTION_ENDED;
}


/*****************      PRIVATE FUNCTIONS DEFINITIONS     *********************/


/*******************************************************************************
* Function Name : ACIM_StartupEnded
* Description   : The function is awakened at the end of the time period defined
*                 by virtual timer ACIMSTARTUP.
*                 1. Speed Closed loop mode: if the local (MC_ACIM_Drive.c) state
*                 machine is still in DRIVE_STARTUP state, the function will put the
*                 variable DriveStatus to FUNCTION_ERROR;at that point, function
*                 driveStartUp will return FUNCTION_ERROR to the main state
*                 machine, which will go to FAULT state;
*                 2. Speed open loop and tacho sensing: the motor speed is checked
*                 vs parameter STARTUP_VAL_SPEED (MC_ACIM_Drive_Param.h): if it
*                 is higher/lower than this threshold, the function will put the variable
*                 DriveStatus to FUNCTION_ENDED/FUNCTION_ERROR; at that point,
*                 function driveStartUp will return FUNCTION_ENDED/FUNCTION_ERROR
*                 to the main state machine, which will go to RUN/FAULT state;
*                 3. Speed open loop: variable DriveStatus is put to
*                 FUNCTION_ENDED; therefore, function driveStartUp will return
*                 FUNCTION_ENDED and the main state machine will go to RUN state
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void ACIM_StartupEnded(void)
{
#ifdef SPEED_CLOSED_LOOP
  if (DriveState == DRIVE_STARTUP)
  {
    //time allowed for startup procedure is ended
    DriveStatus = FUNCTION_ERROR;
  }
#elif defined SPEED_OPEN_LOOP
#ifdef TACHO
  //startup ended successfully if motor speed is higher than STARTUP_VAL_SPEED
  if (actspeed_HzMec > ACIMmotor->pACIM_Const->hStartup_val_speed)
  {
    DriveStatus = FUNCTION_ENDED;
  }
  else
  {
    DriveStatus = FUNCTION_ERROR;
  }    
#else
  //startup ended
  DriveStatus = FUNCTION_ENDED;  
#endif
#endif
}

/*******************************************************************************
* Function Name : ACIM_UpdateInfo
* Description   : It updates motor drive informations towards the LCD UI and
*                 DAC module. Informations are refreshed with the periodicity
*                 defined by virtual timer ACIMUPDATEINFO (VTIM6)
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
#if ((defined DISPLAY)||(defined DAC_FUNCTIONALITY))
void ACIM_UpdateInfo(void)
{  
  if (vtimer_TimerElapsed(V_TIM_ACIMUPDATEINFO))
  {
    vtimer_SetTimer(V_TIM_ACIMUPDATEINFO,V_TIM_ACIMUPDATEINFO_PERIOD,0);
    {
      u16 hBusV;
			
#ifdef TACHO
      ACIM_SetMeasuredRotorSpeed_RPM(Tacho_GetSpeed_RPM());
#endif

      ACIM_Set_HeatsinkTemp((u8)(((((u16)(*pHeatsinkTemp))*4) +
                            ACIMmotor->pACIM_Const->bNTC_alpha*25 -
                            ACIMmotor->pACIM_Const->bNTC_beta)/
                            ACIMmotor->pACIM_Const->bNTC_alpha));
      
      hBusV = (u16)((*pBusVoltage * (s32)(ACIMmotor->pACIM_Const->hDigit_to_BusV_Conv))/256);
      ACIM_Set_Bus_Voltage(hBusV);
      
#ifdef DAC_FUNCTIONALITY
      dev_DACUpdateValues(DAC_CH_1, (u8)(vout));
      dev_DACUpdateValues(DAC_CH_2, (u8)(freqout/4));
      //Modify the lines above in order to monitor other variables, for instance
      //as it done below. Consider that the DAC functionality has an 8 bit
      //resolution, a suitable scaling factor should be applied to user defined
      //variables
      //dev_DACUpdateValues(DAC_CH_?, (u8)(actspeed_HzEl/4));
      //dev_DACUpdateValues(DAC_CH_?, (u8)(hSlip*2+128));
#endif      
    }
  }
}
#endif

/*******************************************************************************
* Function Name : ACIM_Drive
* Description   : The function is awakened at the end of the time period defined
*                 by virtual timer ACIMSTARTUP (VTIM4).
*                 This routine handles all the phases of the motor drive.
*                 SPEED CLOSED LOOP MODE:--------------------------------------
*                 1. motor speed is read and converted in electrical frequency;
*                 2. action is undertaken according to local (MC_ACIM_Drive.c)
*                 state machine:
*                 DRIVE_IDLE -> no actions
*                 DRIVE_STARTUP -> function ACIM_StartUp_ClosedLoop is called;
*                 DRIVE_RUN -> a)target speed is read; b)motor speed is checked
*                 vs parameters MIN_RUN_SPEED and MAX_SPEED_FEEDBACK 
*                 (MC_ACIM_Drive_Param.h): if it's outside the allowed range,
*                 the function will put the variable DriveStatus to
*                 FUNCTION_ERROR; at that point, function driveRun will return 
*                 FUNCTION_ERROR to the main state machine, which will go to 
*                 FAULT state; c) function ACIM_VF_Control or ACIM_MTPA_Control
*                 is called according to the actual control mode;
*                 3. low level control variables updated (virtual registers
*                 VDEV_REG16_ACIM_FREQUENCY, VDEV_REG8_ACIM_MODULATION_INDEX);
*                 4. the low level function dev_driveRun is called;
*                 SPEED OPEN LOOP MODE:----------------------------------------
*                 1. (tacho sensing enabled) motor speed is read;
*                 2. action is undertaken according to local (MC_ACIM_Drive.c)
*                 state machine:
*                 DRIVE_IDLE -> no actions
*                 DRIVE_STARTUP -> function ACIM_StartUp_OpenLoop is called;
*                 DRIVE_RUN -> a)target speed is read; b) (tacho sensing enabled)
*                 motor speed is checked vs parameters MIN_RUN_SPEED and 
*                 MAX_SPEED_FEEDBACK (MC_ACIM_Drive_Param.h): if it's outside 
*                 the allowed range, the function will put the variable 
*                 DriveStatus to FUNCTION_ERROR; at that point, function driveRun
*                 will return FUNCTION_ERROR to the main state machine,which will
*                 go to FAULT state; c) (load compensation enabled) function
*                 ACIM_LoadCompensation is called; d)function ACIM_OpenLoop is
*                 called;
*                 3. low level control variables updated (virtual registers
*                 VDEV_REG16_ACIM_FREQUENCY, VDEV_REG8_ACIM_MODULATION_INDEX);
*                 4. the low level function dev_driveRun is called;
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void ACIM_Drive(void)
{  
  vtimer_SetTimer(V_TIM_ACIMDRIVE,ACIMmotor->pACIM_Const->bControlLoop_Period_ms,(void*)(&ACIM_Drive));
  
#ifdef TACHO
  //reads the motor speed, mechanical Hz*10
  actspeed_HzMec = Tacho_CalcSpeed_HzMec();
#endif

#ifdef SPEED_CLOSED_LOOP
  //motor speed is converted, electrical Hz*10 
  actspeed_HzEl = actspeed_HzMec * ACIMmotor->pACIM_Const->bMotor_pole_Pairs;
#endif
  
#ifdef SET_TARGET_SPEED_BY_POTENTIOMETER
  //reads target speed from potentiometer and loads it in drive structure
  ACIM_SetTargetRotorSpeed_RPM_HzEl((s16)
                                    (((u32)(ACIMmotor->pACIM_Const->hMax_Speed)*
                                      ACIMmotor->pACIM_Var->hUserADC)/255));
#endif  
  
  switch (DriveState)
  {
  case DRIVE_IDLE:
    {
      DriveStatus = FUNCTION_RUNNING;
    }
    break;
  case DRIVE_STARTUP:
    {
#ifdef SPEED_CLOSED_LOOP
      ACIM_StartUp_ClosedLoop();
#elif defined SPEED_OPEN_LOOP
      ACIM_StartUp_OpenLoop();
#endif      
    }
    break;
  case DRIVE_RUN:
    {
      //reads target speed from drive structure
      targetspeed_HzEl = ACIM_GetTargetRotorSpeed_HzEl();
      
#ifdef TACHO
      //motor speed should be within MIN_RUN_SPEED / MAX_SPEED_FEEDBACK
      if (actspeed_HzMec < ACIMmotor->pACIM_Const->hMin_run_speed)
      {
        DriveStatus = FUNCTION_ERROR;        
      }
      else if (actspeed_HzMec > MAX_SPEED_FEEDBACK_HZMEC)
      {
        DriveStatus = FUNCTION_ERROR;
      }
      else
#endif
      {
#ifdef SPEED_CLOSED_LOOP
        //const V/f + slip control / const slip + V control according to
        //actual control mode
        if (ACIMmotor->pACIM_Var->Actual_Control_Mode == SPEED_CLOSEDLOOP_VF)
        {
          ACIM_VF_Control();          
        }
        else if (ACIMmotor->pACIM_Var->Actual_Control_Mode == SPEED_CLOSEDLOOP_MTA)
        {
          ACIM_MTPA_Control();
        }
#elif defined SPEED_OPEN_LOOP
#if (OPENLOOP_CONTROLMODE==SPEED_OPENLOOP_LOAD_COMPENSATION)
        //slip frequency chosen as function of motor speed (load compensation LUT)
        ACIMmotor->pACIM_Var->hSlip = ACIM_LoadCompensation(hSpeedReference);
        
        //V/f ratio could be also defined as function of motor speed; in that
        //case, a function 'f2' and an additional LUT are to be implemented 
        //ACIMmotor->pACIM_Var->hVFConstant = f2(hSpeedReference);                  
#endif 
        hVFConstant = ACIMmotor->pACIM_Var->hVFConstant;
        hSlip = ACIMmotor->pACIM_Var->hSlip;
        ACIM_OpenLoop();
#endif
      } 
    }
    break;    
  }
  
  //low level control variables updated through virtual registers
  //VDEV_REG16_ACIM_FREQUENCY / VDEV_REG8_ACIM_MODULATION_INDEX)  
  *pModulationIndex = (u8)(vout);
  *pFreq = (freqout);
  
  dev_driveRun();
}

#ifdef SPEED_CLOSED_LOOP
/*******************************************************************************
* Function Name : ACIM_MTPA_Control
* Description   : This function implements the MTPA strategy, control area 1
*                 (constant slip applied, V/f ratio managed by 
*                 the dedicated MTPA PID regulator).
*                 The operation mode controller, area1 -> area2 transition,
*                 is implemented.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void ACIM_MTPA_Control(void)
{
  u16 hVoutmax;
    
  //stator electrical frequency = motor speed + constant (optimal) slip speed   
  freqout = actspeed_HzEl + ACIMmotor->pACIM_Var->hMTPAslip;
  
  //with reference to rated V/f ratio and stator electrical frequency to apply,
  //max stator voltage amplitude allowed
  hVoutmax = (u16)(((ACIMmotor->pACIM_Var->hVFConstant * (u32)freqout)/
                    (*pBusVoltage))/256);
  
  if (hVoutmax > 255)
  {
    hVoutmax = 255;
  }      
  
  //Stator voltage managed by the 'MTPA' speed PID regulator
  vout = PID_REG(targetspeed_HzEl,actspeed_HzEl,
                 ACIMmotor->pACIM_Const->pPID_MTPA_Struct);
  
  //Operation mode controller, area1 -> area2 transition:
  //if magnetizing flux > rated flux, switch to V/f and slip control
  if (vout > hVoutmax)
  {
    ACIMmotor->pACIM_Var->Actual_Control_Mode = SPEED_CLOSEDLOOP_VF;
    
    //'V/f' PID regulator integral term initialization
    ACIMmotor->pACIM_Const->pPID_VF_Struct->pPID_Var->wIntegral =
      ACIMmotor->pACIM_Var->hMTPAslip *
      ACIMmotor->pACIM_Const->pPID_VF_Struct->pPID_Const->hKi_Divisor;
    
    vout = hVoutmax;
  }
}

/*******************************************************************************
* Function Name : ACIM_VF_Control
* Description   : With reference to the operation control mode selected (#define
*                 CLOSEDLOOP_CONTROLMODE (MC_ACIM_Drive_Param.h) and the actual
*                 control mode:
*                 SPEED_CLOSEDLOOP_VF mode: this function implements the V/f
*                 and slip control method (constant V/f ratio, slip managed by the
*                 dedicated PID regulator);
*                 SPEED_CLOSEDLOOP_MTA:this function implements the MTPA strategy,
*                 control area 2 (constant V/f, slip managed by the dedicated
*                 PID regulator). The operation mode controller, area2 -> area1
*                 transition, is implemented.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void ACIM_VF_Control(void)
{
  //Slip speed managed by the 'V/f' speed PID regulator
  hSlip = PID_REG(targetspeed_HzEl,actspeed_HzEl,
                  ACIMmotor->pACIM_Const->pPID_VF_Struct);
  
  if (actspeed_HzEl < (-hSlip))
  {
    //negative (reversed) stator frequency should be avoided 
    freqout = 0;
  }
  else
  {
    //stator electrical frequency = slip speed + motor speed
    freqout = hSlip + actspeed_HzEl;
  }
  
  //stator voltage amplitude calculated according the rated V/f ratio
  vout = (u16)(((ACIMmotor->pACIM_Var->hVFConstant * (u32)freqout)
                /(*pBusVoltage))/256);
  
  //vout saturation and flux weakening;
  if (vout > 255)
  {
    vout = 255;
  }

  if (OperationControlMode == SPEED_CLOSEDLOOP_MTA)
  {    
    //Operation mode controller, area2 -> area1 transition:
    //if slip speed < optimum slip, switch to constant slip and V/f control
    if (hSlip > 0)  //deceleration in V/f mode should not give way to MTPA
    {
      if (ACIMmotor->pACIM_Var->hMTPAslip > hSlip)
      {
        ACIMmotor->pACIM_Var->Actual_Control_Mode = SPEED_CLOSEDLOOP_MTA;
        
        //'MTPA' PID regulator integral term initialization
        ACIMmotor->pACIM_Const->pPID_MTPA_Struct->pPID_Var->wIntegral =
          vout * ACIMmotor->pACIM_Const->pPID_MTPA_Struct->pPID_Const->hKi_Divisor;
      }
    }
  }
}

/*******************************************************************************
* Function Name : ACIM_StartUp_ClosedLoop
* Description   : This function implements the closed loop start-up strategy.
*                 It has three phases:
*                 1. enabling: virtual timer ACIMSTARTUP is loaded, start-up
*                 voltage boost is calculated and applied;
*                 2. startup is on going: motor speed is checked vs parameter
*                 STARTUP_VAL_SPEED (MC_ACIM_Drive_Param.h) but it's lower than
*                 the threshold;
*                 2. startup is ended: motor speed is checked vs parameter
*                 STARTUP_VAL_SPEED (MC_ACIM_Drive_Param.h) and it's higher than
*                 the threshold: PID regulators initial integral terms are
*                 calculated on the basis of last applied V/f ratio of slip
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void ACIM_StartUp_ClosedLoop(void)
{
  u16 hVoutmax;
	
  if (vtimer_TimerElapsed(V_TIM_ACIMSTARTUP))
  //enabling: virtual timer ACIMSTARTUP is loaded
  {
    vtimer_SetTimer(V_TIM_ACIMSTARTUP,ACIMmotor->pACIM_Const->hStartup_duration,
                    (void*)(&ACIM_StartupEnded));
    
    //start-up voltage boost is calculated
    vout = (u8)(((ACIMmotor->pACIM_Const->hV_Constant *
                  ACIMmotor->pACIM_Const->bStartup_V0)/(*pBusVoltage))/10);
    
    bV0 = (u8)vout;
    freqout = 0;
  }
  else if (actspeed_HzMec > ACIMmotor->pACIM_Const->hStartup_val_speed)
  //startup procedure ended successfully, speed is higher than STARTUP_VAL_SPEED  
  {
    DriveStatus = FUNCTION_ENDED;
    vtimer_KillTimer(V_TIM_ACIMSTARTUP);
    
    //Speed PID regulator integral term initialization
    if (OperationControlMode == SPEED_CLOSEDLOOP_VF)
    {
      ACIMmotor->pACIM_Const->pPID_VF_Struct->pPID_Var->wIntegral = 
        ACIMmotor->pACIM_Var->hStartUpSlip *
        ACIMmotor->pACIM_Const->pPID_VF_Struct->pPID_Const->hKi_Divisor;
    }
    else if (OperationControlMode == SPEED_CLOSEDLOOP_MTA)
    {
      ACIMmotor->pACIM_Const->pPID_MTPA_Struct->pPID_Var->wIntegral =
        vout * ACIMmotor->pACIM_Const->pPID_MTPA_Struct->pPID_Const->hKi_Divisor;
    }
  }
  else
  //start-up procedure is on going 
  {
    DriveStatus = FUNCTION_RUNNING;        
    
    //stator electrical frequency = startup slip speed + motor speed
    freqout = ACIMmotor->pACIM_Var->hStartUpSlip + actspeed_HzEl;
 
    vout = vout + 1;    //startup ramp
    
    //with reference to startup V/f ratio and stator electrical frequency
    //to apply, max stator voltage amplitude allowed    
    hVoutmax = (u16)(((ACIMmotor->pACIM_Var->hStartUpVFConstant * (u32)freqout)/
                      (*pBusVoltage))/256)+ bV0;
    
    if (hVoutmax > 255)
    {
      hVoutmax = 255;
    }    
    
    if (vout > hVoutmax)
    {
      vout = hVoutmax;
    } 
  }
}
#endif


#if ((defined SPEED_OPEN_LOOP)||(defined SPEED_OPEN_LOOP_TACHO_SENSING))
/*******************************************************************************
* Function Name : ACIM_StartUp_OpenLoop
* Description   : This function implements the open loop start-up strategy.
*                 It has two phases:
*                 1. enabling: virtual timer ACIMSTARTUP is loaded, start-up
*                 voltage boost is calculated and applied;
*                 2. startup is on going:
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void ACIM_StartUp_OpenLoop(void)
{  
  if (vtimer_TimerElapsed(V_TIM_ACIMSTARTUP))
  //enabling: virtual timer ACIMSTARTUP is loaded  
  {
    vtimer_SetTimer(V_TIM_ACIMSTARTUP,ACIMmotor->pACIM_Const->hStartup_duration,
                    (void*)(&ACIM_StartupEnded));
    
    ACIMmotor->pACIM_Var->hActual_rotor_speed_HzEl = 0;
    act_targetspeed_HzEl = 0;
    targetspeed_HzEl = ACIMmotor->pACIM_Const->hStartUpFinalSpeed_HzEl;
    
    hVFConstant = ACIMmotor->pACIM_Var->hStartUpVFConstant;
    
    hSlip = ACIMmotor->pACIM_Var->hStartUpSlip;
    
    //start-up voltage boost is calculated
    bV0 = (u8)(((ACIMmotor->pACIM_Const->hV_Constant *
                  ACIMmotor->pACIM_Const->bStartup_V0)/(*pBusVoltage))/10);
    
  } 
  else
  {
    //ACTIONS DURING STARTUP, if any
  }
  ACIM_OpenLoop();
}

/*******************************************************************************
* Function Name : ACIM_OpenLoop
* Description   : This function implements the open loop control strategy.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void ACIM_OpenLoop(void)
{
  //chech if needed to accelerate the motor
  if (targetspeed_HzEl != ACIMmotor->pACIM_Var->hActual_rotor_speed_HzEl)
  {
    //check if target speed has changed
    if (targetspeed_HzEl != act_targetspeed_HzEl)
    {
      //Calculate the new parameters for the acceleration
      s32 wDeltaSpeed;
  
      act_targetspeed_HzEl = targetspeed_HzEl;
      hInitialSpeed = ACIMmotor->pACIM_Var->hActual_rotor_speed_HzEl;
      wDeltaSpeed = ((s32)(targetspeed_HzEl - ACIMmotor->pACIM_Var->hActual_rotor_speed_HzEl))*1024;
      wAccelerationSteps = (wDeltaSpeed/ACIMmotor->pACIM_Var->hAccelerationSlope);
      
      if (wAccelerationSteps < 0)
      {
        wAccelerationSteps = - wAccelerationSteps;
      }
      
      hDeltaSpeedStep = (s16)(wDeltaSpeed/wAccelerationSteps);
      wDeltaSpeedIncr = 0;
      hSpeedReference = hInitialSpeed;
    }
    
    if (wAccelerationSteps == 0)
    {
      hSpeedReference = act_targetspeed_HzEl;
      ACIMmotor->pACIM_Var->hActual_rotor_speed_HzEl = act_targetspeed_HzEl;
    }
    else
    {
      wAccelerationSteps --;
      ACIMmotor->pACIM_Var->hActual_rotor_speed_HzEl = hSpeedReference;
      wDeltaSpeedIncr += hDeltaSpeedStep;
      hSpeedReference = hInitialSpeed + (s16)(wDeltaSpeedIncr/1024);
    }
  }
  
  if (DriveState == DRIVE_STARTUP)
  {
    //start-up voltage boost is applied
    vout = bV0;
  }
  else
  {
    vout = 0;
  }

  freqout = hSpeedReference + hSlip;
  vout += (u16)(((hVFConstant * (u32)freqout)/(*pBusVoltage))/256);
  
  //vout saturation and flux weakening;
  if (vout > 255)
  {
    vout = 255;
  }
}
#endif


#if (((defined SPEED_OPEN_LOOP)||(defined SPEED_OPEN_LOOP_TACHO_SENSING))&&\
    (OPENLOOP_CONTROLMODE==SPEED_OPENLOOP_LOAD_COMPENSATION))
s16 ACIM_LoadCompensation(s16 hTargetSpeedHzEl)
{
  s16 hSlipRef;
  u8 bSegment;
  
  s16 hTargetSpeedRPM;
  
  hTargetSpeedRPM = (hTargetSpeedHzEl*6)/ACIMmotor->pACIM_Const->bMotor_pole_Pairs;
    
  bSegment = (u8)(hTargetSpeedRPM/ACIMmotor->pACIM_Const->hSegdiv);
  if (bSegment > (u8)SEGMNUM)
  {
    bSegment = (u8)SEGMNUM;
  }
  
  hSlipRef = (s16)((ACIMmotor->pACIM_Const->wangc[bSegment]*hTargetSpeedRPM)
                   /32768 + ACIMmotor->pACIM_Const->wofst[bSegment]);
 
  return (hSlipRef);  
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
