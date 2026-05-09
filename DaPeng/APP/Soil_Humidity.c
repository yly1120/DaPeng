#include "Soil_Humidity.h"

// 土壤湿度相关代码


static uint16_t Soil_Humidity_ADC_Read(void)
{
    return ADC_GetValue(Soil_Humidity_ADC_CHANNEL, Soil_Humidity_ADC_SAMPLETIME);
}

uint16_t Soil_Humidity_GetRaw(void)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < Soil_Humidity_READ_TIMES; i++)
    {
        sum += Soil_Humidity_ADC_Read();
        HAL_Delay(5);
    }
    return (uint16_t)(sum / Soil_Humidity_READ_TIMES);
}


uint16_t Soil_Humidity_GetData(void)
{
    int32_t raw = Soil_Humidity_GetRaw();
    int32_t percent;
    int32_t span = (int32_t)SOIL_ADC_DRY - (int32_t)SOIL_ADC_WET;

    if (span <= 0) return 0;  // 防止配置错误

    // 原始值夹紧到校准区间
    if (raw < SOIL_ADC_WET) raw = SOIL_ADC_WET;
    if (raw > SOIL_ADC_DRY) raw = SOIL_ADC_DRY;

    // 干->0%，湿->100%
    percent = ((int32_t)SOIL_ADC_DRY - raw) * 100 / span;

    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    return (uint16_t)percent;
}



