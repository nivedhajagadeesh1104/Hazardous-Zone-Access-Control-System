#ifndef KEYPAD_H
#define KEYPAD_H
#include "stm32f446xx.h"

#define KEYPAD_MAX_PASSWORD_LEN    (8U)

uint8_t Keypad_Init(void);
char Keypad_GetKey(void);
uint8_t Keypad_GetPassword(char *password, uint8_t max_length);

#endif
