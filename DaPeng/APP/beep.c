#include "beep.h"

uint8_t BEEP_Status;


void BEEP_Set(_Bool status)
{
    HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, (status == BEEP_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    BEEP_Status = status;
}
