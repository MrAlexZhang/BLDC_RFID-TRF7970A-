/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_stm8s_tacho.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Module handling speed feedback provided by a
                       tachogenerator.
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
#include "stm8s_lib.h"
#include "MC_dev_tacho.h"
#include "MC_tacho_param.h"
#include "MC_stm8s_tacho_param.h"
#include "MC_stm8s_clk_param.h"
#include "MC_ControlStage_param.h"

/**** Private define **********************************************************/

static volatile u8 main_index;
static volatile u16 buffer0a;
static volatile u16 buffer0b;
static volatile u16 buffer1a;
static volatile u16 buffer1b;
static volatile s8 index0;
static volatile s8 index1;
static volatile u8 berrors;

static volatile u8 prescaler_0;
static volatile u8 prescaler_1;

static pu8 ppresc_reg;
static pu8 ppulsen_reg;
static pu16 pcounter_reg;

#if (!(defined TACHO))
#undef TIMER2_HANDLES_TACHO
#undef TIMER3_HANDLES_TACHO
#endif

#if ((defined TIMER3_HANDLES_TACHO)&&(defined DAC_FUNCTIONALITY))
#error "Timer3 handles both TACHO sensing and DAC debugging"
#endif

#ifdef TACHO_TIMER2_CHANNEL1
#define TACHO_IC_CHANNEL TIM2_CHANNEL_1
#define TIM2_IT_TACHO_CC TIM2_IT_CC1
#define TACHO_CCRH CCR1H
#define TACHO_CCRL CCR1L
#elif defined TACHO_TIMER2_CHANNEL2
#define TACHO_IC_CHANNEL TIM2_CHANNEL_2
#define TIM2_IT_TACHO_CC TIM2_IT_CC2
#define TACHO_CCRH CCR2H
#define TACHO_CCRL CCR2L
#elif defined TACHO_TIMER2_CHANNEL3
#define TACHO_IC_CHANNEL TIM2_CHANNEL_3
#define TIM2_IT_TACHO_CC TIM2_IT_CC3
#define TACHO_CCRH CCR3H
#define TACHO_CCRL CCR3L
#elif defined TACHO_TIMER3_CHANNEL1
#define TACHO_IC_CHANNEL TIM3_CHANNEL_1
#define TIM3_IT_TACHO_CC TIM3_IT_CC1
#define TACHO_CCRH CCR1H
#define TACHO_CCRL CCR1L
#elif defined TACHO_TIMER3_CHANNEL2
#define TACHO_IC_CHANNEL TIM3_CHANNEL_2
#define TIM3_IT_TACHO_CC TIM3_IT_CC2
#define TACHO_CCRH CCR2H
#define TACHO_CCRL CCR2L
#endif

void dev_tachoInit(pvdev_device_t pdevice)
{
  ppresc_reg = (pdevice->regs.r8+VDEV_REG8_TACHO_PRESCALER);
  ppulsen_reg = (pdevice->regs.r8+VDEV_REG8_TACHO_PULSE_NUMBER);
  pcounter_reg = (pdevice->regs.r16+VDEV_REG16_TACHO_COUNTS);
  
  prescaler_0 = MAX_PRESCALER;
  prescaler_1 = MAX_PRESCALER;
  
  main_index = 0;
  
  index0=index1=0;
  buffer0a=buffer0b=buffer1a=buffer1b=0;
	
	berrors = 0;

#ifdef TIMER2_HANDLES_TACHO  
  
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
  
  // GPIO initialization
  GPIO_Init(TACHO_IC_PORT,TACHO_IC_PIN,GPIO_MODE_IN_FL_NO_IT);
  
  TIM2_DeInit();
  
  //highest prescaler is fixed by MAX_PRESCALER
  TIM2_PrescalerConfig(MAX_PRESCALER, TIM2_PSCRELOADMODE_IMMEDIATE);
  
  TIM2_SetAutoreload(TACHO_TIMER_ARR);
  
  //Input capture functionality initialization: capture is done on falling edge,
  //no prescaler, TIxFPn directly to ICn, input digital filter
  TIM2_ICInit(TACHO_IC_CHANNEL,TIM2_ICPOLARITY_FALLING,
              TIM2_ICSELECTION_DIRECTTI,TIM2_ICPSC_DIV1,IC_FILTER_DURATION);	
  
  // Source of Update event is counter update
  TIM2_UpdateRequestConfig(TIM2_UPDATESOURCE_GLOBAL);
  
  //IC interrupt priority is higher than OVF	
  //ITC_SetSoftwarePriority(ITC_IRQ_TIM2_CAPCOM, ITC_PRIORITYLEVEL_2);
  ITC->ISPR4 |= 0x30;
  ITC->ISPR4 &= 0xCF;
	
  //ITC_SetSoftwarePriority(ITC_IRQ_TIM2_OVF, ITC_PRIORITYLEVEL_1);
  ITC->ISPR4 |= 0x0C;
  ITC->ISPR4 &= 0xF7;  
  
  // Interrupt request is set on counter overflow & input capture
  TIM2_ITConfig(TIM2_IT_UPDATE|TIM2_IT_TACHO_CC, ENABLE);

#elif defined TIMER3_HANDLES_TACHO
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3, ENABLE);
  
  // GPIO initialization
  GPIO_Init(TACHO_IC_PORT,TACHO_IC_PIN,GPIO_MODE_IN_FL_NO_IT);
  
  TIM3_DeInit();
  
  //highest prescaler is fixed by MAX_PRESCALER
  TIM3_PrescalerConfig(MAX_PRESCALER, TIM3_PSCRELOADMODE_IMMEDIATE);
  
  TIM3_SetAutoreload(TACHO_TIMER_ARR);
  
  //Input capture functionality initialization: capture is done on falling edge,
  //no prescaler, TIxFPn directly to ICn, input digital filter
  TIM3_ICInit(TACHO_IC_CHANNEL,TIM3_ICPOLARITY_FALLING,
              TIM3_ICSELECTION_DIRECTTI,TIM3_ICPSC_DIV1,IC_FILTER_DURATION);	
  
  // Source of Update event is counter update
  TIM3_UpdateRequestConfig(TIM3_UPDATESOURCE_GLOBAL);  
  
  //IC interrupt priority is higher than OVF	
  //ITC_SetSoftwarePriority(ITC_IRQ_TIM3_CAPCOM, ITC_PRIORITYLEVEL_2);
  ITC->ISPR5 |= 0x03;
  ITC->ISPR5 &= 0xFC;  
  
  //ITC_SetSoftwarePriority(ITC_IRQ_TIM3_OVF, ITC_PRIORITYLEVEL_1);
  ITC->ISPR4 |= 0xC0;
  ITC->ISPR4 &= 0x7F;    
  
  // Interrupt request is set on counter overflow & input capture
  TIM3_ITConfig(TIM3_IT_UPDATE|TIM3_IT_TACHO_CC, ENABLE);  
#endif
}

void dev_tachoEnable(void)
{
#ifdef TIMER2_HANDLES_TACHO
  TIM2_Cmd(ENABLE);
#elif defined TIMER3_HANDLES_TACHO
  TIM3_Cmd(ENABLE);
#endif
}

void dev_tachoDisable(void)
{
#ifdef TIMER2_HANDLES_TACHO
  TIM2_Cmd(DISABLE);
#elif defined TIMER3_HANDLES_TACHO
  TIM3_Cmd(DISABLE);
#endif
}

#ifdef TIMER2_HANDLES_TACHO
/**
  * @brief Timer2 Update/Overflow/Break Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void TIM2_UPD_OVF_BRK_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
  it is recommended to set a breakpoint on the following instruction.
  */
  s8 indexold;
  
  if (main_index)
  {
    main_index = 0;       //sector0 is now enabled 
    indexold  = index1;   //now reading sector1 old acquisitions
    index1 = -1;
    
    if (indexold > 0)
    {
      berrors = 0;
			*ppresc_reg = prescaler_1;
      
      if (indexold == 127)    //127 is an error code (more pulses arrived)
      {
        *ppulsen_reg = TACHO_PULSE_AVERAGED+1;
        // prescaler tuning
        if (prescaler_1 > TIM2_PRESCALER_1)
        {
          prescaler_1 --;
        }        
        //
      }
      else
      {
        *ppulsen_reg = (u8)(indexold);
      }
      
      *pcounter_reg = buffer1b-buffer1a;      
    }
    else
    {
			berrors ++;
			if (berrors >= MAX_ERROR_NUMBER)
			{
				*pcounter_reg = 0;
			}
    }
    //prescaler tuning
    if (indexold < TACHO_PULSE_AVERAGED)
    {
      if (prescaler_1 < MAX_PRESCALER)
      {
        prescaler_1 ++;
      }
    }
    TIM2->PSCR = prescaler_1;    //prescaler set, forthcoming sector1 acquisitions
  }
  else
  {
    main_index = 1;       //sector1 is now enabled 
    indexold = index0;    //now reading sector0 old acquisitions
    index0 = -1;    
    
    if (indexold > 0)
    {
      berrors = 0;
			*ppresc_reg = prescaler_0;
      
      if (indexold == 127)    //127 is an error code (more pulses arrived)
      {
        *ppulsen_reg = TACHO_PULSE_AVERAGED+1;
        //prescaler tuning
        if (prescaler_0 > TIM2_PRESCALER_1)
        {
          prescaler_0 --;
        }        
        //        
      }
      else
      {
        *ppulsen_reg = (u8)(indexold);
      }
      
      *pcounter_reg = buffer0b-buffer0a;
    }
    else
    {
			berrors ++;
			if (berrors >= MAX_ERROR_NUMBER)
			{
				*pcounter_reg = 0;
			}
    }
    //prescaler tuning
    if (indexold < TACHO_PULSE_AVERAGED)
    {
      if (prescaler_0 < MAX_PRESCALER)
      {
        prescaler_0 ++;
      }
    }
    TIM2->PSCR = prescaler_0;    //prescaler set, forthcoming sector0 acquisitions    
  }
  
  //TIM2_ClearITPendingBit(TIM2_IT_UPDATE);  
  TIM2->SR1 = (u8)(~TIM2_IT_UPDATE);
  return;
}

/**
  * @brief Timer2 Capture/Compare Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void TIM2_CAP_COM_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
	
  u16 tmpccr2 = 0;
  {
    u8 tmpccr2l=0, tmpccr2h=0;  
    tmpccr2h = TIM2->TACHO_CCRH;
    tmpccr2l = TIM2->TACHO_CCRL;
    tmpccr2 = (u16)(tmpccr2l);
    tmpccr2 |= (u16)((u16)tmpccr2h << 8);
  }
	
  if (main_index)
  {
    if (index1 == -1)
    {
      buffer1a=tmpccr2;
      buffer1b=tmpccr2;
      index1 = 0;
    }
    else if (index1 < TACHO_PULSE_AVERAGED)
    {
      if (tmpccr2 > buffer1b)
      {
        buffer1b=tmpccr2;
        index1++;
      }
    }
    else
    {
      if (tmpccr2 > buffer1b)
      {
        buffer1b=tmpccr2;
				index1 = 127;
				//Timer Re-synch
				TIM2->EGR = 0x01;
      }			
    }
  }
  else
  {		
    if (index0 == -1)
    {
      buffer0a=tmpccr2;
      buffer0b=tmpccr2;      
      index0 = 0;
    }
    else if (index0 < TACHO_PULSE_AVERAGED)
    {
      if (tmpccr2 > buffer0b)
      {
        buffer0b=tmpccr2;
        index0++;
      }
    }
    else
    {
      if (tmpccr2 > buffer0b)
      {
        buffer0b=tmpccr2;
				index0 = 127;
				//Timer Re-synch
				TIM2->EGR = 0x01;				
      }			
    }
  }		
  
  //TIM2_ClearITPendingBit(TIM2_IT_TACHO_CC);
  TIM2->SR1 = (u8)(~TIM2_IT_TACHO_CC);
  
  return;
}

#elif defined TIMER3_HANDLES_TACHO
/**
  * @brief Timer3 Update/Overflow/Break Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void TIM3_UPD_OVF_BRK_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
  it is recommended to set a breakpoint on the following instruction.
  */
  s8 indexold;
  
  if (main_index)
  {
    main_index = 0;       //sector0 is now enabled 
    indexold  = index1;   //now reading sector1 old acquisitions
    index1 = -1;
    
    if (indexold > 0)
    {
      berrors = 0;
			*ppresc_reg = prescaler_1;
      
      if (indexold == 127)    //127 is an error code (more pulses arrived)
      {
        *ppulsen_reg = TACHO_PULSE_AVERAGED+1;
        // prescaler tuning
        if (prescaler_1 > TIM3_PRESCALER_1)
        {
          prescaler_1 --;
        }        
        //
      }
      else
      {
        *ppulsen_reg = (u8)(indexold);
      }
      
      *pcounter_reg = buffer1b-buffer1a;      
    }
    else
    {
			berrors ++;
			if (berrors >= MAX_ERROR_NUMBER)
			{
				*pcounter_reg = 0;
			}
    }
    //prescaler tuning
    if (indexold < TACHO_PULSE_AVERAGED)
    {
      if (prescaler_1 < MAX_PRESCALER)
      {
        prescaler_1 ++;
      }
    }
    TIM3->PSCR = prescaler_1;    //prescaler set, forthcoming sector1 acquisitions
  }
  else
  {
    main_index = 1;       //sector1 is now enabled 
    indexold = index0;    //now reading sector0 old acquisitions
    index0 = -1;    
    
    if (indexold > 0)
    {
      berrors = 0;
			*ppresc_reg = prescaler_0;
      
      if (indexold == 127)    //127 is an error code (more pulses arrived)
      {
        *ppulsen_reg = TACHO_PULSE_AVERAGED+1;
        //prescaler tuning
        if (prescaler_0 > TIM3_PRESCALER_1)
        {
          prescaler_0 --;
        }        
        //        
      }
      else
      {
        *ppulsen_reg = (u8)(indexold);
      }
      
      *pcounter_reg = buffer0b-buffer0a;
    }
    else
    {
			berrors ++;
			if (berrors >= MAX_ERROR_NUMBER)
			{
				*pcounter_reg = 0;
			}
    }
    //prescaler tuning
    if (indexold < TACHO_PULSE_AVERAGED)
    {
      if (prescaler_0 < MAX_PRESCALER)
      {
        prescaler_0 ++;
      }
    }
    TIM3->PSCR = prescaler_0;    //prescaler set, forthcoming sector0 acquisitions    
  }
  
  //TIM3_ClearITPendingBit(TIM3_IT_UPDATE);  
  TIM3->SR1 = (u8)(~TIM3_IT_UPDATE);
  return;
}

/**
  * @brief Timer3 Capture/Compare Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void TIM3_CAP_COM_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
	
  u16 tmpccr2 = 0;
  {
    u8 tmpccr2l=0, tmpccr2h=0;  
    tmpccr2h = TIM3->TACHO_CCRH;
    tmpccr2l = TIM3->TACHO_CCRL;
    tmpccr2 = (u16)(tmpccr2l);
    tmpccr2 |= (u16)((u16)tmpccr2h << 8);
  }	
		
  if (main_index)
  {
    if (index1 == -1)
    {
      buffer1a=tmpccr2;
      buffer1b=tmpccr2;
      index1 = 0;
    }
    else if (index1 < TACHO_PULSE_AVERAGED)
    {
      if (tmpccr2 > buffer1b)
      {
        buffer1b=tmpccr2;
        index1++;
      }
    }
    else
    {
      if (tmpccr2 > buffer1b)
      {
        buffer1b=tmpccr2;
				index1 = 127;
				//Timer Re-synch
				TIM3->EGR = 0x01;
      }
    }
  }
  else
  {		
    if (index0 == -1)
    {
      buffer0a=tmpccr2;
      buffer0b=tmpccr2;      
      index0 = 0;
    }
    else if (index0 < TACHO_PULSE_AVERAGED)
    {
      if (tmpccr2 > buffer0b)
      {
        buffer0b=tmpccr2;
        index0++;
      }
    }
    else
    {
      if (tmpccr2 > buffer0b)
      {
        buffer0b=tmpccr2;
				index0 = 127;
				//Timer Re-synch
				TIM3->EGR = 0x01;				
      }	
    }
  }		
  
  //TIM3_ClearITPendingBit(TIM3_IT_TACHO_CC);
  TIM3->SR1 = (u8)(~TIM3_IT_TACHO_CC);

  return;
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
