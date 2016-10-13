/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_BLDC_conf.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : BLDC motor drive configuration
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
#ifndef __MC_BLDC_CONF_H
#define __MC_BLDC_CONF_H

// Configuartion

#define HALL
//#define SENSORLESS

// Optional includes
#ifdef HALL
#include "MC_hall_param.h"
#endif

#if (!defined SENSORLESS)&&(!defined HALL)
#error "Invalid configuration: No speed sensor selected"
#endif

#if (defined SENSORLESS)&&(defined HALL)
#error "Invalid configuration: Two speed sensor selected"
#endif

#endif /*__MC_BLDC_CONF_H*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/