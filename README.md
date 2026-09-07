# Hazardous Zone Access Control System

Target:
- STM32 NUCLEO-F446RE
- STM32F446RET6
- Cortex-M4
- STM32CubeIDE
- Embedded C
- Bare-metal register-level peripheral programming

## Important integration note

This source tree intentionally does not contain STM32 HAL or LL code.

Create a normal STM32CubeIDE project for STM32F446RE and retain only the MCU startup file and linker script generated/provided by the toolchain. Do not generate peripheral initialization code.

Add the folders `Core`, `MCAL`, `HAL`, and `Application` to the project include/source paths.

Recommended include paths:
- Core
- MCAL
- HAL
- Application

## Clock

The drivers intentionally use the internal 16 MHz HSI clock. No PLL is required for this POC.

TIM2 is configured as a 1 MHz free-running timer:
- Timer input clock: 16 MHz
- PSC = 15
- Counter tick = 1 us

USART2:
- PA2 = TX, AF7
- PA3 = RX, AF7
- 115200 baud, 8-N-1

I2C1:
- PB8 = SCL, AF4
- PB9 = SDA, AF4
- Standard mode, 100 kHz
- SSD1306 address = 0x3C

## Pin map

HC-SR04:
- TRIG PA0
- ECHO PA1

IR:
- OUT PC13

Relay:
- PB0

Buzzer:
- PB1

LEDs:
- Green PA5
- Yellow PA6
- Blue PA7
- Red PB6

Keypad:
- Rows PC0-PC3
- Columns PC4-PC7

## Keypad operation

Enter the configured password:
2580

Press `#` to submit.
Press `*` to clear the entered password.

## State machine

IDLE
 -> PERSON_DETECTED
 -> AUTHENTICATION
 -> ACCESS_GRANTED
 -> IDLE

or

IDLE
 -> PERSON_DETECTED
 -> AUTHENTICATION
 -> ACCESS_DENIED
 -> IDLE

At any time:
IR event
 -> INTRUSION_ALERT
 -> IDLE

## Safety note

This is an academic/industrial POC, not a certified safety controller. Do not connect the relay output directly to safety-critical machinery without appropriate isolation, fail-safe design, validation, certification, and a proper safety PLC/safety controller where required.

## Hardware notes

1. HC-SR04 ECHO is commonly a 5 V logic output. STM32F446 GPIO inputs are not universally 5 V tolerant in every configuration. Use a suitable voltage divider/level shifter on ECHO before connecting it to PA1.
2. Verify the relay module input polarity. This implementation assumes active-high ON. If your module is active-low, invert Relay_On/Relay_Off.
3. Verify buzzer module polarity; this implementation assumes active-high.
4. PC13 on the Nucleo board may have board-specific circuitry. For an external IR sensor, verify the electrical behavior before enabling the EXTI trigger.
5. SSD1306 modules vary in controller/address/wiring. The code assumes a 128x64 SSD1306 at I2C address 0x3C.

## Testing sequence

1. Build with the STM32 startup file and linker script.
2. Program the board.
3. Open a serial terminal at 115200, 8-N-1.
4. Confirm `System Booted`.
5. Place a person/object within 100 cm of the HC-SR04.
6. Confirm `Person Detected`.
7. Enter `2580#`.
8. Confirm relay, green LED, OLED, and UART.
9. Enter an incorrect password and confirm buzzer/red LED/OLED/UART.
10. Trigger the IR sensor and confirm intrusion handling.

## Design limitations of this POC

- HC-SR04 echo is measured by GPIO polling for straightforward bring-up. TIM2 also exposes an input-capture API for future hardware-timed echo measurement.
- Keypad scanning is blocking during password entry.
- UART transmit is polling based.
- I2C is polling based.
- No RTOS is required for this POC.
- The password is stored as a compile-time constant and is not a secure credential store.
