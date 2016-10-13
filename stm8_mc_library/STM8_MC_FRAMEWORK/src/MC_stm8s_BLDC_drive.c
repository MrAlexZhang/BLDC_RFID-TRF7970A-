/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_stm8s_BLDC_drive.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : Low level BLDC drive module
********************************************************************************
* History:
* mm/dd/yyyy ver. x.y.z
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Includes ******************************************************/
#include "MC_dev_drive.h"

#include "stm8s.h"//"stm8s_lib.h"

#include "MC_stm8s_clk_param.h"
#include "MC_stm8s_BLDC_param.h"

#include "MC_Faults.h"

#include "MC_vtimer.h" 						
#include "MC_BLDC_timers.h" 			
#include "MC_bldc_motor.h"				
#include "MC_BLDC_Motor_Param.h"  
#include "MC_BLDC_Drive_Param.h"  
#include "MC_PowerStage_Param.h" 	
#include "MC_ControlStage_Param.h" 	

#include "Globe.h"

#include "MC_BLDC_conf.h"  // Include sensor configuration
#ifdef HALL
#include "MC_stm8s_hall_param.h"
#endif

#ifdef DAC_FUNCTIONALITY
#include "MC_dev_DAC.h"
#endif

#ifndef PWM_LOWSIDE_OUTPUT_ENABLE
#define LS_GPIO_CONTROL

#if (defined(LS_GPIO_CONTROL) && (FAST_DEMAG != 0))
#error "Invalid configuration: Is not possible to enable the FAST_DEMAG option with LS_GPIO_CONTROL"
#endif
#endif

void bldc_soft_start(void);

u8 bldc_soft_start_cnt = 0;

/*private prototypes ******************************************************/
void dev_BLDC_driveUpdate(void);
void Application_ADC_Manager( void );
void GetCurrent(void);
void GetSyncUserAdc(void);
void GetBusVoltage(void);
void GetTemperature(void);
void GetNeutralPoint(void);
void GetAsyncUserAdc(void);
void GetAsyncUserAdc2(void);
void GetStepTime(void);
void SpeedMeasurement(void);
void DebugPinsOff(void);
void Init_TIM1( void );
void Init_TIM2( void );
void TIM2_InitCapturePolarity(void);
void ComHandler(void);
void Commutate_Motor( void );
void StopMotor( void );
void BrakeMotor( void );
u8 BootStrap(void);
u8 AlignRotor( void );
void StartMotor( void );
void Init_ADC( void );
void Enable_ADC_BEMF_Sampling( void );
void Enable_ADC_Current_Sampling( void );
void Enable_ADC_User_Sync_Sampling( void );
u16 CheckMaxDuty(u16 bRequested_Duty);
void DelayCoefAdjust(void);
u16 Set_Duty(u16 duty);
u16 Set_Current(u16 current);
void SetSamplingPoint_BEMF(void);
void SetSamplingPoint_Current(void);
void SetSamplingPoint_User_Sync(void);

#ifdef LS_GPIO_CONTROL
void LS_GPIO_MANAGE(void);
void LS_GPIO_OFF(void);
void LS_GPIO_BRAKE(void);
void LS_GPIO_BOOT(void);
u8 LS_Conf;
#endif

/* Private Typedef */
typedef enum
{STARTUP_IDLE,STARTUP_BOOTSTRAP,STARTUP_ALIGN,STARTUP_START,STARTUP_RAMPING} StartUpStatus_t;

static StartUpStatus_t StartUpStatus = STARTUP_IDLE;

/* Private vars and define */
//#define BIT0 0x01
//#define BIT1 0x02
//#define BIT2 0x04
//#define BIT3 0x08
//#define BIT4 0x10
//#define BIT5 0x20
//#define BIT6 0x40
//#define BIT7 0x80

#define ARRTIM2 500

#ifdef SENSORLESS
#define ARR_CURRENT_REF_TIM ARRTIM2
#endif
#ifdef HALL
#define ARR_CURRENT_REF_TIM hArrPwmVal
#endif

#define MILLIAMP_TOCNT(ma) (((((u32)ma * RS_M)/1000) * AOP * (u32)ARR_CURRENT_REF_TIM)/5000)
#define ADC_TOMILLIAMP(adc) ((((u32)adc * 1000 * 1000) / ((u32)1024 * AOP * RS_M)) * 5)

// Internal global pointer to Device and BLDC struct
pvdev_device_t g_pDevice = 0;
PBLDC_Struct_t g_pBLDC_Struct = 0;
static PBLDC_Var_t g_pMotorVar = 0;

u8 ADC_State;
#define ADC_SYNC	0x00
#define ADC_ASYNC	0x01

u8 ADC_Sync_State = 0x04;
#define ADC_BEMF_INIT			0x00
#define ADC_BEMF_SAMPLE			0x01
#define ADC_CURRENT_INIT		0x02
#define ADC_CURRENT_SAMPLE		0x03
#define ADC_USER_SYNC_INIT		0x04
#define ADC_USER_SYNC_SAMPLE	0x05

u8 ADC_Async_State = 0;
#define ADC_BUS_INIT				0x00
#define ADC_BUS_SAMPLE				0x01
#define ADC_TEMP_INIT				0x02
#define ADC_TEMP_SAMPLE				0x03
#define ADC_NEUTRAL_POINT_INIT		0x04
#define ADC_NEUTRAL_POINT_SAMPLE	0x05
#define ADC_USER_ASYNC_INIT			0x06
#define ADC_USER_ASYNC_SAMPLE		0x07
#define ADC_USER_ASYNC_INIT_2		0x08
#define ADC_USER_ASYNC_SAMPLE_2		0x09

// ADC_USER_ASYNC_CHANNEL	
#define ADC_USER_ASYNC_2_CHANNEL		ADC2_CHANNEL_4	  // User Async (Dalay adv)

#define GP_ETR
#define GP_ETR_PORT GPIOA
#define GP_ETR_PIN	BIT2
#define GP_ETR_EXTI_PORT EXTI_PORT_GPIOA
#define GP_ETR_EXTI_PORT_IRQHandler EXTI_PORTA_IRQHandler

#undef H1_PORT
#define H1_PORT GPIOD
#undef H2_PORT
#define H2_PORT GPIOD
#undef H3_PORT
#ifndef TIM2_CH3_REMAP
#define H3_PORT GPIOA
#else
#define H3_PORT GPIOD
#endif

#undef ADC_NEUTRAL_POINT_CHANNEL
#define ADC_NEUTRAL_POINT_CHANNEL	ADC_BUS_CHANNEL	  // Neutral point channel

#define SIZE_ADC_BUFFER 7
#define ADC_CURRENT_INDEX		0
#define ADC_USER_SYNC_INDEX		1 
#define ADC_BUS_INDEX			2
#define ADC_TEMP_INDEX			3
#define ADC_NEUTRAL_POINT_INDEX	4
#define ADC_USER_ASYNC_INDEX	5
#define ADC_USER_ASYNC_2_INDEX	6

u16 ADC_Buffer[ SIZE_ADC_BUFFER ];

static volatile u8 bCSR_Tmp; // Temporary storage of ADC channel selection

u16 hNeutralPoint;

#define NUMBER_PHASE_STEPS 6

#if (defined(ETR_INPUT) && !defined(GP_ETR))
#define OCxCE_ENABLE BIT7
#else
#define OCxCE_ENABLE 0
#endif

//select PWM mode 1, OC1 preload enabled, OCxCE enabled.
#define CCMR_PWM (OCxCE_ENABLE|BIT6|BIT5|BIT3)

//select PWM mode 2, OC1 preload enabled, OCxCE enabled. Low Side
#define CCMR_PWM_LS (OCxCE_ENABLE|BIT6|BIT5|BIT3)

//select OCREF - Forced Inactive, OC1 preload enabled (LS Off, HS On) X_COMP mode
#define CCMR_LOWSIDE (BIT6|BIT3)

//select OCREF - Forced Active, OC1 preload enabled (LS ON, HS Off) X_COMP mode
#define CCMR_HIGHSIDE (BIT6|BIT4|BIT3)

#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_LOW)
#define PWM_POL_LS_A BIT3
#else
#define PWM_POL_LS_A 0
#endif
#if (PWM_U_HIGH_SIDE_POLARITY == ACTIVE_LOW)
#define PWM_POL_HS_A BIT1
#else
#define PWM_POL_HS_A 0
#endif
#define PWM_POL_A PWM_POL_LS_A|PWM_POL_HS_A

#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_LOW)
#define PWM_POL_LS_B BIT7
#else
#define PWM_POL_LS_B 0
#endif
#if (PWM_V_HIGH_SIDE_POLARITY == ACTIVE_LOW)
#define PWM_POL_HS_B BIT5
#else
#define PWM_POL_HS_B 0
#endif
#define PWM_POL_B PWM_POL_LS_B|PWM_POL_HS_B

#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_LOW)
#define PWM_POL_LS_C BIT3
#else
#define PWM_POL_LS_C 0
#endif
#if (PWM_W_HIGH_SIDE_POLARITY == ACTIVE_LOW)
#define PWM_POL_HS_C BIT1
#else
#define PWM_POL_HS_C 0
#endif
#define PWM_POL_C PWM_POL_LS_C|PWM_POL_HS_C

#define A_ON  (BIT0|PWM_POL_A) // PWM mode on High Side
#define A_ON_LS  (BIT2|PWM_POL_A) // PWM mode on Low Side
#ifdef LS_GPIO_CONTROL
#define A_COMP (PWM_POL_A)
#else
#define A_COMP (BIT2|BIT0|PWM_POL_A)
#endif

#define A_OFF PWM_POL_A

#define B_ON (BIT4|PWM_POL_B)	// PWM mode on High Side
#define B_ON_LS (BIT6|PWM_POL_B) // PWM mode on Low Side
#ifdef LS_GPIO_CONTROL
#define B_COMP (PWM_POL_B)
#else
#define B_COMP (BIT6|BIT4|PWM_POL_B)
#endif

#define B_OFF PWM_POL_B

//#define ENABLE_ADC_TRIGGERING_DEBUG 

#if defined(ENABLE_ADC_TRIGGERING_DEBUG)
#define TIM1_CH4_EN BIT4
#else
#define TIM1_CH4_EN 0x00
#endif

#define C_ON (BIT0|PWM_POL_C|TIM1_CH4_EN) // PWM mode on High Side
#define C_ON_LS (BIT2|PWM_POL_C|TIM1_CH4_EN) // PWM mode on Low Side
#ifdef LS_GPIO_CONTROL
#define C_COMP (PWM_POL_C|TIM1_CH4_EN)
#else
#define C_COMP (BIT2|BIT0|PWM_POL_C|TIM1_CH4_EN)
#endif

#define C_OFF (PWM_POL_C|TIM1_CH4_EN)

// BRK settings
#ifdef BKIN
#define DEV_BKIN TIM1_BREAK_ENABLE
#else
#define DEV_BKIN TIM1_BREAK_DISABLE
#endif

#if (BKIN_POLARITY == ACTIVE_HIGH)
#define DEV_BKIN_POLARITY            TIM1_BREAKPOLARITY_HIGH
#else
#define DEV_BKIN_POLARITY            TIM1_BREAKPOLARITY_LOW
#endif

#define BEMF_RISING  1
#define BEMF_FALLING 0

#define SAMPLING_POINT_DURING_TOFF_CNT (u16)(((u32)SAMPLING_POINT_DURING_TOFF * STM8_FREQ_MHZ)/1000) // cnt before ARR
#define SAMPLING_POINT_DURING_TON_CNT (u16)(((u32)SAMPLING_POINT_DURING_TON * STM8_FREQ_MHZ)/1000) // cnt after ARR

u16 hDutyCntTh;
u16 hCntDeadDtime;


#define  hArrPwmVal  ((u16)((STM8_FREQ_MHZ * (u32)1000000)/PWM_FREQUENCY))
#define  hMaxDutyCnt  (hArrPwmVal - (((u32)STM8_FREQ_MHZ * MINIMUM_OFF_TIME)/1000) )
//static const u16 hArrPwmVal = ((u16)((STM8_FREQ_MHZ * (u32)1000000)/PWM_FREQUENCY));
//static const u16 hMaxDutyCnt = hArrPwmVal - (((u32)STM8_FREQ_MHZ * MINIMUM_OFF_TIME)/1000);

typedef struct 
{                 
  u8 CCMR_1; //Phase A 
  u8 CCMR_2; //Phase B
  u8 CCMR_3; //Phase C
  u8 CCER_1;
  u8 CCER_2;
} Phase_Step_s;

// CW Steps
//A-Channel1, B-Channel2, C-Channel3
//A(Hi), B(Lo), C-looking, BEMF Falling
//A(Hi), C(Lo), B-looking, BEMF Rising
//B(Hi), C(Lo), A-looking, BEMF Falling
//B(Hi), A(Lo), C-looking, BEMF Rising
//C(Hi), A(Lo), B-looking, BEMF Falling
//C(Hi), B(Lo), C-looking, BEMF Rising
const Phase_Step_s PhaseSteps_CW[ NUMBER_PHASE_STEPS ] =
{
  {CCMR_PWM, CCMR_LOWSIDE, CCMR_PWM, (A_ON|B_COMP),  C_OFF}, //A-HI, B-Lo, C-Looking
  {CCMR_PWM, CCMR_PWM, CCMR_LOWSIDE, (A_ON|B_OFF), C_COMP},  //A-HI, C-Lo, B-Looking
  {CCMR_PWM, CCMR_PWM, CCMR_LOWSIDE, (A_OFF|B_ON), C_COMP},  //B-HI, C-Lo, A-Looking
  {CCMR_LOWSIDE, CCMR_PWM, CCMR_PWM, (A_COMP|B_ON),  C_OFF}, //B-HI, A-Lo, C-Looking
  {CCMR_LOWSIDE, CCMR_PWM, CCMR_PWM, (A_COMP|B_OFF), C_ON},  //C-Hi, A-Lo, B-Looking
  {CCMR_PWM, CCMR_LOWSIDE, CCMR_PWM, (A_OFF|B_COMP), C_ON}   //C-Hi, B-Lo, A-Looking
};

const Phase_Step_s Fast_Demag_Steps_CW[ NUMBER_PHASE_STEPS ] =
{
  {CCMR_HIGHSIDE, CCMR_PWM_LS, CCMR_PWM, (A_COMP|B_ON_LS),  C_OFF}, //A-HI, B-Lo, C-Looking
  {CCMR_PWM, CCMR_PWM, CCMR_LOWSIDE, (A_ON|B_OFF), C_COMP},  //A-HI, C-Lo, B-Looking
  {CCMR_PWM, CCMR_HIGHSIDE, CCMR_PWM_LS, (A_OFF|B_COMP), C_ON_LS},  //B-HI, C-Lo, A-Looking
  {CCMR_LOWSIDE, CCMR_PWM, CCMR_PWM, (A_COMP|B_ON),  C_OFF}, //B-HI, A-Lo, C-Looking
  {CCMR_PWM_LS, CCMR_PWM, CCMR_HIGHSIDE, (A_ON_LS|B_OFF), C_COMP},  //C-Hi, A-Lo, B-Looking
  {CCMR_PWM, CCMR_LOWSIDE, CCMR_PWM, (A_OFF|B_COMP), C_ON}   //C-Hi, B-Lo, A-Looking
};

#ifdef LS_GPIO_CONTROL

#define LS_A BIT0
#define LS_B BIT1
#define LS_C BIT2

#define LS_NOSW	BIT0
#define LS_A_B	BIT1
#define LS_A_C	BIT2
#define LS_B_A	BIT3
#define LS_B_C	BIT4
#define LS_C_A	BIT5
#define LS_C_B	BIT6

const u8 LS_Steps_CW[ NUMBER_PHASE_STEPS ] =
{
  LS_A | LS_B,
  LS_A | LS_C,
  LS_B | LS_C,
  LS_A | LS_B,
  LS_A | LS_C,
  LS_B | LS_C
};

const u8 LS_Steps_SW_CW[ NUMBER_PHASE_STEPS ] =
{
  LS_C_A,
  LS_B_C,
  LS_A_B,
  LS_C_A,
  LS_B_C,
  LS_A_B
};

const u8 LS_Steps_CCW[ NUMBER_PHASE_STEPS ] =
{
  LS_B|LS_A,
  LS_B|LS_C,
  LS_A|LS_C,
  LS_A|LS_B,
  LS_C|LS_B,
  LS_C|LS_A
};

const u8 LS_Steps_SW_CCW[ NUMBER_PHASE_STEPS ] =
{
  LS_C_B,
  LS_NOSW,
  LS_B_A,
  LS_NOSW,
  LS_A_C,
  LS_NOSW
};

u8* LS_Steps = (u8*)LS_Steps_CW;
u8* LS_Steps_SW = (u8*)LS_Steps_SW_CW;
#endif

Phase_Step_s* PhaseSteps = (Phase_Step_s*)PhaseSteps_CW;
Phase_Step_s* Fast_Demag_Steps = (Phase_Step_s*)Fast_Demag_Steps_CW;

// CCW Steps
//A-Channel1, B-Channel2, C-Channel3
//A(Hi), B(Lo), C-looking, BEMF Rising
//C(Hi), B(Lo), A-looking, BEMF Falling
//C(Hi), A(Lo), B-looking, BEMF Rising
//B(Hi), A(Lo), C-looking, BEMF Falling
//B(Hi), C(Lo), A-looking, BEMF Rising
//A(Hi), C(Lo), B-looking, BEMF Falling
const Phase_Step_s PhaseSteps_CCW[ NUMBER_PHASE_STEPS ] =
{
  {CCMR_PWM, CCMR_LOWSIDE, CCMR_PWM, (A_ON|B_COMP),  C_OFF}, //A-Hi, B-Lo, C-Looking
  {CCMR_PWM, CCMR_LOWSIDE, CCMR_PWM, (A_OFF|B_COMP), C_ON},  //C-Hi, B-Lo, A-Looking
  {CCMR_LOWSIDE, CCMR_PWM, CCMR_PWM, (A_COMP|B_OFF), C_ON},  //C-Hi, A-Lo, B-Looking
  {CCMR_LOWSIDE, CCMR_PWM, CCMR_PWM, (A_COMP|B_ON),  C_OFF}, //B-Hi, A-Lo, C-Looking
  {CCMR_PWM, CCMR_PWM, CCMR_LOWSIDE, (A_OFF|B_ON), C_COMP},  //B-Hi, C-Lo, A-Looking
  {CCMR_PWM, CCMR_PWM, CCMR_LOWSIDE, (A_ON|B_OFF), C_COMP}   //A-Hi, C-Lo, B-Looking
};

const Phase_Step_s Fast_Demag_Steps_CCW[ NUMBER_PHASE_STEPS ] =
{
  {CCMR_HIGHSIDE, CCMR_PWM_LS, CCMR_PWM, (A_COMP|B_ON_LS),  C_OFF},	//A-HI, B-Lo, C-Looking
  {CCMR_PWM, CCMR_LOWSIDE, CCMR_PWM, (A_OFF|B_COMP), C_ON},			//C-HI, B-Lo, A-Looking
  {CCMR_PWM_LS, CCMR_PWM, CCMR_HIGHSIDE, (A_ON_LS|B_OFF), C_COMP},	//C-HI, A-Lo, B-Looking
  {CCMR_LOWSIDE, CCMR_PWM, CCMR_PWM, (A_COMP|B_ON),  C_OFF}, 			//B-HI, A-Lo, C-Looking
  {CCMR_PWM, CCMR_HIGHSIDE, CCMR_PWM_LS, (A_OFF|B_COMP), C_ON_LS},	//B-Hi, C-Lo, A-Looking
  {CCMR_PWM, CCMR_PWM, CCMR_LOWSIDE, (A_ON|B_OFF), C_COMP} 			//A-Hi, C-Lo, B-Looking
};

typedef struct
{
  u8 BEMF_Level;
  u8 ADC_Channel;
} BEMF_Step_s;

// CW Steps
const BEMF_Step_s BEMFSteps_CW[ NUMBER_PHASE_STEPS ] =
{
  {BEMF_FALLING, PHASE_C_BEMF_ADC_CHAN},
  {BEMF_RISING,  PHASE_B_BEMF_ADC_CHAN},
  {BEMF_FALLING, PHASE_A_BEMF_ADC_CHAN},
  {BEMF_RISING,  PHASE_C_BEMF_ADC_CHAN},
  {BEMF_FALLING, PHASE_B_BEMF_ADC_CHAN},
  {BEMF_RISING,  PHASE_A_BEMF_ADC_CHAN}
};

BEMF_Step_s* BEMFSteps = (BEMF_Step_s*)BEMFSteps_CW;

// CCW Steps
const BEMF_Step_s BEMFSteps_CCW[ NUMBER_PHASE_STEPS ] =
{
  {BEMF_RISING, PHASE_C_BEMF_ADC_CHAN},
  {BEMF_FALLING,  PHASE_A_BEMF_ADC_CHAN},
  {BEMF_RISING, PHASE_B_BEMF_ADC_CHAN},
  {BEMF_FALLING,  PHASE_C_BEMF_ADC_CHAN},
  {BEMF_RISING, PHASE_A_BEMF_ADC_CHAN},
  {BEMF_FALLING,  PHASE_B_BEMF_ADC_CHAN}
};

u8 Current_Step;
u8 Current_BEMF;
u8 Current_BEMF_Channel;

u8 MTC_Status;
#define MTC_STEP_MODE         BIT0
#define MTC_STARTUP_FAILED    BIT1
#define MTC_OVER_CURRENT_FAIL BIT2
#define MTC_LAST_FORCED_STEP  BIT3
#define MTC_MOTOR_STALLED     BIT4

u8 Phase_State;
#define PHASE_COMM  0x00
#define PHASE_DEMAG 0x01
#define PHASE_ZERO  0x02

u16 Commutation_Time;
u16 Demag_Time;
u16 Zero_Cross_Time;
u16 Previous_Zero_Cross_Time;
u16 Average_Zero_Cross_Time;
u16 LastSwitchedCom;

u16 CurrentLimitCnt;

u8 Ramp_Step;

u8 Zero_Cross_Count;
u8 Last_Zero_Cross_Count;
#define MINIMUM_CONSECTIVE_ZERO_CROSS 12

u8 BEMF_Sample_Debounce;

u8 Z_Detection_Type;
#define Z_DETECT_PWM_OFF 0
#define Z_DETECT_PWM_ON  1

const u16 RAMP_TABLE[ STEP_RAMP_SIZE ] = 
{
  RAMP_VALUE0,
  RAMP_VALUE1,
  RAMP_VALUE2,
  RAMP_VALUE3,
  RAMP_VALUE4,
  RAMP_VALUE5,
  RAMP_VALUE6,
  RAMP_VALUE7,
  RAMP_VALUE8,
  RAMP_VALUE9,
  RAMP_VALUE10,
  RAMP_VALUE11,
  RAMP_VALUE12,
  RAMP_VALUE13,
  RAMP_VALUE14,
  RAMP_VALUE15,
  RAMP_VALUE16,
  RAMP_VALUE17,
  RAMP_VALUE18,
  RAMP_VALUE19,
  RAMP_VALUE20,
  RAMP_VALUE21,
  RAMP_VALUE22,
  RAMP_VALUE23,
  RAMP_VALUE24,
  RAMP_VALUE25,
  RAMP_VALUE26,
  RAMP_VALUE27,
  RAMP_VALUE28,
  RAMP_VALUE29,
  RAMP_VALUE30,
  RAMP_VALUE31,
  RAMP_VALUE32,
  RAMP_VALUE33,
  RAMP_VALUE34,
  RAMP_VALUE35,
  RAMP_VALUE36,
  RAMP_VALUE37,
  RAMP_VALUE38,
  RAMP_VALUE39,
  RAMP_VALUE40,
  RAMP_VALUE41,
  RAMP_VALUE42,
  RAMP_VALUE43,
  RAMP_VALUE44,
  RAMP_VALUE45,
  RAMP_VALUE46,
  RAMP_VALUE47,
  RAMP_VALUE48,
  RAMP_VALUE49,
  RAMP_VALUE50,
  RAMP_VALUE51,
  RAMP_VALUE52,
  RAMP_VALUE53,
  RAMP_VALUE54,
  RAMP_VALUE55,
  RAMP_VALUE56,
  RAMP_VALUE57,
  RAMP_VALUE58,
  RAMP_VALUE59,
  RAMP_VALUE60,
  RAMP_VALUE61,
  RAMP_VALUE62,
  RAMP_VALUE63
};

u8 Align_State;
#define ALIGN_IDLE 0x00
#define ALIGN_RAMP 0x01
#define ALIGN_DONE 0x02

#define ToCMPxH(CMP,Value)         ( CMP = (u8)((Value >> 8 ) & 0xFF))
#define ToCMPxL(CMP,Value)         ( CMP = (u8)(Value & 0xFF) )

u8 Align_Target;
u8 Align_Index;

#define CAP_STEP_NUM 6 // Do not change this value
u16 cap_val = 0;
u8 cap_index = 0;
u8 first_cap = 0;

u8 BEMF_Falling_Factor;
u8 BEMF_Rising_Factor;

#define BEMF_RISE_DELAY_FACTOR 128 // Strtup values
#define BEMF_FALL_DELAY_FACTOR 128

// BEMF voltage treshold
#define BEMF_RISING_THRESHOLD  (u16)((BEMF_RISING_THRESHOLD_V * 1024)/(EXPECTED_MCU_VOLTAGE))
#define BEMF_FALLING_THRESHOLD (u16)((BEMF_FALLING_THRESHOLD_V * 1024)/(EXPECTED_MCU_VOLTAGE))

u16 Zero_Sample_Count;
u16 Motor_Frequency;

u8 Motor_Stall_Count;
#define MOTOR_STALL_THRESHOLD 6

#ifdef HALL
u8 bHallStartStep;

const u8 bHallSteps_CW[8] = {
  HALL_SENSOR_STEPS_CW_000,
  HALL_SENSOR_STEPS_CW_001,
  HALL_SENSOR_STEPS_CW_010,
  HALL_SENSOR_STEPS_CW_011,
  HALL_SENSOR_STEPS_CW_100,
  HALL_SENSOR_STEPS_CW_101,
  HALL_SENSOR_STEPS_CW_110,
  HALL_SENSOR_STEPS_CW_111
};

const u8 bHallSteps_CCW[8] = {
  HALL_SENSOR_STEPS_CCW_000,
  HALL_SENSOR_STEPS_CCW_001,
  HALL_SENSOR_STEPS_CCW_010,
  HALL_SENSOR_STEPS_CCW_011,
  HALL_SENSOR_STEPS_CCW_100,
  HALL_SENSOR_STEPS_CCW_101,
  HALL_SENSOR_STEPS_CCW_110,
  HALL_SENSOR_STEPS_CCW_111
};

u8* bHallSteps = (u8*)bHallSteps_CW;
#endif

#define MAX_BUS_VOLTAGE16 (u16)((MAX_BUS_VOLTAGE*(u32)1024)/BUSV_CONVERSION)
#define MIN_BUS_VOLTAGE16 (u16)((MIN_BUS_VOLTAGE*(u32)1024)/BUSV_CONVERSION)
#define BRAKE_HYSTERESIS  (u16)((MAX_BUS_VOLTAGE16/16)*15)

u8 bComHanderEnable = 0;
u16 hTim3Cnt = 0,hTim3Th = 0;

static pu16 pcounter_reg;
static pu16 pDutyCycleCounts_reg;

u16 tmp_u16;
u8 tmp_u8;
u8 tmp_TIM1_CCMR1;
u8 tmp_TIM1_CCMR2;
u8 tmp_TIM1_CCMR3;
u8 tmp_TIM1_CCER1;
u8 tmp_TIM1_CCER2;

void dev_driveInit(pvdev_device_t pdevice)
{
  u16 temp=0;
  
  PBLDC_Struct_t pBLDC_Struct = Get_BLDC_Struct();
  g_pBLDC_Struct = pBLDC_Struct;
  g_pMotorVar = pBLDC_Struct->pBLDC_Var;
  g_pDevice = pdevice;
  
#if (BEMF_SAMPLING_METHOD == BEMF_SAMPLING_MIXED)
  hDutyCntTh = (u16)(((u32)hArrPwmVal*DUTY_CYCLE_TH_TON)/100);
#endif
  
  hCntDeadDtime = (u16)(((u32)g_pBLDC_Struct->pBLDC_Const->hDeadTime * STM8_FREQ_MHZ )/1000);
  
  // Current Limitation
  CurrentLimitCnt = (u16)(MILLIAMP_TOCNT (g_pBLDC_Struct->pBLDC_Const->hCurrent_Limitation));
  
#ifdef SENSORLESS
  pcounter_reg = &(pdevice->regs.r16[VDEV_REG16_BEMF_COUNTS]);
#endif
#ifdef HALL
  pcounter_reg = &(pdevice->regs.r16[VDEV_REG16_HALL_COUNTS]);
#endif
  
  pDutyCycleCounts_reg = &(pdevice->regs.r16[VDEV_REG16_BLDC_DUTY_CYCLE_COUNTS]);
  
   //////bldc parameters initial///////////////
  EEPROM_INIT();
  temp = _Merge_16Bit(FLASH_ReadByte(BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address1),FLASH_ReadByte(BLDC_Motor_WORK_ROTOR_TARGET_SPEED_address2));
  if(temp !=0)
    g_pMotorVar->hTarget_rotor_speed = temp;
  
  temp = FLASH_ReadByte( BLDC_MOTOR_WORK_PWM_DUTY_ADDRESS);
  if(temp != 0)
    BLDC_Set_Duty_cycle((u8)temp);//g_pMotorVar->hDuty_cycle = (u8)temp;
  
  FLASH_Lock(FLASH_MEMTYPE_DATA);
  
  Init_TIM1();
  Init_TIM2();
//  Init_ADC();  
#ifdef DEBUG_PINS
  DebugPinsOff();
#endif
  
#ifdef LS_GPIO_CONTROL
  
  //Init LS_GPIO_PORT
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#else
  LS_A_PORT->ODR |= LS_A_PIN;
#endif
  LS_A_PORT->DDR |= LS_A_PIN;
  LS_A_PORT->CR1 |= LS_A_PIN;
  LS_A_PORT->CR2 |= LS_A_PIN;
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#else
  LS_B_PORT->ODR |= LS_B_PIN;
#endif
  LS_B_PORT->DDR |= LS_B_PIN;
  LS_B_PORT->CR1 |= LS_B_PIN;
  LS_B_PORT->CR2 |= LS_B_PIN;
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#else
  LS_C_PORT->ODR |= LS_C_PIN;
#endif
  LS_C_PORT->DDR |= LS_C_PIN;
  LS_C_PORT->CR1 |= LS_C_PIN;
  LS_C_PORT->CR2 |= LS_C_PIN;
  
#endif
  
//  vtimer_SetTimer(ADC_SAMPLE_TIMER,ADC_SAMPLE_TIMEOUT,(void*)&Application_ADC_Manager);
}

void dev_driveIdle(void)
{
}

/* Is used to avoid Ton-BEMF sampling during the ramp-up 
sequence when dynamic BEMF method is selected*/
static u8 StartUpExecution = 0; 

MC_FuncRetVal_t dev_driveStartUpInit(void)
{
  // Set the correct Step table
  if (g_pBLDC_Struct->pBLDC_Var->hTarget_rotor_speed > 0)
//  if (g_pBLDC_Struct->pBLDC_Var->bToggleMode==0)
  {
    if(cylindermotorstatus.cylindermotordirect == 0)
    {
    BEMFSteps = (BEMF_Step_s*)BEMFSteps_CW;
    PhaseSteps = (Phase_Step_s*)PhaseSteps_CW;
    Fast_Demag_Steps = (Phase_Step_s*)Fast_Demag_Steps_CW;
    
#ifdef LS_GPIO_CONTROL
    LS_Steps = (u8*)LS_Steps_CW;
    LS_Steps_SW = (u8*)LS_Steps_SW_CW;
#endif
    
#ifdef HALL
    bHallSteps = (u8*)bHallSteps_CW;
#endif
    
    Current_Step=1;
    }
    else if(cylindermotorstatus.cylindermotordirect == 1)
    {
    BEMFSteps = (BEMF_Step_s*)BEMFSteps_CCW;
    PhaseSteps = (Phase_Step_s*)PhaseSteps_CCW;
    Fast_Demag_Steps = (Phase_Step_s*)Fast_Demag_Steps_CCW;
    
#ifdef LS_GPIO_CONTROL
    LS_Steps = (u8*)LS_Steps_CCW;
    LS_Steps_SW = (u8*)LS_Steps_SW_CCW;
#endif
    
#ifdef HALL
    bHallSteps = (u8*)bHallSteps_CCW;
#endif
    
    Current_Step=0;
    }
    
//    Current_Step=1;
  }
  else
  {
    BEMFSteps = (BEMF_Step_s*)BEMFSteps_CCW;
    PhaseSteps = (Phase_Step_s*)PhaseSteps_CCW;
    Fast_Demag_Steps = (Phase_Step_s*)Fast_Demag_Steps_CCW;
    
#ifdef LS_GPIO_CONTROL
    LS_Steps = (u8*)LS_Steps_CCW;
    LS_Steps_SW = (u8*)LS_Steps_SW_CCW;
#endif
    
#ifdef HALL
    bHallSteps = (u8*)bHallSteps_CCW;
#endif
    
    Current_Step=0;
  }
  
  StartUpStatus = STARTUP_BOOTSTRAP;
  vtimer_SetTimer(MTC_ALIGN_RAMP_TIMER,5,0);
  
  StartUpExecution = 1;
  
  return FUNCTION_ENDED;
}

static u8 bEnableSoftwareCounter = 0;

MC_FuncRetVal_t dev_driveStartUp(void)
{
  u8 step=0;
  
  switch (StartUpStatus)
  {
  case STARTUP_IDLE:
    break;
    
  case STARTUP_BOOTSTRAP:
    if (BootStrap() == TRUE)
    {
      Align_State = ALIGN_IDLE;
#ifdef DEBUG_PINS
      AUTO_SWITCH_PORT &= (u8)(~AUTO_SWITCH_PIN);
#endif
#ifdef SENSORLESS
      StartUpStatus = STARTUP_ALIGN;
      
      // Software counter is disabled during alignment
      bEnableSoftwareCounter = 0;
      
      // Enable update interrupt
      TIM1->IER |= BIT0;
      
#endif
#ifdef HALL
      
      bldc_soft_start();
      
      TIM2_InitCapturePolarity();
      
      //preload next step
      Current_Step = bHallStartStep;
      TIM1->CCMR1 = PhaseSteps[Current_Step].CCMR_1;
      TIM1->CCMR2 = PhaseSteps[Current_Step].CCMR_2;
      TIM1->CCMR3 = PhaseSteps[Current_Step].CCMR_3;
      TIM1->CCER1 = PhaseSteps[Current_Step].CCER_1;
      TIM1->CCER2 = PhaseSteps[Current_Step].CCER_2;
      
      Previous_Zero_Cross_Time = 0;
      Z_Detection_Type = Z_DETECT_PWM_ON;
      
      StartUpStatus = STARTUP_START;
      
      // Enable update interrupt
      TIM1->IER |= BIT0;
#endif
      first_cap = 0;
    }
    break;
    
  case STARTUP_ALIGN:
    if( AlignRotor() )
    {
      StartUpStatus = STARTUP_START;
    }
    break;
    
  case STARTUP_START:
#ifdef SENSORLESS
    MTC_Status |= MTC_STEP_MODE;
#endif
#ifdef HALL
    MTC_Status |= MTC_STEP_MODE;
#endif
    MTC_Status &= (u8)(~(MTC_STARTUP_FAILED|MTC_OVER_CURRENT_FAIL|MTC_LAST_FORCED_STEP|MTC_MOTOR_STALLED));
    
    // Reset software threshold
    hTim3Th = 0;
    
    StartMotor();
    
    // Reset software counter
    hTim3Cnt = 0;
    
    // Enable software counter
    bEnableSoftwareCounter = 1;
    
#ifdef SENSORLESS
    StartUpStatus = STARTUP_RAMPING;
#endif
#ifdef HALL
    vtimer_SetTimer(DEV_DUTY_UPDATE_TIMER,SPEED_PID_SAMPLING_TIME,(void *)&dev_BLDC_driveUpdate);
    StartUpStatus = STARTUP_IDLE;
    return FUNCTION_ENDED;
#endif
    break;
    
  case STARTUP_RAMPING:
    if( (MTC_Status & MTC_STEP_MODE) == 0 )
    {
      vtimer_SetTimer(DEV_DUTY_UPDATE_TIMER,SPEED_PID_SAMPLING_TIME,(void *)&dev_BLDC_driveUpdate);
      StartUpStatus = STARTUP_IDLE;
      return FUNCTION_ENDED;
    }
    else
    {
      if( MTC_Status & (MTC_STARTUP_FAILED|MTC_OVER_CURRENT_FAIL) )
      {
        StartUpStatus = STARTUP_IDLE;
        dev_driveStop();
        return FUNCTION_ERROR;
      }
    }
    break;
  }
  return FUNCTION_RUNNING;
}

MC_FuncRetVal_t dev_driveRun(void)
{
  StartUpExecution = 0;
  
  if( ((MTC_Status & MTC_OVER_CURRENT_FAIL) == MTC_OVER_CURRENT_FAIL) ||
     ((MTC_Status & MTC_MOTOR_STALLED) == MTC_MOTOR_STALLED) )
  {
    dev_driveStop();
    return FUNCTION_ERROR;
  }
  return FUNCTION_RUNNING;
}

MC_FuncRetVal_t dev_driveStop(void)
{
  vtimer_KillTimer(DEV_DUTY_UPDATE_TIMER);
  StopMotor();
#ifdef DEBUG_PINS
  DebugPinsOff();
#endif
  *pcounter_reg = 0;
  
  // Check toggle mode
  if (g_pBLDC_Struct->pBLDC_Var->bToggleMode)
  {
    g_pBLDC_Struct->pBLDC_Var->hTarget_rotor_speed = -g_pBLDC_Struct->pBLDC_Var->hTarget_rotor_speed;
  }
  return FUNCTION_ENDED;
}

MC_FuncRetVal_t dev_driveWait(void)
{
#ifdef ACTIVE_BRAKE
  static u8 st_tim = 0;
  if (st_tim == 0)
  {
    vtimer_SetTimer(MTC_ALIGN_RAMP_TIMER,BRAKE_DURATION,0);
    st_tim = 1;
    BrakeMotor();
  }
  if (vtimer_TimerElapsed(MTC_ALIGN_RAMP_TIMER))
  {
    st_tim = 0;
    StopMotor();
    return FUNCTION_ENDED;
  }
  else
  {
    return FUNCTION_RUNNING;
  }
#else
  return FUNCTION_ENDED;
#endif
}

void dev_BLDC_driveUpdate(void)
{
  vtimer_SetTimer(DEV_DUTY_UPDATE_TIMER,SPEED_PID_SAMPLING_TIME,(void *)&dev_BLDC_driveUpdate);
#if (CURRENT_CONTROL_MODE == VOLTAGE_MODE)
#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
  g_pBLDC_Struct->pBLDC_Var->hDuty_cycle = Set_Duty(*pDutyCycleCounts_reg);
#else
  Set_Duty(*pDutyCycleCounts_reg);
#endif
#endif
#if (CURRENT_CONTROL_MODE == CURRENT_MODE)
#if (SPEED_CONTROL_MODE == CLOSED_LOOP)
  g_pBLDC_Struct->pBLDC_Var->hCurrent_reference = Set_Current(*pDutyCycleCounts_reg);
#else
  Set_Current(*pDutyCycleCounts_reg);
#endif
#endif
}

static u8 bCurrentLimitationActive = 0;
static u8 bTIM1_CCER1_preload;
static u8 bTIM1_CCER2_preload;
static u8 bTIM1_CCMR1_preload;
static u8 bTIM1_CCMR2_preload;
static u8 bTIM1_CCMR3_preload;
static u8 bTIM1_CCER1_actual;
static u8 bTIM1_CCER2_actual;
static u8 bTIM1_CCMR1_actual;
static u8 bTIM1_CCMR2_actual;
static u8 bTIM1_CCMR3_actual;

//@near @interrupt void GP_ETR_EXTI_PORT_IRQHandler (void)
//{
//	// Check if ETR is on
//	if (!(GP_ETR_PORT->IDR & GP_ETR_PIN))
//		return;
//
//	// Disable PWM output
////	TIM1->BKR &= (u8)(~BIT7);
//	// Restore actual + PWM off
//	TIM1->CCER1 =	bTIM1_CCER1_actual;
//	TIM1->CCER2 =	bTIM1_CCER2_actual;
//	TIM1->CCMR1 =	bTIM1_CCMR1_actual;
//	TIM1->CCMR2 =	bTIM1_CCMR2_actual;
//	TIM1->CCMR3 =	bTIM1_CCMR3_actual;
//	TIM1->CCER1 &= (u8)(~TIM1_CCER1_CC1E);
//	TIM1->CCER1 &= (u8)(~TIM1_CCER1_CC2E);
//	TIM1->CCER2 &= (u8)(~TIM1_CCER2_CC3E);
//	TIM1->EGR |= TIM1_EGR_COMG;
//	// Restore actual
//	TIM1->CCER1 =	bTIM1_CCER1_actual;
//	TIM1->CCER2 =	bTIM1_CCER2_actual;
//	
//	// Test
//	PWM_ON_SW_PORT |= (u8)(PWM_ON_SW_PIN);
//
//	bCurrentLimitationActive = 1;
//	return;
//}

//@near @interrupt @svlreg void TIM1_UPD_OVF_TRG_BRK_IRQHandler (void)
#pragma vector=0xD
__interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void)
{
#ifdef DEBUG_PINS
  static u16 bkin_blink_cnt = 0;
#endif
  if( (TIM1->SR1 & BIT7) == BIT7 )
  {
    TIM1->SR1 &= (u8)(~BIT7);
    
#ifdef LS_GPIO_CONTROL
    // Disable low side
    
    // Deactivate A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#else
    LS_A_PORT->ODR |= LS_A_PIN;
#endif
    // Deactivate B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#else
    LS_B_PORT->ODR |= LS_B_PIN;
#endif
    // Deactivate C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#else
    LS_C_PORT->ODR |= LS_C_PIN;
#endif
#endif
    
    StartUpStatus = STARTUP_IDLE;
    
    g_pDevice->regs.r16[VDEV_REG16_HW_ERROR_OCCURRED] |= OVER_CURRENT;
#ifdef DEBUG_PINS
    DebugPinsOff();
    
    // If the BKIN is still active turn on the led to signal this condition
    if (((TIM1->SR1 & BIT7) == BIT7 ) && (bkin_blink_cnt <= 32768))
    {
      Z_DEBUG_PORT |= Z_DEBUG_PIN;
      C_D_DEBUG_PORT |= C_D_DEBUG_PIN;
      AUTO_SWITCH_PORT |= AUTO_SWITCH_PIN;
      PWM_ON_SW_PORT |= PWM_ON_SW_PIN;
    }
    bkin_blink_cnt++;
#endif
    
    
  } 
  else
  {
    // Update management only if BRK is not occurred
    if ((TIM1->SR1 & BIT0) == BIT0)
    {
      if (bCurrentLimitationActive == 1)
      {
        if ((GP_ETR_PORT->IDR & GP_ETR_PIN) == 0)
        {
          // Re-Enable PWM output
          //					TIM1->BKR |= BIT7;
          TIM1->EGR |= TIM1_EGR_COMG;
          // Restore preload
          TIM1->CCER1 =	bTIM1_CCER1_preload;
          TIM1->CCER2 =	bTIM1_CCER2_preload;
          TIM1->CCMR1 =	bTIM1_CCMR1_preload;
          TIM1->CCMR2 =	bTIM1_CCMR2_preload;
          TIM1->CCMR3 =	bTIM1_CCMR3_preload;
          
          // Test
          PWM_ON_SW_PORT &= (u8)(~PWM_ON_SW_PIN);
          
          bCurrentLimitationActive = 0;
        }
      }
      
      if (bEnableSoftwareCounter == 1)
      {
        hTim3Cnt++;
        
        // Check for match
        if (hTim3Cnt >= hTim3Th)
        {
          ComHandler();
        }
      }
      
      // Clear Flag
      TIM1->SR1 &= (u8)(~BIT0);
    }
  }
}

void Application_ADC_Manager( void )
{
  vtimer_SetTimer(ADC_SAMPLE_TIMER,ADC_SAMPLE_TIMEOUT,(void*)&Application_ADC_Manager);
  
  GetCurrent();
  GetSyncUserAdc();
  GetBusVoltage();
  GetTemperature();
  GetNeutralPoint();
  GetAsyncUserAdc();
  GetAsyncUserAdc2();
}

void GetCurrent(void)
{
  u16 hVal;
  disableInterrupts();
  hVal = ADC_Buffer[ADC_CURRENT_INDEX];
  enableInterrupts();
  
  BLDC_Set_Current_measured((u16)(ADC_TOMILLIAMP(hVal)/100));
}

void GetSyncUserAdc(void)
{
  u16 hSyncUserAdc;
  
  disableInterrupts();
  hSyncUserAdc = ADC_Buffer[ADC_USER_SYNC_INDEX];
  enableInterrupts();
}

#ifdef BUS_VOLTAGE_MEASUREMENT
void GetBusVoltage( void )
{
  u16 data;
  
  disableInterrupts();
  data = ADC_Buffer[ ADC_BUS_INDEX ];
  enableInterrupts();
  
  if (data > MAX_BUS_VOLTAGE16)
  {
    
#ifdef DISSIPATIVE_BRAKE
#if (DISSIPATIVE_BRAKE_POL == DISSIPATIVE_BRAKE_ACTIVE_HIGH)
    DISSIPATIVE_BRAKE_PORT->ODR |= DISSIPATIVE_BRAKE_BIT;
#else
    DISSIPATIVE_BRAKE_PORT->ODR &= (u8)(~DISSIPATIVE_BRAKE_BIT);
#endif
#else			
    g_pDevice->regs.r16[VDEV_REG16_HW_ERROR_OCCURRED] |= BUS_OVERVOLTAGE;
    g_pDevice->regs.r16[VDEV_REG16_HW_ERROR_ACTUAL] |= BUS_OVERVOLTAGE;
#endif
  }
  if (data < BRAKE_HYSTERESIS)
  {
#ifdef DISSIPATIVE_BRAKE
#if (DISSIPATIVE_BRAKE_POL == DISSIPATIVE_BRAKE_ACTIVE_HIGH)
    DISSIPATIVE_BRAKE_PORT->ODR &= (u8)(~DISSIPATIVE_BRAKE_BIT);
#else
    DISSIPATIVE_BRAKE_PORT->ODR |= DISSIPATIVE_BRAKE_BIT;
#endif
#else
    g_pDevice->regs.r16[VDEV_REG16_HW_ERROR_ACTUAL] &= (u8)(~BUS_OVERVOLTAGE);
#endif
  }
  if (data < MIN_BUS_VOLTAGE16)
  {
    g_pDevice->regs.r16[VDEV_REG16_HW_ERROR_OCCURRED] |= BUS_UNDERVOLTAGE;
    g_pDevice->regs.r16[VDEV_REG16_HW_ERROR_ACTUAL] |= BUS_UNDERVOLTAGE;      
  }
  else
  {
    g_pDevice->regs.r16[VDEV_REG16_HW_ERROR_ACTUAL] &= (u8)(~BUS_UNDERVOLTAGE);
  }    
  
  g_pDevice->regs.r16[VDEV_REG16_BOARD_BUS_VOLTAGE] = data;
  
  BLDC_Set_Bus_Voltage((u16)((data * BUSV_CONVERSION)/1024));
}
#endif

#ifndef BUS_VOLTAGE_MEASUREMENT
void GetBusVoltage( void )
{
  BLDC_Set_Bus_Voltage((u16)(BUS_VOLTAGE_VALUE));
}
#endif

#ifdef HEAT_SINK_TEMPERATURE_MEASUREMENT
void GetTemperature(void)
{
  u16 data;
  u8 temp;
  
  disableInterrupts();
  data = ADC_Buffer[ ADC_TEMP_INDEX ];
  enableInterrupts();
  g_pDevice->regs.r8[VDEV_REG8_HEATSINK_TEMPERATURE] = (u8)(data >> 2);
  
  if (data > NTC_THRESHOLD)
  {
    g_pDevice->regs.r16[VDEV_REG16_HW_ERROR_OCCURRED] |= HEATSINK_TEMPERATURE;
    g_pDevice->regs.r16[VDEV_REG16_HW_ERROR_ACTUAL] |= HEATSINK_TEMPERATURE;      
  }
  if (data < NTC_HYSTERIS)
  {
    g_pDevice->regs.r16[VDEV_REG16_HW_ERROR_ACTUAL] &= (u8)(~HEATSINK_TEMPERATURE);
  }
  
  temp = (u8)(TEMP_SENS_T0 + (((s16)data - TEMP_SENS_BETA)/TEMP_SENS_ALPHA));
  BLDC_Set_Heatsink_Temperature((u8)(temp));
}
#endif

#ifndef HEAT_SINK_TEMPERATURE_MEASUREMENT
void GetTemperature(void)
{
  BLDC_Set_Heatsink_Temperature((u8)(HEAT_SINK_TEMPERATURE_VALUE));
}
#endif

void GetNeutralPoint(void)
{
  u16 data;
  disableInterrupts();
  data = ADC_Buffer[ ADC_NEUTRAL_POINT_INDEX ];
  enableInterrupts();
  
  // Scaling factor 1.4/2 = 7/10
  hNeutralPoint = (data * 7) / 10;
}

void GetAsyncUserAdc(void)
{
  u16 hAsyncUserAdc;
  
  disableInterrupts();
  hAsyncUserAdc = ADC_Buffer[ADC_USER_ASYNC_INDEX];
  enableInterrupts();
  
#ifdef SET_TARGET_SPEED_BY_POTENTIOMETER
  // Set Target speed
  BLDC_Set_Target_rotor_speed((u16)(((u32)MAX_SPEED_RPM * hAsyncUserAdc)/1023));
#else
  // Call User defined function with hAsyncUserAdc parameter
#endif
}

void GetAsyncUserAdc2(void)
{
  u16 hAsyncUserAdc;
  
  disableInterrupts();
  hAsyncUserAdc = ADC_Buffer[ADC_USER_ASYNC_2_INDEX];
  enableInterrupts();
  
#ifdef SET_RISEFALLDELAY_BY_POTENTIOMETER
  // Set rise/fall delay
  BLDC_Set_Falling_Delay((u8)(hAsyncUserAdc >> 3));
  BLDC_Set_Rising_Delay((u8)(hAsyncUserAdc >> 3));
#else
  // Call User defined function with hAsyncUserAdc parameter
#endif
}

void Init_ADC( void )
{
  u8 value;
  u16 ADC_TDR_tmp;
  
  ADC_Sync_State = ADC_USER_SYNC_INIT;
  ADC_State = ADC_SYNC;
  
  ADC2->CSR = 0; 
  
  //select 4MHz clock based on 16MHz fMaster (div4), single mode
  //       6MHz clock based on 24MHz fMaster (div4)
  ADC2->CR1 = BIT5;
  
  //select internal trigger TIM1 TRGO
  ADC2->CR2 = 0;
  
  //select phase input
  ADC2->CSR = PHASE_C_BEMF_ADC_CHAN;
  
  ADC_TDR_tmp = 0;
  ADC_TDR_tmp |= (u16)(1) << PHASE_A_BEMF_ADC_CHAN;
  ADC_TDR_tmp |= (u16)(1) << PHASE_B_BEMF_ADC_CHAN;
  ADC_TDR_tmp |= (u16)(1) << PHASE_C_BEMF_ADC_CHAN;
  
  ADC_TDR_tmp |= (u16)(1) << ADC_CURRENT_CHANNEL;
  ADC_TDR_tmp |= (u16)(1) << ADC_USER_SYNC_CHANNEL;
  
  ADC_TDR_tmp |= (u16)(1) << ADC_BUS_CHANNEL;
  ADC_TDR_tmp |= (u16)(1) << ADC_NEUTRAL_POINT_CHANNEL;
  ADC_TDR_tmp |= (u16)(1) << ADC_TEMP_CHANNEL;
  ADC_TDR_tmp |= (u16)(1) << ADC_USER_ASYNC_CHANNEL;
  
  ToCMPxH( ADC2->TDRH, ADC_TDR_tmp);
  ToCMPxL( ADC2->TDRL, ADC_TDR_tmp);
  
  //enable trigger
  ADC2->CR2 |= BIT6;
  
  //enable ADC
  ADC2->CR1 |= BIT0;
  //allow ADC to stabilize
  value=30;
  while(value--);                    
  //clear interrupt flag
  ADC2->CSR &= (u8)(~BIT7);
  ADC2->CSR |= BIT5;
}


void Enable_ADC_BEMF_Sampling( void )
{
  //Enable sampling of BEMF
  ADC_Sync_State = ADC_BEMF_INIT;
}

void Enable_ADC_Current_Sampling( void )
{
  //Enable sampling of the current
  ADC_Sync_State = ADC_CURRENT_INIT;
}

void Enable_ADC_User_Sync_Sampling( void )
{
  //allow application use of ADC
  ADC_Sync_State = ADC_USER_SYNC_INIT;
}

// This function capture the time into the Zero_Cross_Time vars
// express in number of PWM period occured from the last capture
void GetStepTime(void)
{
  u16 cur_time;
  
  cur_time = hTim3Cnt;
  // Reset counter
  hTim3Cnt = 0;
  
  Zero_Cross_Time = cur_time;
  Zero_Cross_Count++;
}

// This function manage the speed measurement using a buffer
void SpeedMeasurement(void)
{
  if (first_cap == 0)
  {
    first_cap = 1;
    cap_val = 0;
    cap_index = 0;
  }
  else
  {
    cap_val += Zero_Cross_Time;
    cap_index++;
    
    if (cap_index == CAP_STEP_NUM)
    {
      *pcounter_reg = cap_val;
      cap_val = 0;
      cap_index = 0;
    }
  }
}

#ifdef SENSORLESS	
//@near @interrupt @svlreg void ADC2_IRQHandler (void)
#pragma vector=0x18
__interrupt void ADC2_IRQHandler(void)
{
  if (ADC_State == ADC_SYNC)
  {
    // Syncronous sampling
    
    u16 data;
    u8 delay;
    u16 bemf_threshold;
    
    // Reset bit
    bComHanderEnable = 0;
    
    //clear interrupt flag
    ADC2->CSR &= (u8)(~BIT7);
    
    //left align - read DRH first
    data = ADC2->DRH;
    data <<= 2;
    data |= (ADC2->DRL & 0x03);   
    
    switch( ADC_Sync_State )
    {
    case ADC_BEMF_INIT:
      ADC2->CSR = (u8)((Current_BEMF_Channel|BIT5)); 
      BEMF_Sample_Debounce = 0;
      Zero_Sample_Count = 0;
      ADC_Sync_State = ADC_BEMF_SAMPLE;
      SetSamplingPoint_BEMF();
      break;
      
    case ADC_BEMF_SAMPLE: 
      //detect zero crossing 
      if( Current_BEMF == BEMF_FALLING )
      {
        if( Z_Detection_Type == Z_DETECT_PWM_OFF )
        {
          bemf_threshold = BEMF_FALLING_THRESHOLD;
        }
        else
        {
          bemf_threshold = hNeutralPoint;
        }
        
        if (Ramp_Step > FORCED_STATUP_STEPS)
        {
          if( data <  bemf_threshold  )
          {
            Zero_Sample_Count++;
            BEMF_Sample_Debounce++;
            if( BEMF_Sample_Debounce >= BEMF_SAMPLE_COUNT )
            {
              hTim3Th -= hTim3Cnt;
              GetStepTime();
              
#ifdef DEBUG_PINS
              Z_DEBUG_PORT ^= Z_DEBUG_PIN;
#endif
              
              SpeedMeasurement();
              
              bComHanderEnable = 1;
              
              BEMF_Sample_Debounce = 0;
            }
          }
          else
          {
            BEMF_Sample_Debounce = 0;
          }
        }
      }
      else
      {
        if( Z_Detection_Type == Z_DETECT_PWM_OFF )
        {
          bemf_threshold = BEMF_RISING_THRESHOLD;
        }
        else
        {
          bemf_threshold = hNeutralPoint;
        }
        
        if (Ramp_Step > FORCED_STATUP_STEPS)
        {
          if( data > bemf_threshold )
          {
            Zero_Sample_Count++;
            BEMF_Sample_Debounce++;
            if( BEMF_Sample_Debounce >= BEMF_SAMPLE_COUNT )
            {
              hTim3Th -= hTim3Cnt;
              GetStepTime();
              
#ifdef DEBUG_PINS
              Z_DEBUG_PORT ^= Z_DEBUG_PIN;
#endif
              
              SpeedMeasurement();
              
              bComHanderEnable = 1;
              
              BEMF_Sample_Debounce = 0;
            }
          }
          else
          {
            BEMF_Sample_Debounce = 0;
          }
        }
      }
      break;
      
    case ADC_CURRENT_INIT:
      ADC2->CSR = (ADC_CURRENT_CHANNEL|BIT5); 
      ADC_Sync_State = ADC_CURRENT_SAMPLE;
      SetSamplingPoint_Current();
      break;
      
    default:
    case ADC_USER_SYNC_INIT:
      ADC2->CSR = (ADC_USER_SYNC_CHANNEL|BIT5); 
      ADC_Sync_State = ADC_USER_SYNC_SAMPLE;
      SetSamplingPoint_User_Sync();
      break;
      
      
    case ADC_CURRENT_SAMPLE: 
      ADC_Buffer[ ADC_CURRENT_INDEX ] = data;
      break;
      
    case ADC_USER_SYNC_SAMPLE: 
      ADC_Buffer[ ADC_USER_SYNC_INDEX] = data;
      break;
    }
    
    // Store the current channel selected
    bCSR_Tmp = ADC2->CSR;
    
    // Set the Async sampling channel
    switch (ADC_Async_State)
    {
    default:
    case ADC_BUS_INIT:
      ADC2->CSR = (ADC_BUS_CHANNEL|BIT5); 
      ADC_Async_State = ADC_BUS_SAMPLE;
      break;
      
    case ADC_TEMP_INIT:
      ADC2->CSR = (ADC_TEMP_CHANNEL|BIT5); 
      ADC_Async_State = ADC_TEMP_SAMPLE;
      break;
      
    case ADC_NEUTRAL_POINT_INIT:
      ADC2->CSR = (ADC_NEUTRAL_POINT_CHANNEL|BIT5); 
      ADC_Async_State = ADC_NEUTRAL_POINT_SAMPLE;
      break;
      
    case ADC_USER_ASYNC_INIT:
      ADC2->CSR = (ADC_USER_ASYNC_CHANNEL|BIT5); 
      ADC_Async_State = ADC_USER_ASYNC_SAMPLE;
      break;
      
    case ADC_USER_ASYNC_INIT_2:
      ADC2->CSR = (ADC_USER_ASYNC_2_CHANNEL|BIT5); 
      ADC_Async_State = ADC_USER_ASYNC_SAMPLE_2;
      break;
    }
    
    // Start asyncronous sampling
#ifdef DEV_CUT_1
    // Disable ext. trigger
    ADC2->CR2 &= (u8)(~BIT6);
    //turn on ADC fix bug on cut1 device
    ADC2->CR1 |= BIT0;
    //Start ADC sample
    ADC2->CR1 |= BIT0;
#else
    // Disable ext. trigger
    ADC2->CR2 &= (u8)(~BIT6);
    //Start ADC sample
    ADC2->CR1 |= BIT0;
#endif
    
    ADC_State = ADC_ASYNC;
    
    if (bComHanderEnable == 1)
    {
      ComHandler();
    }
  }
  else
  {
    // Syncronous sampling
    u16 data;
    
    data = ADC2->DRH;
    data <<= 2;
    data |= (ADC2->DRL & 0x03);
    
    //clear interrupt flag
    ADC2->CSR &= (u8)(~BIT7);
    
    // Restore the sync ADC channel
    ADC2->CSR = bCSR_Tmp;
    
    
    // Configure syncronous sampling
#ifdef DEV_CUT_1
    // Enable ext. trigger
    ADC2->CR2 |= BIT6;
    //turn on ADC fix bug on cut1 device
    ADC2->CR1 |= BIT0;  
#else
    // Enable ext. trigger
    ADC2->CR2 |= BIT6;
#endif
    
    // Manage async sampling
    switch (ADC_Async_State)
    {
    default:
    case ADC_BUS_SAMPLE:
      ADC_Buffer[ ADC_BUS_INDEX ] = data;
      ADC_Async_State = ADC_TEMP_INIT;
      break;
      
    case ADC_TEMP_SAMPLE:
      ADC_Buffer[ ADC_TEMP_INDEX ] = data;
      ADC_Async_State = ADC_NEUTRAL_POINT_INIT;
      break;
      
    case ADC_NEUTRAL_POINT_SAMPLE:
      ADC_Buffer[ ADC_NEUTRAL_POINT_INDEX ] = data;
      ADC_Async_State = ADC_USER_ASYNC_INIT;
      break;
      
    case ADC_USER_ASYNC_SAMPLE:
      ADC_Buffer[ ADC_USER_ASYNC_INDEX ] = data;
      ADC_Async_State = ADC_USER_ASYNC_INIT_2;
      break;
      
    case ADC_USER_ASYNC_SAMPLE_2:
      ADC_Buffer[ ADC_USER_ASYNC_2_INDEX ] = data;
      ADC_Async_State = ADC_BUS_INIT;
      break;
    }
    
    ADC_State = ADC_SYNC;
    
  }
}
#endif

#ifdef HALL
//@near @interrupt @svlreg void ADC2_IRQHandler (void)
#pragma vector=0x18
__interrupt void ADC2_IRQHandler(void)
{
  u16 data;
  
  data = ADC2->DRH;
  data <<= 2;
  data |= (ADC2->DRL & 0x03);
  
  //clear interrupt flag
  ADC2->CSR &= (u8)(~BIT7);
  
  // Manage async sampling
  switch (ADC_Async_State)
  {
  default:
  case ADC_BUS_SAMPLE:
    ADC_Buffer[ ADC_BUS_INDEX ] = data;
    ADC_Async_State = ADC_TEMP_INIT;
    break;
    
  case ADC_TEMP_SAMPLE:
    ADC_Buffer[ ADC_TEMP_INDEX ] = data;
    ADC_Async_State = ADC_NEUTRAL_POINT_INIT;
    break;
    
  case ADC_NEUTRAL_POINT_SAMPLE:
    ADC_Buffer[ ADC_NEUTRAL_POINT_INDEX ] = data;
    ADC_Async_State = ADC_USER_ASYNC_INIT;
    break;
    
  case ADC_USER_ASYNC_SAMPLE:
    ADC_Buffer[ ADC_USER_ASYNC_INDEX ] = data;
    ADC_Async_State = ADC_USER_ASYNC_INIT_2;
    break;
    
  case ADC_USER_ASYNC_SAMPLE_2:
    ADC_Buffer[ ADC_USER_ASYNC_2_INDEX ] = data;
    ADC_Async_State = ADC_BUS_INIT;
    break;
  }
  
  // Set the Async sampling channel
  switch (ADC_Async_State)
  {
  default:
  case ADC_BUS_INIT:
    ADC2->CSR = (ADC_BUS_CHANNEL|BIT5); 
    ADC_Async_State = ADC_BUS_SAMPLE;
    break;
    
  case ADC_TEMP_INIT:
    ADC2->CSR = (ADC_TEMP_CHANNEL|BIT5); 
    ADC_Async_State = ADC_TEMP_SAMPLE;
    break;
    
  case ADC_NEUTRAL_POINT_INIT:
    ADC2->CSR = (ADC_NEUTRAL_POINT_CHANNEL|BIT5); 
    ADC_Async_State = ADC_NEUTRAL_POINT_SAMPLE;
    break;
    
  case ADC_USER_ASYNC_INIT:
    ADC2->CSR = (ADC_USER_ASYNC_CHANNEL|BIT5); 
    ADC_Async_State = ADC_USER_ASYNC_SAMPLE;
    break;
    
  case ADC_USER_ASYNC_INIT_2:
    ADC2->CSR = (ADC_USER_ASYNC_2_CHANNEL|BIT5); 
    ADC_Async_State = ADC_USER_ASYNC_SAMPLE_2;
    break;
  }
  
  // Configure syncronous sampling
#ifdef DEV_CUT_1
  // Enable ext. trigger
  ADC2->CR2 |= BIT6;
  //turn on ADC fix bug on cut1 device
  ADC2->CR1 |= BIT0;  
#else
  // Enable ext. trigger
  ADC2->CR2 |= BIT6;
#endif		
}
#endif

void DebugPinsOff(void)
{
#ifdef DEBUG_PINS
  Z_DEBUG_PORT &= (u8)(~Z_DEBUG_PIN);
  C_D_DEBUG_PORT &= (u8)(~C_D_DEBUG_PIN);
  AUTO_SWITCH_PORT &= (u8)(~AUTO_SWITCH_PIN);
  PWM_ON_SW_PORT &= (u8)(~PWM_ON_SW_PIN);
#endif
}

//Timer1 used for main motor PWM outputs and for triggering the ADC sample
//channels 1,2, and 3 are used.  Channel 4 is available
void Init_TIM1(void)
{
  // Enable internal pull-up on BK-IN PD0 - PIN REMAPPED
//  GPIOD->CR1 |= BIT0;
  
  //counter disabled, ARR preload register disabled, up counting, edge aligned mode
  TIM1->CR1 = BIT2;
  
  //Master Mode=TRGO-OC4REF,COMS only on COMG set, Preload CCxE, CCxNE, and OCxM bits
  TIM1->CR2 = BIT0;//(BIT6|BIT5|BIT4|BIT0);  // OC4REF DISABLE
  
  //slave mode disabled
  TIM1->SMCR = 0;
  
#if (defined(ETR_INPUT) && !defined(GP_ETR))
  //Set external trigger filter
  TIM1->ETR = CURRENT_FILTER;
#endif
  
  //disable all interrupts
  TIM1->IER = 0;
  
  TIM1->CCER1 = 0;
  TIM1->CCER2 = 0;
  
  //select PWM mode 1, OC1 preload enabled
  TIM1->CCMR1 = CCMR_PWM;
  //select PWM mode 1, OC2 preload enabled
  TIM1->CCMR2 = CCMR_PWM;
  //select PWM mode 1, OC3 preload enabled
  TIM1->CCMR3 = CCMR_PWM;
  
  //select PWM mode 2, 
//  TIM1->CCMR4 = BIT6|BIT5|BIT4|BIT3;
  
  //prescale = div1 @ 16MHz -> 62.5ns/count, Full scale = 4.09ms 
  TIM1->PSCRH = 0; 
  TIM1->PSCRL = 0;
  
  ToCMPxH( TIM1->ARRH, hArrPwmVal );
  ToCMPxL( TIM1->ARRL, hArrPwmVal );
  
  //disable repetition counter
  TIM1->RCR = 0;
  
  //default to 0% duty cycle
  ToCMPxH( TIM1->CCR1H, 0 );
  ToCMPxL( TIM1->CCR1L, 0 );
  //default to 0% duty cycle
  ToCMPxH( TIM1->CCR2H, 0 );
  ToCMPxL( TIM1->CCR2L, 0 );
  //default to 0% duty cycle
  ToCMPxH( TIM1->CCR3H, 0 );
  ToCMPxL( TIM1->CCR3L, 0 );
  
#ifdef SENSORLESS
  ToCMPxH( TIM1->CCR4H, (hArrPwmVal-SAMPLING_POINT_DURING_TOFF_CNT) );
  ToCMPxL( TIM1->CCR4L, (hArrPwmVal-SAMPLING_POINT_DURING_TOFF_CNT) );
#endif
#ifdef HALL 
//  ToCMPxH( TIM1->CCR4H, hArrPwmVal - MILLIAMP_TOCNT (STARTUP_CURRENT_LIMITATION) );
//  ToCMPxL( TIM1->CCR4L, hArrPwmVal - MILLIAMP_TOCNT (STARTUP_CURRENT_LIMITATION) );
#endif
  
  //set dead time to 6us (with 62.5ns/count)
  TIM1->DTR = (u8)(hCntDeadDtime); 
  
  // Setup IDLE State 
  TIM1->OISR  = 0; // Default inactive 
  
  /* Set the Output Idle state & the Output N Idle state configuration */
#if (PWM_U_HIGH_SIDE_IDLE_STATE == ACTIVE)
  TIM1->OISR |= (u8)(TIM1_OISR_OIS1);
#endif
#if (PWM_U_LOW_SIDE_IDLE_STATE == ACTIVE)
  TIM1->OISR |= (u8)(TIM1_OISR_OIS1N);
#endif
#if (PWM_V_HIGH_SIDE_IDLE_STATE ==  ACTIVE)
  TIM1->OISR |= (u8)(TIM1_OISR_OIS2);
#endif
#if (PWM_V_LOW_SIDE_IDLE_STATE == ACTIVE)
  TIM1->OISR |= (u8)(TIM1_OISR_OIS2N);
#endif
#if (PWM_W_HIGH_SIDE_IDLE_STATE ==  ACTIVE)
  TIM1->OISR |= (u8)(TIM1_OISR_OIS3);
#endif
#if (PWM_W_LOW_SIDE_IDLE_STATE == ACTIVE)
  TIM1->OISR |= (u8)(TIM1_OISR_OIS3N);
#endif
  
  //disable channel outputs
  TIM1->CCER1 = (A_OFF|B_OFF);
  //disable channel outputs
  TIM1->CCER2 = C_OFF;
  
  //enable main outputs, enable break, OSSR=1, OSSI=1, LOCKLEVEL 2, Break Polarity
#define TIM1_OSSRSTATE_ENABLE BIT3
#ifndef LS_GPIO_CONTROL
  TIM1->BKR = (DEV_BKIN_POLARITY|DEV_BKIN|TIM1_OSSRSTATE_ENABLE|TIM1_OSSISTATE_ENABLE|TIM1_LOCKLEVEL_2); 
#else
  // If GPIO is used to drive low side then the disabled outputs of the timer
  // must be not driven by the timer during run time (MOE = 1)
  TIM1->BKR = (DEV_BKIN_POLARITY|DEV_BKIN|TIM1_OSSISTATE_ENABLE|TIM1_LOCKLEVEL_2); 
#endif
  
  //counter enabled
  TIM1->CR1 |= BIT0;
  
  //force timer update
  TIM1->EGR = (BIT5|BIT0);
  
  //enable break interrupt
//  TIM1->IER = BIT7;
  
#if defined(ETR_INPUT)
#if (!defined(GP_ETR))
  //Enable ETR pin
  GPIO_Init(ETR_PORT, ETR_PIN,GPIO_MODE_IN_FL_NO_IT);
#else
  //Enable GP_ETR
  disableInterrupts();
  EXTI_SetExtIntSensitivity(GP_ETR_EXTI_PORT,EXTI_SENSITIVITY_RISE_ONLY);
  enableInterrupts();
  GPIO_Init(GP_ETR_PORT, GP_ETR_PIN,GPIO_MODE_IN_FL_IT);
#endif
#endif
}

void Init_TIM2(void)
{
#ifdef SENSORLESS
  //counter disabled, ARR preload register disabled, up counting, edge aligned mode
  TIM2->CR1 = BIT2;
  
  //disable all interrupts
  TIM2->IER = 0;
  
  TIM2->CCER1 = 0;
  TIM2->CCER2 = 0;
  
  //select PWM mode 2, OC2 preload enabled
  TIM2->CCMR2 = BIT6|BIT5|BIT3;
  
  //prescale = div3 @ 16MHz -> 0.5us/count * 24MHz -> 0.33us/count
  TIM2->PSCR = 0; 
  
  ToCMPxH( TIM2->ARRH, ARRTIM2 );
  ToCMPxL( TIM2->ARRL, ARRTIM2 );
  
  // Current Limitation
  ToCMPxH( TIM2->CCR2H, MILLIAMP_TOCNT (STARTUP_CURRENT_LIMITATION) );
  ToCMPxL( TIM2->CCR2L, MILLIAMP_TOCNT (STARTUP_CURRENT_LIMITATION) );		 
  
  //Enable output
  TIM2->CCER1 |= BIT4;
  
  //counter enabled
  TIM2->CR1 |= BIT0;
  
  //force timer update
  TIM2->EGR = (BIT5|BIT0);
#endif
#ifdef HALL
  // Configure the TIM2 to manage the Hall sensorssignals
  // Set channel 1,2 and 3 as input capture
  
  //counter disabled, ARR preload register disabled, up counting, edge aligned mode
  TIM2->CR1 = BIT2;
  
  //disable all interrupts
  TIM2->IER = 0;
  
  TIM2->CCER1 = 0;
  TIM2->CCER2 = 0;
  
  //select channel 1,2 and 3 as input, channel prescaler 0
  TIM2->CCMR1 = BIT0;
  TIM2->CCMR2 = BIT0;
  TIM2->CCMR3 = BIT0;
  
#define IC_FILTER (u8)(HALL_FILTER << 4)
  
  TIM2->CCMR1 |= IC_FILTER;
  TIM2->CCMR2 |= IC_FILTER;
  TIM2->CCMR3 |= IC_FILTER;
  
  //prescale = div3 @ 16MHz -> 0.5us/count * 24MHz -> 0.33us/count
  TIM2->PSCR = 0; 
  
  ToCMPxH( TIM2->ARRH, 0xFFFF );
  ToCMPxL( TIM2->ARRL, 0xFFFF );
  
  //Enable capture
  TIM2->CCER1 |= BIT0;
  TIM2->CCER1 |= BIT4;
  TIM2->CCER2 |= BIT0;
  
  //counter enabled
  TIM2->CR1 |= BIT0;
#endif
}

#ifdef HALL
void TIM2_InitCapturePolarity(void)
{
  u8 bHStatus = 0;
  // BUG: Depends on remapping
  H1_PORT->DDR &= (u8)(~H1_PIN);
  H2_PORT->DDR &= (u8)(~H2_PIN);
  H3_PORT->DDR &= (u8)(~H3_PIN);
  
  // Read status of H1 and set the expected polarity
  if (H1_PORT->IDR & H1_PIN)
  {
    TIM2->CCER1 |= BIT1;//5;
    bHStatus |= BIT2;
  }
  else
  {
    TIM2->CCER1 &= (u8)(~(BIT1));//5));
  }
  
  // Read status of H2 and set the expected polarity
  if (H2_PORT->IDR & H2_PIN)
  {
    TIM2->CCER1 |= BIT5;//1;
    bHStatus |= BIT1;
  }
  else
  {
    TIM2->CCER1 &= (u8)(~(BIT5));//1));
  }
  
  // Read status of H3 and set the expected polarity
  if (H3_PORT->IDR & H3_PIN)
  {
    TIM2->CCER2 |= BIT1;
    bHStatus |= BIT0;
  }
  else
  {
    TIM2->CCER2 &= (u8)(~(BIT1));
  }
  
  bHallStartStep = bHallSteps[bHStatus];
  
  if (bHallStartStep == NOT_VALID)
  {
    MTC_Status |= MTC_STARTUP_FAILED;
  }
  
  TIM2_ClearITPendingBit(TIM2_IT_CC1);
  TIM2_ClearITPendingBit(TIM2_IT_CC2);
  TIM2_ClearITPendingBit(TIM2_IT_CC3);
  
  // Enable Input Capture Interrupt
  TIM2->IER = BIT1|BIT2|BIT3;
}
#endif

#ifdef HALL
void Hall_Timeout(void)
{
  // Set zero speed
  *pcounter_reg = 0;
  first_cap = 0;
}
#endif

#ifdef TIMER2_HANDLES_HALL
//@near @interrupt @svlreg void TIM2_CAP_COM_IRQHandler (void)
#pragma vector = 0x10
__interrupt void TIM2_CAP_COM_IRQHandler(void)
{
  u8 bHStatus = 0;
  
#ifdef DEBUG_PINS
  Z_DEBUG_PORT ^= Z_DEBUG_PIN;
#endif
  
  GetStepTime();
  
  // Read status of H1 and set the expected polarity
  if (H1_PORT->IDR & H1_PIN)
  {
    TIM2->CCER1 |= BIT1;//5;
    bHStatus |= BIT2;
  }
  else
  {
    TIM2->CCER1 &= (u8)(~(BIT1));//5));
  }
  
  // Read status of H2 and set the expected polarity
  if (H2_PORT->IDR & H2_PIN)
  {
    TIM2->CCER1 |= BIT5;//1;
    bHStatus |= BIT1;
  }
  else
  {
    TIM2->CCER1 &= (u8)(~(BIT5));//1));
  }
  
  // Read status of H3 and set the expected polarity
  if (H3_PORT->IDR & H3_PIN)
  {
    TIM2->CCER2 |= BIT1;
    bHStatus |= BIT0;
  }
  else
  {
    TIM2->CCER2 &= (u8)(~(BIT1));
  }
  
  if (TIM2->SR1 & BIT2)
  {
    TIM2_ClearITPendingBit(TIM2_IT_CC2);
  }
  
  if (TIM2->SR1 & BIT1)
  {
    TIM2_ClearITPendingBit(TIM2_IT_CC1);
  }
  
  if (TIM2->SR1 & BIT3)
  {
    TIM2_ClearITPendingBit(TIM2_IT_CC3);
  }
  
  Current_Step = (u8)(bHallSteps[bHStatus]);
  if (Current_Step == NOT_VALID)
  {
    MTC_Status |= MTC_MOTOR_STALLED;
  }
  
  Current_BEMF = BEMFSteps[Current_Step].BEMF_Level;
  
  TIM1->CCMR1 = PhaseSteps[Current_Step].CCMR_1;
  TIM1->CCMR2 = PhaseSteps[Current_Step].CCMR_2;
  TIM1->CCMR3 = PhaseSteps[Current_Step].CCMR_3;
  TIM1->CCER1 = PhaseSteps[Current_Step].CCER_1;
  TIM1->CCER2 = PhaseSteps[Current_Step].CCER_2;
  
  Phase_State = PHASE_ZERO;
  ComHandler();
  
  // Timeout refresh
  vtimer_SetTimer(HALL_CAPT_TIMEOUT_TIMER,HALL_CAPT_TIMEOUT_MS,(void *)&Hall_Timeout);
  
  SpeedMeasurement();
  
  return;
}
#endif

void bldc_soft_start(void)
{
  u8 bHStatus = 0;
  u32 data=0;
  u16 temp = 0;
  
  if(cylindermotorstatus.cylindermotordirect == 1)
  {
    if(bldc_soft_start_cnt < 2)//( !bldc_soft_start_cnt )//
    {
    BEMFSteps = (BEMF_Step_s*)BEMFSteps_CW;
    PhaseSteps = (Phase_Step_s*)PhaseSteps_CW;
    Fast_Demag_Steps = (Phase_Step_s*)Fast_Demag_Steps_CW;
    
#ifdef LS_GPIO_CONTROL
    LS_Steps = (u8*)LS_Steps_CW;
    LS_Steps_SW = (u8*)LS_Steps_SW_CW;
#endif
    
#ifdef HALL
    bHallSteps = (u8*)bHallSteps_CW;
#endif
    }
    else 
    {
      BEMFSteps = (BEMF_Step_s*)BEMFSteps_CCW;
    PhaseSteps = (Phase_Step_s*)PhaseSteps_CCW;
    Fast_Demag_Steps = (Phase_Step_s*)Fast_Demag_Steps_CCW;
    
#ifdef LS_GPIO_CONTROL
    LS_Steps = (u8*)LS_Steps_CCW;
    LS_Steps_SW = (u8*)LS_Steps_SW_CCW;
#endif
    
#ifdef HALL
    bHallSteps = (u8*)bHallSteps_CCW;
#endif
    }
  }
  else if(cylindermotorstatus.cylindermotordirect == 0)
  {
    if(bldc_soft_start_cnt < 2)//( !bldc_soft_start_cnt )//
    {
    BEMFSteps = (BEMF_Step_s*)BEMFSteps_CCW;
    PhaseSteps = (Phase_Step_s*)PhaseSteps_CCW;
    Fast_Demag_Steps = (Phase_Step_s*)Fast_Demag_Steps_CCW;
    
#ifdef LS_GPIO_CONTROL
    LS_Steps = (u8*)LS_Steps_CCW;
    LS_Steps_SW = (u8*)LS_Steps_SW_CCW;
#endif
    
#ifdef HALL
    bHallSteps = (u8*)bHallSteps_CCW;
#endif
    }
    else
    {
      BEMFSteps = (BEMF_Step_s*)BEMFSteps_CW;
    PhaseSteps = (Phase_Step_s*)PhaseSteps_CW;
    Fast_Demag_Steps = (Phase_Step_s*)Fast_Demag_Steps_CW;
    
#ifdef LS_GPIO_CONTROL
    LS_Steps = (u8*)LS_Steps_CW;
    LS_Steps_SW = (u8*)LS_Steps_SW_CW;
#endif
    
#ifdef HALL
    bHallSteps = (u8*)bHallSteps_CW;
#endif
    }
  }
  
  //Enable capture
  TIM2->CCER1 &= ~BIT0;
  TIM2->CCER1 &= ~BIT4;
  TIM2->CCER2 &= ~BIT0;
  
  //counter enabled
  TIM2->CR1 &= ~BIT0;
     
  // Read status of H1 and set the expected polarity
  if (H1_PORT->IDR & H1_PIN)
    bHStatus |= BIT2;
  
  // Read status of H2 and set the expected polarity
  if (H2_PORT->IDR & H2_PIN)
    bHStatus |= BIT1;
  
  // Read status of H3 and set the expected polarity
  if (H3_PORT->IDR & H3_PIN)
    bHStatus |= BIT0;
   
  Current_Step = (u8)(bHallSteps[bHStatus]);
  if (Current_Step == NOT_VALID)
  {
    MTC_Status |= MTC_MOTOR_STALLED;
  }
  
  data = ((u32)RAMP_DUTY_CYCLE * (u16)hArrPwmVal);
  data = data / (u16)100;
  temp = (u16)data;
  
  // Set Startup Duty
  ToCMPxH( TIM1->CCR1H, temp );
  ToCMPxL( TIM1->CCR1L, temp );
  
  ToCMPxH( TIM1->CCR2H, temp );
  ToCMPxL( TIM1->CCR2L, temp );
  
  ToCMPxH( TIM1->CCR3H, temp );
  ToCMPxL( TIM1->CCR3L, temp );
  
//#ifdef LS_GPIO_CONTROL
//  LS_GPIO_MANAGE();
//#endif
  
//  TIM1->CCMR1 = PhaseSteps[Current_Step].CCMR_1;
//    TIM1->CCMR2 = PhaseSteps[Current_Step].CCMR_2;
//    TIM1->CCMR3 = PhaseSteps[Current_Step].CCMR_3;
//    TIM1->CCER1 = PhaseSteps[Current_Step].CCER_1;
//    TIM1->CCER2 = PhaseSteps[Current_Step].CCER_2;
//    
//  Commutate_Motor();
    
  for(u8 i=0;i<6;i++)
  {
    
    delay_us(4000);//big 5000
//    Phase_State = PHASE_ZERO;
//    ComHandler();
//#ifdef LS_GPIO_CONTROL
//  LS_GPIO_MANAGE();
//#endif
    Current_BEMF = BEMFSteps[Current_Step].BEMF_Level;
    
    TIM1->CCMR1 = PhaseSteps[Current_Step].CCMR_1;
    TIM1->CCMR2 = PhaseSteps[Current_Step].CCMR_2;
    TIM1->CCMR3 = PhaseSteps[Current_Step].CCMR_3;
    TIM1->CCER1 = PhaseSteps[Current_Step].CCER_1;
    TIM1->CCER2 = PhaseSteps[Current_Step].CCER_2;
    
    Commutate_Motor();
  
     
    // Timeout refresh
//    vtimer_SetTimer(HALL_CAPT_TIMEOUT_TIMER,HALL_CAPT_TIMEOUT_MS,(void *)&Hall_Timeout);
    
    Current_Step++;
    if( Current_Step >= NUMBER_PHASE_STEPS )
    {
      Current_Step = 0;
    }
    
  }
  
  if(cylindermotorstatus.cylindermotordirect == 1)
  {
    BEMFSteps = (BEMF_Step_s*)BEMFSteps_CCW;
    PhaseSteps = (Phase_Step_s*)PhaseSteps_CCW;
    Fast_Demag_Steps = (Phase_Step_s*)Fast_Demag_Steps_CCW;
    
#ifdef LS_GPIO_CONTROL
    LS_Steps = (u8*)LS_Steps_CCW;
    LS_Steps_SW = (u8*)LS_Steps_SW_CCW;
#endif
    
#ifdef HALL
    bHallSteps = (u8*)bHallSteps_CCW;
#endif
  }
  else if(cylindermotorstatus.cylindermotordirect == 0)
  {
    BEMFSteps = (BEMF_Step_s*)BEMFSteps_CW;
    PhaseSteps = (Phase_Step_s*)PhaseSteps_CW;
    Fast_Demag_Steps = (Phase_Step_s*)Fast_Demag_Steps_CW;
    
#ifdef LS_GPIO_CONTROL
    LS_Steps = (u8*)LS_Steps_CW;
    LS_Steps_SW = (u8*)LS_Steps_SW_CW;
#endif
    
#ifdef HALL
    bHallSteps = (u8*)bHallSteps_CW;
#endif
  }
  
  //Enable capture
  TIM2->CCER1 |= BIT0;
  TIM2->CCER1 |= BIT4;
  TIM2->CCER2 |= BIT0;
  
  //counter enabled
  TIM2->CR1 |= BIT0;
}

void ComHandler(void)
{
  u16 cur_time;
  u16 temp_time;
  u8 i;
  
    
  switch( Phase_State )
  {
  case PHASE_COMM:
#ifdef DEBUG_PINS
    C_D_DEBUG_PORT |= C_D_DEBUG_PIN;
#endif
    Commutate_Motor();
#ifdef SENSORLESS
    Phase_State = PHASE_DEMAG; 
#endif	
#ifdef HALL
    Phase_State = PHASE_ZERO;
#endif
    Enable_ADC_User_Sync_Sampling();
    break;
    
  case PHASE_DEMAG:
#ifdef DEBUG_PINS
    C_D_DEBUG_PORT &= (u8)(~C_D_DEBUG_PIN);
#endif
    if( (MTC_Status & MTC_STEP_MODE) == MTC_STEP_MODE )
    {
      //load commutation time from ramp table
      Commutation_Time = RAMP_TABLE[ Ramp_Step ];
      if( Ramp_Step < STEP_RAMP_SIZE )
      {
        Ramp_Step++;
      }
      else
      {
        MTC_Status |= MTC_STARTUP_FAILED;                  
      }
      
      //load commutation time into timer
      hTim3Th = LastSwitchedCom + Commutation_Time;
    }
    else
    {
      //load fail safe commutation time in case zero cross not detected (2 zero crossing time)
      temp_time = Previous_Zero_Cross_Time * 2;
      hTim3Th = hTim3Cnt + temp_time;
    }
    
    //commutate the motor
    TIM1->EGR |= BIT5;
    
    // Save actual register
    bTIM1_CCER1_actual = TIM1->CCER1;
    bTIM1_CCER2_actual = TIM1->CCER2;
    bTIM1_CCMR1_actual = TIM1->CCMR1;
    bTIM1_CCMR2_actual = TIM1->CCMR2;
    bTIM1_CCMR3_actual = TIM1->CCMR3;
    
    // Update CCMRx OCxCE bit (Actual)
    TIM1->CCMR1 = (u8)(PhaseSteps[Current_Step].CCMR_1 & 0x80);
    TIM1->CCMR2 = (u8)(PhaseSteps[Current_Step].CCMR_2 & 0x80);
    TIM1->CCMR3 = (u8)(PhaseSteps[Current_Step].CCMR_3 & 0x80);
    
    //preload next step
    Current_Step++;
    if( Current_Step >= NUMBER_PHASE_STEPS )
    {
      Current_Step = 0;
    }
    
    if (g_pBLDC_Struct->pBLDC_Var->bFastDemag == 1)
    {
      // Set fast demag
      TIM1->CCMR1 |= (u8)(Fast_Demag_Steps[Current_Step].CCMR_1 & 0x7F);
      TIM1->CCMR2 |= (u8)(Fast_Demag_Steps[Current_Step].CCMR_2 & 0x7F);
      TIM1->CCMR3 |= (u8)(Fast_Demag_Steps[Current_Step].CCMR_3 & 0x7F);
      tmp_TIM1_CCER1 = Fast_Demag_Steps[Current_Step].CCER_1;
      tmp_TIM1_CCER2 = Fast_Demag_Steps[Current_Step].CCER_2;
    }
    else
    {
      // Set fast demag
      TIM1->CCMR1 |= (u8)(PhaseSteps[Current_Step].CCMR_1 & 0x7F);
      TIM1->CCMR2 |= (u8)(PhaseSteps[Current_Step].CCMR_2 & 0x7F);
      TIM1->CCMR3 |= (u8)(PhaseSteps[Current_Step].CCMR_3 & 0x7F);
      tmp_TIM1_CCER1 = PhaseSteps[Current_Step].CCER_1;
      tmp_TIM1_CCER2 = PhaseSteps[Current_Step].CCER_2;
    }
    
    // Store the values
    tmp_TIM1_CCMR1 = TIM1->CCMR1;
    tmp_TIM1_CCMR2 = TIM1->CCMR2;
    tmp_TIM1_CCMR3 = TIM1->CCMR3;
    
    // Set to frozen for next COM
    TIM1->CCMR1 &= 0x8F;
    TIM1->CCMR2 &= 0x8F;
    TIM1->CCMR3 &= 0x8F;
    
    // Save preload register
    bTIM1_CCER1_preload = TIM1->CCER1;
    bTIM1_CCER2_preload = TIM1->CCER2;
    bTIM1_CCMR1_preload = TIM1->CCMR1;
    bTIM1_CCMR2_preload = TIM1->CCMR2;
    bTIM1_CCMR3_preload = TIM1->CCMR3;
    
    Enable_ADC_BEMF_Sampling();
    Phase_State = PHASE_ZERO; 
    break;
    
  case PHASE_ZERO:
    Enable_ADC_Current_Sampling();
    if( Zero_Cross_Count != Last_Zero_Cross_Count )
    {
#ifdef HALL
#ifdef DEBUG_PINS
      C_D_DEBUG_PORT &= (u8)(~C_D_DEBUG_PIN);
#endif
#endif
      
      if( (MTC_Status & MTC_STEP_MODE) != MTC_STEP_MODE )
      {
        // Autoswitch mode
        
        //adjust commutation time based on motor speed
        
        if( Current_BEMF == BEMF_FALLING )
        {
//#asm
; Commutation_Time = (Previous_Zero_Cross_Time * BEMF_Falling_Factor) >> 8;

asm("ld A,Previous_Zero_Cross_Time");
asm("ld XL,A");
asm("ld A,BEMF_Falling_Factor");
asm("mul X,A");
asm("ldw Commutation_Time,X");
asm("ld A,Previous_Zero_Cross_Time + 1");
asm("ld XL,A");
asm("ld A,BEMF_Falling_Factor");
asm("mul X,A");
asm("ld A,XH");
asm("clrw X");
asm("ld XL,A");
asm("addw X,Commutation_Time");
asm("ldw Commutation_Time,X");
//#endasm
        }
        else
        {
          Motor_Stall_Count = 0;
//#asm
; Commutation_Time = (Previous_Zero_Cross_Time * BEMF_Rising_Factor) >> 8;

asm("ld A,Previous_Zero_Cross_Time");
asm("ld XL,A");
asm("ld A,BEMF_Rising_Factor");
asm("mul X,A");
asm("ldw Commutation_Time,X");
asm("ld A,Previous_Zero_Cross_Time + 1");
asm("ld XL,A");
asm("ld A,BEMF_Rising_Factor");
asm("mul X,A");
asm("ld A,XH");
asm("clrw X");
asm("ld XL,A");
asm("addw X,Commutation_Time");
asm("ldw Commutation_Time,X");
//#endasm
        }
        
        if( Zero_Sample_Count == 1 )
        {                                      
          
          Commutation_Time >>= 1;
        }
        
        hTim3Th = Commutation_Time;
      }
      else
      {
#ifdef SENSORLESS
        //if number of consectutive BEMF events occurred, switch out of stepped mode
        if( Zero_Cross_Count >= MINIMUM_CONSECTIVE_ZERO_CROSS )
        {
          MTC_Status |= MTC_LAST_FORCED_STEP;
          Motor_Stall_Count = 0;
          Commutation_Time = Average_Zero_Cross_Time;
          
          hTim3Th = Average_Zero_Cross_Time;
          
          //load commutation time into timer
          Previous_Zero_Cross_Time = Commutation_Time;
          Zero_Cross_Time = Commutation_Time;
        }
#endif
#ifdef HALL
        MTC_Status &= (u8)(~MTC_STEP_MODE);
        
        // Force the commutation after first edge
        Phase_State = PHASE_COMM;
        ComHandler();
#endif
      }
      
      Average_Zero_Cross_Time = (Previous_Zero_Cross_Time + Zero_Cross_Time) >> 1;
      Previous_Zero_Cross_Time = Zero_Cross_Time;
      Phase_State = PHASE_COMM; 
    }
    else
    {
#ifdef SENSORLESS
      //if zero crossing not detected - commutate motor
      if( (MTC_Status & MTC_STEP_MODE) != MTC_STEP_MODE )
      {
        if( Current_BEMF == BEMF_RISING )
        {
          if( Motor_Stall_Count < MOTOR_STALL_THRESHOLD )
          {
            Motor_Stall_Count++;
          }
          else
          {
            MTC_Status |= MTC_MOTOR_STALLED;
          }
        }
      }  
#ifdef DEBUG_PINS
      C_D_DEBUG_PORT |= C_D_DEBUG_PIN;
#endif
      Commutate_Motor();
      Zero_Cross_Count=0;
#ifdef SENSORLESS
      Phase_State = PHASE_DEMAG; 
#endif
#ifdef HALL
      Phase_State = PHASE_ZERO;
#endif
#endif
    }
    Last_Zero_Cross_Count = Zero_Cross_Count;
    break;
    
  default:
    Phase_State = PHASE_COMM; 
    break;
  }
}

#ifdef SENSORLESS
void Commutate_Motor( void )
{
  u16 cur_time;
  
  cur_time = hTim3Cnt;
  
  // Switch to Frozen
  TIM1->EGR |= BIT5;
  
  // Restore Values
  TIM1->CCMR1 = tmp_TIM1_CCMR1;
  TIM1->CCMR2 = tmp_TIM1_CCMR2;
  TIM1->CCMR3 = tmp_TIM1_CCMR3;
  TIM1->CCER1 = tmp_TIM1_CCER1;
  TIM1->CCER2 = tmp_TIM1_CCER2;
  
  //commutate the motor
  TIM1->EGR |= BIT5;
  
#ifdef LS_GPIO_CONTROL
  {
    LS_Conf = LS_Steps_SW[Current_Step];
    
    if (LS_Conf == LS_A_B)
    {
      // Activate   B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_B_PORT->ODR |= LS_B_PIN;
#else
      LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#endif
      // Deactivate A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#else
      LS_A_PORT->ODR |= LS_A_PIN;
#endif
    }
    
    if (LS_Conf == LS_A_C)
    {
      // Activate   C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_C_PORT->ODR |= LS_C_PIN;
#else
      LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#endif
      // Deactivate A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#else
      LS_A_PORT->ODR |= LS_A_PIN;
#endif
    }
    
    if (LS_Conf == LS_B_A)
    {
      // Activate   A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_A_PORT->ODR |= LS_A_PIN;
#else
      LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#endif
      // Deactivate B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#else
      LS_B_PORT->ODR |= LS_B_PIN;
#endif
    }
    
    if (LS_Conf == LS_B_C)
    {
      // Activate   C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_C_PORT->ODR |= LS_C_PIN;
#else
      LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#endif
      // Deactivate B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#else
      LS_B_PORT->ODR |= LS_B_PIN;
#endif
    }
    
    if (LS_Conf == LS_C_A)
    {
      // Activate   A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_A_PORT->ODR |= LS_A_PIN;
#else
      LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#endif
      // Deactivate C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#else
      LS_C_PORT->ODR |= LS_C_PIN;
#endif
    }
    
    if (LS_Conf == LS_C_B)
    {
      // Activate   B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_B_PORT->ODR |= LS_B_PIN;
#else
      LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#endif
      // Deactivate C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
      LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#else
      LS_C_PORT->ODR |= LS_C_PIN;
#endif
    }
  }
#endif
  
  // Save actual register
  bTIM1_CCER1_actual = TIM1->CCER1;
  bTIM1_CCER2_actual = TIM1->CCER2;
  bTIM1_CCMR1_actual = TIM1->CCMR1;
  bTIM1_CCMR2_actual = TIM1->CCMR2;
  bTIM1_CCMR3_actual = TIM1->CCMR3;
  
  Current_BEMF = BEMFSteps[Current_Step].BEMF_Level;
  Current_BEMF_Channel = BEMFSteps[Current_Step].ADC_Channel;
  
  if (g_pBLDC_Struct->pBLDC_Var->bFastDemag == 0)
  {
    // Update CCMRx OCxCE bit (Actual)
    TIM1->CCMR1 = (u8)(PhaseSteps[Current_Step].CCMR_1 & 0x80);
    TIM1->CCMR2 = (u8)(PhaseSteps[Current_Step].CCMR_2 & 0x80);
    TIM1->CCMR3 = (u8)(PhaseSteps[Current_Step].CCMR_3 & 0x80);
  }
  else
  {
    // Update CCMRx OCxCE bit (Actual)
    TIM1->CCMR1 = (u8)(Fast_Demag_Steps[Current_Step].CCMR_1 & 0x80);
    TIM1->CCMR2 = (u8)(Fast_Demag_Steps[Current_Step].CCMR_2 & 0x80);
    TIM1->CCMR3 = (u8)(Fast_Demag_Steps[Current_Step].CCMR_3 & 0x80);
  }
  
  // Preload next values
  TIM1->CCMR1 |= (u8)(PhaseSteps[Current_Step].CCMR_1 & 0x7F);
  TIM1->CCMR2 |= (u8)(PhaseSteps[Current_Step].CCMR_2 & 0x7F);
  TIM1->CCMR3 |= (u8)(PhaseSteps[Current_Step].CCMR_3 & 0x7F);
  TIM1->CCER1 = PhaseSteps[Current_Step].CCER_1;
  TIM1->CCER2 = PhaseSteps[Current_Step].CCER_2;
  
  // Save preload register
  bTIM1_CCER1_preload = TIM1->CCER1;
  bTIM1_CCER2_preload = TIM1->CCER2;
  bTIM1_CCMR1_preload = TIM1->CCMR1;
  bTIM1_CCMR2_preload = TIM1->CCMR2;
  bTIM1_CCMR3_preload = TIM1->CCMR3;
  
  //calc demag time
  if( (MTC_Status & MTC_STEP_MODE) == MTC_STEP_MODE )
  {
    if( (MTC_Status & MTC_LAST_FORCED_STEP) == MTC_LAST_FORCED_STEP )
    {
      MTC_Status &= (u8)(~MTC_STEP_MODE);
#ifdef DEBUG_PINS
      AUTO_SWITCH_PORT |= AUTO_SWITCH_PIN;
#endif
    }
    
    //Demag_Time = (u16)((u32)(Commutation_Time * BLDC_Get_Demag_Time()) / 100);
    tmp_u8 = BLDC_Get_Demag_Time();
#asm
; tmp_sc_u8 = (u8)((tmp_u8 * 256) / (u8)(100));

ld A,_tmp_u8
clrw x
ld XH,A
ld A,#0x64
div X,A

ld A,XL
ld _tmp_u8,A

; Demag_Time = (Commutation_Time * tmp_sc_u8) >> 8;

ld A,_Commutation_Time
ld XL,A
ld A,_tmp_u8
mul X,A
ldw _tmp_u16,X
ld A,_Commutation_Time + 1
ld XL,A
ld A,_tmp_u8
mul X,A
ld A,XH
clrw X
ld XL,A
addw X,_tmp_u16
ldw _Demag_Time,X
#endasm
  }
  else
  {		
    //Demag_Time = (u16)((u32)(Average_Zero_Cross_Time * BLDC_Get_Demag_Time()) / 100);
    tmp_u8 = BLDC_Get_Demag_Time();
#asm
; tmp_sc_u8 = (u8)((BLDC_Get_Demag_Time() * 256) / (u8)(100));

ld A,_tmp_u8
clrw x
ld XH,A
ld A,#0x64
div X,A

ld A,XL
ld _tmp_u8,A

; Demag_Time = (Average_Zero_Cross_Time * tmp_sc_u8) >> 8;

ld A,_Average_Zero_Cross_Time
ld XL,A
ld A,_tmp_u8
mul X,A
ldw _tmp_u16,X
ld A,_Average_Zero_Cross_Time + 1
ld XL,A
ld A,_tmp_u8
mul X,A
ld A,XH
clrw X
ld XL,A
addw X,_tmp_u16
ldw _Demag_Time,X
#endasm
  }
  
  LastSwitchedCom = hTim3Cnt;
  hTim3Th = hTim3Cnt + Demag_Time;
}
#endif

#ifdef HALL
void Commutate_Motor( void )
{
  //commutate the motor
  TIM1->EGR |= BIT5;
  
#ifdef LS_GPIO_CONTROL
  LS_Conf = LS_Steps[Current_Step];
  
  if ((LS_Conf & LS_A) == LS_A)
  {	// Activate   A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_A_PORT->ODR |= LS_A_PIN;
#else
    LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#endif
  }
  else
  {
    // Deactivate A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#else
    LS_A_PORT->ODR |= LS_A_PIN;
#endif
  }
  
  if ((LS_Conf & LS_B)== LS_B)
  {
    // Activate   B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_B_PORT->ODR |= LS_B_PIN;
#else
    LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#endif
  }
  else
  {
    // Deactivate B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#else
    LS_B_PORT->ODR |= LS_B_PIN;
#endif
  }
  
  if ((LS_Conf & LS_C) == LS_C)
  {
    // Activate   C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_C_PORT->ODR |= LS_C_PIN;
#else
    LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#endif
  }
  else
  {
    // Deactivate C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#else
    LS_C_PORT->ODR |= LS_C_PIN;
#endif
  }
#endif
}
#endif


void StopMotor( void )
{	
  //Disable update interrupt
  TIM1->IER &= (u8)(~BIT0);
  
#ifdef HALL
  // Disable Input Capture Interrupt
  TIM2->IER = 0;
#endif
  
  TIM1->CCMR1 = CCMR_PWM;
  TIM1->CCMR2 = CCMR_PWM;
  TIM1->CCMR3 = CCMR_PWM;
  TIM1->CCER1 = (A_OFF|B_OFF);
  TIM1->CCER2 = C_OFF;
  TIM1->EGR |= BIT5;
  
#ifdef LS_GPIO_CONTROL
  LS_GPIO_OFF();
#endif
  
  Enable_ADC_User_Sync_Sampling();
  Motor_Frequency = 0;
}

void BrakeMotor( void )
{
  u16 brake_pwm_cnt;
  brake_pwm_cnt = (u16)(((u32)hArrPwmVal * BRAKE_DUTY) / 100);
  ToCMPxH( TIM1->CCR1H, brake_pwm_cnt );
  ToCMPxL( TIM1->CCR1L, brake_pwm_cnt );
  ToCMPxH( TIM1->CCR2H, brake_pwm_cnt );
  ToCMPxL( TIM1->CCR2L, brake_pwm_cnt );
  ToCMPxH( TIM1->CCR3H, brake_pwm_cnt );
  ToCMPxL( TIM1->CCR3L, brake_pwm_cnt );
  
  //Disable update interrupt
  TIM1->IER &= (u8)(~BIT0);
  
  TIM1->CCMR1 = CCMR_PWM;
  TIM1->CCMR2 = CCMR_LOWSIDE;
  TIM1->CCMR3 = CCMR_LOWSIDE;
  TIM1->CCER1 = (A_ON|B_COMP);
  TIM1->CCER2 = C_COMP;
  
  TIM1->EGR |= BIT5;
  
#ifdef LS_GPIO_CONTROL
  LS_GPIO_BRAKE(); // Replaced with LS_GPIO_BOOT for L6230
  LS_GPIO_BOOT();
#endif
}

u8 BootStrap(void)
{
  TIM1->CCMR1 = CCMR_LOWSIDE;
  TIM1->CCMR2 = CCMR_LOWSIDE;
  TIM1->CCMR3 = CCMR_LOWSIDE;
  TIM1->CCER1 = (A_COMP|B_COMP);
  TIM1->CCER2 = C_COMP;
  //force update of output states
  TIM1->EGR |= BIT5;
  
  // Enable MC Outputs
  TIM1->BKR |= BIT7;
  
#ifdef LS_GPIO_CONTROL
  LS_GPIO_BOOT();
#endif
  
  return (vtimer_TimerElapsed(MTC_ALIGN_RAMP_TIMER));
}

u8 AlignRotor( void )
{
  u8 status;
  u16 temp;
  u32 temp32;
  
  status = 0;
  switch( Align_State )
  {
  default:
  case ALIGN_IDLE:
    TIM1->CCMR1 = CCMR_PWM;
    TIM1->CCMR2 = CCMR_LOWSIDE;
    TIM1->CCMR3 = CCMR_LOWSIDE;
    TIM1->CCER1 = (A_ON|B_COMP);
    TIM1->CCER2 = C_COMP;
    
    ToCMPxH( TIM1->CCR1H, 0 );
    ToCMPxL( TIM1->CCR1L, 0 );
    ToCMPxH( TIM1->CCR2H, 0 );
    ToCMPxL( TIM1->CCR2L, 0 );
    ToCMPxH( TIM1->CCR3H, 0 );
    ToCMPxL( TIM1->CCR3L, 0 );
    
    Align_Target = ALIGN_DUTY_CYCLE;
    Align_Index = 0;
    
    vtimer_SetTimer(MTC_ALIGN_RAMP_TIMER,ALIGN_SLOPE,0);
    vtimer_SetTimer(MTC_ALIGN_TIMER,ALIGN_DURATION,0);
    
    //force update of output states
    TIM1->EGR |= BIT5;
    
    // Save actual register
    bTIM1_CCER1_actual = TIM1->CCER1;
    bTIM1_CCER2_actual = TIM1->CCER2;
    bTIM1_CCMR1_actual = TIM1->CCMR1;
    bTIM1_CCMR2_actual = TIM1->CCMR2;
    bTIM1_CCMR3_actual = TIM1->CCMR3;
    
#ifdef LS_GPIO_CONTROL
    LS_GPIO_BRAKE(); // Replaced with LS_GPIO_BOOT for L6230
    LS_GPIO_BOOT();
#endif
    
    //preload next step
    TIM1->CCMR1 = PhaseSteps[Current_Step].CCMR_1;
    TIM1->CCMR2 = PhaseSteps[Current_Step].CCMR_2;
    TIM1->CCMR3 = PhaseSteps[Current_Step].CCMR_3;
    TIM1->CCER1 = PhaseSteps[Current_Step].CCER_1;
    TIM1->CCER2 = PhaseSteps[Current_Step].CCER_2;
    
    // Save preload register
    bTIM1_CCER1_preload = TIM1->CCER1;
    bTIM1_CCER2_preload = TIM1->CCER2;
    bTIM1_CCMR1_preload = TIM1->CCMR1;
    bTIM1_CCMR2_preload = TIM1->CCMR2;
    bTIM1_CCMR3_preload = TIM1->CCMR3;
    
    BEMF_Falling_Factor = BEMF_FALL_DELAY_FACTOR;
    BEMF_Rising_Factor = BEMF_RISE_DELAY_FACTOR;
    
    TIM1->DTR = (u8)(hCntDeadDtime);
    
    Align_State = ALIGN_RAMP;
    break;
    
  case ALIGN_RAMP:
    if (vtimer_TimerElapsed(MTC_ALIGN_RAMP_TIMER))
    {
      if( Align_Index < Align_Target )
      {
        Align_Index += 1;
        temp32 = ((u32)Align_Index * (u16)hArrPwmVal);
        temp32 = temp32/(u16)100;
        temp = (u16)temp32;
        
        ToCMPxH( TIM1->CCR1H, temp );
        ToCMPxL( TIM1->CCR1L, temp );
        
        ToCMPxH( TIM1->CCR2H, temp );
        ToCMPxL( TIM1->CCR2L, temp );
        
        ToCMPxH( TIM1->CCR3H, temp );
        ToCMPxL( TIM1->CCR3L, temp );
      }
      vtimer_SetTimer(MTC_ALIGN_RAMP_TIMER,ALIGN_SLOPE,0);
    }                     
    
    if (vtimer_TimerElapsed(MTC_ALIGN_TIMER))
    {
      Align_State = ALIGN_DONE;
      status = 1;
    } 
    break;
    
  case ALIGN_DONE:
    status = 1;
    break;
  } 		
  return( status );
}

void StartMotor( void )
{
  u16 cur_time;
  u16 temp;
  u32 data;
  
  Zero_Cross_Count=0;
  Last_Zero_Cross_Count=0;
  Ramp_Step=0;
  Commutation_Time = RAMP_TABLE[ Ramp_Step ];
  Ramp_Step++;
  
  //force update of output states
  TIM1->EGR |= BIT5;
  
#ifdef LS_GPIO_CONTROL
  LS_GPIO_MANAGE();
#endif
  
  Current_BEMF = BEMFSteps[Current_Step].BEMF_Level;
  Current_BEMF_Channel = BEMFSteps[Current_Step].ADC_Channel;
  
  //preload next step
  Current_Step++;
  if( Current_Step >= NUMBER_PHASE_STEPS )
  {
    Current_Step = 0;
  }
  TIM1->CCMR1 = PhaseSteps[Current_Step].CCMR_1;
  TIM1->CCMR2 = PhaseSteps[Current_Step].CCMR_2;
  TIM1->CCMR3 = PhaseSteps[Current_Step].CCMR_3;
  tmp_TIM1_CCER1 = PhaseSteps[Current_Step].CCER_1;
  tmp_TIM1_CCER2 = PhaseSteps[Current_Step].CCER_2;
  
  // Store the values for the commutation
  tmp_TIM1_CCMR1 = TIM1->CCMR1;
  tmp_TIM1_CCMR2 = TIM1->CCMR2;
  tmp_TIM1_CCMR3 = TIM1->CCMR3;
  
  // Set to frozen for next COM
  TIM1->CCMR1 &= 0x8F;
  TIM1->CCMR2 &= 0x8F;
  TIM1->CCMR3 &= 0x8F;
  
  //load commutation time into timer
  hTim3Th = Commutation_Time;
  
  data = ((u32)RAMP_DUTY_CYCLE * (u16)hArrPwmVal);
  data = data / (u16)100;
  temp = (u16)data;
  
  // Set Startup Duty
  ToCMPxH( TIM1->CCR1H, temp );
  ToCMPxL( TIM1->CCR1L, temp );
  
  ToCMPxH( TIM1->CCR2H, temp );
  ToCMPxL( TIM1->CCR2L, temp );
  
  ToCMPxH( TIM1->CCR3H, temp );
  ToCMPxL( TIM1->CCR3L, temp );
  
  // Current Limitation
#ifdef SENSORLESS
  ToCMPxH( TIM2->CCR2H, MILLIAMP_TOCNT (STARTUP_CURRENT_LIMITATION) );
  ToCMPxL( TIM2->CCR2L, MILLIAMP_TOCNT (STARTUP_CURRENT_LIMITATION) );
#endif
#ifdef HALL
//  ToCMPxH( TIM1->CCR4H, hArrPwmVal - MILLIAMP_TOCNT (STARTUP_CURRENT_LIMITATION) );
//  ToCMPxL( TIM1->CCR4L, hArrPwmVal - MILLIAMP_TOCNT (STARTUP_CURRENT_LIMITATION) );
#endif
  
  Phase_State = PHASE_ZERO; 
}

/*-----------------------------------------------------------------------------
ROUTINE Name : Set_Duty

Description:    This is the update routine of the PWM duty cycle -> feed MCPUH/L
Input is the new duty to be set express in Timer counts

Comments:   Used in Voltage Mode
-----------------------------------------------------------------------------*/
u16 Set_Duty(u16 duty)
{
  u16 time;
  
  // Adjust Delay coefficient
  DelayCoefAdjust();
  
  if( Z_Detection_Type == Z_DETECT_PWM_OFF )
  {
    if (duty > hMaxDutyCnt)
    {
      duty = hMaxDutyCnt;   
    }
    
    //set dead time 
    TIM1->DTR = (u8)(hCntDeadDtime); 
  }
  else
  {
    //adjust deadtime generator to get smaller off time than deadtime allows
    time = hArrPwmVal - duty;
    if( time >= hCntDeadDtime )
    {
      TIM1->DTR = (u8)(hCntDeadDtime); 
    }
    else
    {
      if( time <= 16 )
      {
        // if less than 1us then turn off deadtime generator
        TIM1->DTR = 0;
      }
      else
      {
        TIM1->DTR = (u8)time; 
      }
    }
    
    if( duty > hArrPwmVal )
    {
      //set to 100%
      duty = hArrPwmVal;
    }
  }
  
  ToCMPxH( TIM1->CCR1H, duty );
  ToCMPxL( TIM1->CCR1L, duty );
  
  ToCMPxH( TIM1->CCR2H, duty );
  ToCMPxL( TIM1->CCR2L, duty );
  
  ToCMPxH( TIM1->CCR3H, duty );
  ToCMPxL( TIM1->CCR3L, duty );
  
  // Current Limitation
#ifdef SENSORLESS
  ToCMPxH( TIM2->CCR2H, CurrentLimitCnt );
  ToCMPxL( TIM2->CCR2L, CurrentLimitCnt );
#endif
#ifdef HALL
//  ToCMPxH( TIM1->CCR4H, hArrPwmVal - CurrentLimitCnt );
//  ToCMPxL( TIM1->CCR4L, hArrPwmVal - CurrentLimitCnt );
#endif
  
  *pDutyCycleCounts_reg = duty;
  return duty;
}    

/*-----------------------------------------------------------------------------
ROUTINE Name : Set_Current

Description:   This is the update routine of the PWM duty cycle -> feed MCPUH/L
Input is the new current reference express in mA 

Comments:   Used in Current Mode
-----------------------------------------------------------------------------*/
u16 Set_Current(u16 current)
{
  u16 temp;
  u16 time;
  
  // Adjust Delay coefficient
  DelayCoefAdjust();
  
  //Set 100% of Duty
  temp = hArrPwmVal;
  
  if( Z_Detection_Type == Z_DETECT_PWM_OFF )
  {
    /* Set the pulse width for voltage mode*/
    if (temp > hMaxDutyCnt)
    {
      temp = hMaxDutyCnt;   
    }
    
    //set dead time 
    TIM1->DTR = (u8)(hCntDeadDtime); 
  }
  else
  {
    //adjust deadtime generator to get smaller off time than deadtime allows
    time = hArrPwmVal - temp;
    if( time >= hCntDeadDtime )
    {
      TIM1->DTR = (u8)(hCntDeadDtime);
    }
    else
    {
      if( time <= 16 )
      {
        // if less than 1us then turn off deadtime generator
        TIM1->DTR = 0;
      }
      else
      {
        TIM1->DTR = (u8)time; 
      }
    }
  }
  
  ToCMPxH( TIM1->CCR1H, temp );
  ToCMPxL( TIM1->CCR1L, temp );
  
  ToCMPxH( TIM1->CCR2H, temp );
  ToCMPxL( TIM1->CCR2L, temp );
  
  ToCMPxH( TIM1->CCR3H, temp );
  ToCMPxL( TIM1->CCR3L, temp );
  
  temp = current;
  
  // Current Limitation
  current = (u16)(MILLIAMP_TOCNT (current));
  
  if (current > CurrentLimitCnt)
    current = CurrentLimitCnt;
  
  // Current Limitation
#ifdef SENSORLESS
  ToCMPxH( TIM2->CCR2H, current );
  ToCMPxL( TIM2->CCR2L, current );
#endif
#ifdef HALL
//  ToCMPxH( TIM1->CCR4H, hArrPwmVal - current );
//  ToCMPxL( TIM1->CCR4L, hArrPwmVal - current );
#endif
  
  return temp;
} 

void SetSamplingPoint_BEMF(void)
{
#if (BEMF_SAMPLING_METHOD == BEMF_SAMPLING_MIXED)
  if (StartUpExecution == 0)
  {
    // Run state
    if ((*pDutyCycleCounts_reg  < hDutyCntTh ) || (StartUpExecution == 1))
    {
#ifdef DEBUG_PINS
      // Info PINS
      PWM_ON_SW_PORT &= (u8)(~PWM_ON_SW_PIN);
#endif
      
      Z_Detection_Type = Z_DETECT_PWM_OFF;
      //make control lines floating Input.
      MCI_CONTROL_DDR &= (u8)(~MCI_CONTROL_PINS);
      
      //set sample point xx us before Ton - during PWM-Off time
//      ToCMPxH( TIM1->CCR4H, (hArrPwmVal-SAMPLING_POINT_DURING_TOFF_CNT) );
//      ToCMPxL( TIM1->CCR4L, (hArrPwmVal-SAMPLING_POINT_DURING_TOFF_CNT) );
    }
    else
    {
#ifdef DEBUG_PINS
      // Info PINS
      PWM_ON_SW_PORT |= PWM_ON_SW_PIN;
#endif
      Z_Detection_Type = Z_DETECT_PWM_ON;
      //Make control line (P-P) output low
      MCI_CONTROL_DR &= (u8)(~MCI_CONTROL_PINS);
      MCI_CONTROL_DDR |= MCI_CONTROL_PINS;
      
      // set sampling point xx us after Ton + DeadTime - during PWM_On
//      ToCMPxH( TIM1->CCR4H, (hCntDeadDtime+SAMPLING_POINT_DURING_TON_CNT) );
//      ToCMPxL( TIM1->CCR4L, (hCntDeadDtime+SAMPLING_POINT_DURING_TON_CNT) );
      
    }
  }
  else
  {
    // Ramp-up
    {
#ifdef DEBUG_PINS
      // Info PINS
      PWM_ON_SW_PORT &= (u8)(~PWM_ON_SW_PIN);
#endif
      
      Z_Detection_Type = Z_DETECT_PWM_OFF;
      //make control lines floating Input.
      MCI_CONTROL_DDR &= (u8)(~MCI_CONTROL_PINS);
      
      //set sample point xx us before Ton - during PWM-Off time
//      ToCMPxH( TIM1->CCR4H, (hArrPwmVal-SAMPLING_POINT_DURING_TOFF_CNT) );
//      ToCMPxL( TIM1->CCR4L, (hArrPwmVal-SAMPLING_POINT_DURING_TOFF_CNT) );
    }
  }
#endif
  
#if (BEMF_SAMPLING_METHOD == BEMF_SAMPLING_TOFF)
#ifdef DEBUG_PINS
  // Info PINS
  PWM_ON_SW_PORT &= (u8)(~PWM_ON_SW_PIN);
#endif
  
  Z_Detection_Type = Z_DETECT_PWM_OFF;
  //make control lines floating Input.
  MCI_CONTROL_DDR &= (u8)(~MCI_CONTROL_PINS);
  
  // set sampling point xx us before Ton - during PWM_Off
//  ToCMPxH( TIM1->CCR4H, (hArrPwmVal-SAMPLING_POINT_DURING_TOFF_CNT) );
//  ToCMPxL( TIM1->CCR4L, (hArrPwmVal-SAMPLING_POINT_DURING_TOFF_CNT) );
#endif
  
#if (BEMF_SAMPLING_METHOD == BEMF_SAMPLING_TON)
#ifdef DEBUG_PINS
  // Info PINS
  PWM_ON_SW_PORT |= PWM_ON_SW_PIN;
#endif
  Z_Detection_Type = Z_DETECT_PWM_ON;
  //Make control line (P-P) output low
  MCI_CONTROL_DR &= (u8)(~MCI_CONTROL_PINS);
  MCI_CONTROL_DDR |= MCI_CONTROL_PINS;
  
  // set sampling point xx us after Ton + DeadTime - during PWM_On
//  ToCMPxH( TIM1->CCR4H, (hCntDeadDtime+SAMPLING_POINT_DURING_TON_CNT) );
//  ToCMPxL( TIM1->CCR4L, (hCntDeadDtime+SAMPLING_POINT_DURING_TON_CNT) );
#endif
}

void SetSamplingPoint_Current(void)
{
  // set sampling point xx us after Ton + DeadTime - during PWM_On
//  ToCMPxH( TIM1->CCR4H, (hCntDeadDtime+SAMPLING_POINT_DURING_TON_CNT) );
//  ToCMPxL( TIM1->CCR4L, (hCntDeadDtime+SAMPLING_POINT_DURING_TON_CNT) );
}

void SetSamplingPoint_User_Sync(void)
{
  // set sampling point xx us after Ton + DeadTime - during PWM_On
//  ToCMPxH( TIM1->CCR4H, (hCntDeadDtime+SAMPLING_POINT_DURING_TON_CNT) );
//  ToCMPxL( TIM1->CCR4L, (hCntDeadDtime+SAMPLING_POINT_DURING_TON_CNT) );
}

void DelayCoefAdjust(void)
{
  BEMF_Rising_Factor = g_pBLDC_Struct->pBLDC_Var->bRising_Delay;
  BEMF_Falling_Factor = g_pBLDC_Struct->pBLDC_Var->bFalling_Delay;
}

#ifdef LS_GPIO_CONTROL
void LS_GPIO_MANAGE(void)
{		
  if ((LS_Steps[Current_Step] & LS_A) == 0)
  {
    // Deactivate A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#else
    LS_A_PORT->ODR |= LS_A_PIN;
#endif
  }
  else
  {
    // Activate A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_A_PORT->ODR |= LS_A_PIN;
#else
    LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#endif
  }
  
  if ((LS_Steps[Current_Step] & LS_B) == 0)
  {
    // Deactivate B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#else
    LS_B_PORT->ODR |= LS_B_PIN;
#endif
  }
  else
  {
    // Activate B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_B_PORT->ODR |= LS_B_PIN;
#else
    LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#endif
  }
  
  if ((LS_Steps[Current_Step] & LS_C) == 0)
  {
    // Deactivate C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#else
    LS_C_PORT->ODR |= LS_C_PIN;
#endif
  }
  else
  {
    // Activate C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
    LS_C_PORT->ODR |= LS_C_PIN;
#else
    LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#endif
  }
}

void LS_GPIO_OFF(void)
{
  // Deactivate A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#else
  LS_A_PORT->ODR |= LS_A_PIN;
#endif
  // Deactivate B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#else
  LS_B_PORT->ODR |= LS_B_PIN;
#endif
  // Deactivate C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#else
  LS_C_PORT->ODR |= LS_C_PIN;
#endif
}

void LS_GPIO_BRAKE(void)
{
  // Deactivate A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#else
  LS_A_PORT->ODR |= LS_A_PIN;
#endif
  // Activate B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_B_PORT->ODR |= LS_B_PIN;
#else
  LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#endif
  // Activate C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_C_PORT->ODR |= LS_C_PIN;
#else
  LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#endif
}

void LS_GPIO_BOOT(void)
{
  // Activate A
#if (PWM_U_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_A_PORT->ODR |= LS_A_PIN;
#else
  LS_A_PORT->ODR &= (u8)(~LS_A_PIN);
#endif
  // Activate B
#if (PWM_V_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_B_PORT->ODR |= LS_B_PIN;
#else
  LS_B_PORT->ODR &= (u8)(~LS_B_PIN);
#endif
  // Activate C
#if (PWM_W_LOW_SIDE_POLARITY == ACTIVE_HIGH)
  LS_C_PORT->ODR |= LS_C_PIN;
#else
  LS_C_PORT->ODR &= (u8)(~LS_C_PIN);
#endif
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
