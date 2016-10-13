/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : vdev.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : virtual device c file
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
#include "dev_type.h"
#include "vdev_ios.h"
#include "vdev_callbacks.h"
#include "vdev.h"

/******************************************************************************/
#ifdef VDEV_REG8 
static u8 g_vdevreg8[VDEV_REG8_NUMBER];			
#else
static pu8 g_vdevreg8 = NULL;
#endif //VDEV_REG8

#ifdef VDEV_REG16 
static u16 g_vdevreg16[VDEV_REG16_NUMBER];		
#else
static pu16 g_vdevreg16 = NULL;
#endif //VDEV_REG16

#ifdef VDEV_REG32 
static u32 g_vdevreg32[VDEV_REG32_NUMBER];	
#else
static pu32 g_vdevreg32 = NULL;
#endif //VDEV_REG32

#ifdef VDEV_MEM8 
static u8 g_vdevmem8[VDEV_MEM8_SIZE];			
#else
static pu8 g_vdevmem8 = NULL;
#endif //VDEV_MEM8

#ifdef VDEV_MEM16 
static u8 g_vdevmem16[VDEV_MEM16_SIZE];			
#else
static pu8 g_vdevmem16 = NULL;
#endif //VDEV_MEM16

#ifdef VDEV_MEM32 
static u8 g_vdevmem32[VDEV_MEM32_SIZE];			
#else
static pu8 g_vdevmem32 = NULL;
#endif //VDEV_MEM32

#ifdef VDEV_CALLBACK
static pvdev_fncallback g_vdevcallback[VDEV_CALLBACK_NUMBER];
#else
static pvdev_fncallback g_vdevcallback[] = { NULL };
#endif //VDEV_CALLBACK

static vdev_device_t device;

/******************************************************************************/
errorcode vdev_init(void)
{
	errorcode ret = VDEV_ERROR_NONE;

	//Virtual Registry Set Initialization
	device.regs.r8 = g_vdevreg8; 
	device.regs.r8number = VDEV_REG8_NUMBER;
	device.regs.r16 = g_vdevreg16; 
	device.regs.r16number = VDEV_REG16_NUMBER;
	device.regs.r32 = g_vdevreg32; 
	device.regs.r32number = VDEV_REG32_NUMBER;

	//
	//device.regs.r8[VDEV_REG8_<NAME>_IDX] = ;
	//device.regs.r16[VDEV_REG16_<NAME>_IDX] = ;
	//device.regs.r32[VDEV_REG32_<NAME>_IDX] = ;

	//Memory Banks Initialization
	device.mems.m8 = g_vdevmem8; 
	device.mems.m8size = VDEV_MEM8_SIZE;
	device.mems.m16 = g_vdevreg16; 
	device.mems.m16size = VDEV_MEM16_SIZE;
	device.mems.m32 = g_vdevreg32; 
	device.mems.m32size = VDEV_MEM32_SIZE;

	//Ios Functions 
	device.ios.inp8 = vdev_fninp8;
	device.ios.inp16 = vdev_fninp16;
	device.ios.inp32 = vdev_fninp32;
	device.ios.out8 = vdev_fnout8;
	device.ios.out16 = vdev_fnout16;
	device.ios.out32 = vdev_fnout32;

	//Callbacks Vector Initialization		
	device.callbacks.pfncallback = g_vdevcallback;          
	device.callbacks.pfncallbacksize = VDEV_CALLBACK_NUMBER;

	g_vdevcallback[VDEV_CALLBACK_DIVBY0_IDX] = fncallback_divby0;

		// Extra initialization

	//device.regs.r8[VDEV_REG8_MOTOR_STATUS_IDX] &= (u8)(~VDEV_MOTOR_STOP_REQ);
	//device.regs.r8[VDEV_REG8_MOTOR_STATUS_IDX] &= (u8)(~VDEV_MOTOR_START_REQ); 
	
	return ret;
}

pvdev_device_t vdev_get(void)
{
	return &device;
}

errorcode vdev_exec(pvdev_device_t pdevice)
{
	errorcode ret = VDEV_ERROR_NONE;
	
	return ret;
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
