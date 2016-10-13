/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_stm8s_tacho_param.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : tachometer parameters file
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
#ifndef __MC_STM8S_TACHO_PARAM_H
#define __MC_STM8S_TACHO_PARAM_H

//#define TACHO_TIMER2_CHANNEL1
#define TACHO_TIMER2_CHANNEL2
//#define TACHO_TIMER2_CHANNEL3
//#define TACHO_TIMER3_CHANNEL1
//#define TACHO_TIMER3_CHANNEL2

//#define TIM2_CH3_AND_TIM3_CH1_REMAP

#define TACHO_IC_PORT     GPIOD
#define TACHO_IC_PIN      GPIO_PIN_3

//#define IC_FILTER_DURATION 0x05 //fmaster/2, 8 events: 1us@16MHz, 0.67us@24MHz
#define IC_FILTER_DURATION 0x06 //fmaster/4, 6 events: 1.5us@16MHz, 1us@24MHz
//#define IC_FILTER_DURATION 0x07 //fmaster/4, 8 events: 2us@16MHz, 1.33us@24MHz
//STM8s reference manual reports all the 16 possible digital filter configuration
//which are currently in the range fmaster -> fmaster/32, 8 events

//Advanced settings, see UM 
#define MAX_PRESCALER     ((u8)0x06) //PRESCALER_64
#define TACHO_TIMER_ARR   0xFFFF
#define MAX_ERROR_NUMBER 3

//not to be modified
#ifdef TACHO_TIMER2_CHANNEL1
#define TIMER2_HANDLES_TACHO
#endif
#ifdef TACHO_TIMER2_CHANNEL2
#define TIMER2_HANDLES_TACHO
#endif
#ifdef TACHO_TIMER2_CHANNEL3
#define TIMER2_HANDLES_TACHO
#endif
#ifdef TACHO_TIMER3_CHANNEL1
#define TIMER3_HANDLES_TACHO
#endif
#ifdef TACHO_TIMER3_CHANNEL2
#define TIMER3_HANDLES_TACHO
#endif

#ifdef TIM2_CH3_AND_TIM3_CH1_REMAP
#define TIM2_CH3_REMAP
#endif

#if (!defined TIMER2_HANDLES_TACHO)&&(!defined TIMER3_HANDLES_TACHO)
#error "Tacho signal has not been assigned to any stm8s timer"
#endif

#endif /*__MC_STM8S_TACHO_PARAM_H*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
