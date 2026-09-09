#include "main.h"

/* =========================================================
 * STM32F446RE
 * I2C1:
 * PB8 = SCL
 * PB9 = SDA
 *
 * LCD 16x2 with PCF8574 I2C backpack
 * Common addresses: 0x27 or 0x3F
 * ========================================================= */


/* ---------------------------------------------------------
 * I2C handle
 * --------------------------------------------------------- */

I2C_HandleTypeDef hi2c1;


/* ---------------------------------------------------------
 * LCD definitions
 * --------------------------------------------------------- */

#define LCD_BACKLIGHT   0x08
#define LCD_ENABLE      0x04
#define LCD_RS          0x01

uint16_t LCD_ADDR = 0;


/* =========================================================
 * FUNCTION PROTOTYPES
 * ========================================================= */

void SystemClock_Config(void);

static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t column);
void LCD_SendCommand(uint8_t command);
void LCD_SendData(uint8_t data);
void LCD_SendString(char *string);

static void LCD_SendNibble(uint8_t nibble, uint8_t rs);
static void LCD_EnablePulse(uint8_t data);

uint8_t LCD_FindAddress(void);

void Error_Handler(void);


/* =========================================================
 * LCD FIND ADDRESS
 * ========================================================= */

uint8_t LCD_FindAddress(void)
{
    uint8_t address;

    /*
     * Try common PCF8574 address range:
     * 0x20 - 0x27
     */

    for (address = 0x20; address <= 0x27; address++)
    {
        if (HAL_I2C_IsDeviceReady(&hi2c1,
                                  (address << 1),
                                  2,
                                  100) == HAL_OK)
        {
            LCD_ADDR = (address << 1);
            return 1;
        }
    }


    /*
     * Try common PCF8574A address range:
     * 0x38 - 0x3F
     */

    for (address = 0x38; address <= 0x3F; address++)
    {
        if (HAL_I2C_IsDeviceReady(&hi2c1,
                                  (address << 1),
                                  2,
                                  100) == HAL_OK)
        {
            LCD_ADDR = (address << 1);
            return 1;
        }
    }

    return 0;
}


/* =========================================================
 * LCD ENABLE PULSE
 * ========================================================= */

static void LCD_EnablePulse(uint8_t data)
{
    uint8_t value;


    /* EN HIGH */

    value = data | LCD_ENABLE;

    if (HAL_I2C_Master_Transmit(&hi2c1,
                                LCD_ADDR,
                                &value,
                                1,
                                100) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_Delay(1);


    /* EN LOW */

    value = data & ~LCD_ENABLE;

    if (HAL_I2C_Master_Transmit(&hi2c1,
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
 * SEND 4 BITS TO LCD
 * ========================================================= */

static void LCD_SendNibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data;


    /*
     * nibble contains:
     *
     * D4 -> bit 4
     * D5 -> bit 5
     * D6 -> bit 6
     * D7 -> bit 7
     */


    data = nibble | LCD_BACKLIGHT;


    /* RS */

    if (rs)
    {
        data |= LCD_RS;
    }
    else
    {
        data &= ~LCD_RS;
    }


    /* Send data */

    if (HAL_I2C_Master_Transmit(&hi2c1,
                                LCD_ADDR,
                                &data,
                                1,
                                100) != HAL_OK)
    {
        Error_Handler();
    }


    /* Generate enable pulse */

    LCD_EnablePulse(data);
}


/* =========================================================
 * SEND COMMAND
 * ========================================================= */

void LCD_SendCommand(uint8_t command)
{
    uint8_t upper;
    uint8_t lower;


    /* Upper nibble */

    upper = command & 0xF0;


    /* Lower nibble */

    lower = (command << 4) & 0xF0;


    LCD_SendNibble(upper, 0);

    LCD_SendNibble(lower, 0);


    /*
     * Commands need a little settling time.
     */

    HAL_Delay(2);
}


/* =========================================================
 * SEND DATA / CHARACTER
 * ========================================================= */

void LCD_SendData(uint8_t data)
{
    uint8_t upper;
    uint8_t lower;


    /* Upper nibble */

    upper = data & 0xF0;


    /* Lower nibble */

    lower = (data << 4) & 0xF0;


    LCD_SendNibble(upper, 1);

    LCD_SendNibble(lower, 1);


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
 * CLEAR LCD
 * ========================================================= */

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);

    HAL_Delay(2);
}


/* =========================================================
 * SET CURSOR
 *
 * row:
 * 0 = first line
 * 1 = second line
 *
 * column:
 * 0 - 15
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
 * LCD INITIALIZATION
 * ========================================================= */

void LCD_Init(void)
{
    HAL_Delay(50);


    /*
     * LCD initialization sequence
     * for 4-bit mode.
     */


    LCD_SendNibble(0x30, 0);

    HAL_Delay(5);


    LCD_SendNibble(0x30, 0);

    HAL_Delay(1);


    LCD_SendNibble(0x30, 0);

    HAL_Delay(1);


    /*
     * Switch to 4-bit mode
     */

    LCD_SendNibble(0x20, 0);

    HAL_Delay(1);


    /*
     * Function Set
     *
     * 4-bit
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
     * Entry Mode
     *
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
 * MAIN
 * ========================================================= */

int main(void)
{
    uint8_t lcd_found;


    /* HAL initialization */

    HAL_Init();


    /* Use internal HSI clock */

    SystemClock_Config();


    /* GPIO initialization */

    MX_GPIO_Init();


    /* I2C1 initialization */

    MX_I2C1_Init();


    /*
     * Find LCD address automatically
     */

    lcd_found = LCD_FindAddress();


    if (lcd_found == 0)
    {
        /*
         * LCD not detected.
         *
         * Stay here.
         */

        while (1)
        {
            HAL_Delay(500);
        }
    }


    /*
     * Initialize LCD
     */

    LCD_Init();


    /*
     * Display first line
     */

    LCD_SetCursor(0, 0);

    LCD_SendString("Hello STM32");


    /*
     * Display second line
     */

    LCD_SetCursor(1, 0);

    LCD_SendString("F446RE Working!");


    while (1)
    {
        /*
         * Main application
         */
    }
}


/* =========================================================
 * I2C1 INITIALIZATION
 *
 * STM32F446RE
 * PB8 -> SCL
 * PB9 -> SDA
 * ========================================================= */

static void MX_I2C1_Init(void)
{
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
 * I2C GPIO INITIALIZATION
 *
 * PB8 = I2C1_SCL
 * PB9 = I2C1_SDA
 * ========================================================= */

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    if (hi2c->Instance == I2C1)
    {
        /*
         * Enable clocks
         */

        __HAL_RCC_GPIOB_CLK_ENABLE();

        __HAL_RCC_I2C1_CLK_ENABLE();


        /*
         * Configure PB8 and PB9
         */

        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;

        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;

        GPIO_InitStruct.Pull = GPIO_PULLUP;

        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;


        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}


/* =========================================================
 * GPIO INITIALIZATION
 * ========================================================= */

static void MX_GPIO_Init(void)
{
    /*
     * No additional GPIO required.
     *
     * I2C pins are configured inside
     * HAL_I2C_MspInit().
     */
}


/* =========================================================
 * SYSTEM CLOCK
 *
 * HSI = 16 MHz
 *
 * No PLL required for this LCD test.
 * ========================================================= */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


    /*
     * Enable HSI
     */

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;

    RCC_OscInitStruct.HSIState = RCC_HSI_ON;

    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;


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


    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;

    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;


    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
        /*
         * Stay here if an error occurs.
         */
    }
}
