#ifndef EXTI_H
#define EXTI_H
#include "stm32f446xx.h"

typedef void (*EXTI_Callback_t)(void);
typedef enum { EXTI_TRIGGER_RISING = 0U, EXTI_TRIGGER_FALLING = 1U, EXTI_TRIGGER_BOTH = 2U } EXTI_Trigger_t;

uint8_t EXTI_PC13_Init(EXTI_Trigger_t trigger, EXTI_Callback_t callback);
void EXTI_PC13_ClearPending(void);
void EXTI15_10_IRQHandler(void);

#endif
