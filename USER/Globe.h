 
/************************************************************************************************************************
                版权所有 :蝶和医疗技术有限公司

*************************************************************************************************************************
       文件名:Globe.h
       
       功能描述:
       
	版本号:V2-00-01		
       作者 : Zhu Jianlou       
       日期:
   

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


/*消息循环相关-------------------------------------------------*/
#define MESSAGECOUNT 8;

/*--------------------------------------------------------------------
函数全称：十进制强制转换为十六进制
函数功能：
注意事项：
提示说明：调用changeIntToHex（33），return (dec/10*16 + dec%10) = 0x33
输    入：
返    回：
--------------------------------------------------------------------*/
#define changeIntToHex(dec)		( ( ((dec)/10) <<4 ) + ((dec)%10) )

/*--------------------------------------------------------------------
函数全称：十六进制强制转换为十进制
函数功能：
注意事项：传参必须为 unsigned 类型，否则移位结果可能吓你一跳
提示说明：调用changeHexToInt（0x33），return (hex/16*10 + hex%16) = 33
输    入：
返    回：
--------------------------------------------------------------------*/
#define changeHexToInt(hex)		( ( ((hex) >> 4) * 10 ) + ((hex) % 16) )

/*--------------------------------------------------------------------
函数全称：十进制化为十六进制，并以十六进制格式返回
函数功能：
注意事项：传参必须为 unsigned 类型，否则移位结果可能吓你一跳
提示说明：调用converseIntToHex（47），return (dec/16*10 + dec%16) = 0x2F
输    入：
返    回：
--------------------------------------------------------------------*/
#define converseIntToHex(dec)	( ( ((dec) >> 4) * 10 ) + ((dec) % 16) )

/*--------------------------------------------------------------------
函数全称：十六进制化为十进制，，并以十六进制格式返回
函数功能：
注意事项：
提示说明：调用converseHexToInt（0x2F），return 47
输    入：
返    回：
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


