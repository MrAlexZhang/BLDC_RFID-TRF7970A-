/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_display.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : This module handles a text display
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
#include "MC_display.h"
#include "MC_vtimer.h"

typedef struct
{
	display_index_t SX;
	display_index_t SY;
	display_index_t Cur_x;
	display_index_t Cur_y;
} DEV_Display_t, *PDEV_Display_t;

DEV_Display_t DEV_Display = 
{
	DEV_Display_COL,
	DEV_Display_ROW,
	0,
	0
};
/*****************************************************************************/
// Private variables
static u8 bBlinkState = 0;
static PUserInterface_t g_pUserInterface = 0;
static pvdev_device_t g_pdevice = 0;
static PTab_t g_pSelTab = 0;

// Private functions
void display_clsscr(pvdev_device_t pdevice);
void display_setcurrpos(display_index_t x, display_index_t y);
void display_printchar(pvdev_device_t pdevice,char ch);
void display_printstr(pvdev_device_t pdevice,char *pStr);
void APP_sPrintDec(u8* pStr, u16 Number, u8 Digit);
void display_create(pvdev_device_t pdevice, PUserInterface_t pUserInterface);
void display_create_locked_screen(void);

void displayInit(pvdev_device_t pDevice,PUserInterface_t pUserInterface)
{
	g_pUserInterface = pUserInterface;
	g_pdevice = pDevice;
	
	display_welcome_message();
	vtimer_SetTimer(VTIM_DISPLAY_REFRESH,DISPLAY_REFRESH_TIME,0);
}

void display_clsscr(pvdev_device_t pdevice)
{
	display_index_t i,j;
	for (j = 0; j < DEV_Display_ROW; j++)
	{
		for (i = 0; i < DEV_Display_COL; i++)
		{
			pdevice->mems.m8[j*DEV_Display_COL + i] = 32; // Space ascii
		}
	}
}

void display_setcurrpos(display_index_t x, display_index_t y)
{
	if (x > 0)
	{
		x--;
		if (x < DEV_Display_COL)
		{
			DEV_Display.Cur_x = x;
		}
	}
	if (y > 0)
	{
		y--;
		if (y < DEV_Display_ROW)
		{
			DEV_Display.Cur_y = y;
		}
	}
}

void display_printchar(pvdev_device_t pdevice,char ch)
{
	pdevice->mems.m8[DEV_Display.Cur_y * DEV_Display_COL + DEV_Display.Cur_x] = ch;
}

void display_printstr(pvdev_device_t pdevice,char *pStr)
{
	while ((*pStr != 0) && (DEV_Display.Cur_x < DEV_Display_COL))
	{
		pdevice->mems.m8[DEV_Display.Cur_y * DEV_Display_COL + DEV_Display.Cur_x] = *pStr;
		DEV_Display.Cur_x++;
		pStr++;
	}
	if (DEV_Display.Cur_x >= DEV_Display_COL)
		DEV_Display.Cur_x = DEV_Display_COL - 1; 
}

void display_flush(void)
{
	if (g_pUserInterface->bStatus == UI_UNLOCKED)
	{
		// Write into the Display buffer the user interface
		display_create(g_pdevice, g_pUserInterface);
	}
	else
	{
		// Write into the Display buffer the user interface locked screen
		display_create_locked_screen();
	}
	// Calls the virtual i/o the display showing
	g_pdevice->ios.out8(VDEV_OUT8_DISPLAY_FLUSH,0);
}

// Number < 10000;
// String is already defined whith length 5
void APP_sPrintDec(u8* pStr, u16 Number, u8 Digit)
{
  u16 Nbre1Tmp;
  u16 Nbre2Tmp;

  //if (Number < (u16)10000)
  {
		/* Display first digit of the number : 10000 */
		Nbre1Tmp = (u16)(Number / (u16)10000);
		if (Digit > 4)
		{	
			*pStr = (u8)(Nbre1Tmp + (u8)0x30);
			pStr++;
		}

		/* Display first digit of the number : 1000 */
		//Nbre1Tmp = (u16)(Number / (u16)1000);
		Nbre1Tmp = (u16)(Number - ((u16)10000 * Nbre1Tmp));
		Nbre2Tmp = (u16)(Nbre1Tmp / (u16)1000);
		if (Digit > 3)
		{	
			*pStr = (u8)(Nbre2Tmp + (u8)0x30);
			pStr++;
		}
		
		
		/* Display second digit of the number : 100 */
		Nbre1Tmp = (u16)(Nbre1Tmp - ((u16)1000 * Nbre2Tmp));
		Nbre2Tmp = (u16)(Nbre1Tmp / (u16)100);
		if (Digit > 2)
		{		
			*pStr = (u8)(Nbre2Tmp + (u8)0x30);
			pStr ++;
		}
   
		
		/* Display second digit of the number : 10 */
		Nbre1Tmp = (u16)(Nbre1Tmp - ((u16)100 * Nbre2Tmp));
		Nbre2Tmp = (u16)(Nbre1Tmp / (u16)10);
		if (Digit > 1)
		{
			*pStr = (u8)(Nbre2Tmp + (u8)0x30);
			pStr++;
		}
   
    /* Display last digit of the number : Units */
    Nbre1Tmp = ((u16)(Nbre1Tmp - (u16)((u16)10 * Nbre2Tmp)));
		*pStr = (u8)(Nbre1Tmp + (u8)0x30);
		
		pStr++;
		*pStr = 0; // End of string
  }
}

void display_setpoint_blink(void)
{
	u8 i;
	
	if (g_pUserInterface == 0) return;
	if (g_pUserInterface->bStatus == UI_LOCKED) return;
	if (g_pdevice == 0) return;
	if (g_pSelTab == 0) return;
	for (i=0;i<g_pSelTab->bFieldsNumber;i++)
	{
		if (g_pUserInterface->bField_Focus_Selection == i)
		{
			if (g_pUserInterface->bField_Edit != i)
			{
				if (bBlinkState == 0)
				{
					bBlinkState = 1;
					// Usare una virtual I/O
					g_pdevice->mems.m8[DISPLAYLINE_ADDR]=(u8)(i+1);
					g_pdevice->mems.m8[DISPLAYPOS_ADDR]=8;
					g_pdevice->mems.m8[DISPLAYCH_ADDR]=16;
					g_pdevice->ios.out8(VDEV_OUT8_DISPLAY_PRINTCH,0);
					//g_pdevice->display.displayprintch(i+1,8,16); // Field Editable
				}
				else
				{
					bBlinkState = 0;
					// Usare una virtual I/O
					g_pdevice->mems.m8[DISPLAYLINE_ADDR]=(u8)(i+1);
					g_pdevice->mems.m8[DISPLAYPOS_ADDR]=8;
					g_pdevice->mems.m8[DISPLAYCH_ADDR]=' ';
					g_pdevice->ios.out8(VDEV_OUT8_DISPLAY_PRINTCH,0);
					//g_pdevice->display.displayprintch(i+1,8,' '); // Field Blinking
				}
			}
		}
	}
}

void display_create(pvdev_device_t pdevice, PUserInterface_t pUserInterface)
{
	u8 i,j;
	u8 Out[6];
	u8 selTab = pUserInterface->bSelected_Tab;
	PTab_t pSelTab = &(pUserInterface->pTab[selTab]);
	g_pSelTab = pSelTab;
	
	display_clsscr(pdevice);
	
	for (i=0;i<pSelTab->bFieldsNumber;i++)
	{
		ValSize_t Size = pSelTab->pField[i].Size;
		
		// Print Label
		display_setcurrpos((display_index_t)1,(display_index_t)(i+1));
		display_printstr(pdevice,(char*)pSelTab->pField[i].Label);

		if (Size != TYPE_TXT)
		{
			// Manage setting point
			display_setcurrpos((display_index_t)9,(display_index_t)(i+1));
			
			if (pSelTab->pField[i].Type != EDIT)
			{
				display_printchar(pdevice,' '); // Read Only
			}
			else
			{
				if (pUserInterface->bField_Edit == i)
				{
					display_printchar(pdevice,18); // Edit mode
				}
				else
				{
					display_printchar(pdevice,16); // Field Editable
				}			
			}
		}
	
		switch (Size)
		{
			case TYPE_U8:
				display_setcurrpos((display_index_t)11,(display_index_t)(i+1));
				display_printchar(pdevice,' ');
				display_setcurrpos((display_index_t)12,(display_index_t)(i+1));
				APP_sPrintDec(Out,((PFN_getu8_t)pSelTab->pField[i].pGetValue)(),3);
				display_printstr(pdevice,(char*)Out);
			break;
			case TYPE_S8:
			break;
			case TYPE_S16:
			{
				s16 wVal=((PFN_gets16_t)pSelTab->pField[i].pGetValue)();

				display_setcurrpos((display_index_t)10,(display_index_t)(i+1));
				if (wVal < 0)
				{
					display_printchar(pdevice,'-');
					wVal = -wVal;
				}
				else
				{
					display_printchar(pdevice,' ');
				}
				display_setcurrpos((display_index_t)11,(display_index_t)(i+1));
				APP_sPrintDec(Out,wVal,4);
				display_printstr(pdevice,(char*)Out);
			}
			break;
			case TYPE_S16_NU:
			{
				s16 wVal=((PFN_gets16_t)pSelTab->pField[i].pGetValue)();

				display_setcurrpos((display_index_t)10,(display_index_t)(i+1));
				if (wVal < 0)
				{
					display_printchar(pdevice,'-');
					wVal = -wVal;
				}
				else
				{
					display_printchar(pdevice,' ');
				}
				display_setcurrpos((display_index_t)11,(display_index_t)(i+1));
				APP_sPrintDec(Out,wVal,5);
				display_printstr(pdevice,(char*)Out);
			}
			break;
			case TYPE_F_2_1:
			{
				u16 wVal=((PFN_getu16_t)pSelTab->pField[i].pGetValue)();
				u8 bTmp;
				display_setcurrpos((display_index_t)11,(display_index_t)(i+1));
				bTmp = (u8)(wVal/10);
				APP_sPrintDec(Out,bTmp,2);
				
				display_printstr(pdevice,(char*)Out);
				display_setcurrpos((display_index_t)13,(display_index_t)(i+1));
				display_printchar(pdevice,'.');
				display_setcurrpos((display_index_t)14,(display_index_t)(i+1));
				APP_sPrintDec(Out,wVal-(bTmp*10),1);
				display_printstr(pdevice,(char*)Out);
			}
			break;
			case TYPE_BOOL:
			{
				display_setcurrpos((display_index_t)11,(display_index_t)(i+1));
				display_printchar(pdevice,' ');
				display_setcurrpos((display_index_t)12,(display_index_t)(i+1));
				if (((PFN_getu8_t)pSelTab->pField[i].pGetValue)())
				{
					display_printstr(pdevice,"ON");
				}
				else
				{
					display_printstr(pdevice,"OFF");
				}
			}
		}
		
		// Print unit
		if ((Size != TYPE_TXT) && (Size != TYPE_S16_NU))
		{
			display_setcurrpos((display_index_t)15,(display_index_t)(i+1));
			display_printchar(pdevice,pSelTab->pField[i].Unit);
		}
	}

}

void display_welcome_message(void)
{	
	display_clsscr(g_pdevice);
	display_setcurrpos(1,1);
	display_printstr(g_pdevice,(char*)g_pUserInterface->pWelcomeMSG_Line1);
	display_setcurrpos(1,2);
	display_printstr(g_pdevice,(char*)g_pUserInterface->pWelcomeMSG_Line2);
	
	// Virtual I/O call to display the buffer
	g_pdevice->ios.out8(VDEV_OUT8_DISPLAY_FLUSH,0);
}

static UserInterfaceErrorMsg_t UIErrMsg;

void display_create_locked_screen(void)
{
        UIErrMsg.pErrorStatus = "";
        UserInterface_GetErrorMsg(&UIErrMsg);
        if (UIErrMsg.pErrorStatus != "")
        {
          display_clsscr(g_pdevice);
          display_setcurrpos(1,1);
          display_printstr(g_pdevice,(char*)UIErrMsg.pErrorStatus);
          display_setcurrpos(1,2);
          display_printstr(g_pdevice,(char*)UIErrMsg.pErrorMsg); //g_pUserInterface->pWelcomeMSG_Line2
        }
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
