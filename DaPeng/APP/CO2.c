#include "CO2.h"	

// CO2相关代码


uint8_t Usart3_RxPacket[6];				//定义接收数据包数组
volatile uint8_t Usart3_RxFlag;			//定义接收数据包标志位


uint8_t co2_rx_byte;
uint8_t rx_index;


void CO2_ParseByte(uint8_t b)
{
    if (rx_index == 0)
    {
        if (b != 0x2C) return;   // 等待帧头 Addr=0x2C
    }

    Usart3_RxPacket[rx_index++] = b;

    if (rx_index >= 6)
    {
        uint8_t sum = (uint8_t)(Usart3_RxPacket[0] +
                                Usart3_RxPacket[1] +
                                Usart3_RxPacket[2] +
                                Usart3_RxPacket[3] +
                                Usart3_RxPacket[4]);

        if (sum == Usart3_RxPacket[5])
        {
            Usart3_RxFlag = 1;   // 收到一帧有效数据
        }

        rx_index = 0;            // 准备收下一帧
    }
}

/* 在 main 初始化完 USART3 后调用一次 */
void CO2_UART_StartReceive(void)
{
    Usart3_RxFlag = 0;
    rx_index = 0;
    HAL_UART_Receive_IT(&huart3, &co2_rx_byte, 1);
}


/* HAL 收到 1 字节后的回调：解析 + 继续挂下一字节 */
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
    
// }

/*		防卡死程序		*/
/*
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        __HAL_UART_CLEAR_OREFLAG(huart);
        HAL_UART_Receive_IT(huart, &co2_rx_byte, 1);
    }
}
*/

void CO2GetData(uint16_t *data)
{
    if (Usart3_RxFlag)
    {
        Usart3_RxFlag = 0;
        *data = ((uint16_t)Usart3_RxPacket[1] << 8) | Usart3_RxPacket[2];
    }
}

