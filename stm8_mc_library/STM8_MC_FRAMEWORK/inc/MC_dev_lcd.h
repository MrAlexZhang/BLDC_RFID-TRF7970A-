/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : MC_dev_lcd.h
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Prototype definition for MC_stm8s_lcd.c
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
#ifndef __MC_DEV_LCD_H
#define __MC_DEV_LCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s_lib.h"

/* Exported constants --------------------------------------------------------*/

/* LCD managed as 2 Lines of 15 characters (2Lines * 15Char) */
#define LCD_LINE_MAX_CHAR ((u8)15)
#define LCD_LINE1         ((u8)0x80)
#define LCD_LINE2         ((u8)0x90)

/* LCD Commands */
#define COMMAND_TYPE       ((u8)0xF8)
#define DATA_TYPE          ((u8)0xFA)
#define SET_TEXT_MODE      ((u8)0x30) /* 8-Bits Interface, Normal inst., Text mode */
#define SET_EXTENDED_INST  ((u8)0x34)
#define SET_GRAPHIC_MODE   ((u8)0x36) /* 8-Bits Interface, Extended inst., Graphic mode */
#define DISPLAY_ON         ((u8)0x0C) /* Cursor and blink off */
#define DISPLAY_OFF        ((u8)0x08)
#define DISPLAY_CLR        ((u8)0x01)
#define ENTRY_MODE_SET_INC ((u8)0x06)

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void LCD_ReadStatus(void);
void LCD_ChipSelect(FunctionalState NewState);
void LCD_SendByte(u8 DataType, u8 DataToSend);
void LCD_SendBuffer(u8 *pTxBuffer, u8 *pRxBuffer, u8 NumByte);
void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetTextMode(void);
void LCD_SetGraphicMode(void);
void LCD_ClearLine(u8 Line);
void LCD_SetCursorPos(u8 Line, u8 Offset);
void LCD_PrintChar(u8 Ascii);
void LCD_PrintString(u8 Line, FunctionalState AutoComplete, FunctionalState Append, u8 *ptr);
void LCD_PrintMsg(u8 *ptr);
void LCD_Print(u8 *ptr);
void LCD_PrintDec1(u8 Number);
void LCD_PrintDec2(u8 Number);
void LCD_PrintDec3(u16 Number);
void LCD_PrintDec4(u16 Number);
void LCD_PrintHex1(u8 Number);
void LCD_PrintHex2(u8 Number);
void LCD_PrintHex3(u16 Number);
void LCD_PrintBin2(u8 Number);
void LCD_PrintBin4(u8 Number);
void LCD_DisplayCGRAM0(u8 address, u8 *ptrTable);
void LCD_DisplayCGRAM1(u8 address, u8 *ptrTable);
void LCD_DisplayLogo(u8 address);
void LCD_RollString(u8 Line, u8 *ptr, u16 speed);

#endif /* __MC_DEV_LCD_H */
/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
