#ifndef LED_H
#define LED_H
#include "stm32f446xx.h"

typedef enum { LED_GREEN = 0U, LED_YELLOW = 1U, LED_BLUE = 2U, LED_RED = 3U } LED_Color_t;

uint8_t LED_Init(void);
void LED_On(LED_Color_t color);
void LED_Off(LED_Color_t color);
void LED_AllOff(void);

#endif
