/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_BLDC_Drive_Param.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : BLDC motor types and interface
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
#ifndef __BLDC_DRIVE_PARAM_H
#define __BLDC_DRIVE_PARAM_H

#include "MC_BLDC_conf.h"

// Constant parameters

// BLDC configuration default values
#define SPEED_CONTROL_MODE CLOSED_LOOP // (unit none)
//#define SPEED_CONTROL_MODE OPEN_LOOP // (unit none)

#define CURRENT_CONTROL_MODE VOLTAGE_MODE  // (unit none)
//#define CURRENT_CONTROL_MODE CURRENT_MODE  // (unit none)

// Drive param
#define PWM_FREQUENCY 14400 //18000 // (unit Hz)
#define DEMAG_TIME 20 // (unit % of the step time)
#define CURRENT_LIMITATION 1800 // (unit mA)
#define MINIMUM_OFF_TIME 9500 // (unit ns)
#define DUTY_CYCLE_TH_TON 81 // Threshold for apply sampling during Ton

//Speed PID Parameters
#define SPEED_PID_TYPE PI // (unit none)
//#define SPEED_PID_TYPE PID // (unit none)
#define SPEED_KP_DIVISOR 128 // (unit none)
#define SPEED_KI_DIVISOR 512 // (unit none)
#define SPEED_KD_DIVISOR 16 // (unit none)

#define SPEED_OUT_MAX 1111
#define SPEED_OUT_MIN 0 //
#define SPEED_INTERM_MAX ((s32)SPEED_OUT_MAX * SPEED_KI_DIVISOR) //
#define SPEED_INTERM_MIN 0 //

#define SPEED_PID_SAMPLING_TIME 5 // (unit ms)

// Startup params
#define ALIGN_DUTY_CYCLE 75 // for sensorless mode
#define RAMP_DUTY_CYCLE 75  // for sensorless mode
#define ALIGN_DURATION 300 // 300ms
#define ALIGN_SLOPE 4 // 4ms
#define FORCED_STATUP_STEPS 2//0 // Number of forced step without Z sampling
#define STARTUP_CURRENT_LIMITATION 2800 // (unit mA)
#define MIN_SPEED_01HZ 200 // unit 0.1 Hz

// Sampling Method
#define BEMF_SAMPLING_MIXED 0
#define BEMF_SAMPLING_TON   1
#define BEMF_SAMPLING_TOFF  2

#define BEMF_SAMPLING_METHOD BEMF_SAMPLING_MIXED
//#define BEMF_SAMPLING_METHOD BEMF_SAMPLING_TON
//#define BEMF_SAMPLING_METHOD BEMF_SAMPLING_TOFF

#define SAMPLING_POINT_DURING_TOFF 1500 // (unit ns) before Ton

#if (CURRENT_CONTROL_MODE == VOLTAGE_MODE)
	#define SAMPLING_POINT_DURING_TON  9000 // (unit ns) after Ton
#endif
#if (CURRENT_CONTROL_MODE == CURRENT_MODE)
	#define SAMPLING_POINT_DURING_TON  5000 // (unit ns) after Ton
#endif

// BEMF voltage treshold
#define BEMF_RISING_THRESHOLD_V  0.2 // (unit V)
#define BEMF_FALLING_THRESHOLD_V 0.2 // (unit V)

#define BEMF_SAMPLE_COUNT 2 // Z Filter for sensorless mode

#define ADC_SAMPLE_TIMEOUT 10 // Update of ADC sampled variables 10ms

//#define DEBUG_PINS

//#define ACTIVE_BRAKE // Uncomment if used
#define BRAKE_DURATION 2000 // (unit ms)
#define BRAKE_DUTY 100 // (unit %)

#define RISE_FALL_DELAY_LINK // Comment to unlink rise and falling delays

//Fmin
#define Freq_Min         ((u16)250) //1500       // Setting of min frequency in closed loop mode
#define Rising_Fmin      128  // Frequency min coefficient settings
#define Falling_Fmin     128

//F_1
#define F_1              ((u16)500)//2000        
#define Rising_F_1       20   // Intermediate frequency 1 coefficient settings
#define Falling_F_1      20

//F_2
#define F_2              ((u16)750)  //2500     
#define Rising_F_2       20     // Intermediate frequency 2 coefficient settings
#define Falling_F_2      20

//Fmax  
#define Freq_Max         ((u16)1000) //3000      
#define Rising_Fmax      20  // Frequency max coefficient settings
#define Falling_Fmax     20

////////////////////////////////////////////////////////////////////////////////

// Real time parameters default values
#define TARGET_ROTOR_SPEED 2000 // (unit rpm)
#define DUTY_CYCLE 80 // (unit )
#define CURRENT_REFERENCE 300 // (unit mA)
#ifdef SENSORLESS
	#define FALLING_DELAY 128 // (unit 0-255) for sensorless mode
	#define RISING_DELAY 128 // (unit 0-255)  for sensorless mode
#endif
#ifdef HALL
	#define FALLING_DELAY 1 // (unit 0-255)     for hall sensor mode
	#define RISING_DELAY 1 // (unit 0-255)		for hall sensor mode
#endif

//Real time speed PID Parameters
#if (CURRENT_CONTROL_MODE == VOLTAGE_MODE)
	#define SPEED_KP 20 // (unit none) // Voltage Mode
	#define SPEED_KI 20 // (unit none) // Voltage Mode	
#endif
#if (CURRENT_CONTROL_MODE == CURRENT_MODE)
	#define SPEED_KP 40 // (unit none) // Current Mode
	#define SPEED_KI 10 // (unit none) // Current Mode
#endif
#define SPEED_KD 0 // (unit none)

// Options default values
#define TOGGLE_MODE 0 // Set the toggle mode (0 Off 1 On)
#define FAST_DEMAG 0 // Set the fast demag mode (0 Off 1 On)
#define AUTO_DELAY 0 // Set Delay cofficient according a curve (0 Off, 1 On)

// Configuartion control
#if defined (SENSORLESS)
	#if (CURRENT_CONTROL_MODE == CURRENT_MODE) && (BEMF_SAMPLING_METHOD == BEMF_SAMPLING_MIXED)
		#error "Invalid configuartion: BEMF sampling mixed not possible in current mode"
	#endif
#endif

#endif /* __BLDC_DRIVE_PARAM_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/