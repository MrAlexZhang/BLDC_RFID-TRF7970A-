/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_hall_param.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : hall sensors parameters file
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
#ifndef __MC_HALL_PARAM_H
#define __MC_HALL_PARAM_H

/* Private define do not change it--------------------------------------------*/
#define NOT_VALID 6

/* HALL SENSORS PLACEMENT ----------------------------------------------------*/
#define DEGREES_120 0
#define DEGREES_60  1

/* Define here the mechanical position of the sensors with reference to an 
                                                             electrical cycle */ 
//#define HALL_SENSORS_PLACEMENT DEGREES_60
#define HALL_SENSORS_PLACEMENT DEGREES_120

#if (HALL_SENSORS_PLACEMENT == DEGREES_120)
/* Define here the configuration of the sensors signal with reference 
   to the Step config to be applied  */ 

#define HALL_SENSOR_STEPS_CW_000 NOT_VALID  // H1H2H3 = 0
#define HALL_SENSOR_STEPS_CW_001 5			//			1
#define HALL_SENSOR_STEPS_CW_010 3			//			2
#define HALL_SENSOR_STEPS_CW_011 4			//			3
#define HALL_SENSOR_STEPS_CW_100 1			//			4
#define HALL_SENSOR_STEPS_CW_101 0			//			5
#define HALL_SENSOR_STEPS_CW_110 2			//			6
#define HALL_SENSOR_STEPS_CW_111 NOT_VALID  //			7

#define HALL_SENSOR_STEPS_CCW_000 NOT_VALID  // H1H2H3 =	0
#define HALL_SENSOR_STEPS_CCW_001 4			 //				1
#define HALL_SENSOR_STEPS_CCW_010 0			 //				2
#define HALL_SENSOR_STEPS_CCW_011 5			 //				3
#define HALL_SENSOR_STEPS_CCW_100 2			 //				4
#define HALL_SENSOR_STEPS_CCW_101 3			 //				5
#define HALL_SENSOR_STEPS_CCW_110 1			 //				6
#define HALL_SENSOR_STEPS_CCW_111 NOT_VALID  //				7
#endif

#if (HALL_SENSORS_PLACEMENT == DEGREES_60)
/* Define here the configuration of the sensors signal with reference 
   to the Step config to be applied  */ 

#define HALL_SENSOR_STEPS_CW_000 4		    // H1H2H3 = 0
#define HALL_SENSOR_STEPS_CW_001 5			//			1
#define HALL_SENSOR_STEPS_CW_010 NOT_VALID	//			2
#define HALL_SENSOR_STEPS_CW_011 0			//			3
#define HALL_SENSOR_STEPS_CW_100 4			//			4
#define HALL_SENSOR_STEPS_CW_101 NOT_VALID	//			5
#define HALL_SENSOR_STEPS_CW_110 2			//			6
#define HALL_SENSOR_STEPS_CW_111 1		    //			7

#define HALL_SENSOR_STEPS_CCW_000 5		     // H1H2H3 =	0
#define HALL_SENSOR_STEPS_CCW_001 4			 //				1
#define HALL_SENSOR_STEPS_CCW_010 NOT_VALID	 //				2
#define HALL_SENSOR_STEPS_CCW_011 3			 //				3
#define HALL_SENSOR_STEPS_CCW_100 0			 //				4
#define HALL_SENSOR_STEPS_CCW_101 NOT_VALID	 //				5
#define HALL_SENSOR_STEPS_CCW_110 1			 //				6
#define HALL_SENSOR_STEPS_CCW_111 2		     //				7
#endif

/* Define here the rotor mechanical frequency above which speed feedback is not 
realistic in the application: this allows discriminating glitches for instance 
*/
#define HALL_MAX_SPEED_01HZ          ((u16)2000) // Unit is 0.1Hz

/* Define here the rotor mechanical frequency below which speed feedback is not 
realistic in the application: this allows to discriminate too low freq for 
instance */
#define HALL_MIN_SPEED_01HZ          67 // Unit is 0.1Hz

// Define here the maximum time delay to wait between two hall sensors edge
// before to set the zero speed
#define HALL_CAPT_TIMEOUT_MS 300 // 0,30s 

// Define here the number of speed error mesurement to be occurred 
// before to signaling the error on speed feedback
#define HALL_MAX_ERROR_NUMBER 1 

#endif /*__MC_HALL_PARAM_H*/
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/