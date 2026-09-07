#include "relay.h"
#include "rcc.h"
#include "gpio.h"

static uint8_t relay_state = 0U;

uint8_t Relay_Init(void)
{
    GPIO_Config_t cfg = {GPIOB, GPIO_PIN_0, GPIO_OUTPUT, GPIO_PUSH_PULL, GPIO_NO_PULL, GPIO_SPEED_LOW, 0U};
    RCC_EnableGPIOB();
    relay_state = 0U;
    if (GPIO_InitPin(&cfg) != DRIVER_OK) return DRIVER_ERROR;
    Relay_Off();
    return DRIVER_OK;
}

void Relay_On(void)  { GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_HIGH); relay_state = 1U; }
void Relay_Off(void) { GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_LOW); relay_state = 0U; }
uint8_t Relay_IsOn(void) { return relay_state; }
