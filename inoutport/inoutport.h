#ifndef IN_OUT_PORT_H_
#define IN_OUT_PORT_H_

#ifndef INOUT_PORT_VER
#define PORT_EXT extern
#else
#define PORT_EXT  
#endif

/////////////////INPUT SIGNAL PIN CONFIG/////////
#define SIGNAL_IN0 GPIO_ReadInputPin(GPIOB,GPIO_PIN_2)
#define SIGNAL_IN1 GPIO_ReadInputPin(GPIOB,GPIO_PIN_4)
#define SIGNAL_IN2 GPIO_ReadInputPin(GPIOA,GPIO_PIN_6)
#define SIGNAL_IN3 GPIO_ReadInputPin(GPIOA,GPIO_PIN_2)
#define SIGNAL_IN4 GPIO_ReadInputPin(GPIOE,GPIO_PIN_7)
#define SIGNAL_IN5 GPIO_ReadInputPin(GPIOE,GPIO_PIN_4)

////////////////////OUTPUT PORT PIN CONFIG/////////
#define SIGNAL_OUT0_H GPIO_WriteHigh(GPIOE,GPIO_PIN_0)
#define SIGNAL_OUT0_L GPIO_WriteLow(GPIOE,GPIO_PIN_0)

#define SIGNAL_OUT1_H GPIO_WriteHigh(GPIOE,GPIO_PIN_1)
#define SIGNAL_OUT1_L GPIO_WriteLow(GPIOE,GPIO_PIN_1)

#define SIGNAL_OUT2_H GPIO_WriteHigh(GPIOE,GPIO_PIN_2)
#define SIGNAL_OUT2_L GPIO_WriteLow(GPIOE,GPIO_PIN_2)

#define SIGNAL_OUT3_H GPIO_WriteHigh(GPIOE,GPIO_PIN_3)
#define SIGNAL_OUT3_L GPIO_WriteLow(GPIOE,GPIO_PIN_3)

#define SIGNAL_OUT4_H GPIO_WriteHigh(GPIOF,GPIO_PIN_0)
#define SIGNAL_OUT4_L GPIO_WriteLow(GPIOF,GPIO_PIN_0)

#define SIGNAL_OUT5_H GPIO_WriteHigh(GPIOF,GPIO_PIN_3)
#define SIGNAL_OUT5_L GPIO_WriteLow(GPIOF,GPIO_PIN_3)

#define SIGNAL_OUT6_H GPIO_WriteHigh(GPIOI,GPIO_PIN_0)
#define SIGNAL_OUT6_L GPIO_WriteLow(GPIOI,GPIO_PIN_0)

enum{OUT0,OUT1,OUT2,OUT3,OUT4,OUT5,OUT6,OUT7};
enum{IN0,IN1,IN2,IN3,IN4,IN5,IN6,IN7};

typedef struct inport{
  EXTI_Sensitivity_TypeDef IN0_interrupt_mode ;
  EXTI_Sensitivity_TypeDef IN1_interrupt_mode ;
  EXTI_Sensitivity_TypeDef IN2_interrupt_mode ; //IN0 -- IN2 必须用上升沿中断模式  
  EXTI_Sensitivity_TypeDef IN3_interrupt_mode ;
  EXTI_Sensitivity_TypeDef IN4_interrupt_mode ;
  EXTI_Sensitivity_TypeDef IN5_interrupt_mode ;
  
  bool IN0_TRIG_FLAG ;
  bool IN1_TRIG_FLAG ;
  bool IN2_TRIG_FLAG ;
  bool IN3_TRIG_FLAG ;
  bool IN4_TRIG_FLAG ;
  bool IN5_TRIG_FLAG ;
  
}INPUTPORT;

PORT_EXT INPUTPORT signal_input_state;

PORT_EXT void output_port_opteration(u8 num,u8 type);
PORT_EXT void INPUT_PORT_INITINAL(void);
PORT_EXT u8 SIGNAL_IN_FILTER(u8 num);


#endif