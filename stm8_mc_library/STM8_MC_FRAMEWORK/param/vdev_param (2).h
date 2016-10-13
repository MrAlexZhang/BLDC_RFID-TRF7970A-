/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : vdev_param.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : virtual device parameter file
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
#ifndef __VDEV_PARAM_H__
#define __VDEV_PARAM_H__

/******************************************************************************/
#include "dev_type.h"

// I/O Address ****************************************************************
#define VDEV_INP8_USER_INPUT 			0x01
#define VDEV_OUT8_DISPLAY_FLUSH 	0x02
#define VDEV_OUT8_DISPLAY_PRINTCH 0x03
#define VDEV_OUT8_LED_1 0x04
#define VDEV_OUT8_LED_2 0x05
#define VDEV_OUT8_LED_3 0x06
#define VDEV_OUT8_LED_4 0x07

// I/O Command
// LED management
#define LED_ON     0x00
#define LED_OFF    0x01
#define LED_TOGGLE 0x02

// Virtual Register definition ************************************************
/***************** 8bits virtual register definition*******/
#define VDEV_REG8						/* Uncomment if define u8 regs */
#define VDEV_REG8_TACHO_PRESCALER								0					
#define VDEV_REG8_TACHO_PULSE_NUMBER						1

#define VDEV_REG8_HALL_PRESCALER								2
#define VDEV_REG8_HALL_PULSE_NUMBER							3
#define VDEV_REG8_HALL_ANGLE										4

#define VDEV_REG8_BEMF_PRESCALER								5
#define VDEV_REG8_BEMF_PULSE_NUMBER							6
#define VDEV_REG8_BEMF_ANGLE										7

#define VDEV_REG8_BLDC_DELAY_COUNTS							8
#define VDEV_REG8_ACIM_MODULATION_INDEX					9
#define VDEV_REG8_ACIM_MAX_MODULATION_INDEX			10

#define VDEV_REG8_HEATSINK_TEMPERATURE					11

#define VDEV_REG8_NUMBER												12

/**************** 16bits virtual register definition*******/

#define VDEV_REG16					/* Uncomment if define u16 regs */

#define VDEV_REG16_TACHO_COUNTS											0

#define VDEV_REG16_HALL_COUNTS											1

#define VDEV_REG16_BEMF_COUNTS											2

#define VDEV_REG16_BLDC_MAX_DUTY_CYCLE_COUNTS				3
#define VDEV_REG16_BLDC_DUTY_CYCLE_COUNTS						4

#define VDEV_REG16_ACIM_PHASE												5
#define VDEV_REG16_ACIM_FREQUENCY										6

#define VDEV_REG16_BOARD_BUS_VOLTAGE								7

#define VDEV_REG16_HW_ERROR_OCCURRED													8
#define VDEV_REG16_HW_ERROR_ACTUAL													9

#define VDEV_REG16_NUMBER														10

/**************** 32bits virtual register definition*******/
//#define VDEV_REG32					/* Uncomment if define u32 regs */
//#define VDEV_REG32_<NAME>			0
#define VDEV_REG32_NUMBER				0

// Virtual memory definitions *************************************************
#define VDEV_MEM8						/* Uncomment if define a u8 buffer */
#define VDEV_MEM8_SIZE			DISPLAY_MEM_SIZE // Display buffer + Parameters for dev_displayPrintch function (u8 Line,u8 Pos,u8 ch)

//#define VDEV_MEM16					/* Uncomment if define a u16 buffer */
#define VDEV_MEM16_SIZE					0

//#define VDEV_MEM32					/* Uncomment if define a u32 buffer */
#define VDEV_MEM32_SIZE					0

/******************************************************************************/
// Common defines
#define USER_INPUT_RIGHT 5
#define USER_INPUT_LEFT 4
#define USER_INPUT_UP 3
#define USER_INPUT_DOWN 2
#define USER_INPUT_SEL 1
#define USER_INPUT_KEY 0

#define VDEV_DISPLAY_BUFFER_EMPTY 0
#define VDEV_DISPLAY_BUFFER_READY  1

#define VDEV_MOTOR_START_REQ 							0x01      
#define VDEV_MOTOR_STOP_REQ  							0x02
#define VDEV_MOTOR_STATE_IDLE							0x04
#define VDEV_MOTOR_STATE_RUN  						0x08
#define VDEV_MOTOR_STATE_PI_INIT_INTTERM 	0x10

// Display parameters
#define DEV_Display_COL 15
#define DEV_Display_ROW 2

#define DISPLAYBUFFER_ADDR 0
#define DISPLAYBUFFER_SIZE DEV_Display_COL * DEV_Display_ROW

#define DISPLAYLINE_ADDR DISPLAYBUFFER_ADDR + DISPLAYBUFFER_SIZE
#define DISPLAYLINE_SIZE 1

#define DISPLAYPOS_ADDR DISPLAYLINE_ADDR + DISPLAYLINE_SIZE
#define DISPLAYPOS_SIZE 1

#define DISPLAYCH_ADDR	DISPLAYPOS_ADDR + DISPLAYPOS_SIZE
#define DISPLAYCH_SIZE	1

#define DISPLAY_MEM_SIZE DISPLAYCH_ADDR + DISPLAYCH_SIZE

#endif /* __VDEV_H__ */
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/


