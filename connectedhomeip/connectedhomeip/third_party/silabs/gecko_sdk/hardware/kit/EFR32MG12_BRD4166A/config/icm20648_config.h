/***************************************************************************//**
 * @file
 * @brief Configuration file for the Invensense ICM20648 6-axis motion sensor
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

#ifndef ICM20648_CONFIG_H
#define ICM20648_CONFIG_H

#define ICM20648_USART_INITSYNC                                                            \
  {                                                                                        \
    usartEnable,     /* Enable RX/TX when init completed                                */ \
    0,               /* Use current configured reference clock for configuring baudrate */ \
    1000000,         /* 1 Mbits/s                                                       */ \
    usartDatabits8,  /* 8 databits                                                      */ \
    true,            /* Master mode                                                     */ \
    true,            /* Send least significant bit first                                */ \
    usartClockMode0, /* Clock idle low, sample on rising edge                           */ \
    false,           /* Not USART PRS input mode                                        */ \
    usartPrsRxCh0,   /* PRS channel 0                                                   */ \
    false,           /* No AUTOTX mode                                                  */ \
    false,           /* No AUTOCS mode                                                  */ \
    0,               /* Auto CS Hold cycles                                             */ \
    0                /* Auto CS Setup cycles                                            */ \
  }

/* SPI bus related definitions */
#define ICM20648_SPI_USART           USART3           /**< UART instance               */
#define ICM20648_SPI_CLK             cmuClock_USART3  /**< HFPER Clock                 */

#define ICM20648_LOCATION_SPI_MOSI   18               /**< Location of the MOSI pin    */
#define ICM20648_LOCATION_SPI_MISO   18               /**< Location of the MISO pin    */
#define ICM20648_LOCATION_SPI_SCLK   18               /**< Location of the SCLK pin    */

#define ICM20648_PORT_SPI_MOSI       gpioPortC        /**< MOSI port definition        */
#define ICM20648_PIN_SPI_MOSI        0                /**< MOSI pin definition         */
#define ICM20648_PORT_SPI_MISO       gpioPortC        /**< MISO port definition        */
#define ICM20648_PIN_SPI_MISO        1                /**< MISO pin definition         */
#define ICM20648_PORT_SPI_SCLK       gpioPortC        /**< SCLK port definition        */
#define ICM20648_PIN_SPI_SCLK        2                /**< SCLK pin definition         */
#define ICM20648_PORT_SPI_CS         gpioPortC        /**< ICM20648 CS port definition */
#define ICM20648_PIN_SPI_CS          3                /**< ICM20648 CS pin definition  */

#endif /* ICM20648_CONFIG_H_ */
