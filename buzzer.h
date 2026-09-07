#ifndef BUZZER_H
#define BUZZER_H
#include "stm32f446xx.h"

uint8_t Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
void Buzzer_Beep(uint32_t duration_ms);

#endif
