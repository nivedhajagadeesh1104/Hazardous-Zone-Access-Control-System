#include "hc_sr04.h"
#include "rcc.h"
#include "gpio.h"
#include "tim2.h"

uint8_t HCSR04_Init(void)
{
    GPIO_Config_t trig = {GPIOA, GPIO_PIN_0, GPIO_OUTPUT, GPIO_PUSH_PULL, GPIO_NO_PULL, GPIO_SPEED_HIGH, 0U};
    GPIO_Config_t echo = {GPIOA, GPIO_PIN_1, GPIO_INPUT, GPIO_PUSH_PULL, GPIO_NO_PULL, GPIO_SPEED_HIGH, 0U};

    RCC_EnableGPIOA();

    if ((GPIO_InitPin(&trig) != DRIVER_OK) || (GPIO_InitPin(&echo) != DRIVER_OK)) {
        return DRIVER_ERROR;
    }
    GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_LOW);
    return DRIVER_OK;
}

uint8_t HCSR04_MeasureDistanceCm(uint16_t *distance_cm)
{
    uint32_t start;
    uint32_t pulse_start;
    uint32_t pulse_width;

    if (distance_cm == (void *)0) {
        return DRIVER_ERROR;
    }

    GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_LOW);
    TIM2_DelayUs(2UL);
    GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_HIGH);
    TIM2_DelayUs(10UL);
    GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_LOW);

    start = TIM2_GetCounterUs();
    while (GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_LOW) {
        if ((uint32_t)(TIM2_GetCounterUs() - start) >= HCSR04_TIMEOUT_US) {
            return DRIVER_ERROR;
        }
    }

    pulse_start = TIM2_GetCounterUs();
    while (GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_HIGH) {
        if ((uint32_t)(TIM2_GetCounterUs() - pulse_start) >= HCSR04_TIMEOUT_US) {
            return DRIVER_ERROR;
        }
    }

    pulse_width = (uint32_t)(TIM2_GetCounterUs() - pulse_start);

    /* Speed of sound: approximately 343 m/s.
       Distance cm ~= pulse_width_us / 58. */
    *distance_cm = (uint16_t)(pulse_width / 58UL);
    return DRIVER_OK;
}

uint8_t HCSR04_IsPersonPresent(uint8_t *present)
{
    uint16_t distance;

    if (present == (void *)0) {
        return DRIVER_ERROR;
    }

    if (HCSR04_MeasureDistanceCm(&distance) != DRIVER_OK) {
        *present = 0U;
        return DRIVER_ERROR;
    }

    *present = (distance > 0U) && (distance <= HCSR04_PRESENCE_DISTANCE_CM) ? 1U : 0U;
    return DRIVER_OK;
}
