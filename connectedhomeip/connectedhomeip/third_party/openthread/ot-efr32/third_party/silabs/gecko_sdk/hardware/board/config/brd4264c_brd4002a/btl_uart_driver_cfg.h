/***************************************************************************//**
 * @file
 * @brief Configuration header for bootloader Uart Driver
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
#ifndef BTL_UART_DRIVER_CONFIG_H
#define BTL_UART_DRIVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>USART settings

// <o SL_SERIAL_UART_BAUD_RATE> Baud rate
// <i> Default: 115200
#define SL_SERIAL_UART_BAUD_RATE              115200

// <e SL_SERIAL_UART_FLOW_CONTROL> Hardware flow control
// <i> Default: 0
#define SL_SERIAL_UART_FLOW_CONTROL      0
// </e>

// <o SL_DRIVER_UART_RX_BUFFER_SIZE> Receive buffer size
// <0-2048:1>
// <i> Default: 512 [0-2048]
#define SL_DRIVER_UART_RX_BUFFER_SIZE    512

// <o SL_DRIVER_UART_TX_BUFFER_SIZE> Transmit buffer size
// <0-2048:1>
// <i> Default: 128 [0-2048]
#define SL_DRIVER_UART_TX_BUFFER_SIZE    128

// <e SL_VCOM_ENABLE> Virtual COM Port
// <i> Default: 0
#define SL_VCOM_ENABLE                    0
// </e>

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <usart signal=TX,RX,(CTS),(RTS)> SL_SERIAL_UART
// $[USART_SL_SERIAL_UART]
#define SL_SERIAL_UART_PERIPHERAL                USART0
#define SL_SERIAL_UART_PERIPHERAL_NO             0

// USART0 TX on PA08
#define SL_SERIAL_UART_TX_PORT                   gpioPortA
#define SL_SERIAL_UART_TX_PIN                    8

// USART0 RX on PA09
#define SL_SERIAL_UART_RX_PORT                   gpioPortA
#define SL_SERIAL_UART_RX_PIN                    9

// USART0 CTS on PA10
#define SL_SERIAL_UART_CTS_PORT                  gpioPortA
#define SL_SERIAL_UART_CTS_PIN                   10

// USART0 RTS on PA00
#define SL_SERIAL_UART_RTS_PORT                  gpioPortA
#define SL_SERIAL_UART_RTS_PIN                   0

// [USART_SL_SERIAL_UART]$



// <gpio optional=true> SL_VCOM_ENABLE

// $[GPIO_SL_VCOM_ENABLE]
#define SL_VCOM_ENABLE_PORT                      gpioPortB
#define SL_VCOM_ENABLE_PIN                       0

// [GPIO_SL_VCOM_ENABLE]$


// <<< sl:end pin_tool >>>

#endif // BTL_UART_DRIVER_CONFIG_H