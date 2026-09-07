#include "led.h"
#include "rcc.h"
#include "gpio.h"

typedef struct { GPIO_TypeDef *port; uint8_t pin; } LED_Pin_t;

static const LED_Pin_t led_pins[4U] = {
    {GPIOA, GPIO_PIN_5}, {GPIOA, GPIO_PIN_6}, {GPIOA, GPIO_PIN_7}, {GPIOB, GPIO_PIN_6}
};

uint8_t LED_Init(void)
{
    uint8_t i;
    GPIO_Config_t cfg;

    RCC_EnableGPIOA();
    RCC_EnableGPIOB();

    for (i = 0U; i < 4U; ++i) {
        cfg.port = led_pins[i].port; cfg.pin = led_pins[i].pin;
        cfg.mode = GPIO_OUTPUT; cfg.output_type = GPIO_PUSH_PULL;
        cfg.pull = GPIO_NO_PULL; cfg.speed = GPIO_SPEED_LOW; cfg.alternate_function = 0U;
        if (GPIO_InitPin(&cfg) != DRIVER_OK) return DRIVER_ERROR;
    }
    LED_AllOff();
    return DRIVER_OK;
}

void LED_On(LED_Color_t color)
{
    if (color <= LED_RED) GPIO_WritePin(led_pins[color].port, led_pins[color].pin, GPIO_HIGH);
}

void LED_Off(LED_Color_t color)
{
    if (color <= LED_RED) GPIO_WritePin(led_pins[color].port, led_pins[color].pin, GPIO_LOW);
}

void LED_AllOff(void)
{
    uint8_t i;
    for (i = 0U; i < 4U; ++i) LED_Off((LED_Color_t)i);
}
