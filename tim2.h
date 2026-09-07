#ifndef TIM2_DRIVER_H
#define TIM2_DRIVER_H
#include "stm32f446xx.h"

#define TIM2_TICK_HZ             (1000000UL)
#define TIM2_MAX_COUNT_US        (0xFFFFFFFFUL)

typedef void (*TIM2_Callback_t)(void);

void TIM2_Init(void);
void TIM2_Start(void);
void TIM2_Stop(void);
void TIM2_ResetCounter(void);
uint32_t TIM2_GetCounterUs(void);
void TIM2_DelayUs(uint32_t us);
void TIM2_DelayMs(uint32_t ms);
uint32_t TIM2_GetInputCaptureUs(void);
void TIM2_EnableInputCaptureCH1(void);
void TIM2_IRQHandler_Callback(void);

#endif
