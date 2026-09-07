#ifndef RELAY_H
#define RELAY_H
#include "stm32f446xx.h"

uint8_t Relay_Init(void);
void Relay_On(void);
void Relay_Off(void);
uint8_t Relay_IsOn(void);

#endif
