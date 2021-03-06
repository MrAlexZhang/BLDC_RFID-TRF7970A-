/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_ACIM_Drive_Param.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : ACIM drive parameters
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
#ifndef __MC_ACIM_DRIVE_PARAM_H
#define __MC_ACIM_DRIVE_PARAM_H

/*****  GENERAL DRIVE PARAMETERS  *********************************************/
#define CONTROL_LOOP_PERIOD 5     //ms
#define TARGET_ROTOR_SPEED 1000   //RPM, mechanical speed
#define PWM_FREQUENCY 8000        //Hz
#define PWM_REFRESH_RATE 1      //center-aligned mode: number of full PWM periods
#define DEAD_TIME_NS 800          //ns
#define DEAD_TIME_COMPENSATION  //uncomment to enable the deadtime compensation feat
#define BUS_SAMPLING_FREQ 250     //Hz

/*****  SPEED CLOSED LOOP MODE AND RELATED PARAMETERS  ************************/
//Closed-loop control modes, to be defined in conjunction with MC_ACIM_conf.h
//uncomment one of the two statements to enable MTPA or V/f and slip regulation
#define CLOSEDLOOP_CONTROLMODE  SPEED_CLOSEDLOOP_MTA
//#define CLOSEDLOOP_CONTROLMODE  SPEED_CLOSEDLOOP_VF

#define CLOSEDLOOP_TUNING       //comment to minimize the set of tunable param
//Speed PID Parameters, V/f and slip mode controller
#define VF_PID_TYPE PI          //PI or PID
#define VF_KP 96
#define VF_KI 96
#define VF_KD 100
#define VF_KP_DIVISOR 128
#define VF_KI_DIVISOR 16384
#define VF_KD_DIVISOR 16
#define VF_OUT_MAX MAX_V_F_SLIP
#define VF_OUT_MIN -MIN_V_F_SLIP
#define VF_INTERM_MIN  (s32)(VF_KI_DIVISOR)*VF_OUT_MIN
#define VF_INTERM_MAX  (s32)(VF_KI_DIVISOR)*VF_OUT_MAX

//Speed PID Parameters, MTPA mode controller (V/f controller to be tuned also)
#define MTPA_PID_TYPE PI      //PI or PID
#define MTPA_KP 50
#define MTPA_KI 7
#define MTPA_KD 100
#define MTPA_KP_DIVISOR 128
#define MTPA_KI_DIVISOR 256
#define MTPA_KD_DIVISOR 32
#define MTPA_OUT_MAX 255
#define MTPA_OUT_MIN 0
#define MTPA_INTERM_MIN  (s32)(MTPA_KI_DIVISOR)*MTPA_OUT_MIN
#define MTPA_INTERM_MAX  (s32)(MTPA_KI_DIVISOR)*MTPA_OUT_MAX

/*******  SPEED OPEN LOOP MODE AND RELATED PARAMETERS  ************************/
//Open-loop control modes, to be defined in conjunction with MC_ACIM_conf.h
//uncomment one of the two statements to enable V/f or V/f and load compensation
#define OPENLOOP_CONTROLMODE  SPEED_OPENLOOP
//#define OPENLOOP_CONTROLMODE  SPEED_OPENLOOP_LOAD_COMPENSATION

#define OPEN_LOOP_ACCELERATION_SLOPE 200    //RPM/s
#define OPENLOOP_SLIP 60   //Hz*10, default open loop slip in run state
    
//Open-Loop load-compensation parameters, if selected as operating mode
//automatically generated by the STM8S MC Firmware library builder
#define SEGDIV (s16)(1000)
#define ANGC {0,98,131,98,655,294,327,393}
#define OFST {5,2,0,3,-65,-10,-16,-30}

/******************  OPERATION SPEED CHECK PARAMETERS  ************************/
//This parameters are valid in speed closed loop mode or in speed open loop
//and tacho sensing
#define MIN_RUN_SPEED 200   //RPM, mechanical speed, minimum speed in RUN STATE
#define MAX_SPEED_FEEDBACK 20000 //RPM mech, max realistic speed in the application
#define STALL_SPEED 100     //RPM, mechanical speed, minimum speed for standstill

/******************  STARTUP PHASE RELATED PARAMETERS  ************************/
#define STARTUP_V_F_RATIO 1.30  //ratio of phase voltage and electrical freq (V/Hz)
#define STARTUP_SLIP 60         //Hz*10
#define STARTUP_DURATION 5000   //ms
#define STARTUP_VAL_SPEED 800 //RPM, mechanical speed, minimum speed to validate startup

#endif /* __MC_ACIM_DRIVE_PARAM_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
