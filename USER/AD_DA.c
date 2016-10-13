
#define _AD_DA_VER


#include "Globe.h"







void ADC_Init(void)
{

    ADC2_Init(ADC2_CONVERSIONMODE_CONTINUOUS ,(ADC2_Channel_TypeDef)(ADC2_CHANNEL_4|ADC2_CHANNEL_5), ADC2_PRESSEL_FCPU_D18,\
		ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, ADC2_SCHMITTTRIG_CHANNEL10,DISABLE);
    ADC2_Cmd(DISABLE);
  
}


/**************************************************************************
 * 函数名：MultipleChannelGetADValue
 * 描述  ：打印ADC多通道读取AD值
 * 输入  ：无
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用 
 *************************************************************************/
void MultipleChannelGetADValue(void)
{
 
   // printf("\r\n当前AD Channel is ADC2_CHANNEL_9,Value=%d\r\n",\
            OneChannelGetADValue(ADC2_CHANNEL_9,ADC2_SCHMITTTRIG_CHANNEL9));
     delay_ms(10);
   // printf("\r\n当前AD Channel is ADC2_CHANNEL_10,Value=%d\r\n",\
            OneChannelGetADValue(ADC2_CHANNEL_10,ADC2_SCHMITTTRIG_CHANNEL10));
      delay_ms(10);
    
}
/*******************************************************************************
 * 名称: OneChannelGetADValue
 * 功能: ADC2单通道选择读取AD值
 * 形参: ADC2_Channel,ADC2_SchmittTriggerChannel
 * 返回: 该通道的AD值
 * 说明: 当要读取多通道AD值采调用该函数，否则只是初始化ADC_Init即可 
 ******************************************************************************/
uint16_t OneChannelGetADValue(ADC2_Channel_TypeDef ADC2_Channel,\
                                     ADC2_SchmittTrigg_TypeDef ADC2_SchmittTriggerChannel)
{
    uint16_t ADConversion_Value;
    /**< 连续转换模式 */
    /**< 使能通道 */
    /**< ADC时钟：fADC2 = fcpu/18 */
    /**< 这里设置了从TIM TRGO 启动转换，但实际是没有用到的*/
    /**  不使能 ADC2_ExtTriggerState**/
    /**< 转换数据右对齐 */
    /**< 不使能通道10的斯密特触发器 */
    /**  不使能通道10的斯密特触发器状态 */
    ADC2_Init(ADC2_CONVERSIONMODE_CONTINUOUS , ADC2_Channel, ADC2_PRESSEL_FCPU_D18,\
		ADC2_EXTTRIG_TIM, DISABLE, ADC2_ALIGN_RIGHT, ADC2_SchmittTriggerChannel,DISABLE);
    ADC2_Cmd(ENABLE);
    ADC2_StartConversion();
    ADConversion_Value = ADC2_GetConversionValue();
    return ADConversion_Value;
}





