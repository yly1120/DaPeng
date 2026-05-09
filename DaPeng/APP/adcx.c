#include "adcx.h"


// ADC通道相关代码


/**
  * @brief  ADC初始化程序
  * @param  
  * @retval 
  */
void ADCx_Init(void)
{
	if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
        Error_Handler();
}

/**
  * @brief  获取ADC转换后的数据
  * @param  channel 	选择需要采集的ADC通道
  * @param  samplingTime  选择需要采样时间
  * @retval 返回转换后的模拟信号数值
  */
uint16_t ADC_GetValue(uint32_t channel, uint32_t samplingTime)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    sConfig.Channel      = channel;                 // 例如 ADC_CHANNEL_2 (PA2)
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = samplingTime;            // 例如 ADC_SAMPLETIME_55CYCLES_5
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
        Error_Handler();

    if (HAL_ADC_Start(&hadc1) != HAL_OK) Error_Handler();
    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK)
    {
        HAL_ADC_Stop(&hadc1);
        Error_Handler();
    }

    uint16_t value = (uint16_t)HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return value;
}
