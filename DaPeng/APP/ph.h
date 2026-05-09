#ifndef __PH_H
#define	__PH_H

#include "adcx.h"
#include "math.h"


#define PH_READ_TIMES	10  //PH传感器ADC循环读取次数


// PA4 = ADC1_IN4
#define PH_ADC_CHANNEL    ADC_CHANNEL_4
#define PH_ADC_SAMPLETIME ADC_SAMPLETIME_55CYCLES_5


float PH_GetData(void);


#endif /* __PH_H */
