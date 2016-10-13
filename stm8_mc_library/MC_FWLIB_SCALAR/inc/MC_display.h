/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_display.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Prototype definition for MC_display.c
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
#ifndef __MC_DISPLAY_H__
#define __MC_DISPLAY_H__

/******************************************************************************/
#include "dev_type.h"
#include "vdev.h"
#include "MC_User_Interface.h"

/******************************************************************************/
#define DISPLAY_REFRESH_TIME 300
#define DISPLAY_BLINKING_TIME 300
#define DISPLAY_WELCOME_MESSAGE_TIME 2000

typedef u8 display_index_t;
typedef u8 DEV_Display_CH;

void displayInit(pvdev_device_t pDevice,PUserInterface_t pUserInterface);
void display_flush(void);
void display_setpoint_blink(void);
void display_welcome_message(void);

#endif /* __MC_DISPLAY_H__ */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
