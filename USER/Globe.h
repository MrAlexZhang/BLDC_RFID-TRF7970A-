 
/************************************************************************************************************************
                ��Ȩ���� :����ҽ�Ƽ������޹�˾

*************************************************************************************************************************
       �ļ���:Globe.h
       
       ��������:
       
	�汾��:V2-00-01		
       ���� : Zhu Jianlou       
       ����:
   

************************************************************************************************************************/
//#ifndef globe_h_
//#define globe_h_

#include <math.h>
#include <string.h>
#include <stdlib.h>			// general purpose standard library
#include <stdio.h>			// standard input/output header

#include "stm8s.h"
#include "stm8s_rst.h"
#include "stm8s_tim1.h"

#include "SystemInit.h"

#include "Can.h"

#include "hardware.h"

#include "host.h"
#include "iso14443a.h"
#include "iso14443b.h"
#include "iso15693.h"
#include "mcu.h"
#include "trf797x.h"
#include "types.h"
#include "uart.h"

#include "motorcontral.h"

#include "BLDCcontral.h"

#include "inoutport.h"

#include "mc_statemachine.h"
#include "mc_bldc_motor.h"


/********************************************************************************************************************/


/*��Ϣѭ�����-------------------------------------------------*/
#define MESSAGECOUNT 8;

/*--------------------------------------------------------------------
����ȫ�ƣ�ʮ����ǿ��ת��Ϊʮ������
�������ܣ�
ע�����
��ʾ˵��������changeIntToHex��33����return (dec/10*16 + dec%10) = 0x33
��    �룺
��    �أ�
--------------------------------------------------------------------*/
#define changeIntToHex(dec)		( ( ((dec)/10) <<4 ) + ((dec)%10) )

/*--------------------------------------------------------------------
����ȫ�ƣ�ʮ������ǿ��ת��Ϊʮ����
�������ܣ�
ע��������α���Ϊ unsigned ���ͣ�������λ�����������һ��
��ʾ˵��������changeHexToInt��0x33����return (hex/16*10 + hex%16) = 33
��    �룺
��    �أ�
--------------------------------------------------------------------*/
#define changeHexToInt(hex)		( ( ((hex) >> 4) * 10 ) + ((hex) % 16) )

/*--------------------------------------------------------------------
����ȫ�ƣ�ʮ���ƻ�Ϊʮ�����ƣ�����ʮ�����Ƹ�ʽ����
�������ܣ�
ע��������α���Ϊ unsigned ���ͣ�������λ�����������һ��
��ʾ˵��������converseIntToHex��47����return (dec/16*10 + dec%16) = 0x2F
��    �룺
��    �أ�
--------------------------------------------------------------------*/
#define converseIntToHex(dec)	( ( ((dec) >> 4) * 10 ) + ((dec) % 16) )

/*--------------------------------------------------------------------
����ȫ�ƣ�ʮ�����ƻ�Ϊʮ���ƣ�������ʮ�����Ƹ�ʽ����
�������ܣ�
ע�����
��ʾ˵��������converseHexToInt��0x2F����return 47
��    �룺
��    �أ�
--------------------------------------------------------------------*/
#define converseHexToInt(hex)	( ( ((hex) / 10) << 4 ) + ((hex) % 10) )


///////////////////HC595 PIN ///////////////////////////
#define HC595_SDI_H GPIO_WriteHigh(GPIOG,GPIO_PIN_2)
#define HC595_SDI_L GPIO_WriteLow(GPIOG,GPIO_PIN_2)

#define HC595_SRCLK_H GPIO_WriteHigh(GPIOG,GPIO_PIN_3)
#define HC595_SRCLK_L GPIO_WriteLow(GPIOG,GPIO_PIN_3)

#define HC595_RCLK_H GPIO_WriteHigh(GPIOG,GPIO_PIN_4)
#define HC595_RCLK_L GPIO_WriteLow(GPIOG,GPIO_PIN_4)



//#endif


