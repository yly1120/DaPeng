#include "Light.h"



// 光照强度相关代码

	
static uint16_t Light_ADC_Read(void)
{
    // 读取ADC值并返回
    return ADC_GetValue(LIGHT_ADC_CHANNEL, LIGHT_ADC_SAMPLETIME);
}
	
uint16_t Light_Average_Data(void)
{
    uint32_t tempData = 0;
    for (uint8_t i = 0; i < LDR_READ_TIMES; i++)
    {
        tempData += Light_ADC_Read();
        HAL_Delay(1);
    }

    tempData /= LDR_READ_TIMES;	// 求取平均值
    return (uint16_t)tempData;
}

uint16_t Light_LuxData()
{
	float voltage = 0;	
	float R = 0;	
	uint16_t Lux = 0;
	voltage = (float)Light_Average_Data();
	voltage  = voltage / 4096 * 3.3f;
	
	R = voltage / (3.3f - voltage) * 10000.0f;
	Lux = 40000.0f * pow(R, -0.6021f);
	
	if (Lux > 999) Lux = 999;
	return Lux;
}

