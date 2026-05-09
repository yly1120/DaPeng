#ifndef _SOIL_HUMIDITY_H
#define	_SOIL_HUMIDITY_H

#include "main.h"
#include "adcx.h"

#include "math.h"

#define SOIL_ADC_WET 1650   // 湿土ADC（按实测调整）
#define SOIL_ADC_DRY 4015   // 干土ADC（按实测调整）
#define Soil_Humidity_READ_TIMES	10  //土壤湿度ADC循环读取次数

#define Soil_Humidity_ADC_CHANNEL    ADC_CHANNEL_2
#define Soil_Humidity_ADC_SAMPLETIME ADC_SAMPLETIME_55CYCLES_5

uint16_t Soil_Humidity_GetRaw(void);
uint16_t Soil_Humidity_GetRaw(void);
uint16_t Soil_Humidity_GetData(void);


#endif /* __ADC_H */

