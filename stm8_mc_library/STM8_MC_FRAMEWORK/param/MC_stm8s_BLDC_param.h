/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_stm8s_BLDC_param.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : 
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
#ifndef __BLDC_MTC_PARAM_H
#define __BLDC_MTC_PARAM_H

#include "MC_stm8s_clk_param.h"
#include "MC_ControlStage_param.h"
#include "MC_stm8s_port_param.h"

#define PHASE_A_BEMF_ADC_CHAN ADC2_CHANNEL_0
#define PHASE_B_BEMF_ADC_CHAN ADC2_CHANNEL_1
#define PHASE_C_BEMF_ADC_CHAN ADC2_CHANNEL_2

#define ADC_CURRENT_CHANNEL		ADC2_CHANNEL_10	// Current Feedback channel
#define ADC_USER_SYNC_CHANNEL	ADC2_CHANNEL_9	// Sync User channel

#define ADC_BUS_CHANNEL				ADC2_CHANNEL_3	// BUS Voltage channel
#define ADC_NEUTRAL_POINT_CHANNEL	ADC2_CHANNEL_11 // Neutral point channel
#define ADC_TEMP_CHANNEL			ADC2_CHANNEL_8	// Heatsink temperature sensor channel
#define ADC_USER_ASYNC_CHANNEL		ADC2_CHANNEL_12	// User Async

// Control PINS for samping during Ton
#define MCI_CONTROL_PINS (BIT1|BIT2|BIT3)
#define MCI_CONTROL_DDR GPIOD->DDR
#define MCI_CONTROL_DR  GPIOD->ODR

// Debug Pin
#define Z_DEBUG_PORT DEBUG0_PORT->ODR
#define Z_DEBUG_PIN  DEBUG0_PIN

#define C_D_DEBUG_PORT DEBUG1_PORT->ODR
#define C_D_DEBUG_PIN  DEBUG1_PIN

#define AUTO_SWITCH_PORT DEBUG2_PORT->ODR
#define AUTO_SWITCH_PIN  DEBUG2_PIN

#define PWM_ON_SW_PORT DEBUG3_PORT->ODR
#define PWM_ON_SW_PIN  DEBUG3_PIN

// Low side GPIO Control settings
#define LS_A_PORT GPIOB
#define LS_A_PIN BIT3
#define LS_B_PORT GPIOB
#define LS_B_PIN BIT6
#define LS_C_PORT GPIOB
#define LS_C_PIN BIT7

// ETR Setting
//#define ETR_INPUT

#ifdef ETR_INPUT
	#ifndef TIM1_CHxN_REMAP
		#define ETR_PORT GPIOH
		#define ETR_PIN	BIT4
	#else
		#define ETR_PORT GPIOB
		#define ETR_PIN	BIT3
	#endif
#endif

#define CURRENT_FILTER_NOFILTER 0	// sampling is done at fMASTER.
#define CURRENT_FILTER_F_N2 1		//	fSAMPLING=fMASTER, N=2.
#define CURRENT_FILTER_F_N4 2		//	fSAMPLING=fMASTER, N=4.
#define CURRENT_FILTER_F_N8 3		// fSAMPLING=fMASTER, N=8.
#define CURRENT_FILTER_F2_N6 4		// fSAMPLING=fMASTER/2, N=6.
#define CURRENT_FILTER_F2_N8 5		// fSAMPLING=fMASTER/2, N=8.
#define CURRENT_FILTER_F4_N6 6		// fSAMPLING=fMASTER/4, N=6.
#define CURRENT_FILTER_F4_N8 7		// fSAMPLING=fMASTER/4, N=8.
#define CURRENT_FILTER_F8_N6 8		// fSAMPLING=fMASTER/8, N=6.
#define CURRENT_FILTER_F8_N8 9		// fSAMPLING=fMASTER/8, N=8.
#define CURRENT_FILTER_F16_N5 10	// fSAMPLING=fMASTER/16, N=5.
#define CURRENT_FILTER_F16_N6 11	// fSAMPLING=fMASTER/16, N=6.
#define CURRENT_FILTER_F16_N8 12	// fSAMPLING=fMASTER/16, N=8.
#define CURRENT_FILTER_F32_N5 13	// fSAMPLING=fMASTER/32, N=5.
#define CURRENT_FILTER_F32_N6 14	// fSAMPLING=fMASTER/32, N=6.
#define CURRENT_FILTER_F32_N8 15	// fSAMPLING=fMASTER/32, N=8.

#define CURRENT_FILTER CURRENT_FILTER_F8_N8

#define STEP_RAMP_SIZE 64

#define RAMP_VALUE0	  (u16)	  831
#define RAMP_VALUE1	  (u16)	  344
#define RAMP_VALUE2	  (u16)	  264
#define RAMP_VALUE3	  (u16)	  223
#define RAMP_VALUE4	  (u16)	  196
#define RAMP_VALUE5	  (u16)	  177
#define RAMP_VALUE6	  (u16)	  163
#define RAMP_VALUE7	  (u16)	  152
#define RAMP_VALUE8	  (u16)	  143
#define RAMP_VALUE9	  (u16)	  135
#define RAMP_VALUE10	  (u16)	  128
#define RAMP_VALUE11	  (u16)	  123
#define RAMP_VALUE12	  (u16)	  118
#define RAMP_VALUE13	  (u16)	  113
#define RAMP_VALUE14	  (u16)	  109
#define RAMP_VALUE15	  (u16)	  106
#define RAMP_VALUE16	  (u16)	  102
#define RAMP_VALUE17	  (u16)	  99
#define RAMP_VALUE18	  (u16)	  97
#define RAMP_VALUE19	  (u16)	  94
#define RAMP_VALUE20	  (u16)	  92
#define RAMP_VALUE21	  (u16)	  90
#define RAMP_VALUE22	  (u16)	  88
#define RAMP_VALUE23	  (u16)	  86
#define RAMP_VALUE24	  (u16)	  84
#define RAMP_VALUE25	  (u16)	  82
#define RAMP_VALUE26	  (u16)	  81
#define RAMP_VALUE27	  (u16)	  79
#define RAMP_VALUE28	  (u16)	  78
#define RAMP_VALUE29	  (u16)	  77
#define RAMP_VALUE30	  (u16)	  75
#define RAMP_VALUE31	  (u16)	  74
#define RAMP_VALUE32	  (u16)	  73
#define RAMP_VALUE33	  (u16)	  72
#define RAMP_VALUE34	  (u16)	  71
#define RAMP_VALUE35	  (u16)	  70
#define RAMP_VALUE36	  (u16)	  69
#define RAMP_VALUE37	  (u16)	  68
#define RAMP_VALUE38	  (u16)	  67
#define RAMP_VALUE39	  (u16)	  66
#define RAMP_VALUE40	  (u16)	  65
#define RAMP_VALUE41	  (u16)	  65
#define RAMP_VALUE42	  (u16)	  64
#define RAMP_VALUE43	  (u16)	  63
#define RAMP_VALUE44	  (u16)	  62
#define RAMP_VALUE45	  (u16)	  62
#define RAMP_VALUE46	  (u16)	  61
#define RAMP_VALUE47	  (u16)	  60
#define RAMP_VALUE48	  (u16)	  60
#define RAMP_VALUE49	  (u16)	  59
#define RAMP_VALUE50	  (u16)	  58
#define RAMP_VALUE51	  (u16)	  58
#define RAMP_VALUE52	  (u16)	  57
#define RAMP_VALUE53	  (u16)	  57
#define RAMP_VALUE54	  (u16)	  56
#define RAMP_VALUE55	  (u16)	  56
#define RAMP_VALUE56	  (u16)	  55
#define RAMP_VALUE57	  (u16)	  55
#define RAMP_VALUE58	  (u16)	  54
#define RAMP_VALUE59	  (u16)	  54
#define RAMP_VALUE60	  (u16)	  53
#define RAMP_VALUE61	  (u16)	  53
#define RAMP_VALUE62	  (u16)	  53
#define RAMP_VALUE63	  (u16)	  52

#endif /* __BLDC_MTC_PARAM_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/