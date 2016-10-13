
#define _AD_DA_VER


#include "Globe.h"







void ADC_Init(void)
{

    ADC2_Init(ADC2_CONVERSIONMODE_CONTINUOUS ,(ADC2_Channel_TypeDef)(ADC2_CHANNEL_4|ADC2_CHANNEL_5), ADC2_PRESSEL_FCPU_D18,\
		ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, ADC2_SCHMITTTRIG_CHANNEL10,DISABLE);
    ADC2_Cmd(DISABLE);
  
}


/**************************************************************************
 * ��������MultipleChannelGetADValue
 * ����  ����ӡADC��ͨ����ȡADֵ
 * ����  ����
 *
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ���� 
 *************************************************************************/
void MultipleChannelGetADValue(void)
{
 
   // printf("\r\n��ǰAD Channel is ADC2_CHANNEL_9,Value=%d\r\n",\
            OneChannelGetADValue(ADC2_CHANNEL_9,ADC2_SCHMITTTRIG_CHANNEL9));
     delay_ms(10);
   // printf("\r\n��ǰAD Channel is ADC2_CHANNEL_10,Value=%d\r\n",\
            OneChannelGetADValue(ADC2_CHANNEL_10,ADC2_SCHMITTTRIG_CHANNEL10));
      delay_ms(10);
    
}
/*******************************************************************************
 * ����: OneChannelGetADValue
 * ����: ADC2��ͨ��ѡ���ȡADֵ
 * �β�: ADC2_Channel,ADC2_SchmittTriggerChannel
 * ����: ��ͨ����ADֵ
 * ˵��: ��Ҫ��ȡ��ͨ��ADֵ�ɵ��øú���������ֻ�ǳ�ʼ��ADC_Init���� 
 ******************************************************************************/
uint16_t OneChannelGetADValue(ADC2_Channel_TypeDef ADC2_Channel,\
                                     ADC2_SchmittTrigg_TypeDef ADC2_SchmittTriggerChannel)
{
    uint16_t ADConversion_Value;
    /**< ����ת��ģʽ */
    /**< ʹ��ͨ�� */
    /**< ADCʱ�ӣ�fADC2 = fcpu/18 */
    /**< ���������˴�TIM TRGO ����ת������ʵ����û���õ���*/
    /**  ��ʹ�� ADC2_ExtTriggerState**/
    /**< ת�������Ҷ��� */
    /**< ��ʹ��ͨ��10��˹���ش����� */
    /**  ��ʹ��ͨ��10��˹���ش�����״̬ */
    ADC2_Init(ADC2_CONVERSIONMODE_CONTINUOUS , ADC2_Channel, ADC2_PRESSEL_FCPU_D18,\
		ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, ADC2_SchmittTriggerChannel,DISABLE);
    ADC2_Cmd(ENABLE);
    ADC2_StartConversion();
    ADConversion_Value = ADC2_GetConversionValue();
    return ADConversion_Value;
}





