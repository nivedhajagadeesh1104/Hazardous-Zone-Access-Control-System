#include "gpio.h"

static uint32_t GPIO_PinMask(uint8_t pin)
{
    return (1UL << pin);
}

uint8_t GPIO_InitPin(const GPIO_Config_t *config)
{
    uint32_t shift;
    uint32_t mask;

    if ((config == (void *)0) || (config->port == (void *)0) || (config->pin > 15U)) {
        return DRIVER_ERROR;
    }

    shift = (uint32_t)config->pin * 2UL;
    mask = (3UL << shift);
    config->port->MODER = (config->port->MODER & ~mask) | ((uint32_t)config->mode << shift);

    shift = (uint32_t)config->pin * 2UL;
    config->port->PUPDR = (config->port->PUPDR & ~(3UL << shift)) |
                          ((uint32_t)config->pull << shift);

    config->port->OTYPER = (config->port->OTYPER & ~GPIO_PinMask(config->pin)) |
                           ((uint32_t)config->output_type << config->pin);

    config->port->OSPEEDR = (config->port->OSPEEDR & ~(3UL << shift)) |
                            ((uint32_t)config->speed << shift);

    if (config->mode == GPIO_ALTERNATE) {
        GPIO_SetAlternateFunction(config->port, config->pin, config->alternate_function);
    }
    return DRIVER_OK;
}

void GPIO_SetAlternateFunction(GPIO_TypeDef *port, uint8_t pin, uint8_t af)
{
    uint8_t index = (pin >= 8U) ? 1U : 0U;
    uint8_t position = (uint8_t)((pin % 8U) * 4U);
    uint32_t mask = (15UL << position);

    port->AFR[index] = (port->AFR[index] & ~mask) | ((uint32_t)af << position);
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin)
{
    return ((port->IDR & GPIO_PinMask(pin)) != 0UL) ? GPIO_HIGH : GPIO_LOW;
}

void GPIO_WritePin(GPIO_TypeDef *port, uint8_t pin, uint8_t state)
{
    if (state != GPIO_LOW) {
        port->BSRR = GPIO_PinMask(pin);
    } else {
        port->BSRR = GPIO_PinMask(pin) << 16U;
    }
}

void GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin)
{
    if ((port->ODR & GPIO_PinMask(pin)) != 0UL) {
        port->BSRR = GPIO_PinMask(pin) << 16U;
    } else {
        port->BSRR = GPIO_PinMask(pin);
    }
}
