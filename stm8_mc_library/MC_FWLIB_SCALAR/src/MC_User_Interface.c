/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_user_interface.c
* Author             : IMS Systems Lab  
* Date First Issued  : mm/dd/yyy
* Description        : This moudles handle the user interface
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
#include "MC_Faults.h"
#include "MC_vtimer.h"

/* Private Function */
PTab_t UserInterface_GetSelTab(void);
s16 UserInterface_UpdateField(PEditExtraField_t pExtra,s16 bVal,EditAction_t act);

/* Private Variables */
PUserInterface_t g_pUserInterface; // Pointer to the real user interface
const u8 HW_Error_Message[HW_ERROR_NUM][15]={
  HW_ERROR_MSG_1,
  HW_ERROR_MSG_2,
  HW_ERROR_MSG_3,
  HW_ERROR_MSG_4
};

const u8 SW_Error_Message[SW_ERROR_NUM][15]={
  SW_ERROR_MSG_1,
  SW_ERROR_MSG_2,
  SW_ERROR_MSG_3,
  SW_ERROR_MSG_4
};

static u8 bErrorCode = NO_FAULT;

static pu16 pHWerrorOccurred_reg,pHWerrorActual_reg;
u16 hHWErrorMask = 0;
u8 bHWErrorIndex;

/*----------------------------------------------------------------*/

PTab_t UserInterface_GetSelTab(void)
{
	return g_pUserInterface->pSelTab = &(g_pUserInterface->pTab[g_pUserInterface->bSelected_Tab]); // Get the Selectercd tab pointer
}

//const u8* UserInterface_GetErrorMsg(void)
//{
//  return Error_Message[bErrorCode];
//}
//
//const u8* UserInterface_GetErrorStatusMsg(void)
//{
//  return "FAULT CONDITION";
//}



void UserInterface_GetErrorMsg(PUserInterfaceErrorMsg_t pUIErrMsg)
{
  if (vtimer_TimerElapsed(VTIM_USER_INTERFACE_REFRESH))
  {
    if (hHWErrorMask == 0)
    {
      // Check status error and return it
      if (bErrorCode != NO_FAULT)
      {
        pUIErrMsg->pErrorStatus = "FAULT OCCURRED";
        pUIErrMsg->pErrorMsg =  SW_Error_Message[bErrorCode - 1];
				vtimer_SetTimer(VTIM_USER_INTERFACE_REFRESH,1000,0);
      }
      hHWErrorMask = 1;
			bHWErrorIndex = 0;
    }
    else
    {
      // Check hardware error and return it
      if ((*pHWerrorOccurred_reg & hHWErrorMask) != 0)
      {
        if ((*pHWerrorActual_reg & hHWErrorMask) != 0)
        {
          pUIErrMsg->pErrorStatus = "FAULT CONDITION";
        }
        else
        {
          pUIErrMsg->pErrorStatus = "FAULT OCCURRED";
        }
        pUIErrMsg->pErrorMsg =  HW_Error_Message[bHWErrorIndex];
				vtimer_SetTimer(VTIM_USER_INTERFACE_REFRESH,1000,0);
      }
      hHWErrorMask <<= 1;
			bHWErrorIndex++;
      if (hHWErrorMask == HW_ERROR_NUM_MASK)
      {
        hHWErrorMask = 0;
      }
    }
  }
  
}

void UserInterface_Init(pvdev_device_t pdevice,PUserInterface_t pUserInterface)
{
	g_pUserInterface = pUserInterface;
	g_pUserInterface->bSelected_Tab = 0; //Reset Selected Tab
	g_pUserInterface->pSelTab = &(g_pUserInterface->pTab[0]); // Set the Selectercd tab (0) pointer
	
	g_pUserInterface->bField_Edit = g_pUserInterface->pSelTab->bFieldsNumber; // No field is in edit mode
	UserInterface_ResetFocus();
        
        pHWerrorOccurred_reg = (pdevice->regs.r16+VDEV_REG16_HW_ERROR_OCCURRED);
        pHWerrorActual_reg = (pdevice->regs.r16+VDEV_REG16_HW_ERROR_ACTUAL);
}

void UserInterface_ResetFocus(void)
{
	u8 i;
	for (i = 0; i < g_pUserInterface->pSelTab->bFieldsNumber; i++)
	{
		if (g_pUserInterface->pSelTab->pField[i].Type == EDIT)
		{
			g_pUserInterface->bField_Focus_Selection = i;
			return;
		}
	}
	g_pUserInterface->bField_Focus_Selection = g_pUserInterface->pSelTab->bFieldsNumber;
}

u8 UserInterface_UpField(u8 StartingSel)
{
	u8 Sel = StartingSel;
	
	do
	{
		Sel += g_pUserInterface->pSelTab->bFieldsNumber;
		Sel --;
		Sel %= g_pUserInterface->pSelTab->bFieldsNumber;
		if (g_pUserInterface->pSelTab->pField[Sel].Type == EDIT)
		{
			g_pUserInterface->bField_Focus_Selection = Sel;
			return TRUE;
		}
	}
	while (Sel == StartingSel);
	return FALSE;
}

u8 UserInterface_DownField(u8 StartingSel)
{
	u8 Sel = StartingSel;
	
	do
	{
		Sel ++;
		Sel %= g_pUserInterface->pSelTab->bFieldsNumber;
		if (g_pUserInterface->pSelTab->pField[Sel].Type == EDIT)
		{
			g_pUserInterface->bField_Focus_Selection = Sel;
			return TRUE;
		}
	}
	while (Sel == StartingSel);
	return FALSE;
}

u8 UserInterface_UpDownKey(u8 StartingSel,EditAction_t act)
{
	if (g_pUserInterface->bStatus == UI_LOCKED)
		return FALSE;

	if (g_pUserInterface->bField_Edit == g_pUserInterface->pSelTab->bFieldsNumber)
	{
		if (act == INC_SEL)
		{
			return UserInterface_UpField(StartingSel);
		}
		else
		{
			return UserInterface_DownField(StartingSel);
		}
	}
	else
	{
		return UserInterface_IncField(act);
	}
}

u8 UserInterface_ChangeToTab(u8 bSel)
{
	if (g_pUserInterface->bStatus == UI_LOCKED)
		return FALSE;
		
	g_pUserInterface->bSelected_Tab = bSel; 
	g_pUserInterface->pSelTab = &(g_pUserInterface->pTab[bSel]); 
	
	g_pUserInterface->bField_Edit = g_pUserInterface->pSelTab->bFieldsNumber; // No field is in edit mode
	UserInterface_ResetFocus();
	
	return TRUE;
}

u8 UserInterface_ChangeTab(EditAction_t act)
{
	if (g_pUserInterface->bStatus == UI_LOCKED)
		return FALSE;
	if (g_pUserInterface->bField_Edit != g_pUserInterface->pSelTab->bFieldsNumber)
	{
		g_pUserInterface->bField_Edit = g_pUserInterface->pSelTab->bFieldsNumber;
	}

	// Change TAB
	if (act == INC_SEL)
	{
		g_pUserInterface->bSelected_Tab++;
		g_pUserInterface->bSelected_Tab %= g_pUserInterface->bTabsNumber;
		g_pUserInterface->pSelTab = UserInterface_GetSelTab();
		UserInterface_ResetFocus();
		return TRUE;
	}
	else
	{
		g_pUserInterface->bSelected_Tab += g_pUserInterface->bTabsNumber;
		g_pUserInterface->bSelected_Tab--;
		g_pUserInterface->bSelected_Tab %= g_pUserInterface->bTabsNumber;
		g_pUserInterface->pSelTab = UserInterface_GetSelTab();
		UserInterface_ResetFocus();
		return TRUE;
	}
}

u8 UserInterface_EditField(void)
{
	if (g_pUserInterface->bStatus == UI_LOCKED)
		return FALSE;

	if (g_pUserInterface->bField_Edit == g_pUserInterface->pSelTab->bFieldsNumber)
	{
		g_pUserInterface->bField_Edit = g_pUserInterface->bField_Focus_Selection;
		return TRUE;
	}
	else
	{
		g_pUserInterface->bField_Edit = g_pUserInterface->pSelTab->bFieldsNumber;
		return TRUE;
	}
}

u8 UserInterface_IncField(EditAction_t act)
{
	u8 selField = g_pUserInterface->bField_Focus_Selection;
	PField_t pSelField = &(g_pUserInterface->pSelTab->pField[selField]);

	switch (pSelField->Size)
	{
		case TYPE_U8:
		{
			u8 bVal=((PFN_getu8_t)pSelField->pGetValue)();
			bVal = (u8)(UserInterface_UpdateField(pSelField->pExtra,(s16)(bVal),act));
			((PFN_setu8_t)pSelField->pExtra->pSetValue)(bVal);
			break;
		}
		case TYPE_S16:
		case TYPE_S16_NU:
		{
			s16 hVal=((PFN_gets16_t)pSelField->pGetValue)();
			hVal = (s16)(UserInterface_UpdateField(pSelField->pExtra,hVal,act));
			((PFN_sets16_t)pSelField->pExtra->pSetValue)(hVal);
			break;
		}
		case TYPE_F_2_1:
		{
			s16 hVal=((PFN_gets16_t)pSelField->pGetValue)();
			hVal = (s16)(UserInterface_UpdateField(pSelField->pExtra,hVal,act));
			((PFN_sets16_t)pSelField->pExtra->pSetValue)(hVal);
			break;
                }
		case TYPE_BOOL:
		{
			((PFN_setu8_t)pSelField->pExtra->pSetValue)((u8)(!((PFN_getu8_t)pSelField->pGetValue)()));
			break;
		}                        
	default:
		break;
	}
	return TRUE;
}

s16 UserInterface_UpdateField(PEditExtraField_t pExtra,s16 hVal,EditAction_t act)
{
	s16 hDelta;
	if (act == INC_SEL)
	{
		hDelta = pExtra->hMaxValue - pExtra->hInc_decValue;
		if (hVal <= hDelta)
		{
			hVal += pExtra->hInc_decValue;
		}
		else
		{
			hVal = pExtra->hMaxValue;
		}
	}
	else
	{
		hDelta = (pExtra->hMinValue + pExtra->hInc_decValue);
		if (hVal >= hDelta)
		{
			hVal -= pExtra->hInc_decValue;
		}
		else
		{
			hVal = pExtra->hMinValue;
		}
	}
	return hVal;
}

void UserInterface_Lock(void)
{
	g_pUserInterface->bStatus = UI_LOCKED;
}

void UserInterface_Unlock(void)
{
	g_pUserInterface->bStatus = UI_UNLOCKED;
        bErrorCode = NO_FAULT;
}

void UserInterface_Fault (u8 ErrorCode)
{
  bErrorCode = ErrorCode;
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
