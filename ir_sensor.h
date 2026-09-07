#ifndef IR_SENSOR_H
#define IR_SENSOR_H
#include "stm32f446xx.h"

typedef void (*IR_Callback_t)(void);

uint8_t IR_Sensor_Init(IR_Callback_t callback);
uint8_t IR_Sensor_IsActive(void);
void IR_Sensor_ClearEvent(void);
uint8_t IR_Sensor_GetEvent(void);

#endif
