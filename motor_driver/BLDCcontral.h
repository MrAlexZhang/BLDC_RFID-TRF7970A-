#ifndef __BLDC_H
#define __BLDC_H

///////////////BLDC PIN CONFIG///////////////////////////
#define M1_8332_H GPIO_WriteHigh(GPIOB,GPIO_PIN_5)  //PB5--BLDC M1 PIN
#define M1_8332_L GPIO_WriteLow(GPIOB,GPIO_PIN_5)  //PB5--BLDC M1 PIN

#define RESETA_8332_H GPIO_WriteHigh(GPIOB,GPIO_PIN_3)  //PB3--BLDC RESET PWMA PIN
#define RESETA_8332_L GPIO_WriteLow(GPIOB,GPIO_PIN_3)  //PB3--BLDC RESET PWMA PIN

#define RESETB_8332_H GPIO_WriteHigh(GPIOB,GPIO_PIN_6)  //PB6--BLDC RESET PWMB PIN
#define RESETB_8332_L GPIO_WriteLow(GPIOB,GPIO_PIN_6)  //PB6--BLDC RESET PWMB PIN

#define RESETC_8332_H GPIO_WriteHigh(GPIOB,GPIO_PIN_7)  //PB7--BLDC RESET PWMC PIN
#define RESETC_8332_L GPIO_WriteLow(GPIOB,GPIO_PIN_7)  //PB7--BLDC RESET PWMC PIN

#define PWMA_8332_H GPIO_WriteHigh(GPIOC,GPIO_PIN_1)  //PC1--BLDC PWMA PIN
#define PWMA_8332_L GPIO_WriteLow(GPIOC,GPIO_PIN_1)  //PC1--BLDC PWMA PIN

#define PWMB_8332_H GPIO_WriteHigh(GPIOC,GPIO_PIN_2)  //PC2--BLDC PWMB PIN
#define PWMB_8332_L GPIO_WriteLow(GPIOC,GPIO_PIN_2)  //PC2--BLDC PWMB PIN

#define PWMC_8332_H GPIO_WriteHigh(GPIOC,GPIO_PIN_3)  //PC3--BLDC PWMC PIN
#define PWMC_8332_L GPIO_WriteLow(GPIOC,GPIO_PIN_3)  //PC3--BLDC PWMC PIN

#define HALL1A      GPIO_ReadInputPin(GPIOD,GPIO_PIN_4)
#define HALL1B      GPIO_ReadInputPin(GPIOD,GPIO_PIN_3)
#define HALL1C      GPIO_ReadInputPin(GPIOD,GPIO_PIN_2)


#define Coil1_PWM1 	        {PWMA_8332_H;PWMB_8332_L;PWMC_8332_L;} 
#define Coil1_PWM2 	        {PWMA_8332_L;PWMB_8332_H;PWMC_8332_L;} 
#define Coil1_PWM3 	        {PWMA_8332_L;PWMB_8332_L;PWMC_8332_H;} 
#define Coil1_PWMOFF         {PWMA_8332_L;PWMB_8332_L;PWMC_8332_L;} 

#define Coil1_Reset1         {RESETA_8332_H;RESETB_8332_H;RESETC_8332_L;}
#define Coil1_Reset2         {RESETA_8332_H;RESETB_8332_L;RESETC_8332_H;}
#define Coil1_Reset3         {RESETA_8332_L;RESETB_8332_H;RESETC_8332_H;}
#define Coil1_RESETOFF       {RESETA_8332_H;RESETB_8332_H;RESETC_8332_H;} 


typedef union tagcoilflagval
{ 
  u8 VAL;
  struct tag_coilflag_val
  {
    u8   coilaflag        :1;
    u8   coilbflag        :1;
    u8  coilcflag         :1;
    u8  coildflag         :1;
    u8  coileflag         :1;
    u8  coilfflag        :1;
    u8 rev               :2;
    
  }
  stcoilflag_val ;
}
stcoilflagval;

typedef union tagBLDCmotor1directval
{
  u8 VAL;
  struct tag_direct1_val
  {
    u8   motor1A          :1;
    u8   motor1B          :1;
    u8   motor1C          :1;		     
    u8 motor1step         :5;
    
  }
  stdirect1_val;
}
stBLDCmotor1directval;


typedef union tagBLDCmotor2directval
{
  u8 VAL;
  struct tag_direct2_val
  {
    u8   motor2A         :1;
    u8   motor2B         :1;
    u8   motor2C          :1;		     
    u8 motor2step         :5;
    
  }
  stdirect2_val;
}
stBLDCmotor2directval;


typedef union tagBLDCmotor3directval
{
  u8 VAL;
  struct tag_direct3_val
  {
    u8   motor3A         :1;
    u8   motor3B         :1;
    u8   motor3C          :1;
    
    u8 motor3step                 :5;
    
  }
  stdirect3_val;
}
stBLDCmotor3directval;


typedef  struct tagdrivermotorstatus
{
  u8	 leftdrivermotorRUN 	                 ;
  u8	 leftdrirvermotordirect 	             ;
  u16 leftdrivermotorsteps 	                 ;
  u8  leftdrivermovemode                       ;
  u8  leftdrivermotor_baseposition            ;
  u8  leftdrivermotor_stoponbaseposition      ;
  u8  leftdriver_passbaseNUM                 ;
  u16 leftdriver_speedlevel                       ;
  u8	 rightdrivermotorRUN 	                 ;
  u8	 rightdrivermotordirect 	             ;
  u16 rightdrivermotorsteps 	                 ;
  u8  rightdrivermovemode                     ;
  u8  rightdrivermotor_baseposition           ;
  u8  rightdrivermotor_stoponbaseposition      ;
  u8  rightdriver_passbaseNUM                 ;	
  u16 rightdriver_speedlevel               ;
}
stdrivermotorstatus;


typedef struct	tagcylindermotorstatus
{
  u8 cyclindermotortype;
  u8 cylindermotorRUN       ; 
  u8 cylindermotordirect    ;
  u8 cylindermotorspeed   ; 
  u8 cylindermotorstartspeed ;
  u16 cylindermotorprebootspeed ;
}stcylindermotorstatus;

enum{forward,reverse};
enum{CH0,CH1,CH2,CH3,CH4,CH5,CH6,CH7,CH8};

#ifdef _BLDC_VER
#define BLDC_EXT 
#else
#define BLDC_EXT extern  
#endif

BLDC_EXT bool bldc_fault_flag;

BLDC_EXT u8 bldc_fault_code;

BLDC_EXT u8 STOPFlag;

BLDC_EXT volatile u16 BLDC_LOCKEDROTOR_CNT; 
BLDC_EXT bool bldc_start_flag;


BLDC_EXT u8 motor_startflag;

BLDC_EXT u8 start_pwm;
BLDC_EXT u8 Hallstate;
BLDC_EXT u8 Hallstate1;
BLDC_EXT u8 Hallstate2;
BLDC_EXT u8 Hallstate3;
BLDC_EXT u8 LASTHallstate1;
BLDC_EXT u8 LASTHallstate2;
BLDC_EXT u8 LASTHallstate3;

BLDC_EXT u8 Hall1Startdirect;
BLDC_EXT u8 Hall2Startdirect;
BLDC_EXT u8 Hall3Startdirect;

BLDC_EXT u8 BLDCStart_step;
BLDC_EXT u8 BLDC1Startflag;
BLDC_EXT u8 BLDC2Startflag;
BLDC_EXT u8 BLDC3Startflag;
BLDC_EXT u8 BLDC1SpeedLevel;
BLDC_EXT u8 BLDCStartSpeed;


BLDC_EXT stcoilflagval coilflag; 
BLDC_EXT stdrivermotorstatus drivermotorstatus;
BLDC_EXT stcylindermotorstatus cylindermotorstatus;
BLDC_EXT stBLDCmotor1directval BLDCmotor1direct;
BLDC_EXT stBLDCmotor2directval BLDCmotor2direct;
BLDC_EXT stBLDCmotor3directval BLDCmotor3direct;


BLDC_EXT void DRV8332INIT(void);
BLDC_EXT void BLDC_SYSINIT(void);

BLDC_EXT void MECHARM_Cylinderctrl(u8 type,u8 direct);

BLDC_EXT u8  hall_test(u8 ch);


#endif



