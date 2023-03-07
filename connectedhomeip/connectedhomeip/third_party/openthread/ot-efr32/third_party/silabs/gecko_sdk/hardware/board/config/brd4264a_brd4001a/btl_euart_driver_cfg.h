/***************************************************************************//**
 * @file
 * @brief Configuration header for bootloader euart Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef BTL_EUART_DRIVER_CONFIG_H
#define BTL_EUART_DRIVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>EUART settings

// <o SL_SERIAL_EUART_BAUD_RATE> Baud rate
// <i> Default: 115200
#define SL_SERIAL_EUART_BAUD_RATE              115200

// <e SL_SERIAL_EUART_FLOW_CONTROL> Hardware flow control
// <i> Default: 0
#define SL_SERIAL_EUART_FLOW_CONTROL     0
// </e>

// <o SL_DRIVER_EUART_RX_BUFFER_SIZE> Receive buffer size
// <0-2048:1>
// <i> Default: 512 [0-2048]
#define SL_DRIVER_EUART_RX_BUFFER_SIZE    512

// <o SL_DRIVER_EUART_TX_BUFFER_SIZE> Transmit buffer size
// <0-2048:1>
// <i> Default: 128 [0-2048]
#define SL_DRIVER_EUART_TX_BUFFER_SIZE    128

// <e SL_VCOM_ENABLE> Virtual COM Port
// <i> Default: 0
#define SL_VCOM_ENABLE                    0
// </e>

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <eusart signal=TX,RX,(CTS),(RTS)> SL_SERIAL_EUART
// $[EUSART_SL_SERIAL_EUART]
#define SL_SERIAL_EUART_PERIPHERAL               EUSART2
#define SL_SERIAL_EUART_PERIPHERAL_NO            2

// EUSART2 TX on PD02
#define SL_SERIAL_EUART_TX_PORT                  gpioPortD
#define SL_SERIAL_EUART_TX_PIN                   2

// EUSART2 RX on PD03
#define SL_SERIAL_EUART_RX_PORT                  gpioPortD
#define SL_SERIAL_EUART_RX_PIN                   3

// EUSART2 CTS on PD05
#define SL_SERIAL_EUART_CTS_PORT                 gpioPortD
#define SL_SERIAL_EUART_CTS_PIN                  5

// EUSART2 RTS on PD04
#define SL_SERIAL_EUART_RTS_PORT                 gpioPortD
#define SL_SERIAL_EUART_RTS_PIN                  4

// [EUSART_SL_SERIAL_EUART]$


// <gpio optional=true> SL_VCOM_ENABLE

// $[GPIO_SL_VCOM_ENABLE]
#define SL_VCOM_ENABLE_PORT                      gpioPortA
#define SL_VCOM_ENABLE_PIN                       8

// [GPIO_SL_VCOM_ENABLE]$


// <<< sl:end pin_tool >>>

#endif // BTL_EUART_DRIVER_CONFIG_H