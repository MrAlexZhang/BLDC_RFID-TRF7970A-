#include "MC_dev_DAC.h"
#include "stm8s_lib.h"

#include "MC_ControlStage_param.h"

#ifdef DAC_FUNCTIONALITY

	u8 hMeasurementArray[2];

	void dev_DACInit(void)
	{
		//counter disabled, disable ARR preload register 
		TIM3->CR1 = 0;

		//prescaler 
		TIM3->PSCR = 0;

		//make sure channels are disabled
		TIM3->CCER1 = 0;

		//select PWM1 mode, enable preload for channel OC1, OC2
		TIM3->CCMR1 = 0x68;
		TIM3->CCMR2 = 0x68;

		//set auto reload value at Max
		TIM3->ARRH = 0x00;
		TIM3->ARRL = 0xFF;

		//counter enabled
		TIM3->CR1 |= 0x01;

		//force update
		TIM3->EGR |= 0x01;

		//enable interrupts
		TIM3->IER |= 0x01;

		// 
		TIM3->CCR1H = 0;
		TIM3->CCR1L = 0;
		TIM3->CCR2H = 0;
		TIM3->CCR2L = 0;

		// Output enable
		TIM3->CCER1 = 0x11;
		
		//ITC_SetSoftwarePriority(ITC_IRQ_TIM3_CAPCOM, ITC_PRIORITYLEVEL_2);
	}

	@near @interrupt void TIM3_UPD_OVF_BRK_IRQHandler (void)
	{
		TIM3->SR1 &= (u8)(~0x01);
		TIM3->CCR1L = hMeasurementArray[DAC_CH_1];
		TIM3->CCR2L = hMeasurementArray[DAC_CH_2];
	}

	void dev_DACUpdateValues(u8 bVariable, u8 hValue)
	{
	  hMeasurementArray[bVariable] = hValue;
	}

#endif