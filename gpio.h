#ifndef GPIO_H
#define GPIO_H
#include "stm32f446xx.h"

typedef enum { GPIO_INPUT = 0U, GPIO_OUTPUT = 1U, GPIO_ALTERNATE = 2U, GPIO_ANALOG = 3U } GPIO_Mode_t;
typedef enum { GPIO_PUSH_PULL = 0U, GPIO_OPEN_DRAIN = 1U } GPIO_OutputType_t;
typedef enum { GPIO_NO_PULL = 0U, GPIO_PULLUP = 1U, GPIO_PULLDOWN = 2U } GPIO_Pull_t;
typedef enum { GPIO_SPEED_LOW = 0U, GPIO_SPEED_MEDIUM = 1U, GPIO_SPEED_HIGH = 2U, GPIO_SPEED_VERY_HIGH = 3U } GPIO_Speed_t;

typedef struct {
    GPIO_TypeDef *port;
    uint8_t pin;
    GPIO_Mode_t mode;
    GPIO_OutputType_t output_type;
    GPIO_Pull_t pull;
    GPIO_Speed_t speed;
    uint8_t alternate_function;
} GPIO_Config_t;

uint8_t GPIO_InitPin(const GPIO_Config_t *config);
uint8_t GPIO_ReadPin(GPIO_TypeDef *port, uint8_t pin);
void GPIO_WritePin(GPIO_TypeDef *port, uint8_t pin, uint8_t state);
void GPIO_TogglePin(GPIO_TypeDef *port, uint8_t pin);
void GPIO_SetAlternateFunction(GPIO_TypeDef *port, uint8_t pin, uint8_t af);

#endif
