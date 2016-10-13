/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_dev.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : It performs the bridge between high 
											 level modules and the real device
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
//#include "dev_type.h"
#include "MC_dev.h"
#include "MC_dev_opt.h"
#include "MC_dev_clk.h"
#include "MC_dev_port.h"
#include "MC_controlstage_param.h"
#include "MC_dev_keys.h"
#include "MC_dev_drive.h"
#include "MC_dev_vtimer.h"

#ifdef DISPLAY
	#include "MC_dev_display.h"
#endif

#ifdef DAC_FUNCTIONALITY
	#include "MC_dev_DAC.h"
#endif

static pu16 pHWerrorOccurred_reg,pHWerrorActual_reg;

void devInit(pvdev_device_t pdevice)
{
	pHWerrorOccurred_reg = (pdevice->regs.r16+VDEV_REG16_HW_ERROR_OCCURRED);
	pHWerrorActual_reg = (pdevice->regs.r16+VDEV_REG16_HW_ERROR_ACTUAL);
	
	#ifdef ENABLE_OPTION_BYTE_PROGRAMMING
		dev_optInit();
	#endif

//	dev_clkInit();
	dev_portInit();
	#ifdef DISPLAY
		dev_displayInit(pdevice);
	#endif
	#ifdef DAC_FUNCTIONALITY
		dev_DACInit();
	#endif
	dev_keysInit();  
	dev_vtimerInit();
}

MC_FuncRetVal_t devChkHWErr(void)
{
  MC_FuncRetVal_t a;
  
  if (*pHWerrorOccurred_reg == 0)
  {
    a = FUNCTION_ENDED;
  }
  else
  {
    a = FUNCTION_ERROR;
  }
  
  return a;
}

MC_FuncRetVal_t devChkHWErrEnd(void)
{
  MC_FuncRetVal_t a;
  
  if (*pHWerrorActual_reg == 0)
  {
    a = FUNCTION_ENDED;
  }
  else
  {
    a = FUNCTION_ERROR;
  }
  
  return a;
}

void devChkHWErrClr(void)
{
  *pHWerrorOccurred_reg = 0;
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
