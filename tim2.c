#include "tim2.h"
#include "rcc.h"

#define TIM2_PSC_VALUE          (15UL)
#define TIM2_ARR_VALUE          (0xFFFFFFFFUL)

void TIM2_Init(void)
{
    RCC_EnableTIM2();
    TIM2->CR1 = 0UL;
    TIM2->PSC = TIM2_PSC_VALUE;
    TIM2->ARR = TIM2_ARR_VALUE;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = 0UL;
    TIM2->CR1 = TIM_CR1_CEN;
}

void TIM2_Start(void) { TIM2->CR1 |= TIM_CR1_CEN; }
void TIM2_Stop(void)  { TIM2->CR1 &= ~TIM_CR1_CEN; }
void TIM2_ResetCounter(void) { TIM2->CNT = 0UL; }
uint32_t TIM2_GetCounterUs(void) { return TIM2->CNT; }

void TIM2_DelayUs(uint32_t us)
{
    uint32_t start = TIM2->CNT;
    while ((uint32_t)(TIM2->CNT - start) < us) { }
}

void TIM2_DelayMs(uint32_t ms)
{
    uint32_t i;
    for (i = 0UL; i < ms; ++i) {
        TIM2_DelayUs(1000UL);
    }
}

void TIM2_EnableInputCaptureCH1(void)
{
    TIM2->CCMR1 &= ~(3UL);
    TIM2->CCMR1 |= TIM_CCMR1_CC1S_TI1;
    TIM2->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1P);
    TIM2->CCER |= TIM_CCER_CC1E;
}

uint32_t TIM2_GetInputCaptureUs(void)
{
    return TIM2->CCR1;
}

void TIM2_IRQHandler_Callback(void)
{
    /* Reserved for future timer interrupt use. */
}
