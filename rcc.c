#include "rcc.h"

void RCC_Init(void)
{
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0UL) { }
    RCC->CFGR &= ~(0x3UL);
}

void RCC_EnableGPIOA(void) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; (void)RCC->AHB1ENR; }
void RCC_EnableGPIOB(void) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; (void)RCC->AHB1ENR; }
void RCC_EnableGPIOC(void) { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; (void)RCC->AHB1ENR; }
void RCC_EnableTIM2(void)  { RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; (void)RCC->APB1ENR; }
void RCC_EnableUSART2(void){ RCC->APB1ENR |= RCC_APB1ENR_USART2EN; (void)RCC->APB1ENR; }
void RCC_EnableI2C1(void)  { RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; (void)RCC->APB1ENR; }
void RCC_EnableSYSCFG(void){ RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; (void)RCC->APB2ENR; }

uint32_t RCC_GetAPB1ClockHz(void)
{
    return RCC_HSI_FREQUENCY_HZ;
}
