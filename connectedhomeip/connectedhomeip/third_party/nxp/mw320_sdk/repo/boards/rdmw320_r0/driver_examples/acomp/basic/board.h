/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "clock_config.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The board name */
#define BOARD_NAME "RDMW320-R0"

/*! @brief Macro to judge XIP */
#define BOARD_IS_XIP() \
    (((uint32_t)BOARD_InitDebugConsole >= 0x1F000000U) && ((uint32_t)BOARD_InitDebugConsole < 0x20000000U))

/*! @brief The UART to use for debug messages. */
#define BOARD_DEBUG_UART_TYPE       kSerialPort_Uart
#define BOARD_DEBUG_UART_BASEADDR   (uint32_t) UART0
#define BOARD_DEBUG_UART_INSTANCE   0U
#define BOARD_DEBUG_UART            UART0
#define BOARD_DEBUG_UART_CLK_FREQ   CLOCK_GetUartClkFreq(0)
#define BOARD_DEBUG_UART_CLK_ATTACH kSYS_CLK_to_FAST_UART0
#define BOARD_DEBUG_UART_CLK_DIV    kCLOCK_DivUartFast
#define BOARD_UART_IRQ_HANDLER      UART0_IRQHandler
#define BOARD_UART_IRQ              UART0_IRQn

#ifndef BOARD_DEBUG_UART_BAUDRATE
#define BOARD_DEBUG_UART_BAUDRATE 115200
#endif /* BOARD_DEBUG_UART_BAUDRATE */

/* Board led color mapping */
#define LOGIC_LED_ON  0U
#define LOGIC_LED_OFF 1U

#ifndef BOARD_LED_AMBER_GPIO
#define BOARD_LED_AMBER_GPIO GPIO
#endif
#ifndef BOARD_LED_AMBER_GPIO_PIN
#define BOARD_LED_AMBER_GPIO_PIN 40U
#endif

#ifndef BOARD_LED_YELLOW_GPIO
#define BOARD_LED_YELLOW_GPIO GPIO
#endif
#ifndef BOARD_LED_YELLOW_GPIO_PIN
#define BOARD_LED_YELLOW_GPIO_PIN 41U
#endif

#define LED_AMBER_INIT(output)                                   \
    GPIO_PinInit(BOARD_LED_AMBER_GPIO, BOARD_LED_AMBER_GPIO_PIN, \
                 &(gpio_pin_config_t){kGPIO_DigitalOutput, (output)})                 /*!< Enable target LED_AMBER */
#define LED_AMBER_ON()  GPIO_PinClear(BOARD_LED_AMBER_GPIO, BOARD_LED_AMBER_GPIO_PIN) /*!< Turn on target LED_AMBER */
#define LED_AMBER_OFF() GPIO_PinSet(BOARD_LED_AMBER_GPIO, BOARD_LED_AMBER_GPIO_PIN)   /*!< Turn off target LED_AMBER */
#define LED_AMBER_TOGGLE() \
    GPIO_PinToggle(BOARD_LED_AMBER_GPIO, BOARD_LED_AMBER_GPIO_PIN) /*!< Toggle on target LED_AMBER */

#define LED_YELLOW_INIT(output)                                    \
    GPIO_PinInit(BOARD_LED_YELLOW_GPIO, BOARD_LED_YELLOW_GPIO_PIN, \
                 &(gpio_pin_config_t){kGPIO_DigitalOutput, (output)}) /*!< Enable target LED_YELLOW */
#define LED_YELLOW_ON() \
    GPIO_PinClear(BOARD_LED_YELLOW_GPIO, BOARD_LED_YELLOW_GPIO_PIN) /*!< Turn on target LED_YELLOW */
#define LED_YELLOW_OFF() \
    GPIO_PinSet(BOARD_LED_YELLOW_GPIO, BOARD_LED_YELLOW_GPIO_PIN) /*!< Turn off target LED_YELLOW */
#define LED_YELLOW_TOGGLE() \
    GPIO_PinToggle(BOARD_LED_YELLOW_GPIO, BOARD_LED_YELLOW_GPIO_PIN) /*!< Toggle on target LED_YELLOW */

/* Board SW PIN */
#ifndef BOARD_SW1_GPIO
#define BOARD_SW1_GPIO GPIO
#endif
#ifndef BOARD_SW1_GPIO_PIN
#define BOARD_SW1_GPIO_PIN 24U
#endif
#define BOARD_SW1_IRQ         GPIO_IRQn
#define BOARD_SW1_IRQ_HANDLER GPIO_IRQHandler
#define BOARD_SW1_NAME        "SW1"

#ifndef BOARD_SW2_GPIO
#define BOARD_SW2_GPIO GPIO
#endif
#ifndef BOARD_SW2_GPIO_PIN
#define BOARD_SW2_GPIO_PIN 26U
#endif
#define BOARD_SW2_IRQ         GPIO_IRQn
#define BOARD_SW2_IRQ_HANDLER GPIO_IRQHandler
#define BOARD_SW2_NAME        "SW2"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * API
 ******************************************************************************/

void BOARD_InitDebugConsole(void);

/* Only used for mbedtls entropy, implemented in board_hash.c */
void BOARD_GetHash(uint8_t *buf, uint32_t *len);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */
