#include "led.h"

// 控制LED相关代码

uint8_t Led_Status;


void Led_Set(_Bool status)
{

    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, (status == LED_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    Led_Status = status;
}
