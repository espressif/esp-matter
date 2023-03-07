/***************************************************************************//**
 * @file
 * @brief configuration for on-board serial flash.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef MX25CONFIG_H
#define MX25CONFIG_H

#include "em_device.h"
#include "em_gpio.h"

#define MX25_PORT_MOSI         gpioPortC
#define MX25_PIN_MOSI          1
#define MX25_PORT_MISO         gpioPortC
#define MX25_PIN_MISO          2
#define MX25_PORT_SCLK         gpioPortC
#define MX25_PIN_SCLK          3
#define MX25_PORT_CS           gpioPortC
#define MX25_PIN_CS            4

#define MX25_USART             USART0
#define MX25_USART_ROUTE       GPIO->USARTROUTE[0]
#define MX25_USART_CLK         cmuClock_USART0

#endif // MX25CONFIG_H
