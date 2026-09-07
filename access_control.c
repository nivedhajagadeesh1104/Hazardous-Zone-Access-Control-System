#include "access_control.h"
#include "tim2.h"
#include "uart.h"
#include "hc_sr04.h"
#include "keypad.h"
#include "ssd1306.h"
#include "ir_sensor.h"
#include "relay.h"
#include "buzzer.h"
#include "led.h"

#define PASSWORD_MAX_LEN       (8U)
#define ACCESS_PASSWORD        "2580"
#define ACCESS_GRANTED_TIME_MS (3000UL)
#define ACCESS_DENIED_TIME_MS  (2000UL)
#define INTRUSION_TIME_MS      (3000UL)
#define SENSOR_POLL_PERIOD_MS  (100UL)

static AccessState_t state = ACCESS_STATE_IDLE;
static char password[PASSWORD_MAX_LEN];
static uint32_t last_sensor_poll;

static uint8_t Password_IsCorrect(const char *entered)
{
    uint8_t i = 0U;
    if (entered == (void *)0) return 0U;
    while ((ACCESS_PASSWORD[i] != '\0') && (entered[i] != '\0')) {
        if (ACCESS_PASSWORD[i] != entered[i]) return 0U;
        ++i;
    }
    return (ACCESS_PASSWORD[i] == '\0') && (entered[i] == '\0') ? 1U : 0U;
}

static void ShowIdle(void)
{
    (void)SSD1306_ShowMessage("HAZARDOUS ZONE", "READY");
}

static void HandlePersonDetected(void)
{
    UART2_SendString("Person Detected\r\n");
    (void)SSD1306_ShowMessage("PERSON DETECTED", "ENTER PASSWORD");
    LED_AllOff();
    LED_On(LED_YELLOW);
}

static void HandleAuthentication(void)
{
    uint8_t length;

    UART2_SendString("Authentication Started\r\n");
    length = Keypad_GetPassword(password, PASSWORD_MAX_LEN);

    if (length == 0U) {
        return;
    }

    UART2_SendString("Password Entered\r\n");

    if (Password_IsCorrect(password) != 0U) {
        state = ACCESS_STATE_ACCESS_GRANTED;
    } else {
        state = ACCESS_STATE_ACCESS_DENIED;
    }
}

static void HandleGranted(void)
{
    Relay_On();
    LED_AllOff();
    LED_On(LED_GREEN);
    Buzzer_Off();
    (void)SSD1306_ShowMessage("ACCESS GRANTED", "LOCK OPEN");
    UART2_SendString("Access Granted\r\n");
    TIM2_DelayMs(ACCESS_GRANTED_TIME_MS);
    Relay_Off();
    LED_AllOff();
    ShowIdle();
    state = ACCESS_STATE_IDLE;
}

static void HandleDenied(void)
{
    Relay_Off();
    LED_AllOff();
    LED_On(LED_RED);
    (void)SSD1306_ShowMessage("ACCESS DENIED", "INVALID PASSWORD");
    UART2_SendString("Access Denied\r\n");
    Buzzer_Beep(500UL);
    TIM2_DelayMs(ACCESS_DENIED_TIME_MS);
    LED_AllOff();
    ShowIdle();
    state = ACCESS_STATE_IDLE;
}

static void HandleIntrusion(void)
{
    Relay_Off();
    LED_AllOff();
    LED_On(LED_RED);
    LED_On(LED_BLUE);
    Buzzer_On();
    (void)SSD1306_ShowMessage("!!! WARNING !!!", "INTRUSION ALERT");
    UART2_SendString("Intrusion Alert\r\n");
    TIM2_DelayMs(INTRUSION_TIME_MS);
    Buzzer_Off();
    LED_AllOff();
    IR_Sensor_ClearEvent();
    ShowIdle();
    state = ACCESS_STATE_IDLE;
}

uint8_t AccessControl_Init(void)
{
    RCC_Init();
    TIM2_Init();

    if (UART2_Init(UART_DEFAULT_BAUD) != DRIVER_OK) return DRIVER_ERROR;
    if (HCSR04_Init() != DRIVER_OK) return DRIVER_ERROR;
    if (Keypad_Init() != DRIVER_OK) return DRIVER_ERROR;
    if (Relay_Init() != DRIVER_OK) return DRIVER_ERROR;
    if (Buzzer_Init() != DRIVER_OK) return DRIVER_ERROR;
    if (LED_Init() != DRIVER_OK) return DRIVER_ERROR;
    if (SSD1306_Init() != DRIVER_OK) return DRIVER_ERROR;
    if (IR_Sensor_Init((void *)0) != DRIVER_OK) return DRIVER_ERROR;

    UART2_SendString("System Booted\r\n");
    ShowIdle();
    last_sensor_poll = TIM2_GetCounterUs();
    return DRIVER_OK;
}

void AccessControl_Run(void)
{
    uint8_t person_present = 0U;
    uint16_t distance_cm;

    /* Intrusion has highest priority. */
    if (IR_Sensor_GetEvent() != 0U) {
        state = ACCESS_STATE_INTRUSION_ALERT;
    }

    switch (state) {
        case ACCESS_STATE_IDLE:
            if ((uint32_t)(TIM2_GetCounterUs() - last_sensor_poll) >= (SENSOR_POLL_PERIOD_MS * 1000UL)) {
                last_sensor_poll = TIM2_GetCounterUs();
                if (HCSR04_MeasureDistanceCm(&distance_cm) == DRIVER_OK) {
                    person_present = ((distance_cm > 0U) && (distance_cm <= HCSR04_PRESENCE_DISTANCE_CM)) ? 1U : 0U;
                }
                if (person_present != 0U) {
                    state = ACCESS_STATE_PERSON_DETECTED;
                }
            }
            break;

        case ACCESS_STATE_PERSON_DETECTED:
            HandlePersonDetected();
            state = ACCESS_STATE_AUTHENTICATION;
            break;

        case ACCESS_STATE_AUTHENTICATION:
            HandleAuthentication();
            break;

        case ACCESS_STATE_ACCESS_GRANTED:
            HandleGranted();
            break;

        case ACCESS_STATE_ACCESS_DENIED:
            HandleDenied();
            break;

        case ACCESS_STATE_INTRUSION_ALERT:
            HandleIntrusion();
            break;

        default:
            state = ACCESS_STATE_IDLE;
            break;
    }
}

AccessState_t AccessControl_GetState(void)
{
    return state;
}
