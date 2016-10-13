//----------------------------------------------//
//Here are the processor specific functions.	//
//						//
//----------------------------------------------//
#ifndef _HARD_WARE_H_
#define _HARD_WARE_H_

//#include <MSP430x23x0.h>     		
#include <stdio.h>
#include "Globe.h"
#include "types.h"

//===============================================================

#define ENABLE_INTERRUPTS enableInterrupts()  //¿ªÖÐ¶Ï
#define TRIGGER		0						// if TRIGGER 1 trigger-point at LED 5


//========MCU constants===================================

#define TRF_WRITE 	//P4OUT		//port4 is connected to the
#define TRF_READ  	//P4IN		//TRF796x IO port.
#define TRF_DIR_IN	//P4DIR = 0x00
#define TRF_DIR_OUT	//P4DIR = 0xFF
#define TRF_FUNC		//P4SEL = 0x00

#define ENABLE_SET	GPIOE->DDR |= GPIO_PIN_6   //P1DIR |= BIT0
#define	TRF_ENABLE	GPIO_WriteHigh(GPIOE,GPIO_PIN_6) //P1OUT |= BIT0		//EN pin on the TRF796x
#define TRF_DISABLE	GPIO_WriteLow(GPIOE,GPIO_PIN_6) //P1OUT &= ~BIT0

//PIN operations---------------------------------------------
#define CLK_P_OUT_SET 	//P3DIR |= BIT3		//DATA_CLK on P3.3 (P3.3/UCB0CLK used in GPIO Mode for Parallel operation)
#define CLK_ON		//P3OUT |= BIT3
#define CLK_OFF		//P3OUT &= ~BIT3

#define DIRECT_CLK		//BIT5
#define	DIRECT_PORT		//P4IN
#define DIRECT_ON		//P4IE |= BIT5
#define DIRECT_OFF		//P4IE &= ~BIT5
#define DIRECT_EDGE		//P4IES &= ~BIT5
#define DIRECT_CLR		//P4IFG = 0x00

#define MOD_SET			//P2DIR |= BIT0;
#define MOD_ON			//P2OUT |= BIT0
#define MOD_OFF			//P2OUT &= ~BIT0;


#define IRQ_PIN_SET	GPIOC->DDR &= ~GPIO_PIN_4   //P2DIR &= ~BIT1;
#define IRQ_PIN		GPIO_PIN_4 //BIT1
#define IRQ_PORT		GPIOC->IDR //P2IN
#define IRQ_ON		GPIOC->CR2 |= GPIO_PIN_4 //P2IE |= BIT1		//IRQ on P2.1
#define IRQ_OFF		GPIOC->CR2 &= ~GPIO_PIN_4 //P2IE &= ~BIT1		//IRQ on P2.1  
#define IRQ_EDGE_SET	EXTI->CR1 |= 0X10  //P2IES &= ~BIT1		//rising edge interrupt
#define IRQ_CLR		//P2IFG = 0x00
#define IRQ_REQ_REG	//P2IFG

#define LED_PORT_SET	//P1DIR |= 0xFC;
#define LED_ALL_OFF		//P1OUT &= ~0xFC;
#define LED_ALL_ON		//P1OUT |= 0xFC;
#define LED_POWER_ON	//P1OUT |= BIT7;
#define LED_POWER_OFF	//P1OUT &= ~BIT7;
#define LED_14443A_ON	//P1OUT |= BIT6;
#define LED_14443A_OFF	//P1OUT &= ~BIT6;
#define LED_14443B_ON	//P1OUT |= BIT5;
#define LED_14443B_OFF	//P1OUT &= ~BIT5;
#define LED_15693_ON	LED_OPTION_PROCESS(LED_3,ON)//P1OUT |= BIT4;
#define LED_15693_OFF	LED_OPTION_PROCESS(LED_3,OFF)//P1OUT &= ~BIT4;
#define LED_OPEN1_ON	//P1OUT |= BIT3;
#define LED_OPEN1_OFF	//P1OUT &= ~BIT3;
#define LED_OPEN2_ON 	//P1OUT |= BIT2;
#define LED_OPEN2_OFF 	//P1OUT &= ~BIT2;

// #define SPIMODE				0				// This is set to Vcc for parallel mode regardless of the jumper at GND or VCC)
// #define SPIMODE				1
#define SPIMODE             1 //P2IN & BIT3   //This is set to Vcc for SPI mode and GND for Parallel Mode using a separate jumper

#define SLAVE_SELECT_PORT_SET  GPIOE->DDR |= GPIO_PIN_5  //P3DIR |= BIT0;
#define SLAVE_SELECT_HIGH     GPIO_WriteHigh(GPIOE,GPIO_PIN_5) // P3OUT |= BIT0;
#define SLAVE_SELECT_LOW      GPIO_WriteLow(GPIOE,GPIO_PIN_5) //P3OUT &= ~BIT0;

#define TRF_SPI_SCLK_H GPIO_WriteHigh(GPIOC,GPIO_PIN_5)
#define TRF_SPI_SCLK_L GPIO_WriteLow(GPIOC,GPIO_PIN_5)

#define OOK_DIR_IN	//P2DIR &= ~BIT2;
#define OOK_DIR_OUT	//P2DIR |= BIT2
#define OOK_OFF		//P2OUT &= ~BIT2
#define OOK_ON		//P2OUT |= BIT2
//Counter-timer constants------------------------------------
#define COUNT_VALUE	 TIM3->ARRL////TACCR0			//counter register
//#define startCounter	TACTL |= MC0 + MC1	//start counter in up/down mode
#define START_COUNTER	TIM3_Cmd(ENABLE)//TACTL |=  MC1	//start counter in up mode
#define CLEAR_COUNTER	TIM3_SetCounter(0x0000);//TAR = 0x0000;
#define STOP_COUNTER	TIM3_Cmd(DISABLE) //TACTL &= ~(MC0 + MC1)	//stops the counter

//---------------------------------------------------------------

#define COUNT_1ms		1//847
#define COUNT_60ms		0xC684

//---------------------------------------------------------------

#define DELAY_1ms		6780

#define McuCounterSet() TIMER3_INIT(TIM3_PRESCALER_128,124)

//===============================================================

void Msp430f23x0DelayMillisecond(u32_t n_ms);
void Msp430f23x0OscSel(u08_t mode);
void Msp430f23x0CounterSet(void);


typedef struct rfid
{
  u8 RFID_DELAY_PROTECT_FLAG;
  u16 RFID_DELAY_threshold;
  bool rfid_delay_done;
  u16 delay_cnt;
  u16 delay_ms_cnt;
}rfid_protect;

extern rfid_protect rfid_delay_protect;

#endif
