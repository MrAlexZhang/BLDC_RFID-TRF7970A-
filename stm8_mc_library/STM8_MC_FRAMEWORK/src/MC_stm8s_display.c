/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_stm8s_display.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Low level management of textual display
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
#include "MC_dev_lcd.h"
#include "MC_dev_display.h"

/* Global private vars ********************************************************/
static pu8 pbuffer;
static u16 bufferSize;

/******************************************************************************/
void dev_displayInit(pvdev_device_t pdevice)
{
	u16 i;
	// Display buffer and size 
	pbuffer = pdevice->mems.m8;
	bufferSize = pdevice->mems.m8size;
	
	/* Initialize SPI */
	SPI_DeInit();
	/* Set frequency to fcpu/32 */
	SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_32, SPI_MODE_MASTER,
						SPI_CLOCKPOLARITY_HIGH, SPI_CLOCKPHASE_2EDGE, SPI_DATADIRECTION_1LINE_TX,
						SPI_NSS_SOFT, 0x07);
	SPI_Cmd(ENABLE);

	/* Wait 40ms */
	for (i=0; i<40000; i++)
	{
		/* Wait */
	}

	/* Initialize LCD */
	LCD_Init();
	
	dev_displayClear();
}

void dev_displayClear(void)
{
	LCD_Clear();
}

void dev_displayFlush(void)
{
	static u8 i;
	// Clear LCD lines 
	LCD_Clear();
	
	LCD_SetCursorPos(LCD_LINE1,0);
	for (i=0;i<15;i++)
	{
		LCD_PrintChar(pbuffer[i]);
	}
	LCD_SetCursorPos(LCD_LINE2,0);
	for (i=0;i<15;i++)
	{
		LCD_PrintChar(pbuffer[i+15]);
	}
}

void dev_displayPrintch()
{
	if (pbuffer[DISPLAYLINE_ADDR] == 1)
	{
		LCD_SetCursorPos(LCD_LINE1,(u8)(pbuffer[DISPLAYPOS_ADDR] >> 1));
	}
	else
	{
		LCD_SetCursorPos(LCD_LINE2,(u8)(pbuffer[DISPLAYPOS_ADDR] >> 1));
	}
	LCD_PrintChar(pbuffer[DISPLAYCH_ADDR]);
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

