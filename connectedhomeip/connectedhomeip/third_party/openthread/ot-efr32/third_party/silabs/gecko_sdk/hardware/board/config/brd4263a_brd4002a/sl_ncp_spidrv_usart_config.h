/***************************************************************************//**
 * @file
 * @brief Open thread NCP spidrv usart configuration file.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_NCP_SPIDRV_USART_CONFIG_H
#define SL_NCP_SPIDRV_USART_CONFIG_H
#include "spidrv.h"

// <h> NCP spidrv usart Configuration

// <o SL_NCP_SPIDRV_USART_BIT_ORDER> Bit order on the SPI bus
// <spidrvBitOrderLsbFirst=> LSB transmitted first
// <spidrvBitOrderMsbFirst=> MSB transmitted first
#define SL_NCP_SPIDRV_USART_BIT_ORDER         spidrvBitOrderMsbFirst

// <o SL_NCP_SPIDRV_USART_CLOCK_MODE> SPI clock mode
// <spidrvClockMode0=> SPI mode 0: CLKPOL=0, CLKPHA=0
// <spidrvClockMode1=> SPI mode 1: CLKPOL=0, CLKPHA=1
// <spidrvClockMode2=> SPI mode 2: CLKPOL=1, CLKPHA=0
// <spidrvClockMode3=> SPI mode 3: CLKPOL=1, CLKPHA=1
#define SL_NCP_SPIDRV_USART_CLOCK_MODE        spidrvClockMode0

// <o SL_NCP_SPIDRV_USART_CS_FALLING_EDGE_INT_NO> Chip Select Interrupt Number on Falling Edge
// <i> Default: 10
#define SL_NCP_SPIDRV_USART_CS_FALLING_EDGE_INT_NO  0

// <o SL_NCP_SPIDRV_USART_CS_RISING_EDGE_INT_NO> Chip Select Interrupt Number on Rising Edge
// <i> Default: 9
#define SL_NCP_SPIDRV_USART_CS_RISING_EDGE_INT_NO   1
// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <gpio> SL_NCP_SPIDRV_USART_HOST_INT
// $[GPIO_SL_NCP_SPIDRV_USART_HOST_INT]
#define SL_NCP_SPIDRV_USART_HOST_INT_PORT        gpioPortC
#define SL_NCP_SPIDRV_USART_HOST_INT_PIN         6

// [GPIO_SL_NCP_SPIDRV_USART_HOST_INT]$

// <usart signal=TX,RX,CLK,CS> SL_NCP_SPIDRV_USART
// $[USART_SL_NCP_SPIDRV_USART]
#define SL_NCP_SPIDRV_USART_PERIPHERAL           USART0
#define SL_NCP_SPIDRV_USART_PERIPHERAL_NO        0

// USART0 TX on PC00
#define SL_NCP_SPIDRV_USART_TX_PORT              gpioPortC
#define SL_NCP_SPIDRV_USART_TX_PIN               0

// USART0 RX on PC01
#define SL_NCP_SPIDRV_USART_RX_PORT              gpioPortC
#define SL_NCP_SPIDRV_USART_RX_PIN               1

// USART0 CLK on PC02
#define SL_NCP_SPIDRV_USART_CLK_PORT             gpioPortC
#define SL_NCP_SPIDRV_USART_CLK_PIN              2

// USART0 CS on PC03
#define SL_NCP_SPIDRV_USART_CS_PORT              gpioPortC
#define SL_NCP_SPIDRV_USART_CS_PIN               3

// [USART_SL_NCP_SPIDRV_USART]$
// <<< sl:end pin_tool >>>

#endif /* SL_NCP_SPIDRV_USART_CONFIG_H */
