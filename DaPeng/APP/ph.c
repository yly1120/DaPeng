#include "ph.h"

// 土壤PH相关代码


uint16_t PH_ADC_Read(void)
{
	//设置指定ADC的规则组通道，采样时间
	return ADC_GetValue(PH_ADC_CHANNEL, PH_ADC_SAMPLETIME);
}

float PH_GetData(void)
{
	float  tempData = 0;
	float PH_DAT = 0;

	for (uint8_t i = 0; i < PH_READ_TIMES; i++)
	{
		tempData += PH_ADC_Read();
		HAL_Delay(5);
	}
	tempData /= PH_READ_TIMES;
	
	float v_adc = (tempData / 4095.0f) * 3.3f;
	PH_DAT = -5.7541f * v_adc + 16.654f;
	
	if(PH_DAT>14.0)PH_DAT=14.0;
	if(PH_DAT<0)PH_DAT=0.0;
	
	return PH_DAT;
}

