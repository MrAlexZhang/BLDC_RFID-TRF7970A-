#define _STEP_MOTOR_Driver_VER 

#include "Globe.h"


void MOTOR_INITIAL(void)
{  
  RESET_AB_8432_L; //reset a,b,c,d,output state
  RESET_CD_8432_L;
  delay_ms(10);
  
  RESET_AB_8432_H;
  RESET_CD_8432_H;
  delay_ms(10);
  
  coil_ch1off;
  
}

void motor_move_ctrol(u8 move)
{ 
  if(move==ON)
  {
    if(SetbeltInfo_val.stsetbeltcontral.direction == 1)
    {
      if(SIGNAL_IN_FILTER(IN1))  //shift fork reach position limit sensor trig
        return;
      else step_motor_start_up_parameter.step_motor_limit_sensor = IN1;
    }
    else if(SetbeltInfo_val.stsetbeltcontral.direction == 0)
    {
      if(SIGNAL_IN_FILTER(IN0))  //shift fork reset position limit sensor trig
        return;
      else step_motor_start_up_parameter.step_motor_limit_sensor = IN0;
    }
    
    MOTOR_INITIAL();
        
    step_motor_fault_code = fault_none;
    step_motor_fault_flag = FALSE;
    a_b_pwm_flag = FALSE;
    c_d_pwm_flag = FALSE;
    
    t3_pwm_cnt= 0;
    Driver1_step = 0;
    STEP_MOTOR_PWM_CNT = 0;
    
    step_motor_start_up_parameter.step_motor_acc_step = 0;
    step_frequency_calt_start_flag = TRUE;
    step_motor_start_up_parameter.step_motor_acc_done = FALSE;
    
    step_motor_start_up_parameter.step_motor_soft_start_cnt = 0;
    step_motor_start_up_parameter.step_motor_pwm_duty_cnt = 5;
    
    TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG = TM3_STEPMOTOR;
    
    LED_OPTION_PROCESS(LED_5,ON);//STEP MOTOR RUN INDICATE
    mcu_run_indicate_cnt = 30000;
    tasknum=task_stepmotor;

    SetbeltInfo_val.stsetbeltcontral.run=1;
    
    if(step_motor_start_up_parameter.step_motor_enable_acc_flag == 0x00)
    {
      step_motor_start_up_parameter.step_motor_current_pps = step_motor_start_up_parameter.step_work_pps;
    }
    else if(step_motor_start_up_parameter.step_motor_enable_acc_flag == 0x01)  //acc function enable
    {
      step_motor_start_up_parameter.step_motor_current_pps = step_motor_start_up_parameter.step_motor_start_pps;
      step_motor_start_up_parameter.step_motor_acc_cnt = (u8)(step_motor_start_up_parameter.step_time_interval/(float)((float)1000/step_motor_start_up_parameter.step_motor_current_pps));
    }
    
    step_motor_start_up_parameter.tim_period = 1000000/step_motor_start_up_parameter.step_motor_current_pps;     
    TIMER3_INIT(TIM3_PRESCALER_16,step_motor_start_up_parameter.tim_period);//USER FOR MOTOR pwm
    
    TIMER2_INIT();//USE FOR step motor pwm clk = 10k 
    TIM2_Cmd(ENABLE);
    
    TIM3_Cmd(ENABLE);
//    A_B_ENBL_H;
  }
  if(move==OFF)
  {
    TIM2_Cmd(DISABLE);
    TIM3_Cmd(DISABLE);

    MOTOR_INITIAL();
   
    SetbeltInfo_val.VAL=0;
    step_motor_start_up_parameter.step_motor_total_num = 0x00000000;
    
    LED_OPTION_PROCESS(LED_5,OFF);//STEP MOTOR run INdicate
    LED_OPTION_PROCESS(LED_6,OFF);//STEP MOTOR FAULT INdicate

    TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG = TM3_RFID;
    TIMER3_INIT(TIM3_PRESCALER_128,124);//DELAY 1MS FOR RFID
   
    mcu_run_indicate_cnt = 20;
    
    tasknum= task_rfid;
  }
  
}

void motor_move_mode_contral(void)
{
  
  if(SetbeltInfo_val.stsetbeltcontral.contraltype==1)//according num work
  {
    step_motor_start_up_parameter.step_motor_total_num--;
    if(step_motor_start_up_parameter.step_motor_total_num==0)
    {     
      motor_move_ctrol(OFF);
    }
  }
}

void step_motor_parameter_initial(void)
{
  u16 temp=0;
  
  step_motor_fault_code = fault_none;
  step_motor_fault_flag = FALSE;
  a_b_pwm_flag = FALSE;
  c_d_pwm_flag = FALSE;
  
  EEPROM_INIT();
  
  temp=FLASH_ReadByte( STEPPING_Motor_PPS_address0);
  temp <<=8;
  temp |=FLASH_ReadByte( STEPPING_Motor_PPS_address1);
  
  if(temp != 0) 
    step_motor_start_up_parameter.step_work_pps =temp;
  else 
  {
    step_motor_start_up_parameter.step_work_pps = 800;//default work pps = 800
    FLASH_ProgramByte( STEPPING_Motor_PPS_address0,(step_motor_start_up_parameter.step_work_pps >>8));
    FLASH_ProgramByte( STEPPING_Motor_PPS_address1,step_motor_start_up_parameter.step_work_pps);
  }
  
  temp=FLASH_ReadByte( STEPPING_motor_max_pps_address0);
  temp <<=8;
  temp |=FLASH_ReadByte( STEPPING_motor_max_pps_address1);
  
  if(temp == 0)
  {
    FLASH_ProgramByte( STEPPING_motor_max_pps_address0,0xff); //default step motor max pps = 0xffff
    FLASH_ProgramByte( STEPPING_motor_max_pps_address1,0xff);
    
    step_motor_start_up_parameter.step_max_pps =0xffff;
  }
  else step_motor_start_up_parameter.step_max_pps =temp;
  
  temp=FLASH_ReadByte( STEPPING_motor_pps_acc_address0);
  temp <<=8;
  temp |=FLASH_ReadByte( STEPPING_motor_pps_acc_address1);
  
  if(temp == 0)
  {
    FLASH_ProgramByte( STEPPING_motor_pps_acc_address0,0x00); //default step motor acc pps = 50
    FLASH_ProgramByte( STEPPING_motor_pps_acc_address1,0x32);
    
    step_motor_start_up_parameter.step_acc_value =0x0032;
  }
  else step_motor_start_up_parameter.step_acc_value =temp;
  
  temp=FLASH_ReadByte( STEPPING_motor_acc_time_interval0);
  temp <<=8;
  temp |=FLASH_ReadByte( STEPPING_motor_acc_time_interval1);  
  if(temp == 0)
  {
    FLASH_ProgramByte( STEPPING_motor_acc_time_interval0,0x00); //default step motor acc time interval = 5 ms
    FLASH_ProgramByte( STEPPING_motor_acc_time_interval1,0x05);
    
    step_motor_start_up_parameter.step_time_interval =0x00005;
  }
  else step_motor_start_up_parameter.step_time_interval =temp;
  
  temp=FLASH_ReadByte( STEPPING_motor_pwm_start_pps_address0);  
  temp <<=8;
  temp |=FLASH_ReadByte( STEPPING_motor_pwm_start_pps_address1);
  if(temp ==0)
  {
    FLASH_ProgramByte( STEPPING_motor_pwm_start_pps_address0,0x00); //default step motor start pps = 200  
    FLASH_ProgramByte( STEPPING_motor_pwm_start_pps_address1,0xc8); 
    step_motor_start_up_parameter.step_motor_start_pps =0x00c8;
  }
  else step_motor_start_up_parameter.step_motor_start_pps = temp;
  
  FLASH_Lock(FLASH_MEMTYPE_DATA);
  
  M1_8432_H;
  M2_8432_L;
  M3_8432_L; //four half bridges with OC LATCH SHUTDOWN(NO cycle by cycle current limit)
  delay_ms(10);
  
  MOTOR_INITIAL();  
}



#pragma vector=0x08
__interrupt void PORTD_IRQHandler(void)  //STEPPING MOSTOR FAULT CODE

{
  if(GPIO_ReadInputPin(GPIOD,GPIO_PIN_6) == RESET)
  {
    if(GPIO_ReadInputPin(GPIOD,GPIO_PIN_5) == RESET)
    {
      step_motor_fault_flag = TRUE;
      step_motor_fault_code = fault_overtemperwaring_andoneofthe_overtempershutdown_or_overcurrentshutdown_or_undervoltage_occur;
//      TIM3_Cmd(DISABLE);
    }
    else{
      step_motor_fault_flag = TRUE;
      step_motor_fault_code = fault_overtemperwaring;
//      TIM3_Cmd(DISABLE);
    }
    
  }
  else{
    if(GPIO_ReadInputPin(GPIOD,GPIO_PIN_5) == RESET)
    {
      step_motor_fault_flag = TRUE;
      step_motor_fault_code = fault_voercurrentshutdown_undervoltageprotection_occur;
//      TIM3_Cmd(DISABLE);
    }
    else LED_OPTION_PROCESS(LED_6,OFF);
    
  }
  
}

//
//void shifting_fork_ctrol(u8 type)
//{
//  
//  switch(type)
//  {
//  case :
//    break;
//    
//  default:break;
//  }
//}
















