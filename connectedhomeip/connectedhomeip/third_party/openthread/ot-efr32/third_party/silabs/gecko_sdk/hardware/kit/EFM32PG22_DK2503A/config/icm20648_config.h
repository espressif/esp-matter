/***************************************************************************//**
 * @file
 * @brief Configuration file for the Invensense ICM20648 6-axis motion sensor
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
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
#define ICM20648_SPI_USART           USART0           /**< UART instance               */
#define ICM20648_SPI_USART_INDEX     0                /**< UART index                  */
#define ICM20648_SPI_CLK             cmuClock_USART0  /**< HFPER Clock                 */

#define ICM20648_PORT_SPI_MOSI       gpioPortC        /**< MOSI port definition        */
#define ICM20648_PIN_SPI_MOSI        0                /**< MOSI pin definition         */
#define ICM20648_PORT_SPI_MISO       gpioPortC        /**< MISO port definition        */
#define ICM20648_PIN_SPI_MISO        1                /**< MISO pin definition         */
#define ICM20648_PORT_SPI_SCLK       gpioPortC        /**< SCLK port definition        */
#define ICM20648_PIN_SPI_SCLK        2                /**< SCLK pin definition         */
#define ICM20648_PORT_SPI_CS         gpioPortB        /**< ICM20648 CS port definition */
#define ICM20648_PIN_SPI_CS          2                /**< ICM20648 CS pin definition  */

#endif /* ICM20648_CONFIG_H_ */
