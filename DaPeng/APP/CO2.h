#ifndef __CO2_H
#define __CO2_H

#include "main.h"
#include "usart.h"


extern uint8_t Usart3_RxPacket[6];				//定义接收数据包数组
extern volatile uint8_t Usart3_RxFlag;
extern uint8_t co2_rx_byte;
extern uint8_t rx_index;

void CO2GetData(uint16_t *data);
void CO2_UART_StartReceive(void);
void CO2_ParseByte(uint8_t b);


#endif
