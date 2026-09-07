#ifndef SSD1306_H
#define SSD1306_H
#include "stm32f446xx.h"

#define SSD1306_I2C_ADDRESS      (0x3CU)
#define SSD1306_WIDTH            (128U)
#define SSD1306_HEIGHT           (64U)
#define SSD1306_PAGES            (8U)

uint8_t SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_SetCursor(uint8_t column, uint8_t page);
uint8_t SSD1306_WriteString(const char *text);
uint8_t SSD1306_Update(void);
uint8_t SSD1306_ShowMessage(const char *line1, const char *line2);

#endif
