/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : vdev.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : virtual device header file
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
#ifndef __VDEV_H__
#define __VDEV_H__

/******************************************************************************/
#include "dev_type.h"
#include "vdev_param.h"

/******************************************************************************/
typedef struct vdev_regs
{
	pu8 r8;
	pu16 r16;
	pu32 r32;

	u8 r8number;
	u8 r16number;
	u8 r32number;
} vdev_regs_t, *pvdev_regs_t;

typedef struct vdev_mems
{
	pu8 m8;
	pu16 m16;
	pu32 m32;

	u16 m8size;
	u16 m16size;
	u16 m32size;
} vdev_mems_t, *pvdev_mems_t;

typedef errorcode(*pvdev_fncallback)(pvdev_regs_t pregs, pvdev_mems_t pmems);

typedef struct vdev_callbacks
{
	pvdev_fncallback *pfncallback;

	u8 pfncallbacksize;
} vdev_callbacks_t, *pvdev_callbacks_t;

typedef errorcode(*pvdev_fninp8)(u32 addr, pu8 pportvalue);
typedef errorcode(*pvdev_fninp16)(u32 addr, pu16 pportvalue);
typedef errorcode(*pvdev_fninp32)(u32 addr, pu32 pportvalue);
typedef errorcode(*pvdev_fnout8)(u32 addr, u8 pportvalue);
typedef errorcode(*pvdev_fnout16)(u32 addr, u16 pportvalue);
typedef errorcode(*pvdev_fnout32)(u32 addr, u32 pportvalue);

typedef struct vdev_ios
{
	pvdev_fninp8 inp8;
	pvdev_fninp16 inp16;
	pvdev_fninp32 inp32;
	pvdev_fnout8 out8;
	pvdev_fnout16 out16;
	pvdev_fnout32 out32;
} vdev_ios_t, *pvdev_ios_t;

typedef void(*pvdev_fndisplayvoid)(void);
typedef void(*pvdev_fndisplaycmd)(pu8 pcmd, u8 cmdsize);
typedef void(*pvdev_fndisplayprintch)(u8 Line,u8 Pos,u8 ch);

typedef struct vdev_device
{
	vdev_regs_t regs;
	vdev_mems_t mems;
	vdev_ios_t ios;
	vdev_callbacks_t callbacks;
}vdev_device_t, *pvdev_device_t;

/******************************************************************************/

#define VDEV_ERROR_NONE 0

/******************************************************************************/
errorcode vdev_init(void);
pvdev_device_t vdev_get(void);
errorcode vdev_exec(pvdev_device_t pdevice);

#endif /* __VDEV_H__ */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/


