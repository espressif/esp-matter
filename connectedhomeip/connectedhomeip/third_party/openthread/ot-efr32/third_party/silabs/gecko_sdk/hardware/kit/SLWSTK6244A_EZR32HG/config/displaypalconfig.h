/***************************************************************************//**
 * @file
 * @brief Configuration file for PAL (Platform Abstraction Layer)
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

#ifndef __SILICON_LABS_DISPLAY_PAL_CONFIG_H__
#define __SILICON_LABS_DISPLAY_PAL_CONFIG_H__

/*
 * Select which oscillator should source the RTC clock.
 */
#undef  PAL_RTC_CLOCK_LFXO
#undef  PAL_RTC_CLOCK_LFRCO
#define PAL_RTC_CLOCK_ULFRCO

/*
 * PAL SPI / USART configuration for the EFM32ZG_STK3200.
 * Select which USART and location is connected to the device via SPI.
 */
#define PAL_SPI_USART_UNIT     (USART0)
#define PAL_SPI_USART_CLOCK    (cmuClock_USART0)
#define PAL_SPI_USART_LOCATION (USART_ROUTE_LOCATION_LOC0)

/*
 * Specify the SPI baud rate:
 */
#define PAL_SPI_BAUDRATE       (1100000)

/*
 * On the SLSTK3400A_EFM32HG, we can toggle some GPIO pins with hw only,
 * especially the GPIO port E pin 10 signal which is connected to the
 * polarity inversion (EXTCOMIN) pin on the Sharp Memory LCD. By defining
 * INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY the toggling of EXTCOMIN will
 * be handled by hardware, without software intervention, which saves power.
 */
#define INCLUDE_PAL_GPIO_PIN_AUTO_TOGGLE_HW_ONLY

#endif /* __SILICON_LABS_DISPLAY_PAL_CONFIG_H__ */
