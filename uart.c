#include "uart.h"
#include "rcc.h"
#include "gpio.h"
#include "tim2.h"

#define UART_BRR_115200_16MHZ   (139UL)

uint8_t UART2_Init(uint32_t baudrate)
{
    GPIO_Config_t tx = {GPIOA, GPIO_PIN_2, GPIO_ALTERNATE, GPIO_PUSH_PULL, GPIO_PULLUP, GPIO_SPEED_VERY_HIGH, 7U};
    GPIO_Config_t rx = {GPIOA, GPIO_PIN_3, GPIO_ALTERNATE, GPIO_PUSH_PULL, GPIO_PULLUP, GPIO_SPEED_VERY_HIGH, 7U};

    RCC_EnableGPIOA();
    RCC_EnableUSART2();

    if ((baudrate == 0UL) || (GPIO_InitPin(&tx) != DRIVER_OK) ||
        (GPIO_InitPin(&rx) != DRIVER_OK)) {
        return DRIVER_ERROR;
    }

    USART2->CR1 = 0UL;
    USART2->CR2 = USART_CR2_STOP_1;
    USART2->CR3 = 0UL;

    if (baudrate == UART_DEFAULT_BAUD) {
        USART2->BRR = UART_BRR_115200_16MHZ;
    } else {
        USART2->BRR = (RCC_HSI_FREQUENCY_HZ + (baudrate / 2UL)) / baudrate;
    }

    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
    return DRIVER_OK;
}

void UART2_SendChar(char ch)
{
    while ((USART2->SR & USART_SR_TXE) == 0UL) { }
    USART2->DR = (uint32_t)(uint8_t)ch;
}

void UART2_SendString(const char *str)
{
    if (str != (void *)0) {
        while (*str != '\0') {
            UART2_SendChar(*str);
            ++str;
        }
    }
}

uint8_t UART2_ReceiveChar(char *ch, uint32_t timeout_us)
{
    uint32_t start;

    if (ch == (void *)0) {
        return DRIVER_ERROR;
    }

    start = TIM2_GetCounterUs();
    while ((USART2->SR & USART_SR_RXNE) == 0UL) {
        if ((uint32_t)(TIM2_GetCounterUs() - start) >= timeout_us) {
            return DRIVER_ERROR;
        }
    }
    *ch = (char)(uint8_t)USART2->DR;
    return DRIVER_OK;
}

void UART2_SendUint32(uint32_t value)
{
    char buffer[11];
    uint8_t index = 0U;

    if (value == 0UL) {
        UART2_SendChar('0');
        return;
    }

    while ((value > 0UL) && (index < 10U)) {
        buffer[index] = (char)('0' + (value % 10UL));
        value /= 10UL;
        ++index;
    }
    while (index > 0U) {
        --index;
        UART2_SendChar(buffer[index]);
    }
}
