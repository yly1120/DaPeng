#include "fan.h"


// 风扇相关代码

uint8_t FAN_Status;


void FAN_Set(_Bool status)
{

    HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, (status == FAN_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    FAN_Status = status;
}
