/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : ACIM_Motor_Param.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : ACIM motor parameters
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
#ifndef __ACIM_MOTOR_PARAM_H
#define __ACIM_MOTOR_PARAM_H

#define MOTOR_POLE_PAIRS 1    //stator winding pole pair number  
#define MAX_SPEED_RPM 10000   //RPM, max motor speed in the application

#define V_F_RATIO 1.20  //ratio of phase voltage and electrical frequency (V/Hz)
#define MAX_V_F_SLIP 40 //Hz*10, electrical frequency
#define MIN_V_F_SLIP 25 //Hz*10, electrical frequency
#define MTPA_SLIP 21    //Hz*10, electrical frequency
#define STARTUP_V0 60   //Volts*10, max 25,5V

//not to be modified
#if (MOTOR_POLE_PAIRS == 1)
#define RPM_TO_HZ_AMPL 1024
#elif (MOTOR_POLE_PAIRS == 2)
#define RPM_TO_HZ_AMPL 512
#elif (MOTOR_POLE_PAIRS < 6)
#define RPM_TO_HZ_AMPL 256
#elif (MOTOR_POLE_PAIRS < 12)
#define RPM_TO_HZ_AMPL 128
#else//RPM_TO_HZ_AMPL is the smaller power of two less than 255*6/MOTOR_POLE_PAIRS
#define RPM_TO_HZ_AMPL 64
#endif

#define RPM_TO_HZ_CONV (u8)(((u16)MOTOR_POLE_PAIRS*RPM_TO_HZ_AMPL)/6)
#define SEGMNUM 7 //open loop load compensation coeff no. -1

#endif /* __ACIM_MOTOR_PARAM_H */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/