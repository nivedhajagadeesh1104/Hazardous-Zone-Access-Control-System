#include "keypad.h"
#include "rcc.h"
#include "gpio.h"
#include "tim2.h"

static const uint8_t row_pins[4U] = {0U, 1U, 2U, 3U};
static const uint8_t col_pins[4U] = {4U, 5U, 6U, 7U};

static const char key_map[4U][4U] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

uint8_t Keypad_Init(void)
{
    uint8_t i;
    GPIO_Config_t cfg;

    RCC_EnableGPIOC();

    for (i = 0U; i < 4U; ++i) {
        cfg.port = GPIOC; cfg.pin = row_pins[i]; cfg.mode = GPIO_OUTPUT;
        cfg.output_type = GPIO_PUSH_PULL; cfg.pull = GPIO_PULLUP;
        cfg.speed = GPIO_SPEED_LOW; cfg.alternate_function = 0U;
        if (GPIO_InitPin(&cfg) != DRIVER_OK) return DRIVER_ERROR;
        GPIO_WritePin(GPIOC, row_pins[i], GPIO_HIGH);
    }

    for (i = 0U; i < 4U; ++i) {
        cfg.port = GPIOC; cfg.pin = col_pins[i]; cfg.mode = GPIO_INPUT;
        cfg.output_type = GPIO_PUSH_PULL; cfg.pull = GPIO_PULLUP;
        cfg.speed = GPIO_SPEED_LOW; cfg.alternate_function = 0U;
        if (GPIO_InitPin(&cfg) != DRIVER_OK) return DRIVER_ERROR;
    }
    return DRIVER_OK;
}

char Keypad_GetKey(void)
{
    uint8_t r, c;
    char key = '\0';

    for (r = 0U; r < 4U; ++r) {
        uint8_t i;
        for (i = 0U; i < 4U; ++i) GPIO_WritePin(GPIOC, row_pins[i], GPIO_HIGH);
        GPIO_WritePin(GPIOC, row_pins[r], GPIO_LOW);
        TIM2_DelayUs(100UL);

        for (c = 0U; c < 4U; ++c) {
            if (GPIO_ReadPin(GPIOC, col_pins[c]) == GPIO_LOW) {
                key = key_map[r][c];
                TIM2_DelayMs(20UL);
                while (GPIO_ReadPin(GPIOC, col_pins[c]) == GPIO_LOW) { }
                TIM2_DelayMs(20UL);
                GPIO_WritePin(GPIOC, row_pins[r], GPIO_HIGH);
                return key;
            }
        }
    }
    return key;
}

uint8_t Keypad_GetPassword(char *password, uint8_t max_length)
{
    uint8_t count = 0U;
    char key;

    if ((password == (void *)0) || (max_length < 2U)) {
        return DRIVER_ERROR;
    }

    while (count < (uint8_t)(max_length - 1U)) {
        key = Keypad_GetKey();
        if (key == '\0') continue;

        if (key == '#') {
            password[count] = '\0';
            return count;
        }

        if (key == '*') {
            count = 0U;
            password[0] = '\0';
            continue;
        }

        if (((key >= '0') && (key <= '9')) || ((key >= 'A') && (key <= 'D'))) {
            password[count] = key;
            ++count;
            password[count] = '\0';
        }
    }
    password[count] = '\0';
    return count;
}
