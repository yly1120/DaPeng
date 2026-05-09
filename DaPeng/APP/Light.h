#ifndef __LIGHT_H
#define	__LIGHT_H


#include "main.h"
#include "adcx.h"
#include "math.h"


#define LDR_READ_TIMES	10  //光照传感器ADC循环读取次数

#define LIGHT_ADC_CHANNEL    ADC_CHANNEL_1
#define LIGHT_ADC_SAMPLETIME ADC_SAMPLETIME_55CYCLES_5



uint16_t Light_Average_Data(void);

uint16_t Light_LuxData(void);


#endif /* __ADC_H */

