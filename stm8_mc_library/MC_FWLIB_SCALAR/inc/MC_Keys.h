/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_Keys.h
* Author             : IMS Systems Lab
* Date First Issued  : mm/dd/yyy
* Description        : Prototype definition for MC_Keys.c
* Software package   : 
********************************************************************************
* History:
* 
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MC_KEYS_H
#define __MC_KEYS_H

#include "MC_User_Interface.h"
#include "vdev.h"

/* Exported constants --------------------------------------------------------*/
// These are defines for reading the joystick and SEL button
#define  NOKEY      (u8)0
#define  JOY_SEL    (u8)1
#define  RIGHT      (u8)2
#define  LEFT       (u8)3
#define  UP         (u8)4
#define  DOWN       (u8)5
#define  KEY_HOLD   (u8)6
#define  USER_SEL   (u8)7

/* Exported variables ------------------------------------------------------- */
extern u8 bMenu_index;
extern u8 bEditMode;

/* Exported functions ------------------------------------------------------- */
void keysInit(pvdev_device_t pdevice, PUserInterface_t pUserInterface);
u8 keysProcess(void);
#endif //__MC_KEYS_H

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
