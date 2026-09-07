#ifndef I2C_H
#define I2C_H
#include "stm32f446xx.h"

#define I2C1_SPEED_HZ           (100000UL)
#define I2C1_TIMEOUT_US         (10000UL)

uint8_t I2C1_Init(void);
uint8_t I2C1_Start(void);
void I2C1_Stop(void);
uint8_t I2C1_WriteByte(uint8_t data);
uint8_t I2C1_ReadByte(uint8_t *data, uint8_t ack);
uint8_t I2C1_Write(uint8_t address7, const uint8_t *data, uint16_t length);
uint8_t I2C1_Read(uint8_t address7, uint8_t *data, uint16_t length);

#endif
