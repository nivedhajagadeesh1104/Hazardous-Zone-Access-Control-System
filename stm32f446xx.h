#ifndef STM32F446XX_H
#define STM32F446XX_H

#include <stdint.h>

#define PERIPH_BASE             (0x40000000UL)
#define APB1PERIPH_BASE         (PERIPH_BASE)
#define APB2PERIPH_BASE         (0x40010000UL)
#define AHB1PERIPH_BASE         (0x40020000UL)
#define SCS_BASE                (0xE000E000UL)

#define GPIOA_BASE              (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE              (AHB1PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE              (AHB1PERIPH_BASE + 0x0800UL)
#define RCC_BASE                (AHB1PERIPH_BASE + 0x3800UL)
#define TIM2_BASE               (APB1PERIPH_BASE + 0x0000UL)
#define USART2_BASE             (APB1PERIPH_BASE + 0x4400UL)
#define I2C1_BASE               (APB1PERIPH_BASE + 0x5400UL)
#define SYSCFG_BASE             (APB2PERIPH_BASE + 0x0000UL)
#define EXTI_BASE               (APB2PERIPH_BASE + 0x3C00UL)
#define NVIC_BASE               (SCS_BASE + 0x0100UL)
#define SCB_BASE                (SCS_BASE + 0x0D00UL)

typedef struct {
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR, AHB3RSTR;
    uint32_t RESERVED0;
    volatile uint32_t APB1RSTR, APB2RSTR;
    uint32_t RESERVED1[2];
    volatile uint32_t AHB1ENR, AHB2ENR, AHB3ENR;
    uint32_t RESERVED2;
    volatile uint32_t APB1ENR, APB2ENR;
    uint32_t RESERVED3[2];
    volatile uint32_t AHB1LPENR, AHB2LPENR, AHB3LPENR;
    uint32_t RESERVED4;
    volatile uint32_t APB1LPENR, APB2LPENR;
    uint32_t RESERVED5[2];
    volatile uint32_t BDCR, CSR;
    uint32_t RESERVED6[2];
    volatile uint32_t SSCGR, PLLI2SCFGR, PLLSAICFGR, DCKCFGR, CKGATENR, DCKCFGR2;
} RCC_TypeDef;

typedef struct {
    volatile uint32_t CR1, CR2, SMCR, DIER, SR, EGR, CCMR1, CCMR2, CCER;
    volatile uint32_t CNT, PSC, ARR, RESERVED0, CCR1, CCR2, CCR3, CCR4, RESERVED1[3], DCR, DMAR, ORR;
} TIM_TypeDef;

typedef struct {
    volatile uint32_t SR, DR, BRR, CR1, CR2, CR3, GTPR;
} USART_TypeDef;

typedef struct {
    volatile uint32_t CR1, CR2, OAR1, OAR2, DR, SR1, SR2, CCR, TRISE, FLTR;
} I2C_TypeDef;

typedef struct {
    volatile uint32_t IMR, EMR, RTSR, FTSR, SWIER, PR;
} EXTI_TypeDef;

typedef struct {
    volatile uint32_t MEMRMP, PMC, EXTICR[4], CMPCR, CFGR;
} SYSCFG_TypeDef;

typedef struct {
    volatile uint32_t ISER[8];
    uint32_t RESERVED0[24];
    volatile uint32_t ICER[8];
    uint32_t RESERVED1[24];
    volatile uint32_t ISPR[8];
    uint32_t RESERVED2[24];
    volatile uint32_t ICPR[8];
    uint32_t RESERVED3[24];
    volatile uint32_t IABR[8];
    uint32_t RESERVED4[56];
    volatile uint8_t IPR[240];
} NVIC_TypeDef;

typedef struct {
    volatile uint32_t CPUID, ICSR, VTOR, AIRCR, SCR, CCR;
    volatile uint8_t SHP[12];
    volatile uint32_t SHCSR;
} SCB_TypeDef;

#define GPIOA                   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB                   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC                   ((GPIO_TypeDef *)GPIOC_BASE)
#define RCC                     ((RCC_TypeDef *)RCC_BASE)
#define TIM2                    ((TIM_TypeDef *)TIM2_BASE)
#define USART2                  ((USART_TypeDef *)USART2_BASE)
#define I2C1                    ((I2C_TypeDef *)I2C1_BASE)
#define SYSCFG                  ((SYSCFG_TypeDef *)SYSCFG_BASE)
#define EXTI                    ((EXTI_TypeDef *)EXTI_BASE)
#define NVIC                    ((NVIC_TypeDef *)NVIC_BASE)
#define SCB                     ((SCB_TypeDef *)SCB_BASE)

#define GPIO_PIN_0              (0U)
#define GPIO_PIN_1              (1U)
#define GPIO_PIN_2              (2U)
#define GPIO_PIN_3              (3U)
#define GPIO_PIN_4              (4U)
#define GPIO_PIN_5              (5U)
#define GPIO_PIN_6              (6U)
#define GPIO_PIN_7              (7U)
#define GPIO_PIN_8              (8U)
#define GPIO_PIN_9              (9U)
#define GPIO_PIN_10             (10U)
#define GPIO_PIN_11             (11U)
#define GPIO_PIN_12             (12U)
#define GPIO_PIN_13             (13U)
#define GPIO_PIN_14             (14U)
#define GPIO_PIN_15             (15U)

#define GPIO_MODE_INPUT         (0UL)
#define GPIO_MODE_OUTPUT        (1UL)
#define GPIO_MODE_ALTERNATE     (2UL)
#define GPIO_MODE_ANALOG        (3UL)
#define GPIO_OTYPE_PUSHPULL     (0UL)
#define GPIO_OTYPE_OPENDRAIN    (1UL)
#define GPIO_PULL_NONE          (0UL)
#define GPIO_PULL_UP            (1UL)
#define GPIO_PULL_DOWN          (2UL)
#define GPIO_SPEED_LOW          (0UL)
#define GPIO_SPEED_MEDIUM       (1UL)
#define GPIO_SPEED_HIGH         (2UL)
#define GPIO_SPEED_VERY_HIGH    (3UL)

#define RCC_AHB1ENR_GPIOAEN     (1UL << 0U)
#define RCC_AHB1ENR_GPIOBEN     (1UL << 1U)
#define RCC_AHB1ENR_GPIOCEN     (1UL << 2U)
#define RCC_APB1ENR_TIM2EN      (1UL << 0U)
#define RCC_APB1ENR_USART2EN    (1UL << 17U)
#define RCC_APB1ENR_I2C1EN      (1UL << 21U)
#define RCC_APB2ENR_SYSCFGEN    (1UL << 14U)

#define RCC_CR_HSION            (1UL << 0U)
#define RCC_CR_HSIRDY           (1UL << 1U)
#define RCC_CFGR_SW_HSI         (0UL)
#define RCC_CFGR_SWS_HSI        (0UL)

#define TIM_CR1_CEN             (1UL << 0U)
#define TIM_CR1_ARPE            (1UL << 7U)
#define TIM_DIER_UIE            (1UL << 0U)
#define TIM_DIER_CC1IE          (1UL << 1U)
#define TIM_SR_UIF              (1UL << 0U)
#define TIM_SR_CC1IF            (1UL << 1U)
#define TIM_EGR_UG              (1UL << 0U)
#define TIM_CCER_CC1E           (1UL << 0U)
#define TIM_CCER_CC1P           (1UL << 1U)
#define TIM_CCMR1_CC1S_TI1      (1UL << 0U)
#define TIM_CCMR1_CC1S_TI2      (2UL << 0U)

#define USART_SR_RXNE           (1UL << 5U)
#define USART_SR_TC             (1UL << 6U)
#define USART_SR_TXE            (1UL << 7U)
#define USART_CR1_RE            (1UL << 2U)
#define USART_CR1_TE            (1UL << 3U)
#define USART_CR1_RXNEIE        (1UL << 5U)
#define USART_CR1_UE            (1UL << 13U)
#define USART_CR2_STOP_1        (0UL << 12U)

#define I2C_CR1_PE              (1UL << 0U)
#define I2C_CR1_START           (1UL << 8U)
#define I2C_CR1_STOP            (1UL << 9U)
#define I2C_CR1_ACK             (1UL << 10U)
#define I2C_CR1_SWRST           (1UL << 15U)
#define I2C_SR1_SB              (1UL << 0U)
#define I2C_SR1_ADDR            (1UL << 1U)
#define I2C_SR1_BTF             (1UL << 2U)
#define I2C_SR1_RXNE            (1UL << 6U)
#define I2C_SR1_TXE             (1UL << 7U)
#define I2C_SR1_BERR            (1UL << 8U)
#define I2C_SR1_ARLO            (1UL << 9U)
#define I2C_SR1_AF              (1UL << 10U)
#define I2C_SR2_BUSY            (1UL << 1U)
#define I2C_SR2_TRA             (1UL << 2U)
#define I2C_CCR_FS              (1UL << 15U)

#define EXTI_LINE_13            (1UL << 13U)
#define SYSCFG_EXTI_PORTA       (0UL)
#define SYSCFG_EXTI_PORTB       (1UL)
#define SYSCFG_EXTI_PORTC       (2UL)

typedef enum {
    NonMaskableInt_IRQn = -14, HardFault_IRQn = -13, MemoryManagement_IRQn = -12,
    BusFault_IRQn = -11, UsageFault_IRQn = -10, SVCall_IRQn = -5,
    DebugMonitor_IRQn = -4, PendSV_IRQn = -2, SysTick_IRQn = -1,
    WWDG_IRQn = 0, PVD_IRQn = 1, TAMP_STAMP_IRQn = 2, RTC_WKUP_IRQn = 3,
    FLASH_IRQn = 4, RCC_IRQn = 5, EXTI0_IRQn = 6, EXTI1_IRQn = 7, EXTI2_IRQn = 8,
    EXTI3_IRQn = 9, EXTI4_IRQn = 10, DMA1_Stream0_IRQn = 11, DMA1_Stream1_IRQn = 12,
    DMA1_Stream2_IRQn = 13, DMA1_Stream3_IRQn = 14, DMA1_Stream4_IRQn = 15,
    DMA1_Stream5_IRQn = 16, DMA1_Stream6_IRQn = 17, ADC_IRQn = 18,
    CAN1_TX_IRQn = 19, CAN1_RX0_IRQn = 20, CAN1_RX1_IRQn = 21, CAN1_SCE_IRQn = 22,
    EXTI9_5_IRQn = 23, TIM1_BRK_TIM9_IRQn = 24, TIM1_UP_TIM10_IRQn = 25,
    TIM1_TRG_COM_TIM11_IRQn = 26, TIM1_CC_IRQn = 27, TIM2_IRQn = 28,
    TIM3_IRQn = 29, TIM4_IRQn = 30, I2C1_EV_IRQn = 31, I2C1_ER_IRQn = 32,
    I2C2_EV_IRQn = 33, I2C2_ER_IRQn = 34, SPI1_IRQn = 35, SPI2_IRQn = 36,
    USART1_IRQn = 37, USART2_IRQn = 38, USART3_IRQn = 39,
    EXTI15_10_IRQn = 40
} IRQn_Type;

#define NVIC_IRQ_INDEX(IRQ)     ((uint32_t)(IRQ) / 32UL)
#define NVIC_IRQ_MASK(IRQ)      (1UL << ((uint32_t)(IRQ) % 32UL))

/* Project pin mapping */
#define HCSR04_TRIG_PORT        GPIOA
#define HCSR04_TRIG_PIN         GPIO_PIN_0
#define HCSR04_ECHO_PORT        GPIOA
#define HCSR04_ECHO_PIN         GPIO_PIN_1
#define IR_SENSOR_PORT          GPIOC
#define IR_SENSOR_PIN           GPIO_PIN_13
#define OLED_I2C                I2C1
#define OLED_SCL_PORT           GPIOB
#define OLED_SCL_PIN            GPIO_PIN_8
#define OLED_SDA_PORT           GPIOB
#define OLED_SDA_PIN            GPIO_PIN_9
#define UART2_TX_PORT           GPIOA
#define UART2_TX_PIN            GPIO_PIN_2
#define UART2_RX_PORT           GPIOA
#define UART2_RX_PIN            GPIO_PIN_3
#define RELAY_PORT              GPIOB
#define RELAY_PIN               GPIO_PIN_0
#define BUZZER_PORT             GPIOB
#define BUZZER_PIN              GPIO_PIN_1
#define GREEN_LED_PORT          GPIOA
#define GREEN_LED_PIN           GPIO_PIN_5
#define YELLOW_LED_PORT         GPIOA
#define YELLOW_LED_PIN          GPIO_PIN_6
#define BLUE_LED_PORT            GPIOA
#define BLUE_LED_PIN             GPIO_PIN_7
#define RED_LED_PORT             GPIOB
#define RED_LED_PIN              GPIO_PIN_6
#define KEYPAD_PORT              GPIOC
#define KEYPAD_ROW0_PIN         GPIO_PIN_0
#define KEYPAD_ROW1_PIN         GPIO_PIN_1
#define KEYPAD_ROW2_PIN         GPIO_PIN_2
#define KEYPAD_ROW3_PIN         GPIO_PIN_3
#define KEYPAD_COL0_PIN         GPIO_PIN_4
#define KEYPAD_COL1_PIN         GPIO_PIN_5
#define KEYPAD_COL2_PIN         GPIO_PIN_6
#define KEYPAD_COL3_PIN         GPIO_PIN_7

#define DRIVER_OK               (0U)
#define DRIVER_ERROR            (1U)
#define GPIO_LOW                (0U)
#define GPIO_HIGH               (1U)

#endif /* STM32F446XX_H */
