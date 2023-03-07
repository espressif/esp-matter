/***************************************************************************//**
 * Copyright 2021 Silicon Laboratories Inc. www.silabs.com
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available here[1]. This software is distributed to you in
 * Source Code format and is governed by the sections of the MSLA applicable to
 * Source Code.
 *
 * [1] www.silabs.com/about-us/legal/master-software-license-agreement
 *
 ******************************************************************************/
#ifndef SL_WSRCP_UART_CONFIG_H
#define SL_WSRCP_UART_CONFIG_H

#if defined(EUSART_PRESENT)

#include <em_eusart.h>
#define UART_PERIPHERAL         EUSART0
#define UART_CLOCK              cmuClock_EUSART0
#define UART_RX_IRQ             EUSART0_RX_IRQn
#define UART_LDMA_SIGNAL_RX     ldmaPeripheralSignal_EUSART0_RXFL
#define UART_LDMA_SIGNAL_TX     ldmaPeripheralSignal_EUSART0_TXFL

#define UART_PORT_TX            gpioPortA
#define UART_PIN_TX             8
#define UART_LOC_TX             0

#define UART_PORT_RX            gpioPortA
#define UART_PIN_RX             9
#define UART_LOC_RX             0


#else

#include <em_usart.h>
#define UART_PERIPHERAL         USART0
#define UART_CLOCK              cmuClock_USART0
#define UART_RX_IRQ             USART0_RX_IRQn
#define UART_LDMA_SIGNAL_RX     ldmaPeripheralSignal_USART0_RXDATAV
#define UART_LDMA_SIGNAL_TX     ldmaPeripheralSignal_USART0_TXBL

#define UART_PORT_TX            gpioPortA
#define UART_PIN_TX             0
#define UART_LOC_TX             0

#define UART_PORT_RX            gpioPortA
#define UART_PIN_RX             1
#define UART_LOC_RX             0

#endif

#endif
