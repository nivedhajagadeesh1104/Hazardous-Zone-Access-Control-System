#ifndef RCC_H
#define RCC_H
#include "stm32f446xx.h"

#define RCC_HSI_FREQUENCY_HZ    (16000000UL)

void RCC_Init(void);
void RCC_EnableGPIOA(void);
void RCC_EnableGPIOB(void);
void RCC_EnableGPIOC(void);
void RCC_EnableTIM2(void);
void RCC_EnableUSART2(void);
void RCC_EnableI2C1(void);
void RCC_EnableSYSCFG(void);
uint32_t RCC_GetAPB1ClockHz(void);

#endif
