/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_tacho.c
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
/* Includes ------------------------------------------------------------------*/
#include "MC_tacho.h"
#include "MC_tacho_param.h"
#include "MC_ControlStage_param.h"

/* Private define ------------------------------------------------------------*/
#define HZ_TO_RPM_CONV_CONST (u8)6

/* Private variable ----------------------------------------------------------*/
//pointers to virtual registers
static pu8 ppresc_reg;
static pu8 ppulsen_reg;
static pu16 pcounter_reg;

static u16 speedHz10;     //last motor measured speed

/*******************************************************************************
* Function Name : Tacho_Init
* Description   : It's called by driveInit (MC_ACIM_Drive.c) when in sensored
*                 configurations.
*                 This function initializes local pointers to the virtual
*                 registers.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void Tacho_Init(pvdev_device_t pdevice)
{
  ppresc_reg = (pdevice->regs.r8+VDEV_REG8_TACHO_PRESCALER);
  ppulsen_reg = (pdevice->regs.r8+VDEV_REG8_TACHO_PULSE_NUMBER);
  pcounter_reg = (pdevice->regs.r16+VDEV_REG16_TACHO_COUNTS);
}

/*******************************************************************************
* Function Name : Tacho_CalcSpeed_HzMec
* Description   : Calculates the motor mechanical speed, Hz*10
* Input         : None.
* Output        : None.
* Return        : motor mechanical speed, Hz*10.
*******************************************************************************/
s16 Tacho_CalcSpeed_HzMec(void)
{
  u16 hdivfactor;
  u32 wtemp;	
  
  if (*pcounter_reg != 0)
  {		
    hdivfactor = 1<<(*ppresc_reg);
    wtemp = (u32)(*pcounter_reg) *	hdivfactor;
    wtemp /= *ppulsen_reg;
    speedHz10 = (u16)((u32)(STM8_FREQ_MHZ*1000000*10/TACHO_PULSE_PER_REV)/wtemp);
  }
  else
  {
    speedHz10 = 0;
  }
  return speedHz10;
}

/*******************************************************************************
* Function Name : Tacho_GetSpeed_HzMec
* Description   : Returns the motor measured mechanical speed, Hz*10
* Input         : None.
* Output        : None.
* Return        : motor mechanical speed, Hz*10.
*******************************************************************************/
s16 Tacho_GetSpeed_HzMec(void)
{
  return speedHz10;
}

/*******************************************************************************
* Function Name : Tacho_GetSpeed_RPM
* Description   : Returns the motor measured mechanical speed, RPM
* Input         : None.
* Output        : None.
* Return        : motor mechanical speed, RPM.
*******************************************************************************/
s16 Tacho_GetSpeed_RPM(void)
{
  return (s16)(speedHz10*HZ_TO_RPM_CONV_CONST);
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
