/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_BLDC_User_Interface.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Holder of user interface structure for BLDC application
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
#include "MC_User_Interface.h"
#include "MC_BLDC_User_Interface_Param.h"

_EEXTRAFIELD_INSTANCE_N
_FIELD_INSTANCE_N

Tab_t sTab[TAB_NUM]= TAB_N;

UserInterface_t UserInterface =
{
	UI_UNLOCKED, //bStatus
	0, //bSelected_TAB
	0, //pSelTab
	0, //bField_Focus_Selection
	0, //bField_Edit
	TAB_NUM, //bTabsNumber
	sTab, // Tabs
	UI_DEFAULT_TAB, // bDefaultTab
	WELCOME_MSG_LINE1,// pWelcomeMSG_Line1
	WELCOME_MSG_LINE2// pWelcomeMSG_Line2
};

PUserInterface_t Get_UserInterface(void)
{
	return &UserInterface;
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
