/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_stm8s_ACIM_it.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Unused interrupt handler for ACIM
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

/* Includes ------------------------------------------------------------------*/
#include "stm8s_it.h"
#include "MC_stm8s_ACIM_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

/** @addtogroup IT_Functions
  * @{
  */

/**
  * @brief Dummy interrupt routine
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
*/
@near @interrupt void NonHandledInterrupt(void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief TRAP interrupt routine
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
*/
@near @interrupt void TRAP_IRQHandler(void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief Top Level Interrupt Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void TLI_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief Auto Wake Up Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void AWU_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief Clock Controller Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void CLK_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief External Interrupt PORTA Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void EXTI_PORTA_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief External Interrupt PORTB Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void EXTI_PORTB_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief External Interrupt PORTC Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void EXTI_PORTC_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief External Interrupt PORTD Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void EXTI_PORTD_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief External Interrupt PORTE Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void EXTI_PORTE_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief CAN RX Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void CAN_RX_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief CAN TX Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void CAN_TX_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief SPI Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void SPI_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

// Timer1 Update/Overflow/Trigger/Break Interruption routine is used in stm8sMC_ACIM_drive.c

/**
  * @brief Timer1 Capture/Compare Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void TIM1_CAP_COM_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

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
#ifndef TIMER2_HANDLES_TACHO	
@near @interrupt void TIM2_UPD_OVF_BRK_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}
#endif

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
#ifndef TIMER2_HANDLES_TACHO	
@near @interrupt void TIM2_CAP_COM_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}
#endif

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
#ifndef TIMER3_HANDLES_TACHO
#ifndef DAC_FUNCTIONALITY
@near @interrupt void TIM3_UPD_OVF_BRK_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}
#endif
#endif

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
#ifndef TIMER3_HANDLES_TACHO	
@near @interrupt void TIM3_CAP_COM_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}
#endif

/**
  * @brief UART1 TX Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void UART1_TX_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief UART1 RX Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void UART1_RX_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief I2C Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void I2C_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @brief UART2 TX interrupt routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void UART2_TX_IRQHandler (void)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    return;
}

/**
  * @brief UART2 RX interrupt routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void UART2_RX_IRQHandler (void)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    return;
}

/**
  * @brief UART3 TX interrupt routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void UART3_TX_IRQHandler (void)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    return;
}

/**
  * @brief UART3 RX interrupt routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void UART3_RX_IRQHandler (void)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    return;
}

/**
  * @brief ADC1 interrupt routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void ADC1_IRQHandler (void)
{

    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    return;

}

// ADC2 EOC Interruption routine is used in stm8sMC_ACIM_drive.c

// Timer4 Update/Overflow Interruption routine is used in MC_stm8s_vtimer.c

/**
  * @brief Eeprom EEC Interruption routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void EEPROM_EEC_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
  return;
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
