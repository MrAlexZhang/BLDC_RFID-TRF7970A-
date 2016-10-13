/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MC_stm8s_ACIM_Drive.c
* Author             : IMS Systems Lab 
* Date First Issued  : mm/dd/yyy
* Description        : low level ACIM drive implementation module
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
#include "stm8s_lib.h"
#include "MC_dev_drive.h"

#include "MC_ACIM_Drive_Param.h"
#include "MC_PowerStage_Param.h"
#include "MC_ControlStage_Param.h" 
#include "MC_stm8s_ACIM_param.h"
#include "MC_ACIM_Motor.h"
#include "MC_stm8s_ACIM_conf.h"
#include "MC_Faults.h"
#include "MC_dev_DAC.h"

//not to be modified
#define BUSCONVERSION_REPRATE (u16)(PWM_FREQUENCY/BUS_SAMPLING_FREQ)
#define HEATSINK_REPRATE (u16)(PWM_FREQUENCY/HEATSINK_SAMPLING_FREQ)
#define USERADC_REPRATE (u16)(PWM_FREQUENCY/USERADC_SAMPLING_FREQ)

#define MAX_BUS_VOLTAGE8 (u8)((MAX_BUS_VOLTAGE*256.0)/BUSV_CONVERSION)
#define MIN_BUS_VOLTAGE8 (u8)((MIN_BUS_VOLTAGE*256.0)/BUSV_CONVERSION)
#define BUS_VOLTAGE_VALUE8 (u8)((BUS_VOLTAGE_VALUE*256.0)/BUSV_CONVERSION)
#define BRAKE_HYSTERESIS8 (u8)((((MAX_BUS_VOLTAGE*256.0)/BUSV_CONVERSION)/16.0)*15.0)

#define MAX_HEATSINK_TEMP8 (u8)(NTC_THRESHOLD/4)
#define MIN_HEATSINK_TEMP8 (u8)(NTC_HYSTERIS/4)

#define HEAT_SINK_TEMPERATURE_TEMP8 (u8)(((TEMP_SENS_ALPHA * (HEAT_SINK_TEMPERATURE_VALUE - TEMP_SENS_T0)) + TEMP_SENS_BETA)/4)

#define ToCMPxH(CMP,Value)         ( CMP = (u8)((Value >> 8 ) & 0xFF))
#define ToCMPxL(CMP,Value)         ( CMP = (u8)(Value & 0xFF) )

typedef enum
{ADC_BUS,ADC_HEATSINK,ADC_USER1} ADCState_t;

struct ADC_Var_t
{
  ADCState_t Signal;
  u8 Channel;
  u8 Conversion;
  struct ADC_Var_t *NextSignal;
};

static @near struct ADC_Var_t BusNode = {ADC_BUS,BUS_ADC_CHANNEL,0,&BusNode};
static @near struct ADC_Var_t HeatNode = {ADC_HEATSINK,HEATSINK_ADC_CHANNEL,0,&BusNode};
static @near struct ADC_Var_t UserNode = {ADC_USER1,USER1_ADC_CHANNEL,0,&BusNode};
static @near struct ADC_Var_t *pADCNode = &BusNode;

static u16 hBusRepRate = BUSCONVERSION_REPRATE;
static u16 hHeatSinkRepRate = HEATSINK_REPRATE;
static u16 hUserADCRepRate = USERADC_REPRATE;

static pu8 pModulationIndex;
static pu16 pFreq;
static pu16 pBusVoltage;
static pu8 pHeatsinkTemp;
static pu16 pReg16;

static @near u8 bBusVolt_Buffer[BUSVOLT_BUFFER_SIZE];
static u8 bBusVolt_Buffer_Index = 0;

static PACIM_Struct_t pACIM_Motor;
vu8 SineMag = 0;
vu16 SineFreq = 0;
vu16 Phase = 0;
vu8 bOffset;
vu8 bBusVoltage;
vu16 hSineMag16;
vu8 bSineMag8;
u8 bBusTemp;
static vu8 bRepCnt;
const vs8 PhaseShiftB = 85;
const vs8 PhaseShiftC = 86;

/*private prototypes ******************************************************/
void stm8_TIM1_ADC2_Init(pvdev_device_t pDevice);

/*exported functions*/
void dev_driveInit(pvdev_device_t pDevice)
{
  pACIM_Motor = Get_ACIM_Struct();

  pModulationIndex = pDevice->regs.r8+VDEV_REG8_ACIM_MODULATION_INDEX;
  pFreq = pDevice->regs.r16+VDEV_REG16_ACIM_FREQUENCY;
  pBusVoltage =  pDevice->regs.r16 + VDEV_REG16_BOARD_BUS_VOLTAGE;
  pHeatsinkTemp = pDevice->regs.r8 + VDEV_REG8_HEATSINK_TEMPERATURE;
  pReg16 = pDevice->regs.r16;
  
#ifdef TACHO
  dev_tachoInit(pDevice);
  dev_tachoEnable();
#endif
  
  stm8_TIM1_ADC2_Init(pDevice);
}

MC_FuncRetVal_t dev_driveStartUpInit(void)
{
  Phase = STARTUP_ANGLE;
  SineMag = 0;
  SineFreq = 0;
  
  TIM1_CtrlPWMOutputs(ENABLE);
  return FUNCTION_ENDED;
}

MC_FuncRetVal_t dev_driveRun(void)
{
  u16 hSineMagtemp;
  u16 hSineFreqtemp;
  
#ifdef DEAD_TIME_COMPENSATION  
  hSineMagtemp = (((u16)(*pModulationIndex))*
                  pACIM_Motor->pACIM_Const->bPWM_Timer_MMI)/256 + 
                  pACIM_Motor->pACIM_Const->bPWM_DeadTime/
                  ((pACIM_Motor->pACIM_Const->hPWM_Prescaler+1)*2) + 1;
#else
  hSineMagtemp = (((u16)(*pModulationIndex))*
                  pACIM_Motor->pACIM_Const->bPWM_Timer_MMI)/256;
#endif
  
  if (hSineMagtemp > pACIM_Motor->pACIM_Const->bPWM_Timer_MMI)
  {
    hSineMagtemp = pACIM_Motor->pACIM_Const->bPWM_Timer_MMI;
  }  
  
  hSineFreqtemp = (s16)(((pACIM_Motor->pACIM_Const->hHz_to_DPP_Conv *
                          (u32)(*pFreq))/256)*pACIM_Motor->pACIM_Var->bDirection);
  
  disableInterrupts();
  SineMag = (u8)hSineMagtemp;
  SineFreq = hSineFreqtemp;
  enableInterrupts();
  
  return FUNCTION_ENDED;
}

MC_FuncRetVal_t dev_driveStop(void)
{
  TIM1_CtrlPWMOutputs(DISABLE);
  SineMag = 0;
  SineFreq = 0;
  return FUNCTION_ENDED;  
}

MC_FuncRetVal_t dev_driveStartUp(void){return FUNCTION_ENDED;}

MC_FuncRetVal_t dev_driveWait(void){return FUNCTION_ENDED;}

void stm8_TIM1_ADC2_Init(pvdev_device_t pDevice)
{  
  /******************  TIM1 configuration **************************************/
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1,ENABLE);
  TIM1_DeInit();  
  
  *(pDevice->regs.r8+VDEV_REG8_ACIM_MAX_MODULATION_INDEX) = pACIM_Motor->pACIM_Const->bPWM_Timer_MMI;
  
  //Initializes the Advanced timer TIM1 according to the specified parameters:
  //prescaler, counter mode, period, repetition counter(update rate of the
  //compare registers
  TIM1_TimeBaseInit(pACIM_Motor->pACIM_Const->hPWM_Prescaler,
                    TIM1_COUNTERMODE_CENTERALIGNED1,
                    pACIM_Motor->pACIM_Const->hPWM_Timer_ARR,
                    (u8)(pACIM_Motor->pACIM_Const->bPWM_RefreshRate*2-1));
  
  //Repetition counter alias
  bRepCnt = (u8)((pACIM_Motor->pACIM_Const->bPWM_RefreshRate*2-1) + 2);
  
  //PWM offset duty cycle
  bOffset = pACIM_Motor->pACIM_Const->bPWM_Timer_MMI;
  
  // Enables TIM1 peripheral Preload register on ARR
  TIM1_ARRPreloadConfig(ENABLE);
  
  // Source of Update Request is set to be only counter overflow/underflow
  TIM1_UpdateRequestConfig(TIM1_UPDATESOURCE_REGULAR);

  //Advanced Timer output compare init:  
  TIM1_OC1Init(TIM1_OCMODE_PWM1,TIM1_OUTPUTSTATE_ENABLE,
               DEV_OUTPUTNSTATE,
               (u16)(pACIM_Motor->pACIM_Const->bPWM_Timer_MMI),
               DEV_PWM_U_HIGH_SIDE_POLARITY,DEV_PWM_U_LOW_SIDE_POLARITY,
               DEV_PWM_U_HIGH_SIDE_IDLE_STATE,DEV_PWM_U_LOW_SIDE_IDLE_STATE);
  TIM1_OC2Init(TIM1_OCMODE_PWM1,TIM1_OUTPUTSTATE_ENABLE,
               DEV_OUTPUTNSTATE,
               (u16)(pACIM_Motor->pACIM_Const->bPWM_Timer_MMI),
               DEV_PWM_V_HIGH_SIDE_POLARITY,DEV_PWM_V_LOW_SIDE_POLARITY,
               DEV_PWM_V_HIGH_SIDE_IDLE_STATE,DEV_PWM_V_LOW_SIDE_IDLE_STATE);
  TIM1_OC3Init(TIM1_OCMODE_PWM1,TIM1_OUTPUTSTATE_ENABLE,
               DEV_OUTPUTNSTATE,
               (u16)(pACIM_Motor->pACIM_Const->bPWM_Timer_MMI),
               DEV_PWM_W_HIGH_SIDE_POLARITY,DEV_PWM_W_LOW_SIDE_POLARITY,
               DEV_PWM_W_HIGH_SIDE_IDLE_STATE,DEV_PWM_W_LOW_SIDE_IDLE_STATE);
  
  TIM1_OC4Init(TIM1_OCMODE_PWM1,TIM1_OUTPUTSTATE_DISABLE,
               (pACIM_Motor->pACIM_Const->hPWM_Timer_ARR)-1,
               TIM1_OCPOLARITY_HIGH,
               TIM1_OCIDLESTATE_RESET);

  // Interrupt request priority on counter overflow and break event
  //ITC_SetSoftwarePriority(ITC_IRQ_TIM1_OVF,ITC_PRIORITYLEVEL_3);
  ITC->ISPR3 |= 0xC0;
  ITC->ISPR3 &= 0xFF;
  
  //TIM1 BDTR config: 
  //OCi outputs forced with their idle level as soon as OCi output is enabled,
  //Lock_level 2, deadtime, break enabled, break polarity, automatic output
  //disabled (Main Output Enable can be set only by software)
  TIM1_BDTRConfig(TIM1_OSSISTATE_ENABLE,TIM1_LOCKLEVEL_2,
                  pACIM_Motor->pACIM_Const->bPWM_DeadTime,
                  TIM1_BREAK_STATUS,DEV_BKIN_POLARITY,
                  TIM1_AUTOMATICOUTPUT_DISABLE);
  
  //Enables the TIM1 peripheral
  TIM1_Cmd(ENABLE);
	
  //Timer Re-synch
  TIM1->EGR = 0x01;

  //TIM1_SelectOutputTrigger(TIM1_TRGOSOURCE_OC4REF);
  TIM1->CR2 = (u8)((TIM1->CR2 & (u8)(~TIM1_CR2_MMS)) | (u8) ((u8)0x70));
	
    // Interrupt request is set on break event and counter overflow
  TIM1_ITConfig(TIM1_IT_UPDATE|TIM1_IT_BREAK, ENABLE);
  
  /******************  ADC2 configuration **************************************/
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC,ENABLE);
  ADC2_DeInit();
  
  //BUS voltage measurement port initialization  
  GPIO_Init(BUS_ADC_PORT, BUS_ADC_PIN, GPIO_MODE_IN_FL_NO_IT);
  
  //heatsink temperature measurement port initialization  
  GPIO_Init(HEATSINK_ADC_PORT,HEATSINK_ADC_PIN, GPIO_MODE_IN_FL_NO_IT);
  
  //user1 (potentiometer) measurement port initialization  
  GPIO_Init(USER1_ADC_PORT,USER1_ADC_PIN, GPIO_MODE_IN_FL_NO_IT);

  //Disable Schmitt trigger functionality for selected ADC channels
  {
    u16 ADC_TDR_tmp = 0;
    ADC_TDR_tmp |= (u16)(1) << BUS_ADC_CHANNEL;
    ADC_TDR_tmp |= (u16)(1) << HEATSINK_ADC_CHANNEL;
    ADC_TDR_tmp |= (u16)(1) << USER1_ADC_CHANNEL;
    
    ToCMPxH( ADC2->TDRH, ADC_TDR_tmp);
    ToCMPxL( ADC2->TDRL, ADC_TDR_tmp);
  }
  
  //fADC = fMaster/4
  ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);  
    
  ADC2_ConversionConfig(ADC2_CONVERSIONMODE_SINGLE,BUS_ADC_CHANNEL,ADC2_ALIGN_LEFT);

  ADC2_Cmd(ENABLE);

  ADC2_ExternalTriggerConfig(ADC2_EXTTRIG_TIM,ENABLE);  
  
  //same software priority as TachoTimer Ovf handler and TIM4 handler, but ADC Hw priority is lower
  //ITC_SetSoftwarePriority(ITC_IRQ_ADC,ITC_PRIORITYLEVEL_3);
  ITC->ISPR6 |= 0x30;
  ITC->ISPR6 &= 0xFF;
  
  ADC2_ITConfig(ENABLE);  
}

@near @interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
  it is recommended to set a breakpoint on the following instruction.
  */

//if (TIM1_GetITStatus(TIM1_IT_UPDATE)==SET)
if ((TIM1->SR1 & (u8)TIM1_IT_UPDATE) != 0)
{
  u8 bAsynchStart = 0;
  
  //Resynch Repetition counter alias
  bRepCnt = (u8)((pACIM_Motor->pACIM_Const->bPWM_RefreshRate*2-1) + 2);
  
  //ADC manager
  hBusRepRate--;
  hHeatSinkRepRate--;
  hUserADCRepRate--;
  
  //chain linkage
  pADCNode->Signal = ADC_BUS;

  if (hUserADCRepRate == 1)
  {
    pADCNode = &UserNode;
    HeatNode.NextSignal = &UserNode;
    bAsynchStart = 1;
  }
  else
  {
    HeatNode.NextSignal = &BusNode;
  }
  
  if (hHeatSinkRepRate == 1)
  {
    pADCNode = &HeatNode;
    BusNode.NextSignal = &HeatNode; //
    bAsynchStart = 1;
  }
  else
  {
    BusNode.NextSignal = &UserNode; //
  }
  
  //ADC start
  if (bAsynchStart == 1)
  {
    /* Clear the ADC2 channels */
    ADC2->CSR &= (u8)(~ADC2_CSR_CH);
    /* Select the ADC2 channel */
    ADC2->CSR |= (u8)(pADCNode->Channel);
    
    //Start single conversion      
    //ADC2_StartConversion();
    ADC2->CR1 |= ADC2_CR1_ADON;
  }
  
  if (hBusRepRate == 0)
#ifdef BUS_VOLTAGE_MEASUREMENT
  {
    hBusRepRate = BUSCONVERSION_REPRATE;    
    
    //u16 ADC2_GetConversionValue(void) , /* Left alignment */
    /* Read MSB only, so as to have an 8 bit conversion*/ 
    bBusVolt_Buffer[bBusVolt_Buffer_Index] = BusNode.Conversion;
    
    bBusVolt_Buffer_Index++;
    if (bBusVolt_Buffer_Index == BUSVOLT_BUFFER_SIZE) 
    {
      u8 i;
      u16 htemp=0;
      
      bBusVolt_Buffer_Index = 0;
      for (i=0;i<BUSVOLT_BUFFER_SIZE;i++)
      {
        htemp += bBusVolt_Buffer[i];
      }
      
      htemp /= BUSVOLT_BUFFER_SIZE;      
      
      *pBusVoltage = htemp;    	
    }    
  }  
#else
  {
    hBusRepRate = BUSCONVERSION_REPRATE;
    *pBusVoltage = BUS_VOLTAGE_VALUE8;		
  }
#endif
  
  if (hHeatSinkRepRate == 0)
  {
    hHeatSinkRepRate = HEATSINK_REPRATE;
		
#ifdef HEAT_SINK_TEMPERATURE_MEASUREMENT    
    *pHeatsinkTemp = HeatNode.Conversion;
#else
		*pHeatsinkTemp = HEAT_SINK_TEMPERATURE_TEMP8;
#endif		
    
    if (*pHeatsinkTemp > MAX_HEATSINK_TEMP8)
    {
      *(pReg16+VDEV_REG16_HW_ERROR_OCCURRED) |= HEATSINK_TEMPERATURE;
      *(pReg16+VDEV_REG16_HW_ERROR_ACTUAL) |= HEATSINK_TEMPERATURE;      
    }
    else if (*pHeatsinkTemp < MIN_HEATSINK_TEMP8)
    {
      *(pReg16+VDEV_REG16_HW_ERROR_ACTUAL) &= (u8)(~HEATSINK_TEMPERATURE);
    }
  }
  
  if (hUserADCRepRate == 0)
  {
    hUserADCRepRate = USERADC_REPRATE;
    
    pACIM_Motor->pACIM_Var->hUserADC = UserNode.Conversion;
  }  

//TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
TIM1->SR1 = (u8)(~(u8)TIM1_IT_UPDATE);
}
//else if (TIM1_GetITStatus(TIM1_IT_BREAK)==SET)
else if ((TIM1->SR1 & (u8)TIM1_IT_BREAK) != 0)
{
  if ((*(pReg16+VDEV_REG16_HW_ERROR_OCCURRED) & BUS_UNDERVOLTAGE) == 0)
  {
    *(pReg16+VDEV_REG16_HW_ERROR_OCCURRED) |= OVER_CURRENT;  
  }
  
  //TIM1_ClearITPendingBit(TIM1_IT_BREAK);  
  TIM1->SR1 = (u8)(~(u8)TIM1_IT_BREAK);
}

return;
}

/**
  * @brief ADC2 interrupt routine.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  * @par Called functions:
  * None
  */
@near @interrupt void ADC2_IRQHandler (void)
{
  /* In order to detect unexpected events during development,
  it is recommended to set a breakpoint on the following instruction.
  */
	
  //if (TIM1_GetITStatus(TIM1_IT_CC4) !=0)
  if ((TIM1->SR1 & (u8)TIM1_IT_CC4) != 0)
  {
    //TIM1_ClearFlag(TIM1_FLAG_CC4);
    TIM1->SR1 = (u8)(~(u8)(TIM1_FLAG_CC4)); //overcapture flag is not cleared
    
    if (pADCNode->Signal == ADC_BUS)
    {
			
#ifdef BUS_VOLTAGE_MEASUREMENT
bBusVoltage = ADC2->DRH;
#endif
      
      bRepCnt -= 2;
      if (bRepCnt == 1)
      {
				
#if (!defined BUS_VOLTAGE_MEASUREMENT)
      bSineMag8 = SineMag;
#elif defined BUS_VOLTAGE_MEASUREMENT      
      bBusTemp = (u8)(*pBusVoltage);
      // Bus voltage compensation, uncomment the asm code below
      //hSineMag16 = (SineMag * (*pBusVoltage))/bBusVoltage;
      #asm 

				clr a
        ld	a,_SineMag
        ldw	x,_bBusTemp
				swapw x
        mul     x,a
        ld      a,_bBusVoltage
        div     x,a
        ldw     _hSineMag16,x
          

      #endasm
        
      if (hSineMag16 > pACIM_Motor->pACIM_Const->bPWM_Timer_MMI)
      {
        hSineMag16 = pACIM_Motor->pACIM_Const->bPWM_Timer_MMI;
      }        

      bSineMag8 = (u8)(hSineMag16);
      
      // No bus voltage compensation, uncomment the line below
      // bSineMag8 = SineMag;
#endif				

      //3phase sinewaves generation
#asm
	;Phase += SineFreq;
        ldw   Y,_Phase
	addw  Y,_SineFreq
	ldw   _Phase,Y

        ;Y = Phase>>8 
	clr A
	swapw Y
	ld YH,A         	;YL hold LUT pointer

;********************* PHASE U duty cycle processing ***************************
	ld A,(_SINE3RDHARM,Y)	; Get entry from table
	ldw X,_bSineMag8		; Scale According to SineMag
	swapw X
	mul X,A			; 16-bit Result in X
	btjt _Phase,#7,nextU	; Phase represents offset in LUT
				; jmp if entry is positive, first half of table
	cplw X			; Otherwise negate result
	incw X			; Increase MSB if 2_s complement of A is 100
nextU:
	sraw X
	sraw X
	sraw X
	sraw X
	sraw X
	sraw X
	sraw X
          
        swapw X
        ld    A,XH
        add   A,_bOffset
        jrnc  cond1
        incw  X
        cond1: ld   XH,A
        ld    A,XL
        ld    0x5265,A                          ;TIM1->CCR1H
        ld    A,XH
        ld    0x5266,A                          ;TIM1->CCR1L          

;********************* PHASE V duty cycle processing ***************************
	ld A, YL
	add A,_PhaseShiftB
	ld YL, A
	ld A,(_SINE3RDHARM,Y)	; Get entry from table
	ldw X,_bSineMag8		; Scale According to SineMag
	swapw X
	mul X,A			; 16-bit Result in X
	ld A, YL
	tnz A
	jrmi nextV
	cplw X			; Otherwise negate result
	incw X			; Increase MSB if 2_s complement of A is 100
nextV:
	sraw X
	sraw X
	sraw X
	sraw X
	sraw X
	sraw X
	sraw X
          
        swapw X
        ld    A,XH
        add   A,_bOffset
        jrnc  cond2
        incw  X          
        cond2: ld   XH,A
        ld    A,XL
        ld    0x5267,A                          ;TIM2->CCR1H
        ld    A,XH
        ld    0x5268,A                          ;TIM2->CCR1L 

;********************* PHASE W duty cycle processing ***************************
	ld A, YL
	add A,_PhaseShiftC
	ld YL, A
	ld A,(_SINE3RDHARM,Y)	; Get entry from table
	ldw X,_bSineMag8		; Scale According to SineMag
	swapw X
	mul X,A			; 16-bit Result in X
	ld A, YL
	tnz A
	jrmi nextW
	cplw X			; Otherwise negate result
	incw X			; Increase MSB if 2_s complement of A is 100
nextW:
	sraw X
	sraw X
	sraw X
	sraw X
	sraw X
	sraw X
	sraw X
          
        swapw X
        ld    A,XH
        add   A,_bOffset
        jrnc  cond3
        incw  X
        cond3: ld   XH,A
        ld    A,XL
        ld    0x5269,A                          ;TIM3->CCR1H
        ld    A,XH
        ld    0x526A,A                          ;TIM3->CCR1L         

#endasm
      }
      
    //error check
#ifdef BUS_VOLTAGE_MEASUREMENT
      
    pADCNode->Conversion = bBusVoltage;
    if (bBusVoltage > MAX_BUS_VOLTAGE8)
    {
#ifdef DISSIPATIVE_BRAKE
#if (DISSIPATIVE_BRAKE_POL == DISSIPATIVE_BRAKE_ACTIVE_HIGH)
      DISSIPATIVE_BRAKE_PORT->ODR |= DISSIPATIVE_BRAKE_BIT;
#else
      DISSIPATIVE_BRAKE_PORT->ODR &= (u8)(~DISSIPATIVE_BRAKE_BIT);
#endif
#else			
      *(pReg16+VDEV_REG16_HW_ERROR_OCCURRED) |= BUS_OVERVOLTAGE;
      *(pReg16+VDEV_REG16_HW_ERROR_ACTUAL) |= BUS_OVERVOLTAGE;
#endif
    }
#ifdef DISSIPATIVE_BRAKE    
    else if (bBusVoltage < BRAKE_HYSTERESIS8)
    {
#if (DISSIPATIVE_BRAKE_POL == DISSIPATIVE_BRAKE_ACTIVE_HIGH)
      DISSIPATIVE_BRAKE_PORT->ODR &= (u8)(~DISSIPATIVE_BRAKE_BIT);
#else
      DISSIPATIVE_BRAKE_PORT->ODR |= DISSIPATIVE_BRAKE_BIT;
#endif
    }
#else
    else      
    {
      *(pReg16+VDEV_REG16_HW_ERROR_ACTUAL) &= (u8)(~BUS_OVERVOLTAGE);
    }
#endif
    if (bBusVoltage < MIN_BUS_VOLTAGE8)
    {
      *(pReg16+VDEV_REG16_HW_ERROR_OCCURRED) |= BUS_UNDERVOLTAGE;
      *(pReg16+VDEV_REG16_HW_ERROR_ACTUAL) |= BUS_UNDERVOLTAGE;      
    }
    else
    {
      *(pReg16+VDEV_REG16_HW_ERROR_ACTUAL) &= (u8)(~BUS_UNDERVOLTAGE);
    }
#endif
    
    }
    else
    {
      pADCNode = &BusNode;//error, bad synchronization; re-synchronize
      pADCNode->NextSignal = &BusNode;
      hBusRepRate = BUSCONVERSION_REPRATE;
      hHeatSinkRepRate = HEATSINK_REPRATE;
      hUserADCRepRate = USERADC_REPRATE;			
    }
  }
  else //Asynch ADCs management
  {
    pADCNode->Conversion = ADC2->DRH;
    
    pADCNode = pADCNode->NextSignal;
    
    /* Clear the ADC2 channels */
    ADC2->CSR &= (u8)(~ADC2_CSR_CH);
    /* Select the ADC2 channel */
    ADC2->CSR |= (u8)(pADCNode->Channel);
    
    if (pADCNode->Signal != ADC_BUS)
    {      
      //Start single conversion      
      //ADC2_StartConversion();
      ADC2->CR1 |= ADC2_CR1_ADON;    
    }
    
  }
  
    //ADC2_ClearITPendingBit();
  ADC2->CSR &= (u8)(~ADC2_CSR_EOC);

  return;
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
