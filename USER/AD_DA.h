#ifndef __AD_DA_H
#define __AD_DA_H


extern u16 Conversion_open_Value1;
extern u16 Conversion_open_Value2;
extern u16 Conversion_open_Value3;

extern u16 Conversion_ward_Value1;
extern u16 Conversion_ward_Value2;
extern u16 Conversion_ward_Value3;

extern u16 SetDAC_open_Value1;
extern u16 SetDAC_open_Value2;
extern u16 SetDAC_open_Value3;

extern u16 SetDAC_ward_Value1;
extern u16 SetDAC_ward_Value2;
extern u16 SetDAC_ward_Value3;

extern u16 trackDAC1_voltage;
extern u16 trackDAC2_voltage;
extern u16 trackDAC3_voltage;

extern u8 write_eeprom_ConversionValue[6];
extern u8  read_eeprom_ConversionValue[6];////将存入到STM8S105的EEPROM数据值读取出来
extern u16 voltage_Value[];
extern u8 track_DAC;
extern u32 DAC_ADJUST_DELAY;
extern u8 DAC_save_size;



void ADC_Init(void);
void Send_ADC_Value(u16 AD_Value);
void MultipleChannelGetADValue(void);
u16 OneChannelGetADValue(ADC2_Channel_TypeDef ADC2_Channel,ADC2_SchmittTrigg_TypeDef ADC2_SchmittTriggerChannel);
#endif