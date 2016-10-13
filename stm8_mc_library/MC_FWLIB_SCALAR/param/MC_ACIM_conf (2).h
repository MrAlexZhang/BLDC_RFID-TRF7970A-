/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_ACIM_conf.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : ACIM motor drive configuration
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
#ifndef __MC_ACIM_CONF_H
#define __MC_ACIM_CONF_H

#include "MC_ControlStage_param.h"

#define SPEED_CLOSED_LOOP

//#define SPEED_OPEN_LOOP

//#define SPEED_OPEN_LOOP_TACHO_SENSING


//not to be modified
#if (((defined SPEED_CLOSED_LOOP)&&(defined SPEED_OPEN_LOOP))||\
    ((defined SPEED_CLOSED_LOOP)&&(defined SPEED_OPEN_LOOP_TACHO_SENSING))||\
    ((defined SPEED_OPEN_LOOP)&&(defined SPEED_OPEN_LOOP_TACHO_SENSING)))
#error "Non-univocal speed control mode selected"
#endif

#if (((!defined SPEED_CLOSED_LOOP)&&(!defined SPEED_OPEN_LOOP))&&\
  (!defined SPEED_OPEN_LOOP_TACHO_SENSING))
#error "No speed control mode selected"
#endif

#ifdef SPEED_CLOSED_LOOP
#define TACHO
#elif defined SPEED_OPEN_LOOP_TACHO_SENSING
#define SPEED_OPEN_LOOP
#define TACHO
#endif

#ifdef TACHO
#include "MC_tacho.h"
#include "MC_tacho_param.h"
#endif

#endif /*__MC_ACIM_CONF_H*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
