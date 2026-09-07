#include "exti.h"
#include "rcc.h"
#include "gpio.h"

static EXTI_Callback_t exti_pc13_callback = (void *)0;

uint8_t EXTI_PC13_Init(EXTI_Trigger_t trigger, EXTI_Callback_t callback)
{
    GPIO_Config_t cfg = {GPIOC, GPIO_PIN_13, GPIO_INPUT, GPIO_PUSH_PULL, GPIO_PULLUP, GPIO_SPEED_LOW, 0U};

    RCC_EnableGPIOC();
    RCC_EnableSYSCFG();

    if (GPIO_InitPin(&cfg) != DRIVER_OK) {
        return DRIVER_ERROR;
    }

    SYSCFG->EXTICR[3] &= ~(0xFUL << 4U);
    SYSCFG->EXTICR[3] |= (SYSCFG_EXTI_PORTC << 4U);

    EXTI->IMR |= EXTI_LINE_13;
    EXTI->EMR &= ~EXTI_LINE_13;

    EXTI->RTSR &= ~EXTI_LINE_13;
    EXTI->FTSR &= ~EXTI_LINE_13;

    if (trigger == EXTI_TRIGGER_RISING) {
        EXTI->RTSR |= EXTI_LINE_13;
    } else if (trigger == EXTI_TRIGGER_FALLING) {
        EXTI->FTSR |= EXTI_LINE_13;
    } else {
        EXTI->RTSR |= EXTI_LINE_13;
        EXTI->FTSR |= EXTI_LINE_13;
    }

    exti_pc13_callback = callback;
    EXTI_PC13_ClearPending();

    NVIC->ISER[NVIC_IRQ_INDEX(EXTI15_10_IRQn)] |= NVIC_IRQ_MASK(EXTI15_10_IRQn);
    return DRIVER_OK;
}

void EXTI_PC13_ClearPending(void)
{
    EXTI->PR = EXTI_LINE_13;
}

void EXTI15_10_IRQHandler(void)
{
    if ((EXTI->PR & EXTI_LINE_13) != 0UL) {
        EXTI_PC13_ClearPending();
        if (exti_pc13_callback != (void *)0) {
            exti_pc13_callback();
        }
    }
}
