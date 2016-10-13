/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : vdev_ios.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : virtual ios header file
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
#ifndef __VDEVS_IOS_H__
#define __VDEVS_IOS_H__

/*****************************************************************************/
#include "dev_type.h"

/*****************************************************************************/
errorcode vdev_fninp8(u32 addr, pu8 pportvalue);
errorcode vdev_fninp16(u32 addr, pu16 pportvalue);
errorcode vdev_fninp32(u32 addr, pu32 pportvalue);
errorcode vdev_fnout8(u32 addr, u8 pportvalue);
errorcode vdev_fnout16(u32 addr, u16 pportvalue);
errorcode vdev_fnout32(u32 addr, u32 pportvalue);

#endif  /* __VDEVS_IOS_H__ */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

