/**
 * @file
 * Serial API Configuration
 * @copyright 2022 Silicon Laboratories Inc.
 */
#ifndef SERIAL_API_CONFIG_H
#define SERIAL_API_CONFIG_H

#include <em_gpio.h>

// <<< sl:start pin_tool >>>

// <usart signal=TX,RX> SERIAL_API

// $[USART_SERIAL_API]
#define SERIAL_API_PERIPHERAL     USART0
#define SERIAL_API_PERIPHERAL_NO  0

#define SERIAL_API_TX_PORT        gpioPortA
#define SERIAL_API_TX_PIN         0
#define SERIAL_API_TX_LOC         0

#define SERIAL_API_RX_PORT        gpioPortA
#define SERIAL_API_RX_PIN         1
#define SERIAL_API_RX_LOC         0
// [USART_SERIAL_API]$

// <<< sl:end pin_tool >>>

#endif // SERIAL_API_CONFIG_H
