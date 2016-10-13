#define INOUT_PORT_VER 

#include "Globe.h"

void output_port_opteration(u8 num,u8 type)
{
  switch(num)
  {
  case OUT0:
    if(type == ON)
      SIGNAL_OUT0_H;
    else SIGNAL_OUT0_L;
    break;
  case OUT1:
    if(type == ON)
      SIGNAL_OUT1_H;
    else SIGNAL_OUT1_L;
    break;
  case OUT2:
    if(type == ON)
      SIGNAL_OUT2_H;
    else SIGNAL_OUT2_L;
    break;
  case OUT3:
    if(type == ON)
      SIGNAL_OUT3_H;
    else SIGNAL_OUT3_L;
    break;
  case OUT4:
    if(type == ON)
      SIGNAL_OUT4_H;
    else SIGNAL_OUT4_L;
    break;
  case OUT5:
    if(type == ON)
      SIGNAL_OUT5_H;
    else SIGNAL_OUT5_L;
    break;
  case OUT6:
    if(type == ON)
      SIGNAL_OUT6_H;
    else SIGNAL_OUT6_L;
    break;
  default:break;
  }
  
}

void INPUT_PORT_INITINAL(void)
{
  signal_input_state.IN0_interrupt_mode = EXTI_SENSITIVITY_RISE_FALL;//IN0，IN1 下降沿中断
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB , signal_input_state.IN0_interrupt_mode);
  signal_input_state.IN0_TRIG_FLAG = FALSE;
  signal_input_state.IN1_TRIG_FLAG = FALSE;
  
  signal_input_state.IN2_interrupt_mode = EXTI_SENSITIVITY_RISE_ONLY;//IN2，IN3 下降沿中断
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA , signal_input_state.IN2_interrupt_mode);
  signal_input_state.IN2_TRIG_FLAG = FALSE;
  signal_input_state.IN3_TRIG_FLAG = FALSE;
  
  signal_input_state.IN4_interrupt_mode = EXTI_SENSITIVITY_RISE_ONLY; //IN4,IN5 下降沿中断
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOE , signal_input_state.IN4_interrupt_mode);
  signal_input_state.IN4_TRIG_FLAG = FALSE; 
  signal_input_state.IN5_TRIG_FLAG = FALSE;
}

u8 SIGNAL_IN_FILTER(u8 num)
{
  u8 g=0,temp=0,res=0;
  
  switch(num)
  {
  case IN0:
    while (g < 3)     // while less that 3 good samples
    {
      temp=SIGNAL_IN0;
        
        if(temp == res)         // if the same
          g++;              // one more good
        else
          g = 0;            // else start over
        res = temp;            // update res
    }
    break;
  case IN1:
    while (g < 3)     // while less that 3 good samples
    {
      temp=SIGNAL_IN1;
        
        if(temp == res)         // if the same
          g++;              // one more good
        else
          g = 0;            // else start over
        res = temp;            // update res
    }
    break;
  case IN2:
    while (g < 3)     // while less that 3 good samples
    {
      temp=SIGNAL_IN2;
        
        if(temp == res)         // if the same
          g++;              // one more good
        else
          g = 0;            // else start over
        res = temp;            // update res
    }
    break;
  case IN3:
    while (g < 3)     // while less that 3 good samples
    {
      temp=SIGNAL_IN3;
        
        if(temp == res)         // if the same
          g++;              // one more good
        else
          g = 0;            // else start over
        res = temp;            // update res
    }
    break;
  case IN4:
    while (g < 3)     // while less that 3 good samples
    {
      temp=SIGNAL_IN4;
        
        if(temp == res)         // if the same
          g++;              // one more good
        else
          g = 0;            // else start over
        res = temp;            // update res
    }
    break;
  case IN5:
    while (g < 3)     // while less that 3 good samples
    {
      temp=SIGNAL_IN5;
      
      if(temp == res)         // if the same
        g++;              // one more good
      else
        g = 0;            // else start over
      res = temp;            // update res
    }
    break;
    
  default:break;
  
  }
  
  return res;
}


