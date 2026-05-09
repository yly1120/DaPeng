#ifndef _ADCX_H_
#define _ADCX_H_


#include <stdint.h>
#include "adc.h"


void ADCx_Init(void);
uint16_t ADC_GetValue(uint32_t channel, uint32_t samplingTime);

#endif
