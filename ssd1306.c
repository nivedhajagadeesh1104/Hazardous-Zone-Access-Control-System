#include "ssd1306.h"
#include "i2c.h"
#include "tim2.h"

static uint8_t display_buffer[SSD1306_WIDTH * SSD1306_PAGES];
static uint8_t cursor_column;
static uint8_t cursor_page;

static const uint8_t font5x7[96U][5U] = {
#include "ssd1306_font.inc"
};

static uint8_t SSD1306_Command(uint8_t command)
{
    uint8_t packet[2U] = {0x00U, command};
    return I2C1_Write(SSD1306_I2C_ADDRESS, packet, 2U);
}

static uint8_t SSD1306_Data(const uint8_t *data, uint16_t length)
{
    uint8_t packet[17U];
    uint16_t offset = 0U;
    uint8_t chunk;

    while (offset < length) {
        uint16_t remaining = (uint16_t)(length - offset);
        chunk = (remaining > 16U) ? 16U : (uint8_t)remaining;
        packet[0] = 0x40U;
        {
            uint8_t i;
            for (i = 0U; i < chunk; ++i) packet[i + 1U] = data[offset + i];
        }
        if (I2C1_Write(SSD1306_I2C_ADDRESS, packet, (uint16_t)(chunk + 1U)) != DRIVER_OK)
            return DRIVER_ERROR;
        offset = (uint16_t)(offset + chunk);
    }
    return DRIVER_OK;
}

uint8_t SSD1306_Init(void)
{
    static const uint8_t init_sequence[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40,
        0x8D, 0x14, 0x20, 0x00, 0xA1, 0xC8, 0xDA, 0x12,
        0x81, 0x7F, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF
    };
    uint8_t i;

    if (I2C1_Init() != DRIVER_OK) return DRIVER_ERROR;
    TIM2_DelayMs(100UL);

    for (i = 0U; i < sizeof(init_sequence); ++i) {
        if (SSD1306_Command(init_sequence[i]) != DRIVER_OK) return DRIVER_ERROR;
    }
    SSD1306_Clear();
    return SSD1306_Update();
}

void SSD1306_Clear(void)
{
    uint16_t i;
    for (i = 0U; i < sizeof(display_buffer); ++i) display_buffer[i] = 0x00U;
    cursor_column = 0U;
    cursor_page = 0U;
}

void SSD1306_SetCursor(uint8_t column, uint8_t page)
{
    cursor_column = (column < SSD1306_WIDTH) ? column : 0U;
    cursor_page = (page < SSD1306_PAGES) ? page : 0U;
}

uint8_t SSD1306_WriteString(const char *text)
{
    if (text == (void *)0) return DRIVER_ERROR;

    while ((*text != '\0') && (cursor_page < SSD1306_PAGES)) {
        char ch = *text;
        if ((ch < 32) || (ch > 127)) ch = '?';

        if ((uint8_t)(cursor_column + 6U) >= SSD1306_WIDTH) {
            cursor_column = 0U;
            ++cursor_page;
            if (cursor_page >= SSD1306_PAGES) break;
        }

        {
            uint16_t base = (uint16_t)(cursor_page * SSD1306_WIDTH + cursor_column);
            uint8_t i;
            uint8_t glyph = (uint8_t)(ch - 32);
            for (i = 0U; i < 5U; ++i) display_buffer[base + i] = font5x7[glyph][i];
            display_buffer[base + 5U] = 0x00U;
            cursor_column = (uint8_t)(cursor_column + 6U);
        }
        ++text;
    }
    return DRIVER_OK;
}

uint8_t SSD1306_Update(void)
{
    uint8_t page;
    for (page = 0U; page < SSD1306_PAGES; ++page) {
        if (SSD1306_Command((uint8_t)(0xB0U + page)) != DRIVER_OK) return DRIVER_ERROR;
        if (SSD1306_Command(0x00U) != DRIVER_OK) return DRIVER_ERROR;
        if (SSD1306_Command(0x10U) != DRIVER_OK) return DRIVER_ERROR;
        if (SSD1306_Data(&display_buffer[(uint16_t)page * SSD1306_WIDTH], SSD1306_WIDTH) != DRIVER_OK)
            return DRIVER_ERROR;
    }
    return DRIVER_OK;
}

uint8_t SSD1306_ShowMessage(const char *line1, const char *line2)
{
    SSD1306_Clear();
    SSD1306_SetCursor(0U, 2U);
    if (SSD1306_WriteString(line1) != DRIVER_OK) return DRIVER_ERROR;
    SSD1306_SetCursor(0U, 4U);
    if (SSD1306_WriteString(line2) != DRIVER_OK) return DRIVER_ERROR;
    return SSD1306_Update();
}
