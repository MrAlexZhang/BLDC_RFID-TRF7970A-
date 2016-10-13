#ifndef __Driver_H
#define __Driver_H


#define Coil_ch1A_H 	   GPIO_WriteHigh(GPIOG,GPIO_PIN_6)  //PG6 -- PWM_AB
#define Coil_ch1A_L	   GPIO_WriteLow(GPIOG,GPIO_PIN_6)  //PG6 -- PWM_AB

#define Coil_ch1B_H 	   GPIO_WriteHigh(GPIOG,GPIO_PIN_7)  //PG7 -- PWM_CD
#define Coil_ch1B_L 	   GPIO_WriteLow(GPIOG,GPIO_PIN_7)  //PG7 -- PWM_CD

#define  A_B_ENBL_H       GPIO_WriteHigh(GPIOD,GPIO_PIN_0)  //--PD0
#define  A_B_ENBL_L       GPIO_WriteLow(GPIOD,GPIO_PIN_0)  //--PD0

//#define C_D_ENABL_H GPIO_WriteHigh(GPIOA,GPIO_PIN_1)  //--PA1
//#define C_D_ENABL_L GPIO_WriteLow(GPIOA,GPIO_PIN_1)  //--PA1

#define RESET_AB_8432_H GPIO_WriteHigh(GPIOG,GPIO_PIN_5)  //--PG5--RESET_AB
#define RESET_AB_8432_L GPIO_WriteLow(GPIOG,GPIO_PIN_5)  //--PG5--RESET_AB

#define RESET_CD_8432_H GPIO_WriteHigh(GPIOD,GPIO_PIN_7)  //--PD7
#define RESET_CD_8432_L GPIO_WriteLow(GPIOD,GPIO_PIN_7)  //--PD7

#define M1_8432_H GPIO_WriteHigh(GPIOA,GPIO_PIN_5)  //--PA5
#define M1_8432_L GPIO_WriteLow(GPIOA,GPIO_PIN_5)  //--PA5

#define M2_8432_H GPIO_WriteHigh(GPIOA,GPIO_PIN_4)  //--PA4
#define M2_8432_L GPIO_WriteLow(GPIOA,GPIO_PIN_4)  //--PA4

#define M3_8432_H GPIO_WriteHigh(GPIOA,GPIO_PIN_3)  //--PA3
#define M3_8432_L GPIO_WriteLow(GPIOA,GPIO_PIN_3)  //--PA3

#define OTW_IRQ_ON		GPIOD->CR2 |= GPIO_PIN_6 	//ENABLE OTW IRQ
#define OTW_IRQ_OFF		GPIOD->CR2 &= ~GPIO_PIN_6   //DISABLE OTW IRQ

#define FAULT_IRQ_ON		GPIOD->CR2 |= GPIO_PIN_5 	//ENABLE FAULT IRQ
#define FAULT_IRQ_OFF		GPIOD->CR2 &= ~GPIO_PIN_5   //DISABLE FAULT IRQ



#define coil1_ch1A      {Coil_ch1A_H; Coil_ch1B_H;}     
#define coil1_ch1B      {Coil_ch1A_H; Coil_ch1B_L;}
#define coil1_ch1C      {Coil_ch1A_L; Coil_ch1B_L;}  
#define coil1_ch1D      {Coil_ch1A_L; Coil_ch1B_H;} 

#define coil2_ch1A      {Coil_ch1A_H; Coil_ch1B_H;}     
#define coil2_ch1B      {Coil_ch1A_L; Coil_ch1B_H;}
#define coil2_ch1C      {Coil_ch1A_L; Coil_ch1B_L;}  
#define coil2_ch1D      {Coil_ch1A_H; Coil_ch1B_L;} 

#define coil_ch1off   {Coil_ch1A_H;Coil_ch1B_H;A_B_ENBL_L;}


//
//enum{medicine1_putouttype,medicine2_putouttype,medicine1_putintype,medicine2_putintype};
//enum{STOP,ON,OFF,PUSH,RETURNBACK};
//enum{forward,reverse};
//enum{LeftCH,RightCH};
//enum{pushmotor,leftboardmotor,rightboardmotor,leftdrivermotor,rightdrivermotor,allmotor};
//enum{non_error,
//plat_pushbaseerror,
//plat_boardbaseerror,
//ARM_SENDNUMerror,
//ARM_NONmedicineerror,
//ARM_Leftboarderror,
//ARM_Rightboarderror,
//ARM_Leftdrivererror,
//ARM_Rightdrivererror,
//ARM_PUSHdrivererror,
//ARM_IRerror,
//ARM_PLCsignalerror,
//ARM_Leftboardbaseerror,
//ARM_Rightboardbaseerror,
//ARM_Leftdriverbaseerror,
//ARM_Rightdriverbaseerror,
//ARM_Pushleftlimiterror,
//ARM_PushRightlimitpointerror,
//ARM_TIMEOUTerror,
//ARM_cylindererror,
//};


typedef  struct tagPushmotorstatus
{
  u8	 PushmotorRUN 	               ;
  u8	 Pushmotordirect 	           ;
  u16 Pushmotorsteps 	           ;
  u8  movemode                      ;
  u8 Pushmotor_leftlimitpoint	   ;
  u8 Pushmotor_rightlimitpoint 	   ;		 
}
stPushmotorstatus;


typedef  struct tagboardmotorstatus
{
  u8	 leftboardmotorRUN 	          ;
  u8	 leftboardmotordirect 	      ;
  u16 leftboardmotorsteps 	      ;
  u8  leftboardmovemode                      ;
  u8  leftboardmotor_baseposition      ;
  u8  leftboardmotor_limitpoint	      ;
  u8	 rightboardmotorRUN 	          ;
  u8	 rightboardmotordirect 	          ;
  u16 rightboardmotorsteps 	          ;
  u8  rightboardmovemode              ;
  u8  rightboardmotor_baseposition     ;
  u8  rihgtboardmotor_limitpoint	      ;			 
  
}
stboardmotorstatus;



typedef struct tagplatformstatus 
{
  u8  platemotorstaus       ;
  u8  platemovedirect       ;
  u16 platemotorsteps       ;
  u8  Platemotorposition    ;
  u8  boardmotorstaus       ;
  u16 boardmotorsteps       ;
  u8  boardmovedirect        ;
  u8  boardmotorposition    ;
  u8  KEYstatus             ;
  u8  LEDstatus             ;
  u8 pushmedicine           ;
  u8 perparemedicine        ;
  u8 PLCOnPlace             ;
  u8 updata_medicinelength  ;
  u8  Gratingflag   ;
}
stSetplatformstatus ;



typedef struct tagMECHARMinfo
{
  u8   getmedicinestaus              ;
  u8   returnmedicinestaus           ;
  u8  sendmedicinestaus              ;
  u8  getmedicineNUM                 ;
  u8  getmedicineNUMback             ;
  u16 getmedicinetotalwide           ;
  u16 getmedicinetotalwideadjust     ;
  u8  putmedeicinedirect             ;
  u8  ARMdetectIR                    ;
  u8  sendmedicineNUM                ;
  u8  sendTOlastslot                 ;
  u8  sendslotNUM                    ;
  u8  currentsendmedicine             ;
}
stMECHARMinfo ;


typedef struct tagmotorparameter
{
  u16  motor1PPS       ;		
  u16  motor2PPS       ;
  u16  motor3PPS       ;			
  u8   motor1DAC       ;
  u8   motor2DAC       ;
  u8   motor3DAC       ;
  
}
SETmotorparameter ;

typedef struct stepmotorstartupparameter
{
  u16 step_work_pps;
  u16 step_motor_current_pps;
  u16 step_max_pps;
  u16 step_min_pps;
  
  u8 step_motor_limit_sensor;
  
  u16 step_acc_value;
  u16 step_time_interval;
  u8 step_motor_acc_cnt;
  u8 step_motor_acc_step;
  u16 tim_period;
  u32 step_motor_total_num;
  u8 step_motor_enable_acc_flag;
  u16 step_motor_start_pps;
  bool step_motor_acc_done;
  
  u16 step_motor_soft_start_cnt;
  u8 step_motor_pwm_duty_cnt;
  
  stSetplatformstatus stepmotorstatus;
}motorstartupparameter;

enum{fault_none,fault_overtemperwaring,fault_voercurrentshutdown_undervoltageprotection_occur,fault_overtemperwaring_andoneofthe_overtempershutdown_or_overcurrentshutdown_or_undervoltage_occur,run_lockedrotor,BLDC_WORK_FAULT};
enum{TM3_NONE,TM3_STEPMOTOR,TM3_BLDC,TM3_RFID};

#ifdef _STEP_MOTOR_Driver_VER
#define MOTOT_EXT 
#else
#define MOTOT_EXT extern 
#endif

MOTOT_EXT motorstartupparameter step_motor_start_up_parameter;

MOTOT_EXT u8 step_motor_fault_code;
MOTOT_EXT bool step_motor_fault_flag;

MOTOT_EXT bool a_b_pwm_flag;
MOTOT_EXT bool c_d_pwm_flag;
MOTOT_EXT volatile u8 t3_pwm_cnt;
MOTOT_EXT volatile bool step_frequency_calt_start_flag;

MOTOT_EXT u8 TIM3_USERFOR_BLDC_OR_STEPMOTOR_FLAG;

MOTOT_EXT u8 STEP_MOTOR_PWM_CNT;


MOTOT_EXT void MOTOR_INITIAL(void);
MOTOT_EXT void motor_move_ctrol(u8 move);
MOTOT_EXT void motor_move_mode_contral(void);
MOTOT_EXT void step_motor_parameter_initial(void);


#endif



