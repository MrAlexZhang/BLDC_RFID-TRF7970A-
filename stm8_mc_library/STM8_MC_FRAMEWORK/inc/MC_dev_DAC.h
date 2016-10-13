#ifndef __MC_DEV_DAC_H_
#define __MC_DEV_DAC_H_

#include "dev_type.h"

#define DAC_CH_1 0
#define DAC_CH_2 1

void dev_DACInit(void);
void dev_DACUpdateValues(u8 bVariable, u8 hValue);

#endif // __MC_DEV_DAC_H_