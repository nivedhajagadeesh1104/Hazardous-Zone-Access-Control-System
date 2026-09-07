#include "ir_sensor.h"
#include "gpio.h"
#include "exti.h"

static volatile uint8_t intrusion_event = 0U;

static void IR_EXTI_Callback(void)
{
    intrusion_event = 1U;
}

uint8_t IR_Sensor_Init(IR_Callback_t callback)
{
    (void)callback;
    intrusion_event = 0U;
    return EXTI_PC13_Init(EXTI_TRIGGER_FALLING, IR_EXTI_Callback);
}

uint8_t IR_Sensor_IsActive(void)
{
    return (GPIO_ReadPin(IR_SENSOR_PORT, IR_SENSOR_PIN) == GPIO_LOW) ? 1U : 0U;
}

void IR_Sensor_ClearEvent(void)
{
    intrusion_event = 0U;
}

uint8_t IR_Sensor_GetEvent(void)
{
    return intrusion_event;
}
