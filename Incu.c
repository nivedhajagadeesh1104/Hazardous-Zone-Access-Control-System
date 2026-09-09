#include "stm32f4xx_hal.h"

/* =========================================================
 * STM32F446RE + 16x2 I2C LCD
 *
 * PB8 = I2C1_SCL
 * PB9 = I2C1_SDA
 *
 * LCD:
 * VCC -> 5V
 * GND -> GND
 * SCL -> PB8
 * SDA -> PB9
 * ========================================================= */


/* =========================================================
 * I2C HANDLE
 * ========================================================= */

I2C_HandleTypeDef hi2c1;


/* =========================================================
 * LCD CONFIGURATION
 * ========================================================= */

/*
 * Common PCF8574 LCD backpack address:
 * 0x27
 *
 * HAL uses 8-bit address, therefore:
 * 0x27 << 1 = 0x4E
 */

#define LCD_ADDR        (0x27 << 1)


/*
 * Common PCF8574 LCD mapping:
 *
 * P0 = RS
 * P1 = RW
 * P2 = EN
 * P3 = Backlight
 * P4 = D4
 * P5 = D5
 * P6 = D6
 * P7 = D7
 */

#define LCD_RS          0x01
#define LCD_EN          0x04
#define LCD_BACKLIGHT   0x08


/* =========================================================
 * FUNCTION PROTOTYPES
 * ========================================================= */

void SystemClock_Config(void);

void I2C1_Init(void);

void LCD_Init(void);
void LCD_SendCommand(uint8_t command);
void LCD_SendData(uint8_t data);
void LCD_SendString(char *string);
void LCD_SetCursor(uint8_t row, uint8_t column);
void LCD_Clear(void);

static void LCD_SendNibble(uint8_t data);
static void LCD_PulseEnable(uint8_t data);

void Error_Handler(void);


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    /* Initialize HAL */

    HAL_Init();


    /* Configure system clock */

    SystemClock_Config();


    /* Configure I2C1 */

    I2C1_Init();


    /* Wait for LCD power-up */

    HAL_Delay(100);


    /* Initialize LCD */

    LCD_Init();


    /* Display first line */

    LCD_SetCursor(0, 0);

    LCD_SendString("Hello STM32");


    /* Display second line */

    LCD_SetCursor(1, 0);

    LCD_SendString("F446RE LCD");


    while (1)
    {
        /* Main application */

    }
}


/* =========================================================
 * I2C1 INITIALIZATION
 *
 * PB8 = SCL
 * PB9 = SDA
 *
 * I2C speed = 100 kHz
 * ========================================================= */

void I2C1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    /* Enable GPIOB clock */

    __HAL_RCC_GPIOB_CLK_ENABLE();


    /* Enable I2C1 clock */

    __HAL_RCC_I2C1_CLK_ENABLE();


    /*
     * PB8 = I2C1_SCL
     * PB9 = I2C1_SDA
     */

    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;

    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;

    GPIO_InitStruct.Pull = GPIO_PULLUP;

    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;


    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


    /*
     * I2C configuration
     */

    hi2c1.Instance = I2C1;

    hi2c1.Init.ClockSpeed = 100000;

    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;

    hi2c1.Init.OwnAddress1 = 0;

    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;

    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;

    hi2c1.Init.OwnAddress2 = 0;

    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;

    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;


    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
}


/* =========================================================
 * LCD ENABLE PULSE
 * ========================================================= */

static void LCD_PulseEnable(uint8_t data)
{
    uint8_t value;


    /* EN = HIGH */

    value = data | LCD_EN;


    if (HAL_I2C_Master_Transmit(
            &hi2c1,
            LCD_ADDR,
            &value,
            1,
            100) != HAL_OK)
    {
        Error_Handler();
    }


    HAL_Delay(1);


    /* EN = LOW */

    value = data & ~LCD_EN;


    if (HAL_I2C_Master_Transmit(
            &hi2c1,
            LCD_ADDR,
            &value,
            1,
            100) != HAL_OK)
    {
        Error_Handler();
    }


    HAL_Delay(1);
}


/* =========================================================
 * SEND ONE 4-BIT NIBBLE
 * ========================================================= */

static void LCD_SendNibble(uint8_t data)
{
    uint8_t value;


    /*
     * Keep backlight ON
     */

    value = data | LCD_BACKLIGHT;


    /*
     * Send data through I2C
     */

    if (HAL_I2C_Master_Transmit(
            &hi2c1,
            LCD_ADDR,
            &value,
            1,
            100) != HAL_OK)
    {
        Error_Handler();
    }


    /*
     * Generate EN pulse
     */

    LCD_PulseEnable(value);
}


/* =========================================================
 * SEND COMMAND
 * ========================================================= */

void LCD_SendCommand(uint8_t command)
{
    uint8_t upper;
    uint8_t lower;


    /*
     * Upper 4 bits
     */

    upper = command & 0xF0;


    /*
     * Lower 4 bits
     */

    lower = (command << 4) & 0xF0;


    /*
     * RS = 0
     * Therefore command mode
     */

    LCD_SendNibble(upper);

    LCD_SendNibble(lower);


    HAL_Delay(2);
}


/* =========================================================
 * SEND CHARACTER
 * ========================================================= */

void LCD_SendData(uint8_t data)
{
    uint8_t upper;
    uint8_t lower;


    /*
     * Upper nibble
     */

    upper = (data & 0xF0) | LCD_RS;


    /*
     * Lower nibble
     */

    lower = ((data << 4) & 0xF0) | LCD_RS;


    /*
     * RS = 1
     * Therefore data mode
     */

    LCD_SendNibble(upper);

    LCD_SendNibble(lower);


    HAL_Delay(1);
}


/* =========================================================
 * SEND STRING
 * ========================================================= */

void LCD_SendString(char *string)
{
    while (*string != '\0')
    {
        LCD_SendData((uint8_t)*string);

        string++;
    }
}


/* =========================================================
 * SET LCD CURSOR
 *
 * row = 0 -> first line
 * row = 1 -> second line
 *
 * column = 0 to 15
 * ========================================================= */

void LCD_SetCursor(uint8_t row, uint8_t column)
{
    uint8_t address;


    if (row == 0)
    {
        address = 0x80 + column;
    }
    else
    {
        address = 0xC0 + column;
    }


    LCD_SendCommand(address);
}


/* =========================================================
 * CLEAR LCD
 * ========================================================= */

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);

    HAL_Delay(2);
}


/* =========================================================
 * LCD INITIALIZATION
 * ========================================================= */

void LCD_Init(void)
{
    HAL_Delay(50);


    /*
     * LCD startup sequence
     */

    LCD_SendNibble(0x30);

    HAL_Delay(5);


    LCD_SendNibble(0x30);

    HAL_Delay(1);


    LCD_SendNibble(0x30);

    HAL_Delay(1);


    /*
     * Switch LCD to 4-bit mode
     */

    LCD_SendNibble(0x20);

    HAL_Delay(1);


    /*
     * Function set:
     *
     * 4-bit mode
     * 2 lines
     * 5x8 font
     */

    LCD_SendCommand(0x28);


    /*
     * Display OFF
     */

    LCD_SendCommand(0x08);


    /*
     * Clear display
     */

    LCD_SendCommand(0x01);

    HAL_Delay(2);


    /*
     * Entry mode:
     * Cursor moves right
     */

    LCD_SendCommand(0x06);


    /*
     * Display ON
     * Cursor OFF
     * Blink OFF
     */

    LCD_SendCommand(0x0C);
}


/* =========================================================
 * SYSTEM CLOCK
 *
 * Uses internal HSI = 16 MHz
 * ========================================================= */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


    /*
     * Enable HSI
     */

    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;

    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;

    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;


    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }


    /*
     * Select HSI as system clock
     */

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;


    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_HSI;

    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV1;

    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV1;


    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}


/* =========================================================
 * ERROR HANDLER
 * ========================================================= */

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}
