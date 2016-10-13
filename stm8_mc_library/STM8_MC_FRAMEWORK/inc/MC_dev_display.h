/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_dev_display.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Prototype definition for MC_stm8s_display.c
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
#ifndef __MC_DEV_DISPLAY_H__
#define __MC_DEV_DISPLAY_H__

/*****************************************************************************/
#include "stddef.h"
#include "vdev.h"

/*****************************************************************************/
void dev_displayInit(pvdev_device_t pdevice);
void dev_displayClear(void);
void dev_displayFlush(void);
void dev_displayPrintch(void);

#endif  /* __MC_DEV_DISPLAY_H__ */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
