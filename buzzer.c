#include "buzzer.h"
#include "rcc.h"
#include "gpio.h"
#include "tim2.h"

uint8_t Buzzer_Init(void)
{
    GPIO_Config_t cfg = {GPIOB, GPIO_PIN_1, GPIO_OUTPUT, GPIO_PUSH_PULL, GPIO_NO_PULL, GPIO_SPEED_LOW, 0U};
    RCC_EnableGPIOB();
    if (GPIO_InitPin(&cfg) != DRIVER_OK) return DRIVER_ERROR;
    Buzzer_Off();
    return DRIVER_OK;
}

void Buzzer_On(void)  { GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_HIGH); }
void Buzzer_Off(void) { GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_LOW); }

void Buzzer_Beep(uint32_t duration_ms)
{
    Buzzer_On();
    TIM2_DelayMs(duration_ms);
    Buzzer_Off();
}
