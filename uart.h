#ifndef UART_H
#define UART_H
#include "stm32f446xx.h"

#define UART_DEFAULT_BAUD       (115200UL)

uint8_t UART2_Init(uint32_t baudrate);
void UART2_SendChar(char ch);
void UART2_SendString(const char *str);
uint8_t UART2_ReceiveChar(char *ch, uint32_t timeout_us);
void UART2_SendUint32(uint32_t value);

#endif
