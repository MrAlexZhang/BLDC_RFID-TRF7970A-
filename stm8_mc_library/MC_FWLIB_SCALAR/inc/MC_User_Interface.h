/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_User_Interface.h
* Author             : IMS Systems Lab
* Date First Issued  : mm/dd/yyy
* Description        : Prototype definition for MC_user_interface.c
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MC_USER_INTERFACE_H
#define __MC_USER_INTERFACE_H

#include "dev_type.h"
#include "vdev.h"

typedef enum {TYPE_U8,TYPE_S8,TYPE_S16,TYPE_S16_NU,TYPE_F_2_1,TYPE_TXT,TYPE_BOOL} ValSize_t;
typedef enum {EDIT,READ_ONLY} ValType_t;
typedef enum {INC_SEL,DEC_SEL} EditAction_t;

#define UI_UNLOCKED 0
#define UI_LOCKED 1

// Function pointer
//Get
typedef u8 (*PFN_getu8_t)(void);
typedef u16 (*PFN_getu16_t)(void);
typedef s16 (*PFN_gets16_t)(void);
//Set
typedef void (*PFN_setu8_t)(u8);
typedef void (*PFN_sets16_t)(s16);

typedef const struct
{
	void* pSetValue; // Pointer to the set variable function (8/16 bit) 
	s16 hMaxValue;
	s16 hMinValue;
	u16 hInc_decValue;
} EditExtraField_t,*PEditExtraField_t;

typedef const struct
{
	u8* Label;
	u8 Unit;
	ValType_t Type; // Edit, Read only	
	ValSize_t Size; // S8 bit, S16 bit, F2.1
	void* pGetValue; // Pointer to the get variable function (8/16 bit) 
	PEditExtraField_t pExtra; // Pointer to extra field for Edit values
} Field_t,*PField_t;

typedef const struct
{
	u8 bFieldsNumber;
	PField_t pField;
} Tab_t, *PTab_t;

typedef struct
{
	u8 bStatus;
	u8 bSelected_Tab;
	PTab_t pSelTab;
	u8 bField_Focus_Selection;
	u8 bField_Edit;
	u8 bTabsNumber;
	PTab_t pTab;
	u8 bDefaultTab;
	u8* pWelcomeMSG_Line1;
	u8* pWelcomeMSG_Line2;
} UserInterface_t,*PUserInterface_t;

typedef struct
{
  const u8* pErrorStatus;
  const u8* pErrorMsg;
} UserInterfaceErrorMsg_t,*PUserInterfaceErrorMsg_t;

// Prototypes
void UserInterface_Init(pvdev_device_t pdevice,PUserInterface_t pUserInterface);
void UserInterface_ResetFocus(void);
u8 UserInterface_UpField(u8 StartingSel);
u8 UserInterface_DownField(u8 StartingSel);
u8 UserInterface_UpDownKey(u8 StartingSel,EditAction_t act);
u8 UserInterface_ChangeToTab(u8 bSel);
u8 UserInterface_ChangeTab(EditAction_t act);
u8 UserInterface_IncField(EditAction_t act);
u8 UserInterface_EditField(void);
void UserInterface_Lock(void);
void UserInterface_Unlock(void);

PUserInterface_t Get_UserInterface(void); // Virtual
//const u8* UserInterface_GetErrorMsg(void);
//const u8* UserInterface_GetErrorStatusMsg(void);
void UserInterface_GetErrorMsg(PUserInterfaceErrorMsg_t pUIErrMsg);
void UserInterface_Fault (u8 ErrorCode);

#endif //__MC_USER_INTERFACE_H

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
