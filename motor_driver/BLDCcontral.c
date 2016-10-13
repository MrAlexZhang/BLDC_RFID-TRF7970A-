#define _BLDC_VER  

#include "Globe.h"


u8 motor_startflag=0;

u8 start_pwm=0;
u8 Hallstate=0;
u8 Hallstate1=0;
u8 Hallstate2=0;
u8 Hallstate3=0;
u8 LASTHallstate1=0;
u8 LASTHallstate2=0;
u8 LASTHallstate3=0;

u8 Hall1Startdirect=0;
u8 Hall2Startdirect=0;
u8 Hall3Startdirect=0;
u8 BLDCStart_step=0;
u8 BLDC1Startflag=0;
u8 BLDC2Startflag=0;
u8 BLDC3Startflag=0;
u8 BLDC1SpeedLevel=0;
u8 BLDCStartSpeed=0;

stcoilflagval coilflag={0};
stBLDCmotor1directval BLDCmotor1direct={0};
stBLDCmotor2directval BLDCmotor2direct={0};
stBLDCmotor3directval BLDCmotor3direct={0};

stdrivermotorstatus drivermotorstatus={0}  ;
stcylindermotorstatus cylindermotorstatus={0}  ;


u8  Bigmotor_reversestart(u8 ch,u16 delaytime1);
u8  Bigmotor_forwardstart(u8 ch,u16 delaytime1);
u8 Bigmotor_Start(u8 ch,u16 delaytime1,u8 direct);

void BLDCmotor_normallyrun(u8 ch,u8 direct);


u8  hall_test(u8 ch);
void Coil_PWM1(u8 ch);
void Coil_PWM2(u8 ch);
void Coil_PWM3(u8 ch);
void Coil_PWMOFF(u8 ch);

void Coil_Reset1(u8 ch);
void Coil_Reset2(u8 ch);
void Coil_Reset3(u8 ch);



void BLDC_SYSINIT(void)
{
  u16 temp = 0;
  
//  DRV8332INIT();
  
  EEPROM_INIT();
  
  temp = _Merge_16Bit(FLASH_ReadByte(BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address1),FLASH_ReadByte(BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address2));   
  if(temp == 0)
  {
    FLASH_ProgramByte( BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address1,(u8)(0x07d0>>8) );  //default bldc target speed = 20000
    FLASH_ProgramByte( BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address2,(u8)(0x07d0) );
  }
  
  temp = FLASH_ReadByte(BLDC_MOTOR_WORK_PWM_DUTY_ADDRESS);
  if(temp == 0)
    FLASH_ProgramByte( BLDC_MOTOR_WORK_PWM_DUTY_ADDRESS,80);
    
  FLASH_Lock(FLASH_MEMTYPE_DATA);

}

void DRV8332INIT(void)
{ 
  M1_8332_H;//three half bridges with no cycle by cycle current limit 
  delay_ms(10);
  
  Coil1_PWMOFF;
  
  RESETA_8332_L;
  RESETB_8332_L;
  RESETC_8332_L;
  delay_ms(10);
  
  Coil1_RESETOFF;
  delay_ms(10); 
  
//  PWMA_8332_H;
//  PWMB_8332_H;
//  PWMC_8332_H;
  
}



u8 Bigmotor_Start(u8 ch,u16 delaytime1,u8 direct)
{
  u8 res=0;
  u8 i=0;
  if(direct==forward)
  {
    while(i<=10)
    {
      res=Bigmotor_forwardstart(ch,delaytime1);
      i++;
      if(res==TRUE)
        break;  
    }
  }
  if(direct==reverse)
  {
    while(i<=10)
    {
      res=Bigmotor_reversestart(ch,delaytime1);
      i++;
      if(res==TRUE)
        break;  
    }
    
  }
  return res; 
}



u8  Bigmotor_reversestart(u8 ch,u16 delaytime1)
{ 
  u8 res=0;
  u16 i=0;
  
  start_pwm=0;
  if(start_pwm==0)
  {    
    start_pwm++;
    coilflag.VAL=0;
    Hall1Startdirect=0;
    if(ch==CH1)
      BLDC1Startflag=1;
    Hallstate=hall_test(ch);
    TIM1_Cmd(ENABLE);
    BLDCStart_step=1; 
    BLDCStartSpeed=cylindermotorstatus.cylindermotorstartspeed;
  }
  
  while(start_pwm<6)
  {     
    if(Hallstate==0x40)
    {    
      Coil_Reset1(ch);
      coilflag.VAL=0;
      coilflag.stcoilflag_val.coilaflag=1;
      delay_us(delaytime1);
      Hallstate=0XC0;
    }
    if(Hallstate==0xC0)
    { 
      Coil_Reset2(ch);
      coilflag.VAL=0;
      coilflag.stcoilflag_val.coilaflag=1;
      delay_us(delaytime1);
      Hallstate=0x80;
    }
    if(Hallstate==0x80)
    {   
      Coil_Reset3(ch);
      coilflag.VAL=0;
      coilflag.stcoilflag_val.coilbflag=1;
      delay_us(delaytime1);
      Hallstate=0xA0;
    }
    if(Hallstate==0xA0)
    {
      Coil_Reset1(ch);
      coilflag.VAL=0;
      coilflag.stcoilflag_val.coilbflag=1;
      delay_us(delaytime1);
      Hallstate=0x20;   
      
    }
    if(Hallstate==0x20)
    { 
      Coil_Reset2(ch);
      coilflag.VAL=0;
      coilflag.stcoilflag_val.coilcflag=1;
      delay_us(delaytime1);
      Hallstate=0x60;
    }
    if(Hallstate==0x60)
    {  
      Coil_Reset3(ch);
      coilflag.VAL=0; 
      coilflag.stcoilflag_val.coilcflag=1;  
      delay_us(delaytime1);
      Hallstate=0x40;
    }
    
    start_pwm++;
    if(start_pwm==6)
    { 
      LASTHallstate1=0;
      BLDCStartSpeed +=2;
      if(BLDCStartSpeed > cylindermotorstatus.cylindermotorspeed)
        BLDCStartSpeed = cylindermotorstatus.cylindermotorspeed;
    }
    
  }
  while(start_pwm>=6)
  { 
    BLDCmotor_normallyrun(ch,reverse);
    
    if(Hall1Startdirect==6)
    {
      BLDC1Startflag=0;
      Hall1Startdirect=0;
      
      TIM1_Cmd(DISABLE); 
      res=TRUE;
      break   ;  
    }   
    
    if(i>=1200)
    {
      BLDC1Startflag=0;
      Hall1Startdirect=0;
      BLDC2Startflag=0;
      Hall2Startdirect=0;
      BLDC3Startflag=0;
      Hall3Startdirect=0;
      TIM1_Cmd(DISABLE); 
      res=FALSE;
      break;
    }
    i++;
  }
  
  return res;
}


u8 Bigmotor_forwardstart(u8 ch,u16 delaytime1)
{ 
  u8 res=0;
  u16 i=0;
  
  start_pwm=0;
  if(start_pwm==0)
  {  
    start_pwm++;
    coilflag.VAL=0;
    Hall1Startdirect=0;
    if(ch==CH1)
      BLDC1Startflag=1;
    Hallstate=hall_test(ch);
    TIM1_Cmd(ENABLE);
    BLDCStart_step=1; 
    BLDCStartSpeed=cylindermotorstatus.cylindermotorstartspeed;
  }  
  
  while(start_pwm<6)
  { 
    if(Hallstate==0x05)
    {  
      Coil_Reset1(ch);
      coilflag.VAL=0;
      coilflag.stcoilflag_val.coilaflag=1;
      delay_us(delaytime1);            
      Hallstate=0x04;	//0x20     
    }
    if(Hallstate==0x01)
    {  
      Coil_Reset3(ch); 
      coilflag.VAL=0;      
      coilflag.stcoilflag_val.coilcflag=1;
      delay_us(delaytime1);
      Hallstate=0x05;  //0XA0    
    }
    if(Hallstate==0x03)
    {    
      Coil_Reset2(ch);  
      coilflag.VAL=0;
      coilflag.stcoilflag_val.coilcflag=1; 
      delay_us(delaytime1);
      Hallstate=0x01;	//0X80				
    }
    if(Hallstate==0x02)
    {  
      Coil_Reset1(ch);
      coilflag.VAL=0;
      coilflag.stcoilflag_val.coilbflag=1;
      delay_us(delaytime1);
      Hallstate=0x03;	//0XC0			     
    }
    if(Hallstate==0x06)
    {  
      Coil_Reset3(ch);
      coilflag.VAL=0;
      coilflag.stcoilflag_val.coilbflag=1;
      delay_us(delaytime1);
      Hallstate=0x02;		//0X40	      
    } 
    if(Hallstate==0x04)
    { 
      Coil_Reset2(ch);
      coilflag.VAL=0;
      coilflag.stcoilflag_val.coilaflag=1;
      delay_us(delaytime1);
      Hallstate=0x06;	//0x60			     
    }
    
    start_pwm++;
    if(start_pwm==6)
    { 
      LASTHallstate1=0;
      BLDCStartSpeed +=2;
      if(BLDCStartSpeed > cylindermotorstatus.cylindermotorspeed)
        BLDCStartSpeed = cylindermotorstatus.cylindermotorspeed;
    }   
  }
  
  while(start_pwm>=6)
  { 
    BLDCmotor_normallyrun(ch,forward);
    
    if(Hall1Startdirect==6)
    {    
      BLDC1Startflag=0;
      Hall1Startdirect=0;
      res=TRUE;
      
      TIM1_Cmd(DISABLE);
      break;  
    }
    
    if(i>=12000)
    {
      BLDC1Startflag=0;
      Hall1Startdirect=0;
      BLDC2Startflag=0;
      Hall2Startdirect=0;
      BLDC3Startflag=0;
      Hall3Startdirect=0;
      BLDCStartSpeed=0;
      
      TIM1_Cmd(DISABLE); 
      res=FALSE;
      break;
    }
    i++;   
  }
  
  return res; 
}

void BLDCmotor_normallyrun(u8 ch,u8 direct)
{
  if(ch==CH1)
    Hallstate1=hall_test(ch);
  
  if(ch==CH2)
    Hallstate2=hall_test(ch);
  
  if(ch==CH3)
    Hallstate3=hall_test(ch);
  
  if(direct==forward)
  {
    if(ch==CH1)	      
    {
      if(LASTHallstate1!=Hallstate1)
      {
        BLDC_LOCKEDROTOR_CNT = 0;
        
        if(BLDC1Startflag==1)
        {
          Hall1Startdirect++;
        }
        
        if(Hallstate1==0x05)        
        {
          LASTHallstate1=Hallstate1;
          
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilaflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1A=1;
          Coil_Reset1(ch);
          Coil_PWM1(ch);
                   
        }       
        if(Hallstate1==0x04)      
        {
          LASTHallstate1=Hallstate1;
          
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilaflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1A=1;
          Coil_Reset2(ch);
          Coil_PWM1(ch);
          
        }        
        if(Hallstate1==0x06)         
        {
          LASTHallstate1=Hallstate1;
          
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilbflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1B=1;
          Coil_Reset3(ch);
          Coil_PWM2(ch);
          
        }        
        if(Hallstate1==0x02)
        {
          LASTHallstate1=Hallstate1;
          
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilbflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1B=1;
          Coil_Reset1(ch);
          Coil_PWM2(ch);
          
        }        
        if(Hallstate1==0x03)
        {
          LASTHallstate1=Hallstate1;
          
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilcflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1C=1;
          Coil_Reset2(ch);
          Coil_PWM3(ch);
          
        }        
        if(Hallstate1==0x01)
        {
          LASTHallstate1=Hallstate1;
          
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilcflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1C=1;
          Coil_Reset3(ch);
          Coil_PWM3(ch);
          
        }
        
      } 
    }
    else if(ch==CH2)      
    {
      if(LASTHallstate2!=Hallstate2)
      { 
        if(BLDC2Startflag==1)
        {
          Hall2Startdirect++;          
        }
        if(Hallstate2==0x05)         
        {
          LASTHallstate2=Hallstate2;
          Coil_Reset1(ch);
          Coil_PWM1(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilaflag=1;
          BLDCmotor2direct.VAL=0;
          BLDCmotor2direct.stdirect2_val.motor2A=1;          
        }
        
        
        if(Hallstate2==0x01)          
        {
          LASTHallstate2=Hallstate2;
          
          Coil_Reset2(ch);
          Coil_PWM1(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilaflag=1;
          BLDCmotor2direct.VAL=0;
          BLDCmotor2direct.stdirect2_val.motor2A=1;
        }
        
        if(Hallstate2==0x03)          
        {
          LASTHallstate2=Hallstate2;
          
          Coil_Reset3(ch);
          Coil_PWM2(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilbflag=1;
          BLDCmotor2direct.VAL=0;
          BLDCmotor2direct.stdirect2_val.motor2B=1;
        }
        
        if(Hallstate2==0x02)         
        {
          LASTHallstate2=Hallstate2;
          
          Coil_Reset1(ch);
          Coil_PWM2(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilbflag=1;
          BLDCmotor2direct.VAL=0;
          BLDCmotor2direct.stdirect2_val.motor2B=1;
        }
        
        if(Hallstate2==0x06)         
        {
          LASTHallstate2=Hallstate2;
          
          Coil_Reset2(ch);
          Coil_PWM3(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilcflag=1;
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilbflag=1;
          BLDCmotor2direct.VAL=0;
          BLDCmotor2direct.stdirect2_val.motor2C=1;
        }
        
        if(Hallstate2==0x04)         
        {
          LASTHallstate2=Hallstate2;
          Coil_Reset3(ch);
          Coil_PWM3(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilcflag=1;
          BLDCmotor2direct.VAL=0;
          BLDCmotor2direct.stdirect2_val.motor2C=1;
        }       
      }
    }
    else if(ch==CH3)       
    {
      if(LASTHallstate3!=Hallstate3)
      { 
        if(BLDC3Startflag==1)
        {
          Hall3Startdirect++;
        }
        if(Hallstate3==0x05)
	  
        {
          LASTHallstate3=Hallstate3;
          Coil_Reset1(ch);
          Coil_PWM1(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilaflag=1;
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3A=1;          
        }
        
        if(Hallstate3==0x01)          
        {
          LASTHallstate3=Hallstate3;
	  
          Coil_Reset2(ch);
          Coil_PWM1(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilaflag=1;
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3A=1;
        }
        
        if(Hallstate3==0x03)          
        {
          LASTHallstate3=Hallstate3;
	  
          Coil_Reset3(ch);
          Coil_PWM2(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilbflag=1;
          
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3B=1;
        }
        
        if(Hallstate3==0x02)          
        {
          LASTHallstate3=Hallstate3;
	  
          Coil_Reset1(ch);
          Coil_PWM2(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilbflag=1;
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3B=1;
        }
        
        if(Hallstate3==0x06)          
        {
          LASTHallstate3=Hallstate3;
	  
          Coil_Reset2(ch);
          Coil_PWM3(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilcflag=1;
          
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3C=1;
        }
        
        if(Hallstate3==0x04)          
        {
          LASTHallstate3=Hallstate3;
          Coil_Reset3(ch);
          Coil_PWM3(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilcflag=1;
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3C=1;
        }
        
      }
    }
    
  }
  
  if(direct==reverse)   
  {
    if(ch==CH1)	    
    {
      if(LASTHallstate1!=Hallstate1)
      {
        BLDC_LOCKEDROTOR_CNT = 0;
        
        if(BLDC1Startflag==1)
        {
          Hall1Startdirect++;
        }
        
        if(Hallstate1==0x40)          
        {
          LASTHallstate1=Hallstate1;
          Coil_Reset1(ch);
          Coil_PWM1(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilaflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1A=1;
          
        }
        
        if(Hallstate1==0xC0)        
        {
          LASTHallstate1=Hallstate1;
          
          Coil_Reset2(ch);
          Coil_PWM1(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilaflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1A=1;
        }
        
        if(Hallstate1==0x80)          
        {
          LASTHallstate1=Hallstate1;
          Coil_Reset3(ch);
          Coil_PWM2(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilbflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1B=1;
        }
        
        if(Hallstate1==0xA0)
        {
          LASTHallstate1=Hallstate1;
          Coil_Reset1(ch);
          Coil_PWM2(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilbflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1B=1;
        }
        
        if(Hallstate1==0x20)
        {
          LASTHallstate1=Hallstate1;
          Coil_Reset2(ch);
          Coil_PWM3(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilcflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1C=1;
        }
        
        
        if(Hallstate1==0x60)         
        {
          LASTHallstate1=Hallstate1;
          Coil_Reset3(ch);
          Coil_PWM3(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilcflag=1;
          BLDCmotor1direct.VAL=0;
          BLDCmotor1direct.stdirect1_val.motor1C=1;
        }       
      }
    }
    else if(ch==CH2)       
    {
      if(LASTHallstate2!=Hallstate2)
      { 
        if(BLDC2Startflag==1)
        {
          Hall2Startdirect++;
        }
      }
      if(Hallstate2==0x02)        
      {
        LASTHallstate2=Hallstate2;
        Coil_Reset1(ch);
        Coil_PWM1(ch);
        coilflag.VAL=0;
        coilflag.stcoilflag_val.coilaflag=1;
        BLDCmotor2direct.VAL=0;
        BLDCmotor2direct.stdirect2_val.motor2A=1;
        
      }
      
      if(Hallstate2==0x06)
        
      {
        LASTHallstate2=Hallstate2;
        
        Coil_Reset2(ch);
        Coil_PWM1(ch);
        coilflag.VAL=0;
        coilflag.stcoilflag_val.coilaflag=1;
        BLDCmotor2direct.VAL=0;
        BLDCmotor2direct.stdirect2_val.motor2A=1;
      }
      
      if(Hallstate2==0x04)        
      {
        LASTHallstate2=Hallstate2;
        
        Coil_Reset3(ch);
        Coil_PWM2(ch);
        
        coilflag.VAL=0;
        coilflag.stcoilflag_val.coilaflag=1;
        BLDCmotor2direct.VAL=0;
        BLDCmotor2direct.stdirect2_val.motor2B=1;
      }
      
      if(Hallstate2==0x05)       
      {
        LASTHallstate2=Hallstate2;
        
        Coil_Reset1(ch);
        Coil_PWM2(ch);
        coilflag.VAL=0;
        coilflag.stcoilflag_val.coilaflag=1;
        BLDCmotor2direct.VAL=0;
        BLDCmotor2direct.stdirect2_val.motor2B=1;
      }
      
      if(Hallstate2==0x01)        
      {
        LASTHallstate2=Hallstate2;
        
        Coil_Reset2(ch);
        Coil_PWM3(ch);
        coilflag.VAL=0;
        coilflag.stcoilflag_val.coilcflag=1;
        BLDCmotor2direct.VAL=0;
        BLDCmotor2direct.stdirect2_val.motor2C=1;
      }
      
      if(Hallstate2==0x03)        
      {
        LASTHallstate2=Hallstate2;
        Coil_Reset3(ch);
        Coil_PWM3(ch);
        coilflag.VAL=0;
        coilflag.stcoilflag_val.coilcflag=1;
        BLDCmotor2direct.VAL=0;
        BLDCmotor2direct.stdirect2_val.motor2C=1;
      }
      
    } 
    else if(ch==CH3)       
    {
      if(LASTHallstate3!=Hallstate3)
      { 
        if(BLDC3Startflag==1)
        {
          Hall3Startdirect++;
        }
        if(Hallstate3==0x02)
	  
        {
          LASTHallstate3=Hallstate3;
          Coil_Reset1(ch);
          Coil_PWM1(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilaflag=1;
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3A=1;         
        }
        
        if(Hallstate3==0x06)          
        {
          LASTHallstate3=Hallstate3;
	  
          Coil_Reset2(ch);
          Coil_PWM1(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilaflag=1;
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3A=1;
        }
        
        if(Hallstate3==0x04)          
        {
          LASTHallstate3=Hallstate3;
	  
          Coil_Reset3(ch);
          Coil_PWM2(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilbflag=1;
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3B=1;
        }
        
        if(Hallstate3==0x05)          
        {
          LASTHallstate3=Hallstate3;
	  
          Coil_Reset1(ch);
          Coil_PWM2(ch);
          coilflag.VAL=0;
          
          coilflag.stcoilflag_val.coilbflag=1;
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3B=1;
        }
        
        if(Hallstate3==0x01)         
        {
          LASTHallstate3=Hallstate3;
	  
          Coil_Reset2(ch);
          Coil_PWM3(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilcflag=1;
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3C=1;
        }
        
        
        if(Hallstate3==0x03)          
        {
          LASTHallstate3=Hallstate3;
          Coil_Reset3(ch);
          Coil_PWM3(ch);
          coilflag.VAL=0;
          coilflag.stcoilflag_val.coilcflag=1;
          BLDCmotor3direct.VAL=0;
          BLDCmotor3direct.stdirect3_val.motor3C=1;
        }
        
      }
    }
    
  }
  
}




void Coil_PWM1(u8 ch)
{
  if(ch==CH1)
  {
    Coil1_PWM1;
    
  }
  //  else if(ch==CH2)
  //  {
  //    Coil2_PWM1;
  //    
  //  }
  //  else if(ch==CH3)
  //  {Coil3_PWM1;}
  
}


void Coil_PWM2(u8 ch)
{
  if(ch==CH1)
  {
    Coil1_PWM2;
    
  }
  //  else if(ch==CH2)
  //  {
  //    Coil2_PWM2;
  //    
  //  }
  //  else if(ch==CH3)
  //  {Coil3_PWM2;}
  
}
void Coil_PWM3(u8 ch)
{
  if(ch==CH1)
  {
    Coil1_PWM3;
    
  }
  //  else if(ch==CH2)
  //  {
  //    Coil2_PWM3;
  //    
  //  }
  //  else if(ch==CH3)
  //  {Coil3_PWM3;}
  
}

void Coil_PWMOFF(u8 ch)
{
  
  if(ch==CH1)
  {
    Coil1_PWMOFF;
    
  }
  //  else if(ch==CH2)
  //  {
  //    Coil2_PWMOFF;
  //    
  //  }
  //  else if(ch==CH3)
  //  {Coil3_PWMOFF;}
  
  
}

void Coil_Reset1(u8 ch)
{
  if(ch==CH1)
  {
    Coil1_Reset1;   
  }
  //  else if(ch==CH2)
  //  {
  //    Coil2_Reset1;
  //    
  //  }
  //  else if(ch==CH3)
  //  {Coil3_Reset1;}
  
}

void Coil_Reset2(u8 ch)
{
  if(ch==CH1)
  {
    Coil1_Reset2;
    
  }
  //  else if(ch==CH2)
  //  {
  //    Coil2_Reset2;
  //    
  //  }
  //  else if(ch==CH3)
  //  {Coil3_Reset2;}
  
}


void Coil_Reset3(u8 ch)
{
  if(ch==CH1)
  {
    Coil1_Reset3;
    
  }
  //  else if(ch==CH2)
  //  {
  //    Coil2_Reset3;
  //    
  //  }
  //  else if(ch==CH3)
  //  {Coil3_Reset3;}
  
}


void Coil_ResetOFF(u8 ch)
{
  if(ch==CH1)
  {
    Coil1_RESETOFF;
    
  }
  //  else if(ch==CH2)
  //  {
  //    Coil2_RESETOFF;
  //    
  //  }
  //  else if(ch==CH3)
  //  {Coil3_RESETOFF;}
  
}

u8 hall_test(u8 ch)	  
{
  u8 res=0;
  u8 temp=0,g=0;
  
  if(ch==CH1)
  {  
    while (g < 3)     // while less that 3 good samples
    {
      temp=(HALL1A<<2);//(HALL1C>>2);  
      temp |=(HALL1B<<1);//(HALL1B>>2);
      temp |=(HALL1C);//(HALL1A>>2);
      
      if (temp == res)         // if the same
        g++;              // one more good
      else
        g = 0;            // else start over
      res = temp;            // update res
    }
  }
  
  //  if(ch==CH2)
  //  {
  //    res=HALL2C;  
  //    res=(res<<1)|HALL2B;
  //    res=(res<<1)|HALL2A;
  //  }
  //  if(ch==CH3)
  //  {
  //    res=HALL3C;  
  //    res=(res<<1)|HALL3B;
  //    res=(res<<1)|HALL3A;
  //  }
  return res;
}



void MECHARM_Cylinderctrl(u8 type,u8 direct)
{ 
  if((type==ON)&&(STOPFlag!=TRUE))
  {
    bldc_fault_flag = FALSE;
    bldc_fault_code = fault_none;
    STOPFlag = TRUE;
    BLDC_LOCKEDROTOR_CNT = 0;
    bldc_start_flag = FALSE;
    
    TIMER3_INIT(TIM3_PRESCALER_16,50);//work pwm
    TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG = TM3_BLDC;
    if(cylindermotorstatus.cylindermotorRUN==FALSE)	 
    {
      cylindermotorstatus.cylindermotordirect=direct;	
      cylindermotorstatus.cylindermotorRUN=Bigmotor_Start(CH1,cylindermotorstatus.cylindermotorprebootspeed,cylindermotorstatus.cylindermotordirect);
      if(cylindermotorstatus.cylindermotorRUN==TRUE)
      {
        //        TIM_Cmd(TIM5,ENABLE);
        TIM4_Cmd(ENABLE);       
        if(cylindermotorstatus.cylindermotorspeed!=0)          
        {
          BLDCmotor1direct.stdirect1_val.motor1step=0;
          //          TIM_Cmd(TIM7,ENABLE);
          TIM3_Cmd(ENABLE);
        }
        tasknum=task_bldc;
        bldc_start_flag = TRUE;
      }
      else{
        cylindermotorstatus.cylindermotorRUN=FALSE;        
        TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG = TM3_RFID;
        
        TIMER3_INIT(TIM3_PRESCALER_128,124);//DELAY 1MS FOR RFID
        
        Coil_PWMOFF(CH1);
        Coil_ResetOFF(CH1);

          //      TIM_Cmd(TIM5,DISABLE);
          TIM4_Cmd(DISABLE);

        
        bldc_fault_flag = FALSE;
        bldc_fault_code = fault_none;
        STOPFlag = FALSE;
        BLDC_LOCKEDROTOR_CNT = 0;
        
        LED_OPTION_PROCESS(LED_7,OFF);//bldc run indicate led
        LED_OPTION_PROCESS(LED_8,OFF);//bldc FAULT indicate led
        
        bldc_start_flag = FALSE;
        tasknum=task_rfid;
      }      
    }
  }
  else if(type==OFF)
  {
    cylindermotorstatus.cylindermotorRUN=FALSE;
    
    Coil_PWMOFF(CH1);
    Coil_ResetOFF(CH1);
    if(cylindermotorstatus.cylindermotorRUN==FALSE)//((cylindermotorstatus.cylindermotorRUN==OFF)&&(drivermotorstatus.leftdrivermotorRUN==OFF)&&(drivermotorstatus.rightdrivermotorRUN==OFF))
    {
      //      TIM_Cmd(TIM5,DISABLE);
      TIM4_Cmd(DISABLE);
      //          TIM_Cmd(TIM7,DISABLE);      
      TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG = TM3_NONE;
      TIM3_Cmd(DISABLE);
    }
    
    bldc_fault_flag = FALSE;
    bldc_fault_code = fault_none;
    STOPFlag = FALSE;
    BLDC_LOCKEDROTOR_CNT = 0;
    
    LED_OPTION_PROCESS(LED_7,OFF);//bldc run indicate led
    LED_OPTION_PROCESS(LED_8,OFF);//bldc FAULT indicate led
    
    bldc_start_flag = FALSE;
    tasknum=task_rfid;
  }
  
}


//
//#pragma vector=0xD
//__interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void)
//{ 
//  u8 ch=0;
//  
//  if (TIM1_GetITStatus(TIM1_IT_UPDATE) != RESET)    
//  {   
//    if(BLDC1Startflag)
//      ch=CH1;
//    if(BLDC2Startflag)
//      ch=CH2;
//    if(BLDC3Startflag)
//      ch=CH3;  
//    
//    if(coilflag.stcoilflag_val.coilaflag==1)
//    {
//      if(BLDCStart_step<=BLDCStartSpeed)
//        Coil_PWM1(ch);					 
//      if(BLDCStart_step>BLDCStartSpeed) 				
//        Coil_PWMOFF(ch);			      
//    }
//    
//    
//    if(coilflag.stcoilflag_val.coilbflag==1)
//    {
//      if(BLDCStart_step<=BLDCStartSpeed)
//        Coil_PWM2(ch);
//      if(BLDCStart_step>BLDCStartSpeed)
//        Coil_PWMOFF(ch);            
//    }
//    
//    if(coilflag.stcoilflag_val.coilcflag==1)
//    {
//      
//      if(BLDCStart_step<=BLDCStartSpeed)
//        Coil_PWM3(ch);
//      if(BLDCStart_step>BLDCStartSpeed)
//        Coil_PWMOFF(ch);
//    }
//    
//    BLDCStart_step++;
//    if(BLDCStart_step==11)
//    {BLDCStart_step=1;}
//    
//  }
//  
//  TIM1_ClearITPendingBit(TIM1_IT_UPDATE|TIM1_IT_CC1|TIM1_IT_CC2|TIM1_IT_CC3|TIM1_IT_CC4);
//}
//
//
////#pragma vector=0x06
////__interrupt void PORTB_IRQHandler(void)   //BLDC fault code
////{
////  if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_0) == RESET)
////  {
////    if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_1) == RESET)
////    {
////      bldc_fault_flag = TRUE;
////      bldc_fault_code = fault_overtemperwaring_andoneofthe_overtempershutdown_or_overcurrentshutdown_or_undervoltage_occur;
////    }
////    else{
////      bldc_fault_flag = TRUE;
////      bldc_fault_code = fault_overtemperwaring;
////    }
////    
////  }
////  else{
////    if(GPIO_ReadInputPin(GPIOB,GPIO_PIN_1) == RESET)
////    {
////      bldc_fault_code = TRUE;
////      step_motor_fault_code = fault_voercurrentshutdown_undervoltageprotection_occur;
////    }
////    
////  } 
////}
//
//#pragma vector=25
//__interrupt void TIM4_OVR_UIF_vector(void)
//{    
//  if (TIM4_GetITStatus(TIM4_IT_UPDATE) != RESET)   
//  {	     
//    //    if(drivermotorstatus.leftdrivermotorRUN)
//    //    { BLDCmotor_normallyrun(CH2,drivermotorstatus.leftdrirvermotordirect);
//    //    
//    //    if(drivermotorstatus.leftdrivermotor_stoponbaseposition)
//    //    {
//    //      
//    //      drivermotorstatus.leftdrivermotorsteps++;
//    //      if(drivermotorstatus.leftdrivermotorsteps>=10000)
//    //      {
//    //        MECHARM_Leftdrivermotorctrl(STOP,forward);
//    //        SPM_Respond(MechanicalARM_warnning,ARM_Leftdrivererror); 
//    //        MECHARM_TASK=non_task;
//    //      }
//    //    }
//    //    }
//    //    
//    //    if(drivermotorstatus.rightdrivermotorRUN)
//    //    {
//    //      BLDCmotor_normallyrun(CH3,drivermotorstatus.rightdrivermotordirect);
//    //      
//    //      if(drivermotorstatus.rightdrivermotor_stoponbaseposition)
//    //      {
//    //        drivermotorstatus.rightdrivermotorsteps++;
//    //        if(drivermotorstatus.rightdrivermotorsteps>=10000)
//    //        {
//    //          
//    //          MECHARM_Rightdrivermotorctrl(STOP,forward);
//    //          SPM_Respond(MechanicalARM_warnning,ARM_Rightdrivererror); 
//    //          MECHARM_TASK=non_task;
//    //        }
//    //      }
//    //    }
//    //
//
//    if( BLDC_LOCKEDROTOR_CNT >= 100 )
//    {
//      bldc_fault_flag = TRUE;
//      bldc_fault_code = run_lockedrotor;
//    }
//    BLDC_LOCKEDROTOR_CNT++;
//    
//    if(cylindermotorstatus.cylindermotorRUN)
//      BLDCmotor_normallyrun(CH1,cylindermotorstatus.cylindermotordirect);   
//  }
//  
//  TIM4_ClearITPendingBit(TIM4_IT_UPDATE); 
//}




