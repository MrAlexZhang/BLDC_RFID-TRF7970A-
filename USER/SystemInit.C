
#define _SystemInit_VER


#include "Globe.h"





/*******************************************************************************
*                         ==时钟初始化函数==
* CLK_DeInit();                                   --> 复位时钟寄存器
* CLK_HSICmd(ENABLE);                             --> 使能内部高速时钟
* CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV2); --> 配置内部高速时钟(时钟分频器:2分频)
* CLK_ClockSecuritySystemEnable();                --> 开启时钟安全系统
*******************************************************************************/
void CLK_INIT(void)                
{
  CLK_DeInit();
  CLK_HSICmd(ENABLE);
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  CLK_ClockSecuritySystemEnable();
  //CLK_HSECmd(ENABLE);
  
  
  
  //FLASH_Unlock(FLASH_MEMTYPE_DATA);/*要对选项字节写操作，必须要先解锁FLASH*/
  // while(!(FLASH->IAPSR & FLASH_IAPSR_DUL));/*等待解锁完毕*/
  /*如果外部时钟是大于16MHz或者小于等于24MHz,flash需要插入一个等待周期，小于或者等于16MHz则不需要*/
  
  //FLASH_ProgramOptionByte(FLASH_WAIT_STATES_ADDRESS,HSE_Clock);
  
  //FLASH_ProgramOptionByte(HSE_CLOCK_STARTUP_ADDRESS,HSECNT_8CLK);
  /*外部时钟启动稳定周期*/
  //FLASH_Lock(FLASH_MEMTYPE_DATA);/*操作完要加锁*/
  
  //while (!CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE,\
  //	   CLK_CURRENTCLOCKSTATE_DISABLE));
  
  
  
}


/*******************************************************************************
*                         == 看门狗初始化函数==
*******************************************************************************/
void iDog_INIT(void)
{
  IWDG_Enable();
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  IWDG_SetReload(0xff);
  IWDG_ReloadCounter();
}

/*******************************************************************************
*                           ==启用外部晶振函数==
*******************************************************************************/
void HSE_CLK_INIT(void)
{
  CLK_DeInit(); 
  CLK_HSECmd(ENABLE);
  while(SET != CLK_GetFlagStatus(CLK_FLAG_HSERDY));
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
  CLK_ClockSwitchCmd(ENABLE); 
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO,CLK_SOURCE_HSE,DISABLE,CLK_CURRENTCLOCKSTATE_ENABLE);
}



/*******************************************************************************
*                         ==I/O口初始化函数==
*******************************************************************************/
void GPIO_INIT(void)
{
  
  GPIO_DeInit(GPIOA);
  GPIO_DeInit(GPIOB);
  GPIO_DeInit(GPIOC);
  GPIO_DeInit(GPIOD);
  GPIO_DeInit(GPIOE);
  GPIO_DeInit(GPIOG);
  
  //  GPIO_Init(GPIOB,GPIO_PIN_ALL,GPIO_MODE_OUT_PP_HIGH_FAST);//BORID 输入
  
  GPIO_Init(GPIOG,(GPIO_Pin_TypeDef)(GPIO_PIN_0), GPIO_MODE_OUT_PP_HIGH_SLOW);//CAN 
  GPIO_Init(GPIOG,(GPIO_Pin_TypeDef)(GPIO_PIN_1), GPIO_MODE_IN_PU_NO_IT);
  
  GPIO_Init(GPIOG,(GPIO_Pin_TypeDef)(GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4), GPIO_MODE_OUT_PP_HIGH_SLOW);//HC595 PIN 
  
  
  GPIO_Init(GPIOD,(GPIO_Pin_TypeDef)(GPIO_PIN_0|GPIO_PIN_7), GPIO_MODE_OUT_PP_LOW_SLOW);//// step motor pwm ctrol  PD0--ENABLE;;PD7--RESET_CD
  GPIO_Init(GPIOG,(GPIO_Pin_TypeDef)(GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7), GPIO_MODE_OUT_PP_LOW_SLOW);//PG5--RESET_AB;;PG6 --PWM_AB;;PG7---PWM_CD
  
  GPIO_Init(GPIOA,(GPIO_Pin_TypeDef)(GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5), GPIO_MODE_OUT_PP_LOW_SLOW);//STEP MOTOR MODE CTROL
  GPIO_Init(GPIOD,(GPIO_Pin_TypeDef)GPIO_PIN_6,GPIO_MODE_IN_PU_IT); //STEP MOTOR FAULT INTERRUPT  PD6 -- OTW
  GPIO_Init(GPIOD,(GPIO_Pin_TypeDef)GPIO_PIN_5,GPIO_MODE_IN_PU_IT);//STEP MOTOR FAULT INTERRUPT   PD5 -- FAULT
  
//  GPIO_Init(GPIOA,(GPIO_Pin_TypeDef)(GPIO_PIN_1), GPIO_MODE_OUT_PP_LOW_SLOW);
  
  GPIO_Init(GPIOE , (GPIO_Pin_TypeDef)(GPIO_PIN_6), GPIO_MODE_OUT_PP_LOW_SLOW);// PE6-- TRF_EN  
  GPIO_Init(GPIOE , (GPIO_Pin_TypeDef)(GPIO_PIN_5), GPIO_MODE_OUT_PP_LOW_FAST);//PE5 --- TRF_SPI_NSS
  GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)(GPIO_PIN_6|GPIO_PIN_5), GPIO_MODE_OUT_PP_LOW_FAST);//PC6 -- TRF_SPI_MOSI;;;PC5---TRF_SPI_SCK
  GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)(GPIO_PIN_7), GPIO_MODE_IN_FL_NO_IT);//PC7 -- TRF_SPI_MISO 
  GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)(GPIO_PIN_4), GPIO_MODE_IN_FL_IT);//PC4--- TRF_IRQ 输入
  
  GPIO_Init(GPIOB,(GPIO_Pin_TypeDef)(GPIO_PIN_0|GPIO_PIN_1),GPIO_MODE_IN_PU_IT); //BLDC interrupt PB0--8332_otw pin;pb1-- 8332_fault
  GPIO_Init(GPIOB,(GPIO_Pin_TypeDef)(GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7),GPIO_MODE_OUT_PP_LOW_FAST);//PB3--RESET_A;;PB5--M1;;PB6--RESET_B;PB7--RESET_C
//  GPIO_Init(GPIOC,(GPIO_Pin_TypeDef)(GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3),GPIO_MODE_OUT_PP_LOW_FAST);//BLDC PWM PIN   PC1--PWMA;;PC2-PWMB;;PC3 --PWMC
//  GPIO_Init(GPIOD,(GPIO_Pin_TypeDef)(GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4), GPIO_MODE_IN_PU_IT);//BLDC HALL SENSOR INPUT  PD4--H1;PD3--H2;;PD2--H3
  
  GPIO_Init(GPIOB,(GPIO_Pin_TypeDef)(GPIO_PIN_2|GPIO_PIN_4),GPIO_MODE_IN_PU_IT);//INPUT SIGNAL   PB2--IN0;;PB4 -- IN1
  GPIO_Init(GPIOA,(GPIO_Pin_TypeDef)(GPIO_PIN_2|GPIO_PIN_6), GPIO_MODE_IN_PU_IT);//INPUT SIGNAL  PA2--IN3 ;; PA6 --IN2
  GPIO_Init(GPIOE,(GPIO_Pin_TypeDef)(GPIO_PIN_4|GPIO_PIN_7), GPIO_MODE_IN_PU_IT);//INPUT SIGNAL   PE7--IN4  ;;; PE4--IN5
  
  GPIO_Init(GPIOE , (GPIO_Pin_TypeDef)(GPIO_PIN_0|GPIO_PIN_3), GPIO_MODE_OUT_PP_LOW_SLOW);//PE0--OUT0,,PE3--OUT3,
  GPIO_Init(GPIOE , (GPIO_Pin_TypeDef)(GPIO_PIN_1|GPIO_PIN_2), GPIO_MODE_OUT_OD_LOW_FAST);//PE1--OUT1,,PE2--OUT2,,
  GPIO_Init(GPIOF,(GPIO_Pin_TypeDef)(GPIO_PIN_0|GPIO_PIN_3), GPIO_MODE_OUT_PP_LOW_SLOW);//PF0--OUT4 ;; PF3 -- OUT5
  GPIO_Init(GPIOI,(GPIO_Pin_TypeDef)(GPIO_PIN_0), GPIO_MODE_OUT_PP_LOW_SLOW);//PI0--OUT6
  
  GPIO_Init(GPIOF,(GPIO_Pin_TypeDef)(GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7), GPIO_MODE_IN_PU_NO_IT);//PF4,PF5,PF6,PF7 BOARD TYPE CFG-PIN    INPUT 

}



/*******************************************************************************
*                         ==外部中断初始化函数==
* EXTI_DeInit();                                                            --> 初始化寄存器
* EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD , EXTI_SENSITIVITY_FALL_ONLY); --> 设定下降沿触发外部中断 
*******************************************************************************/
void EXTI_INIT(void)
{
  EXTI_DeInit();
  
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC , EXTI_SENSITIVITY_RISE_ONLY);//上升沿触发  PC4 -- TRF INTERRUPT
  
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD , EXTI_SENSITIVITY_RISE_FALL);//下降/上升沿沿触发  PD6,PD5 -- STEP MOTOR FAULT INTERRUPT
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB , EXTI_SENSITIVITY_RISE_FALL);//下降/上升沿触发  PB0 ,PB1 ---  BLDC FAULT INTERRUPT
  
  
}

void EEPROM_INIT(void)
{
  FLASH_DeInit();
  FLASH_Unlock(FLASH_MEMTYPE_DATA);//FLASH_MEMTYPE_PROG
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
}



/*******************************************************************************
*                         == 系统初始化函数==
*******************************************************************************/
void  SYSTEMInit(void)
{
  tasknum = task_rfid;
  
  TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG = TM3_RFID; 
  
  EEPROM_INIT();
  
  boardID=_Merge_32Bit(FLASH_ReadByte(boardid_address0),FLASH_ReadByte(boardid_address1),FLASH_ReadByte(boardid_address2),FLASH_ReadByte(boardid_address3));
   
  rfid_delay_protect.RFID_DELAY_PROTECT_FLAG = FLASH_ReadByte(RFID_DELAY_EN_EEPROM_ADDR);
  rfid_delay_protect.RFID_DELAY_threshold = _Merge_16Bit(FLASH_ReadByte(RFID_DELAY_EEPROM_ADDR1),FLASH_ReadByte(RFID_DELAY_EEPROM_ADDR2));
  
  FLASH_Lock(FLASH_MEMTYPE_DATA);
  
  if( rfid_delay_protect.RFID_DELAY_PROTECT_FLAG = FLASH_ReadByte(RFID_DELAY_EN_EEPROM_ADDR) == 1)
  {
    TIM4_FUNCTION_FLAG = TIM4_RFID;
    
    TIMER4_INIT();//delay 1ms
  }
  
  
  
  step_motor_parameter_initial();//step motor parameter initial
  
  BLDC_SYSINIT();//bldc initial
  
  INPUT_PORT_INITINAL();

}


////////////////////////////TIM1 1ms///////////////////
void TIMER1_INIT(u16 period)
{
  /* TIM1 Peripheral Configuration */ 
  TIM1_DeInit();
  
  TIM1_TimeBaseInit(16,TIM1_COUNTERMODE_UP,period,0);
  
  TIM1_ARRPreloadConfig(ENABLE);
  TIM1_ITConfig(TIM1_IT_UPDATE , ENABLE);
  TIM1_Cmd(DISABLE);
}

//////////////////TIM2 1MS////////////////
void TIMER2_INIT(void)
{

  /* TIM1 Peripheral Configuration */ 
  TIM2_DeInit();
  
  TIM2_TimeBaseInit(TIM2_PRESCALER_64,2);//10k
  
  
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  TIM2_Cmd(DISABLE);
  
}

void TIMER3_INIT(TIM3_Prescaler_TypeDef TIM3_Prescaler,u16 period)  //1us *124=1ms
{
  TIM3_DeInit();
  TIM3_TimeBaseInit(TIM3_Prescaler, period);

  TIM3_ITConfig(TIM3_IT_UPDATE, ENABLE);
  TIM3_Cmd(DISABLE);
}

void TIM3_PWMFREQUENCY_CHANNE2_INIT(void)
{
  TIM3_DeInit();
  
  TIM3_TimeBaseInit(TIM3_PRESCALER_1, 1600); //PWM FREQUENCY = 10k

  /* PWM1 Mode configuration: Channel2 */         
  TIM3_OC2Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_ENABLE,800, TIM3_OCPOLARITY_HIGH);
  TIM3_OC2PreloadConfig(ENABLE);

  TIM3_ARRPreloadConfig(ENABLE);
  
  TIM3_Cmd(DISABLE);
}


void TIMER4_INIT(void)
{
  TIM4_DeInit();
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, 125);  // 
  TIM4_ARRPreloadConfig(ENABLE);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_Cmd(ENABLE);
}


void delay_us(uint16_t delay)
{
  uint16_t i,j;
  uint16_t counter_delay_ms;
  counter_delay_ms = delay;
  for(j=0;j<counter_delay_ms;j++)
    for(i=0;i<100;i--);
}



void delay_ms(unsigned int ms) //  延时1ms  
{ 
  while(ms--) 
  {    
    delay_us(1000);      
    IWDG_ReloadCounter();  
  } 
} 

void CLR_MEMORY(u8 *DATAaddress, u8 length)
{
  
  for(u8 i=0;i<length;i++)
  {
    *DATAaddress=0;
    DATAaddress++;
  }
  
}

u16 _Merge_16Bit(u8 mbyte,u8 lbyte)
{
  u16 _data = 0;
  _data |= mbyte;
  _data <<= 8;
  _data |= lbyte;
  
  return _data;
}

u32 _Merge_32Bit(u8 xbyte,u8 hbyte,u8 mbyte,u8 lbyte)
{
  u32 _data = 0;
  _data |= xbyte;
  _data <<= 8;
  _data |= hbyte;
  _data <<= 8;
  _data |= mbyte;
  _data <<= 8;
  _data |= lbyte;
  
  return _data;
}

u8 Lsb_8Bit(u16 word)
{   u8 _data;
word&=0x00ff;
_data=word;
return _data;
}



u8 Msb_8Bit(u16 word)
{   u8 _data;
word>>=8;
_data=word;
return _data;
}

u8 *Memcpy(u8 *dst,u8 *src,u8 n)  //不做长度检测
{
  u8 *pd = dst;
  u8 *ps = src;
  if((dst == 0) || (src == 0) || (n == 0) || (dst == src))
    return FALSE;
  while(n--)
    *pd++ = *ps++;
  return pd;
  
}


//送出一个字节到HC595
void SendByte_to_hc595(unsigned char dat)
{
  unsigned char i;
  
  for(i=0;i<8;i++)
  {
    HC595_SRCLK_L;
    
    if(dat&0x80)
      HC595_SDI_H;
    else
      HC595_SDI_L;
    
    HC595_SRCLK_H;
    
    dat=dat<<1;
  }
}


//// LED output ctrol //////////////////////
void LED_OPTION_PROCESS(u8 num,u8 type)
{
  u8 led_current_data = 0 ;
  
  led_current_data = LED_OPTION_DATA;
  
  if(type)
    led_current_data |= (0x01<<num);
  else led_current_data &= ~(0x01<<num);
  
  LED_OPTION_DATA = led_current_data;
  
  SendByte_to_hc595(led_current_data);
  
  HC595_RCLK_L;
  
//  asm("nop");
//  asm("nop");
//  asm("nop");
//  asm("nop");
  
  HC595_RCLK_H;
}

//*********************ST单片机编码读函数**************************

