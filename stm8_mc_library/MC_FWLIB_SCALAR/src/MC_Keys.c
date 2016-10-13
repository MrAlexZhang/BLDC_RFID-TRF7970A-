/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_Keys.c
* Author             : IMS Systems Lab
* Date First Issued  : mm/dd/yyy
* Description        : This module handles Joystick and button management
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
/* Includes ------------------------------------------------------------------*/
#include "MC_Keys.h"
#include "MC_vtimer.h"
#include "MC_ControlStage_Param.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define  USER_SEL_FLAG   (u8)0x01
#define  JOY_SEL_FLAG    (u8)0x02
#define  RIGHT_FLAG      (u8)0x04
#define  LEFT_FLAG       (u8)0x08
#define  UP_FLAG         (u8)0x10
#define  DOWN_FLAG       (u8)0x20

#define KEY_HOLD_TIME 300
#define KEY_REPEAT_TIME 100

/* Private function-----------------------------------------------------------*/
u8 keysRead (void);
/* Private variables ---------------------------------------------------------*/
static PUserInterface_t g_pUserInterface = 0;
static pvdev_device_t g_pdevice = 0;

static u8 bPrevious_key;
static u8 bKey_Flag;

u8 bEditMode = 0;

void keysInit(pvdev_device_t pdevice, PUserInterface_t pUserInterface)
{
	g_pdevice = pdevice;
	g_pUserInterface = pUserInterface;
}

/*******************************************************************************
* Function Name  : KEYS_Read
* Description    : Reads key from the board.
* Input          : None
* Output         : None
* Return         : Return RIGHT, LEFT, UP, DOWN, JOY_SEL, USER_SEL, KEY_HOLD or NOKEY
*******************************************************************************/
u8 keysRead (void)
{
	static u8 user_input;
	g_pdevice->ios.inp8(VDEV_INP8_USER_INPUT,&user_input);

	#ifdef JOYSTICK
		if (((user_input & (1 << USER_INPUT_RIGHT)) != 0))
		{
			/* "RIGHT" key is pressed */
			if (bPrevious_key == RIGHT) 
			{
				return KEY_HOLD;
			}
			else
			{
				bPrevious_key = RIGHT;
				return RIGHT;
			}
		}

		if (((user_input & (1 << USER_INPUT_LEFT)) != 0))
		{
			/* "LEFT" key is pressed */
			if (bPrevious_key == LEFT) 
			{
				return KEY_HOLD;
			}
			else
			{
				bPrevious_key = LEFT;
				return LEFT;
			}
		}

		if(((user_input & (1 << USER_INPUT_SEL)) != 0))
		{
			/* "JOY_SEL" key is pressed */
			if (bPrevious_key == JOY_SEL) 
			{
				return KEY_HOLD;
			}
			else
			{
				if ((bKey_Flag & JOY_SEL_FLAG) == 0)
				{
					bKey_Flag |= JOY_SEL_FLAG;
				}
				else 
				if ((bKey_Flag & JOY_SEL_FLAG) == JOY_SEL_FLAG)
				{
					bKey_Flag &= (u8)(~JOY_SEL_FLAG);
					bPrevious_key = JOY_SEL;
					return JOY_SEL;
				}
				return NOKEY;
			}
		}
	#endif
	
	#ifndef AUTO_START_UP
		if(((user_input & (1 << USER_INPUT_KEY)) != 0))
		{
			/* "USER_SEL" key is pressed */
			if (bPrevious_key == USER_SEL) 
			{
				return KEY_HOLD;
			}
			else
			{
				bPrevious_key = USER_SEL;
				return USER_SEL;
			}
		}
	#endif
	
	#ifdef JOYSTICK
		if(((user_input & (1 << USER_INPUT_UP)) != 0))
		{
			if (bPrevious_key == UP) 
			{
				/* "UP" key is pressed */
				if (vtimer_TimerElapsed(VTIM_KEY))
				{
					vtimer_SetTimer(VTIM_KEY,KEY_REPEAT_TIME,0);
					return UP;
				}
				else
				{
					return KEY_HOLD;
				}
			}
			else
			{
			  bPrevious_key = UP;
					vtimer_SetTimer(VTIM_KEY,KEY_HOLD_TIME,0);
			  return UP;
			}
		}
		
		if(((user_input & (1 << USER_INPUT_DOWN)) != 0))
		{
			/* "DOWN" key is pressed */
			if (bPrevious_key == DOWN) 
			{
			  if (vtimer_TimerElapsed(VTIM_KEY))
					{
						vtimer_SetTimer(VTIM_KEY,KEY_REPEAT_TIME,0);
						return DOWN;
					}
					else
					{
						return KEY_HOLD;
					}
			}
			else
			{
			  bPrevious_key = DOWN;
					vtimer_SetTimer(VTIM_KEY,KEY_HOLD_TIME,0);
			  return DOWN;
			}
		}
	#endif

	/* No key is pressed */
	bPrevious_key = NOKEY;
	return NOKEY;
}

/*******************************************************************************
* Function Name  : KEYS_process
* Description    : Get the key pressed and comunicate it to 
									 the user interface 
* Input          : none
* Output         : Return code of the key pressed force the 
									 display refresh if required
* Return         : u8
*******************************************************************************/
u8 keysProcess(void)
{
	#if (!defined(JOYSTICK) && defined(AUTO_START_UP))
		return NOKEY;
	#else
		u8 bKey;
		bKey = keysRead();    // read key pushed (if any...)

		switch(bKey)
		{  
		#ifdef JOYSTICK
			case UP:
				{
					UserInterface_UpDownKey(g_pUserInterface->bField_Focus_Selection,INC_SEL);
				}
			break;
			
			case DOWN:
				UserInterface_UpDownKey(g_pUserInterface->bField_Focus_Selection,DEC_SEL);
			break;
			
			case RIGHT:
				UserInterface_ChangeTab(INC_SEL);
			break;
			
			case LEFT:
				UserInterface_ChangeTab(DEC_SEL);
			break;
			
			case JOY_SEL:
				UserInterface_EditField();
			break; 
		#endif
		#ifndef AUTO_START_UP
			case USER_SEL:
			// The user interface should be updated if the state is Idle
			// Switch to tab 1 to see the speed
				UserInterface_ChangeToTab(g_pUserInterface->bDefaultTab);
			break;
		#endif
		default:
		break;
		}  
		return bKey;
	#endif
}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
