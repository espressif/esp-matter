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
#ifndef SL_WSRCP_UART_H
#define SL_WSRCP_UART_H

#include <stdint.h>
#include <stdbool.h>
#include <cmsis_os2.h>
#include <em_device.h>
#include <em_ldma.h>
#include <dmadrv.h>

#if defined(EUSART_PRESENT)
#include <em_eusart.h>
#else
#include <em_usart.h>
#endif

#include "sl_ring.h"

struct sl_wsrcp_uart {
#if defined(EUSART_PRESENT)
    EUSART_TypeDef *hw_regs;
#else
    USART_TypeDef  *hw_regs;
#endif

    unsigned int dma_chan_tx;
    int descr_cnt_tx;
    LDMA_Descriptor_t descr_tx[2];
    // Application will be less efficient if a message need to be sent in 2 or
    // more buffers. Choose a correct size depending of yours needs.
    uint8_t buf_tx[2][1024];

    unsigned int dma_chan_rx;
    int descr_cnt_rx;
    LDMA_Descriptor_t descr_rx[16];
    // Reserve enough buffers since interactive session may consume one buffer
    // for each char.
    uint8_t buf_rx[16][128];

    osMutexId_t tx_lock;
    osSemaphoreId_t tx_dma_lock;
    int irq_overflow_cnt;

    // Note it may be possible to drop rx_ring and save 4kB of data. The user
    // would get the data directly from buf_rx. However, navigating in buf_rx is
    // not easy and is error prone.
    struct ring rx_ring;
    uint8_t rx_ring_data[4096];
};

void uart_init(struct sl_wsrcp_uart *uart);
int uart_tx(struct sl_wsrcp_uart *uart, const void *buf, int buf_len);
int uart_rx(struct sl_wsrcp_uart *uart, void *buf, int buf_len);

// Called when a CRC error is detected in receveided frames before the frame is
// discarded. This funtion is declared "weak". So, the user can overload it and
// choose to increment a counter or report the error to the host.
void uart_crc_error(struct sl_wsrcp_uart *uart, uint16_t crc, int frame_len, uint8_t header, uint8_t irq_err_counter);

// Called from IRQ when new data are available. This funtion is declared "weak".
// So, the user can overload it. The user can post de necessary events from this
// callback (using osEventFlagsSet, osSemaphoreRelease, etc...). The user MUST
// NOT sleep in this function. He should neither handle data in this callback.
void uart_rx_ready(struct sl_wsrcp_uart *uart);

// Helpers for EUSART and USART drivers
void uart_handle_rx_dma_timeout(struct sl_wsrcp_uart *uart);
void uart_handle_rx_overflow(struct sl_wsrcp_uart *uart);

// Must be implemented by EUSART and USART drivers
void uart_hw_init(struct sl_wsrcp_uart *uart);

#endif
