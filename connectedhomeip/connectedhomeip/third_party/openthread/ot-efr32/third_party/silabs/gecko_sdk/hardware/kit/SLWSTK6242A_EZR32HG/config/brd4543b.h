/***************************************************************************//**
 * @file
 * @brief Provide HAL configuration parameters.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef BRD4543B_H
#define BRD4543B_H

#include "hal-config-types.h"
/* Compatibility mapping to similar radio board. Frequency, matching is the same.
 *    Micro can be different though. */
#define RADIO_BOARD BRD4503A

// -----------------------------------------------------------------------------
/* BUTTON */
// Enable two buttons, 0 and 1
#define HAL_BUTTON_COUNT     2
#define HAL_BUTTON_ENABLE    { 0, 1 }
// Board has two buttons
#define BSP_BUTTON_COUNT     2
#define BSP_BUTTON_INIT                    \
  {                                        \
    { BSP_BUTTON0_PORT, BSP_BUTTON0_PIN }, \
    { BSP_BUTTON1_PORT, BSP_BUTTON1_PIN }  \
  }
// Initialize button GPIO DOUT to 0
#define BSP_BUTTON_GPIO_DOUT HAL_GPIO_DOUT_LOW
// Initialize button GPIO mode as input
#define BSP_BUTTON_GPIO_MODE HAL_GPIO_MODE_INPUT
// Define individual button GPIO port/pin
#define BSP_BUTTON0_PORT     gpioPortC
#define BSP_BUTTON0_PIN      9
#define BSP_BUTTON1_PORT     gpioPortC
#define BSP_BUTTON1_PIN      10

// -----------------------------------------------------------------------------
/* CLK */
// Set up HFCLK source as HFXO
#define HAL_CLK_HFCLK_SOURCE HAL_CLK_HFCLK_SOURCE_HFXO
// Setup LFCLK source as LFRCO
#define HAL_CLK_LFCLK_SOURCE HAL_CLK_LFCLK_SOURCE_LFRCO
// Set HFXO frequency as 24MHz
#define BSP_CLK_HFXO_FREQ 24000000UL
// HFXO initialization settings
#define BSP_CLK_HFXO_INIT CMU_HFXOINIT_DEFAULT
// Board has HFXO
#define BSP_CLK_HFXO_PRESENT 1
// Set LFXO frequency as 32.768kHz
#define BSP_CLK_LFXO_FREQ 32768UL
// Board has LFXO
#define BSP_CLK_LFXO_PRESENT 1

// -----------------------------------------------------------------------------
/* EXTDEV */
#define HAL_EXTDEV_ENABLE       1
// BSP_EXTDEV_SDN, BSP_EXTDEV_INT, BSP_EXTDEV_CS defined by device header

// -----------------------------------------------------------------------------
/* EZRADIOPRO */
#define HAL_EZRADIOPRO_ENABLE         1
#define BSP_EZRADIOPRO_USART          USARTRF1
#define BSP_EZRADIOPRO_FREQ           10
#define HAL_EZRADIOPRO_SHUTDOWN_SLEEP 1

// -----------------------------------------------------------------------------
/* LED */
// Enable two LEDs, 0 and 1
#define HAL_LED_COUNT  2
#define HAL_LED_ENABLE { 0, 1 }
// Board has two LEDs
#define BSP_LED_COUNT  2
#define BSP_LED_INIT                 \
  {                                  \
    { BSP_LED0_PORT, BSP_LED0_PIN }, \
    { BSP_LED1_PORT, BSP_LED1_PIN }  \
  }
// Define individual LED GPIO port/pin
#define BSP_LED0_PIN  4
#define BSP_LED0_PORT gpioPortF
#define BSP_LED1_PIN  2
#define BSP_LED1_PORT gpioPortF

// -----------------------------------------------------------------------------
/* LEUART0 */
#define HAL_LEUART0_BAUD_RATE             115200
#define HAL_LEUART0_FLOW_CONTROL          uartdrvFlowControlHw
#define HAL_LEUART0_RXSTART               16
#define HAL_LEUART0_RXSTOP                16
#define HAL_LEUART0_RX_QUEUE_SIZE         64
#define HAL_LEUART0_TX_QUEUE_SIZE         128
#define BSP_LEUART0_CTS_PIN               2
#define BSP_LEUART0_CTS_PORT              gpioPortF
#define BSP_LEUART0_ROUTE_LOC             _LEUART_ROUTE_LOCATION_LOC0
#define BSP_LEUART0_RTS_PIN               4
#define BSP_LEUART0_RTS_PORT              gpioPortF
#define BSP_LEUART0_RX_PIN                5
#define BSP_LEUART0_RX_PORT               gpioPortD
#define BSP_LEUART0_TX_PIN                4
#define BSP_LEUART0_TX_PORT               gpioPortD

// -----------------------------------------------------------------------------
/* USART0 */
#define BSP_USART0_CLK_PIN               12
#define BSP_USART0_CLK_PORT              gpioPortE
#define BSP_USART0_CS_PIN                13
#define BSP_USART0_CS_PORT               gpioPortE
#define BSP_USART0_MISO_PIN              11
#define BSP_USART0_MISO_PORT             gpioPortE
#define BSP_USART0_MOSI_PIN              10
#define BSP_USART0_MOSI_PORT             gpioPortE
#define BSP_USART0_ROUTE_LOC             _USART_ROUTE_LOCATION_LOC0

// -----------------------------------------------------------------------------
/* VCOM */
#define BSP_VCOM_PRESENT                 1
#define BSP_VCOM_ENABLE_PIN              8
#define BSP_VCOM_ENABLE_PORT             gpioPortC
#define BSP_VCOM_CTS_PIN                 2
#define BSP_VCOM_CTS_PORT                gpioPortF
#define BSP_VCOM_ROUTE_LOC               _LEUART_ROUTE_LOCATION_LOC0
#define BSP_VCOM_RTS_PIN                 4
#define BSP_VCOM_RTS_PORT                gpioPortF
#define BSP_VCOM_RX_PIN                  5
#define BSP_VCOM_RX_PORT                 gpioPortD
#define BSP_VCOM_TX_PIN                  4
#define BSP_VCOM_TX_PORT                 gpioPortD
#define BSP_VCOM_USART                   HAL_SERIAL_PORT_LEUART0

#endif //BRD4543B_H
