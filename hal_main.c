/* USER CODE BEGIN Header */
/*
 * Hazardous Zone Access Control System
 *
 * MCU      : STM32F446RETx
 *
 * Hardware :
 * 4x4 Keypad
 * IR Sensor
 * White LED
 * Blue LED
 * Red LED
 * Buzzer
 *
 * No Ultrasonic Sensor
 * No Relay
 * No OLED
 * No UART
 * No BH1750
 */
/* USER CODE END Header */


/* Includes --------------------------------------------------*/
#include "main.h"
#include <stdint.h>


/* =========================================================
   PIN DEFINITIONS
   ========================================================= */

/* ---------- WHITE LED ---------- */
#define WHITE_LED_PORT      GPIOA
#define WHITE_LED_PIN       GPIO_PIN_5


/* ---------- BLUE LED ---------- */
#define BLUE_LED_PORT       GPIOA
#define BLUE_LED_PIN        GPIO_PIN_7


/* ---------- RED LED ---------- */
#define RED_LED_PORT        GPIOB
#define RED_LED_PIN         GPIO_PIN_6


/* ---------- BUZZER ---------- */
#define BUZZER_PORT         GPIOB
#define BUZZER_PIN          GPIO_PIN_1


/* ---------- IR SENSOR ---------- */
#define IR_PORT             GPIOB
#define IR_PIN              GPIO_PIN_10


/* ---------- 4x4 KEYPAD ---------- */

#define KEYPAD_PORT         GPIOC

#define ROW1_PIN            GPIO_PIN_0
#define ROW2_PIN            GPIO_PIN_1
#define ROW3_PIN            GPIO_PIN_2
#define ROW4_PIN            GPIO_PIN_3

#define COL1_PIN            GPIO_PIN_4
#define COL2_PIN            GPIO_PIN_5
#define COL3_PIN            GPIO_PIN_6
#define COL4_PIN            GPIO_PIN_7


/* =========================================================
   APPLICATION SETTINGS
   ========================================================= */

#define PASSWORD_LENGTH     4U

#define ACCESS_TIME_MS      5000U

#define BUZZER_TIME_MS      1000U

#define DENIED_TIME_MS      1500U


/* =========================================================
   PASSWORD
   ========================================================= */

static const char PASSWORD[PASSWORD_LENGTH + 1U] = "2580";


/* =========================================================
   KEYPAD MAP
   ========================================================= */

static const char KeyMap[4][4] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


/* =========================================================
   FUNCTION PROTOTYPES
   ========================================================= */

void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static char Keypad_GetKey(void);

static uint8_t CheckPassword(void);

static uint8_t IR_Detected(void);

static void LEDs_Off(void);

static void Access_Granted(void);

static void Access_Denied(void);

static void Intrusion_Alarm(void);


/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    /* Initialize HAL */
    HAL_Init();


    /* Configure system clock */
    SystemClock_Config();


    /* Initialize GPIO */
    MX_GPIO_Init();


    /* -----------------------------------------------------
       INITIAL STATE

       White LED ON
       = System is waiting for a person
       ----------------------------------------------------- */

    HAL_GPIO_WritePin(
        WHITE_LED_PORT,
        WHITE_LED_PIN,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        BLUE_LED_PORT,
        BLUE_LED_PIN,
        GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        RED_LED_PORT,
        RED_LED_PIN,
        GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        BUZZER_PORT,
        BUZZER_PIN,
        GPIO_PIN_RESET
    );


    /* =====================================================
       MAIN APPLICATION LOOP
       ===================================================== */

    while (1)
    {
        /*
         * IR sensor detects a person.
         *
         * Typical IR obstacle sensor:
         *
         * LOW  = object detected
         * HIGH = no object
         */

        if (IR_Detected())
        {
            /*
             * Person detected.
             *
             * Turn OFF waiting LED.
             */

            HAL_GPIO_WritePin(
                WHITE_LED_PORT,
                WHITE_LED_PIN,
                GPIO_PIN_RESET
            );


            /*
             * Start password authentication.
             */

            if (CheckPassword())
            {
                /*
                 * Correct password.
                 */
                Access_Granted();
            }
            else
            {
                /*
                 * Wrong password.
                 *
                 * Treat as unauthorized access attempt.
                 */
                Access_Denied();
            }


            /*
             * Wait until the person leaves
             * the IR detection area.
             */

            while (IR_Detected())
            {
                HAL_Delay(50);
            }


            /*
             * Return to standby state.
             */

            LEDs_Off();

            HAL_GPIO_WritePin(
                WHITE_LED_PORT,
                WHITE_LED_PIN,
                GPIO_PIN_SET
            );

            HAL_Delay(300);
        }
        else
        {
            /*
             * No person detected.
             *
             * White LED indicates
             * standby state.
             */

            HAL_GPIO_WritePin(
                WHITE_LED_PORT,
                WHITE_LED_PIN,
                GPIO_PIN_SET
            );
        }


        HAL_Delay(50);
    }
}


/* =========================================================
   IR SENSOR
   ========================================================= */

static uint8_t IR_Detected(void)
{
    /*
     * Typical IR obstacle sensor:
     *
     * LOW  -> Object detected
     * HIGH -> No object
     */

    if (HAL_GPIO_ReadPin(
            IR_PORT,
            IR_PIN) == GPIO_PIN_RESET)
    {
        return 1U;
    }

    return 0U;
}


/* =========================================================
   KEYPAD SCANNING
   ========================================================= */

static char Keypad_GetKey(void)
{
    uint16_t rows[4] =
    {
        ROW1_PIN,
        ROW2_PIN,
        ROW3_PIN,
        ROW4_PIN
    };

    uint16_t cols[4] =
    {
        COL1_PIN,
        COL2_PIN,
        COL3_PIN,
        COL4_PIN
    };


    /*
     * Scan all four rows.
     */

    for (uint8_t row = 0U;
         row < 4U;
         row++)
    {
        /*
         * Set all rows HIGH.
         */

        for (uint8_t i = 0U;
             i < 4U;
             i++)
        {
            HAL_GPIO_WritePin(
                KEYPAD_PORT,
                rows[i],
                GPIO_PIN_SET
            );
        }


        /*
         * Set current row LOW.
         */

        HAL_GPIO_WritePin(
            KEYPAD_PORT,
            rows[row],
            GPIO_PIN_RESET
        );


        /*
         * Small settling delay.
         */

        HAL_Delay(1);


        /*
         * Check all columns.
         */

        for (uint8_t col = 0U;
             col < 4U;
             col++)
        {
            if (HAL_GPIO_ReadPin(
                    KEYPAD_PORT,
                    cols[col])
                    == GPIO_PIN_RESET)
            {
                char key = KeyMap[row][col];


                /*
                 * Debounce delay.
                 */

                HAL_Delay(20);


                /*
                 * Wait for key release.
                 */

                while (HAL_GPIO_ReadPin(
                           KEYPAD_PORT,
                           cols[col])
                       == GPIO_PIN_RESET)
                {
                    HAL_Delay(5);
                }


                /*
                 * Restore all rows HIGH.
                 */

                for (uint8_t i = 0U;
                     i < 4U;
                     i++)
                {
                    HAL_GPIO_WritePin(
                        KEYPAD_PORT,
                        rows[i],
                        GPIO_PIN_SET
                    );
                }


                return key;
            }
        }
    }


    /*
     * No key pressed.
     */

    return 0;
}


/* =========================================================
   PASSWORD CHECK
   ========================================================= */

static uint8_t CheckPassword(void)
{
    char entered[PASSWORD_LENGTH + 1U] = {0};

    uint8_t index = 0U;

    char key;


    /*
     * White LED ON.
     *
     * Indicates authentication mode.
     */

    HAL_GPIO_WritePin(
        WHITE_LED_PORT,
        WHITE_LED_PIN,
        GPIO_PIN_SET
    );


    /*
     * Wait until four digits are entered.
     */

    while (index < PASSWORD_LENGTH)
    {
        key = Keypad_GetKey();


        /*
         * No key pressed.
         */

        if (key == 0)
        {
            continue;
        }


        /*
         * '*' clears the entered password.
         */

        if (key == '*')
        {
            index = 0U;

            for (uint8_t i = 0U;
                 i < PASSWORD_LENGTH;
                 i++)
            {
                entered[i] = 0;
            }

            continue;
        }


        /*
         * Accept only numeric keys.
         */

        if ((key >= '0') &&
            (key <= '9'))
        {
            entered[index] = key;

            index++;
        }
    }


    /*
     * Compare entered password
     * with stored password.
     */

    for (uint8_t i = 0U;
         i < PASSWORD_LENGTH;
         i++)
    {
        if (entered[i] != PASSWORD[i])
        {
            return 0U;
        }
    }


    return 1U;
}


/* =========================================================
   ACCESS GRANTED
   ========================================================= */

static void Access_Granted(void)
{
    /*
     * White LED OFF.
     */

    HAL_GPIO_WritePin(
        WHITE_LED_PORT,
        WHITE_LED_PIN,
        GPIO_PIN_RESET
    );


    /*
     * Red LED OFF.
     */

    HAL_GPIO_WritePin(
        RED_LED_PORT,
        RED_LED_PIN,
        GPIO_PIN_RESET
    );


    /*
     * Blue LED ON.
     *
     * Access granted.
     */

    HAL_GPIO_WritePin(
        BLUE_LED_PORT,
        BLUE_LED_PIN,
        GPIO_PIN_SET
    );


    /*
     * Keep blue LED ON for 5 seconds.
     */

    HAL_Delay(ACCESS_TIME_MS);


    /*
     * Blue LED OFF.
     */

    HAL_GPIO_WritePin(
        BLUE_LED_PORT,
        BLUE_LED_PIN,
        GPIO_PIN_RESET
    );
}


/* =========================================================
   ACCESS DENIED
   ========================================================= */

static void Access_Denied(void)
{
    /*
     * Red LED ON.
     */

    HAL_GPIO_WritePin(
        RED_LED_PORT,
        RED_LED_PIN,
        GPIO_PIN_SET
    );


    /*
     * Buzzer ON.
     */

    HAL_GPIO_WritePin(
        BUZZER_PORT,
        BUZZER_PIN,
        GPIO_PIN_SET
    );


    /*
     * Buzzer duration.
     */

    HAL_Delay(BUZZER_TIME_MS);


    /*
     * Buzzer OFF.
     */

    HAL_GPIO_WritePin(
        BUZZER_PORT,
        BUZZER_PIN,
        GPIO_PIN_RESET
    );


    /*
     * Keep red LED ON.
     */

    HAL_Delay(DENIED_TIME_MS);


    /*
     * Red LED OFF.
     */

    HAL_GPIO_WritePin(
        RED_LED_PORT,
        RED_LED_PIN,
        GPIO_PIN_RESET
    );
}


/* =========================================================
   INTRUSION ALARM
   ========================================================= */

static void Intrusion_Alarm(void)
{
    /*
     * Red LED ON.
     */

    HAL_GPIO_WritePin(
        RED_LED_PORT,
        RED_LED_PIN,
        GPIO_PIN_SET
    );


    /*
     * Buzzer ON.
     */

    HAL_GPIO_WritePin(
        BUZZER_PORT,
        BUZZER_PIN,
        GPIO_PIN_SET
    );


    HAL_Delay(500);


    /*
     * Buzzer OFF.
     */

    HAL_GPIO_WritePin(
        BUZZER_PORT,
        BUZZER_PIN,
        GPIO_PIN_RESET
    );


    HAL_Delay(200);


    /*
     * Red LED OFF.
     */

    HAL_GPIO_WritePin(
        RED_LED_PORT,
        RED_LED_PIN,
        GPIO_PIN_RESET
    );
}


/* =========================================================
   TURN ALL OUTPUTS OFF
   ========================================================= */

static void LEDs_Off(void)
{
    HAL_GPIO_WritePin(
        WHITE_LED_PORT,
        WHITE_LED_PIN,
        GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        BLUE_LED_PORT,
        BLUE_LED_PIN,
        GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        RED_LED_PORT,
        RED_LED_PIN,
        GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        BUZZER_PORT,
        BUZZER_PIN,
        GPIO_PIN_RESET
    );
}


/* =========================================================
   GPIO INITIALIZATION
   ========================================================= */

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    /* -----------------------------------------------------
       Enable GPIO clocks
       ----------------------------------------------------- */

    __HAL_RCC_GPIOA_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();


    /* =====================================================
       INITIAL OUTPUT STATES
       ===================================================== */

    HAL_GPIO_WritePin(
        GPIOA,
        WHITE_LED_PIN | BLUE_LED_PIN,
        GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        RED_LED_PIN | BUZZER_PIN,
        GPIO_PIN_RESET
    );


    /* =====================================================
       PA5 - WHITE LED
       PA7 - BLUE LED
       ===================================================== */

    GPIO_InitStruct.Pin =
        WHITE_LED_PIN |
        BLUE_LED_PIN;

    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;

    GPIO_InitStruct.Pull =
        GPIO_NOPULL;

    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(
        GPIOA,
        &GPIO_InitStruct
    );


    /* =====================================================
       PB6 - RED LED
       PB1 - BUZZER
       ===================================================== */

    GPIO_InitStruct.Pin =
        RED_LED_PIN |
        BUZZER_PIN;

    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;

    GPIO_InitStruct.Pull =
        GPIO_NOPULL;

    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(
        GPIOB,
        &GPIO_InitStruct
    );


    /* =====================================================
       PB10 - IR SENSOR
       ===================================================== */

    GPIO_InitStruct.Pin =
        IR_PIN;

    GPIO_InitStruct.Mode =
        GPIO_MODE_INPUT;

    GPIO_InitStruct.Pull =
        GPIO_PULLUP;

    HAL_GPIO_Init(
        GPIOB,
        &GPIO_InitStruct
    );


    /* =====================================================
       PC0-PC3 - KEYPAD ROWS
       ===================================================== */

    GPIO_InitStruct.Pin =
        ROW1_PIN |
        ROW2_PIN |
        ROW3_PIN |
        ROW4_PIN;

    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;

    GPIO_InitStruct.Pull =
        GPIO_NOPULL;

    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(
        GPIOC,
        &GPIO_InitStruct
    );


    /* =====================================================
       PC4-PC7 - KEYPAD COLUMNS
       ===================================================== */

    GPIO_InitStruct.Pin =
        COL1_PIN |
        COL2_PIN |
        COL3_PIN |
        COL4_PIN;

    GPIO_InitStruct.Mode =
        GPIO_MODE_INPUT;

    GPIO_InitStruct.Pull =
        GPIO_PULLUP;

    HAL_GPIO_Init(
        GPIOC,
        &GPIO_InitStruct
    );


    /* =====================================================
       KEYPAD ROWS INITIAL STATE
       ===================================================== */

    HAL_GPIO_WritePin(
        GPIOC,
        ROW1_PIN |
        ROW2_PIN |
        ROW3_PIN |
        ROW4_PIN,
        GPIO_PIN_SET
    );
}


/* =========================================================
   SYSTEM CLOCK CONFIGURATION
   ========================================================= */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


    /* Enable Power Controller clock */

    __HAL_RCC_PWR_CLK_ENABLE();


    /* Voltage scaling */

    __HAL_PWR_VOLTAGESCALING_CONFIG(
        PWR_REGULATOR_VOLTAGE_SCALE3
    );


    /* -----------------------------------------------------
       Use internal 16 MHz HSI
       ----------------------------------------------------- */

    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;

    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;

    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_NONE;


    if (HAL_RCC_OscConfig(&RCC_OscInitStruct)
            != HAL_OK)
    {
        Error_Handler();
    }


    /* -----------------------------------------------------
       Configure system clocks
       ----------------------------------------------------- */

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
   ERROR HANDLER
   ========================================================= */

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
        /*
         * Stay here if a HAL initialization
         * error occurs.
         */
    }
}
