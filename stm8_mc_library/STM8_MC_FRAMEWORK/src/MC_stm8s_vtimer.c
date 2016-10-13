/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_stm8s_vtimer.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Low level virtual timer implementation module
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
#include "stm8s.h"//"stm8s_lib.h"
#include "MC_stm8s_clk_param.h"
#include "MC_vtimer.h"
#include "Globe.h"

#define DELTAT_MS 1 // ms

#define ARRVALUE ((DELTAT_MS * STM8_FREQ_MHZ * 1000) / 128) 

void dev_vtimerInit(void)
{
	TIM4_DeInit();
	
	/* Time base configuration */ 
	TIM4_TimeBaseInit(TIM4_PRESCALER_128,ARRVALUE); // Setting for 1ms Delta time
	//ITC_SetSoftwarePriority(ITC_IRQ_TIM4_OVF, ITC_PRIORITYLEVEL_1);
	ITC->ISPR6 |= 0xC0;
	ITC->ISPR6 &= 0x7F;
	
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
//	enableInterrupts();
	
	 /* Enable TIM4 */
	TIM4_Cmd(ENABLE);
}

/**
  * @brief Timer4 Update/Overflow Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
//@near @interrupt @svlreg void TIM4_UPD_OVF_IRQHandler (void)
#pragma vector=0x19
__interrupt void TIM4_OVR_UIF_vector(void)
{
	/* In order to detect unexpected events during development,
	 it is recommended to set a breakpoint on the following instruction.
	*/
          if(TIM4_FUNCTION_FLAG == TIM4_RFID)
          {
            if(rfid_delay_protect.RFID_DELAY_PROTECT_FLAG)
            {
              rfid_delay_protect.delay_ms_cnt++;
              
              if( rfid_delay_protect.delay_cnt >= rfid_delay_protect.RFID_DELAY_threshold )
              {
                rfid_delay_protect.delay_ms_cnt = 0;
                rfid_delay_protect.rfid_delay_done = TRUE;
              }
              else 
              {
                if(rfid_delay_protect.delay_ms_cnt >= 1000)
                {
                  rfid_delay_protect.delay_ms_cnt = 0;
                  rfid_delay_protect.delay_cnt++; 
                }
              }
            }
          }
          else if(TIM4_FUNCTION_FLAG == TIM4_BLDC)
          {
            
            vtimer_UpdateHandler();
          }
        
         TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
