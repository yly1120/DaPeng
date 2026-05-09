#include "spray.h"

// 水雾相关代码

uint8_t SPRAY_Status;


void SPRAY_Set(_Bool status)
{
    HAL_GPIO_WritePin(SPRAY_GPIO_Port, SPRAY_Pin, (status == SPRAY_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    SPRAY_Status = status;
}
