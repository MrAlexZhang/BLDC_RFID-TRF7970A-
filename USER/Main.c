
#include "Globe.h"


//===============================================================

u08_t buf[300];

u08_t TRF_INITIAL_DONE = 0;

u08_t i_reg = 0x01;							// interrupt register

u08_t irq_flag = 0x00;
u08_t rx_error_flag = 0x00;
s08_t rxtx_state = 1;							// used for transmit recieve byte count

s16_t nfc_state;

u08_t host_control_flag = 0;

u08_t remote_flag = 0;
u08_t stand_alone_flag = 1;
u08_t reader_mode = 0x00;					// determines how interrups will be handled


u8 flag = 0;

static u8 board_type = 0xF0;//default   0xF0 -- no use step motor;  0xe0 -- use step motor


void MAIN_TASK_PROCESS(void);
void input_task();

//===============================================================
//
//===============================================================

/*******************************************************************************
*                           ==MAIN 函数==
*******************************************************************************/
void main(void)
{
  static u16 run_cnt = 0;
  
  CLK_INIT();        //内部时钟初始化16Mhz
  
  RST_ClearFlag(RST_FLAG_ILLOPF);
  RST_ClearFlag(RST_FLAG_WWDGF);
  RST_ClearFlag(RST_FLAG_IWDGF);
  RST_ClearFlag(RST_FLAG_SWIMF);
  
  //  iDog_INIT();          //看门狗初始化
  
  GPIO_INIT();        //GPIOx初始化
  
  LED_OPTION_PROCESS(0,OFF); /// ALL LED OFF
  
  board_type = (GPIO_ReadInputData(GPIOF)&0xf0);//get board type
  
  EXTI_INIT();
  
//  TIMER1_INIT(50);//use for bldc start pwm period
//  TIMER4_INIT();//use for bldc work pps
  
//  TIMER2_INIT();//USE FOR step motor pwm clk = 10k 
  
  TIMER3_INIT(TIM3_PRESCALER_16,1250);//USER FOR MOTOR and trf7960a default PPS=800
 
  SLAVE_SELECT_HIGH;
  
#ifdef ENABLE_HOST
  UartSetup();					// settings for UART
#endif
  
  ENABLE_SET;							// P1.0 is switched in output direction
  TRF_ENABLE;							// P1.0 = 1, EN pin on the TRF 796x
  delay_ms(10);				// wait until system clock started
  
  Trf797xCommunicationSetup();		// settings for communication with TRF
  
  delay_ms(1);
  
  Trf797xInitialSettings();			// Set MCU Clock Frequency to 6.78 MHz and OOK Modulation
  
  // Now switch from DCO to external 6.78 MHz clock
  //	McuOscSel(0x00);       				// set the oscilator
  delay_ms(10);
  
  // Re-configure the USART with this external clock
  
  //	Trf797xReConfig();
  
  SYSTEMInit();//system initial
  
  EEPROM_INIT();
  
  boardID=_Merge_32Bit(FLASH_ReadByte(boardid_address0),FLASH_ReadByte(boardid_address1),FLASH_ReadByte(boardid_address2),FLASH_ReadByte(boardid_address3));
  
  FLASH_Lock(FLASH_MEMTYPE_DATA);
  delay_ms(10);
  
  CAN_Init_Conf(); //can initial

 
  ENABLE_INTERRUPTS;					// General enable interrupts
  
  LED_POWER_ON;						// board ist powered, green LED
  
  OOK_DIR_IN;							// set OOK port tristate
  TRF_INITIAL_DONE = 1;							// indicates, that setting are done
  
  reader_mode = 0x00;
  stand_alone_flag = 1;				// stand alone mode
  remote_flag = 0;					// host is not active
    //	Settings done

  
  delay_ms(10);
  LED_OPTION_PROCESS(LED_1,ON); //mcu run indicate 
  
    ////// //shift fork reset position  /////////
  if(board_type == 0xe0) // board use step motor
  {
   
    SetbeltInfo_val.VAL = 0x05;
    step_motor_start_up_parameter.step_motor_total_num =  0x000001ff;//default step
    motor_move_ctrol(ON);
    
  }


  while(1)							// infinite loop
  {
    
//    StateMachineExec();
    
    if(run_cnt++ == 0)
      LED_OPTION_PROCESS(LED_1,ON); //mcu run indicate
    else if(run_cnt == mcu_run_indicate_cnt )
      LED_OPTION_PROCESS(LED_1,OFF); //mcu run indicate
    else if(run_cnt >= (mcu_run_indicate_cnt<<1) )
      run_cnt = 0;
    
    MAIN_TASK_PROCESS();
    
    if(CAN_Command_flag!=0) 
    {
      CAN_Command_exectue();
      CLR_MEMORY((u8*)(&msg.FRME_HEAD),8);
      CAN_Command_flag=0;     
    }
    
    input_task();
  }
}


void MAIN_TASK_PROCESS(void)
{
  
  switch(tasknum)
  {
  case task_rfid:   
#if TRIGGER						// in Mcu.h
    LED_OPEN1_ON;
    delay_ms(1);
    LED_OPEN1_OFF;
#endif
    if(remote_flag == 1) 			// if in remote mode
    {								// remote mode can be disabled in host.h
#ifdef ENABLE_HOST
      buf[4] = 0xff;			// "TRF7960 EVM" message in GUI
      stand_alone_flag = 0;
      LED_ALL_OFF;
      LED_POWER_ON;
      HostCommands();			// device works according host commands
#endif
    }
    else
    {	
#ifdef ENABLE15693				// this standard can be disabled in ISO15693.h
      Iso15693FindTag();			// detects ISO15693 in stand-alone mode
#endif
      if(remote_flag == 0)
      {	
#ifdef ENABLE14443A			// this standard can be disabled in ISO14443A.h 
        Iso14443aFindTag();		// detects ISO14443A in stand-alone mode
#endif
      }
      if(remote_flag == 0)
      {
#ifdef ENABLE14443B			// this standard can be disabled in ISO14443B.h
        Iso14443bFindTag();		// detects ISO14443B in stand-alone mode
#endif
      }
      if(remote_flag == 0)
      {
#ifdef ENABLE_FELICA		// this standard can be disabled in felica.h
        FindFelica();			// detects FeliCa in stand-alone mode
#endif
      }
    }
    break;
    
  case task_stepmotor:    
    
    if(step_motor_start_up_parameter.step_motor_enable_acc_flag == 0x01)  //acc function enable
    {
      if( (step_frequency_calt_start_flag == FALSE) && (step_motor_start_up_parameter.step_motor_acc_done == FALSE) )
      {
        step_motor_start_up_parameter.step_motor_current_pps += step_motor_start_up_parameter.step_acc_value;
        if(step_motor_start_up_parameter.step_motor_current_pps > step_motor_start_up_parameter.step_work_pps)
          step_motor_start_up_parameter.step_motor_current_pps = step_motor_start_up_parameter.step_work_pps;
        
        step_motor_start_up_parameter.tim_period = 1000000/step_motor_start_up_parameter.step_motor_current_pps; 
        step_motor_start_up_parameter.step_motor_acc_cnt = (u8)(step_motor_start_up_parameter.step_time_interval/(float)(1000/step_motor_start_up_parameter.step_motor_current_pps));
        
        step_frequency_calt_start_flag = TRUE;
      }
    }
    
    if(step_motor_fault_flag == TRUE)
    {
      if(step_motor_fault_code != fault_none)
      { 
        motor_move_ctrol(OFF);
        LED_OPTION_PROCESS(LED_6,ON);//step motor fault indicate
 
      }
      else LED_OPTION_PROCESS(LED_6,OFF);
      
      SPM_MESSAGE_OUT(1,1,SEND_STEPPING_MOTOR_FAULT_CODE,0,&step_motor_fault_code);//SEND FAULT CODE OUT
      step_motor_fault_flag = FALSE;      
    }
    break;
  case task_bldc:       
    StateMachineExec();
    
    if(bldc_fault_flag == TRUE)
    {
      SPM_MESSAGE_OUT(1,1,SEND_BLDC_FAULT_CODE,0,&bldc_fault_code);//SEND FAULT CODE OUT
      
      if(bldc_fault_code != fault_none)
      {
        LED_OPTION_PROCESS(LED_8,ON);//bldc fault indicate led
        
//        DRV8332INIT();
//        MECHARM_Cylinderctrl(OFF,0);//TURN OFFF BLDC
        
        tasknum=task_rfid;
      }
      else LED_OPTION_PROCESS(LED_8,OFF);//bldc fault indicate led
      
      bldc_fault_flag = FALSE;
      
    }   
    break;
  default:break;
    
  }
  
  
}

void input_task()
{
  u8 temp = 0;
  
  if(signal_input_state.IN0_TRIG_FLAG == TRUE)
  {
    temp = IN0;
    SPM_MESSAGE_OUT(1,1,INPUT_SIGNAL_MESSAGE,0,&temp);
    
    delay_ms(50);
    signal_input_state.IN0_TRIG_FLAG = FALSE;
  }
  
  if(signal_input_state.IN1_TRIG_FLAG == TRUE)
  {
    temp = IN1;
    SPM_MESSAGE_OUT(1,1,INPUT_SIGNAL_MESSAGE,0,&temp);
    
    delay_ms(50);
    signal_input_state.IN1_TRIG_FLAG = FALSE;
  }
  
  if(signal_input_state.IN2_TRIG_FLAG == TRUE)
  {
    temp = IN2;
    SPM_MESSAGE_OUT(1,1,INPUT_SIGNAL_MESSAGE,0,&temp);
    
    delay_ms(50);
    signal_input_state.IN2_TRIG_FLAG = FALSE;
  }
  
  if(signal_input_state.IN3_TRIG_FLAG == TRUE)
  {
    temp = IN3;
    SPM_MESSAGE_OUT(1,1,INPUT_SIGNAL_MESSAGE,0,&temp);
    
    delay_ms(50);
    signal_input_state.IN3_TRIG_FLAG = FALSE;
  }
  
  if(signal_input_state.IN4_TRIG_FLAG == TRUE)
  {
    temp = IN4;
    SPM_MESSAGE_OUT(1,1,INPUT_SIGNAL_MESSAGE,0,&temp);
    
    delay_ms(50);
    signal_input_state.IN4_TRIG_FLAG = FALSE;
  }
  
  if(signal_input_state.IN5_TRIG_FLAG == TRUE)
  {
    temp = IN5;
    SPM_MESSAGE_OUT(1,1,INPUT_SIGNAL_MESSAGE,0,&temp);
    
    delay_ms(50);
    signal_input_state.IN5_TRIG_FLAG = FALSE;
  }

}





/*******************************************************************************
*                           ==中断服务函数 函数==
* PORTA 端口0
* 
*******************************************************************************/
#pragma vector=0x05
__interrupt void PORTA_IRQHandler(void)
{
  
  if( signal_input_state.IN2_interrupt_mode == EXTI_SENSITIVITY_RISE_ONLY)
  {   
    if(SIGNAL_IN3 != RESET)
    {
      if( SIGNAL_IN_FILTER(IN3) )
        signal_input_state.IN3_TRIG_FLAG = TRUE;
    }
    
    if(SIGNAL_IN2 != RESET)
    {
      if( SIGNAL_IN_FILTER(IN2) )
        signal_input_state.IN2_TRIG_FLAG = TRUE;
    }
    
  }
  else if( signal_input_state.IN2_interrupt_mode == EXTI_SENSITIVITY_FALL_ONLY)
  {
    if(SIGNAL_IN3 == RESET)
    {
      if( !SIGNAL_IN_FILTER(IN3) )
        signal_input_state.IN3_TRIG_FLAG = TRUE;
    }
    
    if(SIGNAL_IN2 == RESET)
    {
      if( !SIGNAL_IN_FILTER(IN2) )
        signal_input_state.IN2_TRIG_FLAG = TRUE;
    }
  }
   
}


#pragma vector=0x06
__interrupt void PORTB_IRQHandler(void)   
{
  
  if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_0) == RESET)//BLDC fault code
  {
    if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_1) == RESET)
    {
      bldc_fault_flag = TRUE;
      bldc_fault_code = fault_overtemperwaring_andoneofthe_overtempershutdown_or_overcurrentshutdown_or_undervoltage_occur;
    }
    else{
      bldc_fault_flag = TRUE;
      bldc_fault_code = fault_overtemperwaring;
    }
    
  }
  else{
    if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_1) == RESET)
    {
      bldc_fault_code = TRUE;
      step_motor_fault_code = fault_voercurrentshutdown_undervoltageprotection_occur;
    }
    
  }
  
     //IN SIGANL /////////////////
  if( signal_input_state.IN0_interrupt_mode == EXTI_SENSITIVITY_RISE_ONLY )
  {   
    if(SIGNAL_IN0 != RESET)
    {
      if( SIGNAL_IN_FILTER(IN0) )
        signal_input_state.IN0_TRIG_FLAG = TRUE;
    }
    
    if(SIGNAL_IN1 != RESET)
    {    
      if( SIGNAL_IN_FILTER(IN1) )
        signal_input_state.IN1_TRIG_FLAG = TRUE;
    }
  }
  else if( signal_input_state.IN0_interrupt_mode == EXTI_SENSITIVITY_FALL_ONLY)
  {
    if(SIGNAL_IN0 == RESET)
    {
      if( !SIGNAL_IN_FILTER(IN0) )
        signal_input_state.IN0_TRIG_FLAG = TRUE;
    }
    
    if(SIGNAL_IN1 == RESET)
    {
      if( !SIGNAL_IN_FILTER(IN1) )
        signal_input_state.IN1_TRIG_FLAG = TRUE;
    }
    
  }
  else if( signal_input_state.IN0_interrupt_mode == EXTI_SENSITIVITY_RISE_FALL)
  {
    if(SIGNAL_IN0 != RESET)
    {
      if( SIGNAL_IN_FILTER(IN0) )
      {
        signal_input_state.IN0_TRIG_FLAG = TRUE;       
        
        if( (step_motor_start_up_parameter.step_motor_limit_sensor == IN0) && (TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG == TM3_STEPMOTOR) )
        {
          SetbeltInfo_val.VAL = 0x00;
          step_motor_start_up_parameter.step_motor_total_num =  0x00000000;//default step
          motor_move_ctrol(OFF);
        }

      }

    }
    
    if(SIGNAL_IN1 != RESET)
    {     
      if( SIGNAL_IN_FILTER(IN1) )
      {
        signal_input_state.IN1_TRIG_FLAG = TRUE;
        
        if((step_motor_start_up_parameter.step_motor_limit_sensor == IN1) && (TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG == TM3_STEPMOTOR) )
        {
          SetbeltInfo_val.VAL = 0x00;
          step_motor_start_up_parameter.step_motor_total_num =  0x00000000;//default step
          motor_move_ctrol(OFF);
        }

      }
    }
  }
  
}





/*******************************************************************************
*                           ==中断服务函数 函数==
* PORTE 端口0
* 
*******************************************************************************/
#pragma vector=0x09
__interrupt void PORTE_IRQHandler(void)
{
  
  if( signal_input_state.IN4_interrupt_mode == EXTI_SENSITIVITY_RISE_ONLY )
  {   
    if(SIGNAL_IN4 != RESET)
    {
      if( SIGNAL_IN_FILTER(IN4) )
        signal_input_state.IN4_TRIG_FLAG = TRUE;
    }
    
    if(SIGNAL_IN5 != RESET)
    {
      if( SIGNAL_IN_FILTER(IN5) )
        signal_input_state.IN5_TRIG_FLAG = TRUE;
    }
    
  }
  else if( signal_input_state.IN4_interrupt_mode == EXTI_SENSITIVITY_FALL_ONLY)
  {
    if(SIGNAL_IN4 == RESET)
    {
      if( !SIGNAL_IN_FILTER(IN4) )
        signal_input_state.IN4_TRIG_FLAG = TRUE;
    }
    
    if(SIGNAL_IN5 == RESET)
    {
      if( !SIGNAL_IN_FILTER(IN5) )
        signal_input_state.IN5_TRIG_FLAG = TRUE;
    }
  }
   
}


/*******************************************************************************
*                           ==中断函数==
*******************************************************************************/
#pragma vector=0x0F
__interrupt void TIM2_UPD_OVF_IRQHandler(void)
{
//  static u8 step_motor_pwm_clk_flag = 0;
  
  if( (STEP_MOTOR_PWM_CNT < step_motor_start_up_parameter.step_motor_pwm_duty_cnt ) && (SetbeltInfo_val.stsetbeltcontral.run == 1) )//( (!step_motor_pwm_clk_flag) && (SetbeltInfo_val.stsetbeltcontral.run == 1) )//
    A_B_ENBL_H;
  else A_B_ENBL_L;
  
  if(STEP_MOTOR_PWM_CNT++ >= 10)
    STEP_MOTOR_PWM_CNT = 0;
//  step_motor_pwm_clk_flag = ~step_motor_pwm_clk_flag;
//  u08_t irq_status[4];  
//  
//  STOP_COUNTER; 
//  
//  //  if(flag)
//  //    LED_OPTION_PROCESS(LED_2,OFF);
//  //  else LED_OPTION_PROCESS(LED_2,ON);
//  //  flag = ~flag;
//  
//  irq_flag = 0x02;  
//  
//  //  LED_OPTION_PROCESS(LED_3,OFF);
//  
//  Trf796xReadIrqStatus(irq_status);
//  
//  *irq_status = *irq_status & 0xF7;				// set the parity flag to 0
//  
//  if(*irq_status == 0x00 || *irq_status == 0x80)
//  {  	
//    i_reg = 0x00;								// timer interrupt
//  }
//  else
//  {	
//    i_reg = 0x01;
//  }
//  //__low_power_mode_off_on_exit();
  
  TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
  
}


#pragma vector=0x11
__interrupt void TIM3_UPD_OVF_BRK_IRQHandler(void)
{ 
  if(TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG == TM3_STEPMOTOR)
  {
    if(SetbeltInfo_val.stsetbeltcontral.direction==1)
    {
      if(Driver1_step==0)
      {
        coil2_ch1A;     
      }
      
      if(Driver1_step==1)
      {
        coil2_ch1B;
      }
      
      if(Driver1_step==2)
      {
        coil2_ch1C;      
      }
      
      if(Driver1_step==3)
      {
        coil2_ch1D;
      }
      
    }   
    else
    {
      if(Driver1_step==0)
      {
        coil1_ch1A;       
      }
      
      if(Driver1_step==1)
      {
        coil1_ch1B;
      }
      
      if(Driver1_step==2)
      {
        coil1_ch1C;        
      }
      
      if(Driver1_step==3)
      {
        coil1_ch1D;
      }
      
    }
    
    Driver1_step++;
    if(Driver1_step>=4)
    {
      Driver1_step=0;
    }
    
    if(step_motor_start_up_parameter.step_motor_soft_start_cnt >= 200)
    {
      step_motor_start_up_parameter.step_motor_pwm_duty_cnt = 9;
    }
    else step_motor_start_up_parameter.step_motor_soft_start_cnt++;
    
    
    motor_move_mode_contral();
    
    ////////////////add pps task///////////////////////////////
    if(step_motor_start_up_parameter.step_motor_enable_acc_flag == 0x01)  //acc function enable
    {
      if( (t3_pwm_cnt++ >= step_motor_start_up_parameter.step_motor_acc_cnt) && (step_motor_start_up_parameter.step_motor_acc_done == FALSE) ) //modify frequency
      {
        
        TIMER3_INIT(TIM3_PRESCALER_16,step_motor_start_up_parameter.tim_period);//USER FOR MOTOR pwm
        
        t3_pwm_cnt = 0;    
        step_frequency_calt_start_flag = FALSE;
        step_motor_start_up_parameter.step_motor_acc_step++;
        
        if(step_motor_start_up_parameter.step_motor_current_pps == step_motor_start_up_parameter.step_work_pps)
          step_motor_start_up_parameter.step_motor_acc_done = TRUE;
        
        TIM3_Cmd(ENABLE);
      }
    }
  }
  else if(TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG == TM3_RFID)
  {
    u08_t irq_status[4];
    
    STOP_COUNTER;
    CLEAR_COUNTER
      
    irq_flag = 0x03;
    
    Trf797xReadIrqStatus(irq_status);
    
    *irq_status = *irq_status & 0xF7;				// set the parity flag to 0
    
    if(*irq_status == 0x00 || *irq_status == 0x80)
    {
      i_reg = 0x00;								// timer interrupt
    }
    else
    {
      i_reg = 0x01;
    }
    //__low_power_mode_off_on_exit();
  }

  TIM3_ClearITPendingBit(TIM3_IT_UPDATE);      
  
}




void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}




/*******************************************************************************/


