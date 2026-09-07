#include "i2c.h"
#include "rcc.h"
#include "gpio.h"
#include "tim2.h"

static uint8_t I2C1_WaitSR1(uint32_t flag, uint8_t set)
{
    uint32_t start = TIM2_GetCounterUs();

    while (((I2C1->SR1 & flag) != 0UL) != (set != 0U)) {
        if ((uint32_t)(TIM2_GetCounterUs() - start) >= I2C1_TIMEOUT_US) {
            return DRIVER_ERROR;
        }
    }
    return DRIVER_OK;
}

static void I2C1_ClearAddr(void)
{
    volatile uint32_t sr1;
    volatile uint32_t sr2;

    sr1 = I2C1->SR1;
    sr2 = I2C1->SR2;
    (void)sr1;
    (void)sr2;
}

static uint8_t I2C1_SendAddress(uint8_t address7, uint8_t read)
{
    uint8_t address = (uint8_t)(address7 << 1U);

    if (read != 0U) {
        address = (uint8_t)(address | 1U);
    }

    I2C1->DR = address;

    if (I2C1_WaitSR1(I2C_SR1_ADDR, 1U) != DRIVER_OK) {
        return DRIVER_ERROR;
    }

    I2C1_ClearAddr();
    return DRIVER_OK;
}

uint8_t I2C1_Init(void)
{
    GPIO_Config_t scl = {
        GPIOB, GPIO_PIN_8, GPIO_ALTERNATE, GPIO_OPEN_DRAIN,
        GPIO_PULLUP, GPIO_SPEED_VERY_HIGH, 4U
    };
    GPIO_Config_t sda = {
        GPIOB, GPIO_PIN_9, GPIO_ALTERNATE, GPIO_OPEN_DRAIN,
        GPIO_PULLUP, GPIO_SPEED_VERY_HIGH, 4U
    };

    RCC_EnableGPIOB();
    RCC_EnableI2C1();

    if ((GPIO_InitPin(&scl) != DRIVER_OK) ||
        (GPIO_InitPin(&sda) != DRIVER_OK)) {
        return DRIVER_ERROR;
    }

    I2C1->CR1 = I2C_CR1_SWRST;
    I2C1->CR1 = 0UL;

    /* APB1 clock = 16 MHz. Standard-mode I2C = 100 kHz.
       CCR = 16 MHz / (2 * 100 kHz) = 80.
       TRISE = 16 MHz * 1000 ns + 1 = 17. */
    I2C1->CR2 = 16UL;
    I2C1->CCR = 80UL;
    I2C1->TRISE = 17UL;
    I2C1->CR1 = I2C_CR1_PE | I2C_CR1_ACK;

    return DRIVER_OK;
}

uint8_t I2C1_Start(void)
{
    I2C1->CR1 |= I2C_CR1_START;
    return I2C1_WaitSR1(I2C_SR1_SB, 1U);
}

void I2C1_Stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
}

uint8_t I2C1_WriteByte(uint8_t data)
{
    if (I2C1_WaitSR1(I2C_SR1_TXE, 1U) != DRIVER_OK) {
        return DRIVER_ERROR;
    }

    I2C1->DR = data;

    if (I2C1_WaitSR1(I2C_SR1_BTF, 1U) != DRIVER_OK) {
        return DRIVER_ERROR;
    }

    return DRIVER_OK;
}

uint8_t I2C1_ReadByte(uint8_t *data, uint8_t ack)
{
    if (data == (void *)0) {
        return DRIVER_ERROR;
    }

    if (ack != 0U) {
        I2C1->CR1 |= I2C_CR1_ACK;
    } else {
        I2C1->CR1 &= ~I2C_CR1_ACK;
    }

    if (I2C1_WaitSR1(I2C_SR1_RXNE, 1U) != DRIVER_OK) {
        return DRIVER_ERROR;
    }

    *data = (uint8_t)I2C1->DR;
    return DRIVER_OK;
}

uint8_t I2C1_Write(uint8_t address7, const uint8_t *data, uint16_t length)
{
    uint16_t i;

    if ((data == (void *)0) || (length == 0U)) {
        return DRIVER_ERROR;
    }

    if (I2C1_Start() != DRIVER_OK) {
        I2C1_Stop();
        return DRIVER_ERROR;
    }

    if (I2C1_SendAddress(address7, 0U) != DRIVER_OK) {
        I2C1_Stop();
        return DRIVER_ERROR;
    }

    for (i = 0U; i < length; ++i) {
        if (I2C1_WriteByte(data[i]) != DRIVER_OK) {
            I2C1_Stop();
            return DRIVER_ERROR;
        }
    }

    I2C1_Stop();
    return DRIVER_OK;
}

uint8_t I2C1_Read(uint8_t address7, uint8_t *data, uint16_t length)
{
    uint16_t i;

    if ((data == (void *)0) || (length == 0U)) {
        return DRIVER_ERROR;
    }

    if (I2C1_Start() != DRIVER_OK) {
        I2C1_Stop();
        return DRIVER_ERROR;
    }

    if (I2C1_SendAddress(address7, 1U) != DRIVER_OK) {
        I2C1_Stop();
        return DRIVER_ERROR;
    }

    for (i = 0U; i < length; ++i) {
        uint8_t ack = ((i + 1U) < length) ? 1U : 0U;

        if (I2C1_ReadByte(&data[i], ack) != DRIVER_OK) {
            I2C1_Stop();
            return DRIVER_ERROR;
        }
    }

    I2C1_Stop();
    I2C1->CR1 |= I2C_CR1_ACK;
    return DRIVER_OK;
}
