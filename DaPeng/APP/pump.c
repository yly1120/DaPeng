#include "pump.h"


// 水泵相关代码

uint8_t PUMP_Status;

void PUMP_Set(_Bool status)
{
    HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, (status == PUMP_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    PUMP_Status = status;
}
