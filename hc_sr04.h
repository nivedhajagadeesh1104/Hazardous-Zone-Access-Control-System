#ifndef HC_SR04_H
#define HC_SR04_H
#include "stm32f446xx.h"

#define HCSR04_PRESENCE_DISTANCE_CM    (100U)
#define HCSR04_TIMEOUT_US              (30000UL)

uint8_t HCSR04_Init(void);
uint8_t HCSR04_MeasureDistanceCm(uint16_t *distance_cm);
uint8_t HCSR04_IsPersonPresent(uint8_t *present);

#endif
