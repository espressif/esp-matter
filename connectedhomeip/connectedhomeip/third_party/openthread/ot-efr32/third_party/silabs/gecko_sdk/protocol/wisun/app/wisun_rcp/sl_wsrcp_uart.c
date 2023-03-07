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

#include <string.h>
#include <em_cmu.h>
#include <em_gpio.h>

#include "sl_wsrcp.h"
#include "sl_wsrcp_crc.h"
#include "sl_wsrcp_log.h"
#include "sl_wsrcp_utils.h"
#include "sl_wsrcp_uart.h"
#include "sl_wsrcp_uart_config.h"
#include "sl_wsrcp_mac.h"

// Used for debug to display the data sent/received on the bus
static char trace_buffer[128];

__WEAK void uart_rx_ready(struct sl_wsrcp_uart *uart_ctxt)
{
    (void)uart_ctxt;
}

__WEAK void uart_crc_error(struct sl_wsrcp_uart *uart_ctxt, uint16_t crc, int frame_len, uint8_t header, uint8_t irq_overflow_cnt)
{
    (void)uart_ctxt;
    (void)crc;
    (void)frame_len;
    (void)header;
    (void)irq_overflow_cnt;
}

static bool uart_handle_rx_dma_complete(unsigned int chan, unsigned int seq_num, void *user_param)
{
    struct sl_wsrcp_uart *uart_ctxt = user_param;
    int ret;
    unsigned int i;

    (void)chan;
    (void)seq_num;

    for (i = 0; i < sizeof(uart_ctxt->buf_rx[0]); i++) {
        ret = ring_push(&uart_ctxt->rx_ring, uart_ctxt->buf_rx[uart_ctxt->descr_cnt_rx][i]);
        BUG_ON(ret, "buffer overflow");
    }
    uart_ctxt->descr_cnt_rx += 1;
    uart_ctxt->descr_cnt_rx %= ARRAY_SIZE(uart_ctxt->buf_rx);
    uart_rx_ready(uart_ctxt);
    return true;
}

static bool uart_handle_tx_dma_complete(unsigned int chan, unsigned int seq_num, void *user_param)
{
    struct sl_wsrcp_uart *uart_ctxt = user_param;

    (void)chan;
    (void)seq_num;

    osSemaphoreRelease(uart_ctxt->tx_dma_lock);
    return true;
}

void uart_handle_rx_dma_timeout(struct sl_wsrcp_uart *uart_ctxt)
{
    LDMA_TransferCfg_t ldma_cfg = LDMA_TRANSFER_CFG_PERIPHERAL(UART_LDMA_SIGNAL_RX);
    int remaining, descr_cnt_rx, ret;
    size_t i;

    // Begin of realtime constrained section
    // (with USART, we need to execute that in less than 5µs for a 2Mbps UART link)
    // (with EUSART, thanks to it 16bytes depth fifo , we need to execute the
    // code below in less than 40µs for a 4Mbps UART link)
    DMADRV_StopTransfer(uart_ctxt->dma_chan_rx);
    DMADRV_TransferRemainingCount(uart_ctxt->dma_chan_rx, &remaining);
    descr_cnt_rx = uart_ctxt->descr_cnt_rx;
    uart_ctxt->descr_cnt_rx += 1;
    uart_ctxt->descr_cnt_rx %= ARRAY_SIZE(uart_ctxt->buf_rx);
    DMADRV_LdmaStartTransfer(uart_ctxt->dma_chan_rx, &ldma_cfg,
                             &(uart_ctxt->descr_rx[uart_ctxt->descr_cnt_rx]),
                             uart_handle_rx_dma_complete, uart_ctxt);
    // End of realtime constrained section

    for (i = 0; i < sizeof(uart_ctxt->buf_rx[0]) - remaining; i++) {
        ret = ring_push(&uart_ctxt->rx_ring, uart_ctxt->buf_rx[descr_cnt_rx][i]);
        BUG_ON(ret, "buffer overflow");
    }
    uart_rx_ready(uart_ctxt);
}

void uart_handle_rx_overflow(struct sl_wsrcp_uart *uart_ctxt)
{
    WARN("IRQ overflow");
    uart_ctxt->irq_overflow_cnt++;
}

static int append_escaped_byte(uint8_t *buffer, uint8_t byte)
{
    if (byte == 0x7D || byte == 0x7E) {
        buffer[0] = 0x7D;
        buffer[1] = byte ^ 0x20;
        return 2;
    } else {
        buffer[0] = byte;
        return 1;
    }
}

int uart_tx(struct sl_wsrcp_uart *uart_ctxt, const void *buf, int buf_len)
{
    LDMA_TransferCfg_t ldma_cfg = LDMA_TRANSFER_CFG_PERIPHERAL(UART_LDMA_SIGNAL_TX);
    LDMA_Descriptor_t *dma_descr;
    uint16_t crc = crc16(buf, buf_len);
    const uint8_t *buf8 = buf;
    uint8_t *dma_buf;
    int buf_cnt = 0;
    size_t xfer_cnt;

    // Only double buffering is supported
    BUG_ON(ARRAY_SIZE(uart_ctxt->descr_tx) != 2);
    BUG_ON(sizeof(uart_ctxt->buf_tx[0]) > DMADRV_MAX_XFER_COUNT);

    osMutexAcquire(uart_ctxt->tx_lock, osWaitForever);
    while (buf_cnt < buf_len) {
        dma_buf = uart_ctxt->buf_tx[uart_ctxt->descr_cnt_tx];
        dma_descr = &uart_ctxt->descr_tx[uart_ctxt->descr_cnt_tx];
        xfer_cnt = 0;
        while (buf_cnt < buf_len && xfer_cnt < sizeof(uart_ctxt->buf_tx[0]) - 7) {
            xfer_cnt += append_escaped_byte(dma_buf + xfer_cnt, buf8[buf_cnt]);
            buf_cnt++;
        }
        if (buf_cnt == buf_len) {
            xfer_cnt += append_escaped_byte(dma_buf + xfer_cnt, crc & 0xFF);
            xfer_cnt += append_escaped_byte(dma_buf + xfer_cnt, crc >> 8);
            dma_buf[xfer_cnt++] = 0x7E;
        }
        xfer_cnt--;
        dma_descr->xfer.xferCnt = xfer_cnt;
        osSemaphoreAcquire(uart_ctxt->tx_dma_lock, osWaitForever);
        DMADRV_LdmaStartTransfer(uart_ctxt->dma_chan_tx, &ldma_cfg,
                                 dma_descr, uart_handle_tx_dma_complete, uart_ctxt);
        uart_ctxt->descr_cnt_tx = (uart_ctxt->descr_cnt_tx + 1) % ARRAY_SIZE(uart_ctxt->descr_tx);
    }
    osMutexRelease(uart_ctxt->tx_lock);

    TRACE(TR_HDLC, "hdlc tx: %s (%d bytes)",
           bytes_str(buf, buf_len, NULL, trace_buffer, sizeof(trace_buffer), DELIM_SPACE | ELLIPSIS_STAR), buf_len);
    return buf_len;
}

int uart_rx(struct sl_wsrcp_uart *uart_ctxt, void *buf, int buf_len)
{
    uint8_t *buf8 = buf;
    uint16_t crc;
    int i, frame_len;
    int data;

    while (ring_get(&uart_ctxt->rx_ring, 0) == 0x7E)
        ring_pop(&uart_ctxt->rx_ring);

    for (i = 0, data = 0; data != 0x7E; i++) {
        data = ring_get(&uart_ctxt->rx_ring, i);
        if (data < 0)
            return 0;
    }

    frame_len = 0;
    do {
        BUG_ON(frame_len >= buf_len);
        data = ring_pop(&uart_ctxt->rx_ring);
        BUG_ON(data < 0);
        if (data == 0x7D)
            buf8[frame_len++] = ring_pop(&uart_ctxt->rx_ring) ^ 0x20;
        else if (data != 0x7E)
            buf8[frame_len++] = data;
    } while (data != 0x7E);

    BUG_ON(frame_len <= 2);
    frame_len -= sizeof(uint16_t);
    crc = crc16(buf8, frame_len);
    if (memcmp(buf8 + frame_len, &crc, sizeof(uint16_t))) {
        WARN("bad crc, frame dropped");
        uart_crc_error(uart_ctxt, *(uint16_t *)(buf8 + frame_len), frame_len, buf8[0], uart_ctxt->irq_overflow_cnt);
        uart_ctxt->irq_overflow_cnt = 0;
        return 0;
    }
    TRACE(TR_HDLC, "hdlc rx: %s (%d bytes)",
           bytes_str(buf, frame_len, NULL, trace_buffer, sizeof(trace_buffer), DELIM_SPACE | ELLIPSIS_STAR), frame_len);
    return frame_len;
}

void uart_init(struct sl_wsrcp_uart *uart_ctxt)
{
    LDMA_TransferCfg_t ldma_cfg = LDMA_TRANSFER_CFG_PERIPHERAL(UART_LDMA_SIGNAL_RX);
    unsigned int i, next;

    ring_init(&uart_ctxt->rx_ring, uart_ctxt->rx_ring_data, sizeof(uart_ctxt->rx_ring_data));
    uart_ctxt->tx_lock = osMutexNew(NULL);
    uart_ctxt->tx_dma_lock = osSemaphoreNew(1, 1, NULL);
    uart_ctxt->hw_regs = UART_PERIPHERAL;
    for (i = 0; i < ARRAY_SIZE(uart_ctxt->descr_rx); i++) {
        uart_ctxt->descr_rx[i].xfer.structType  = ldmaCtrlStructTypeXfer;
        uart_ctxt->descr_rx[i].xfer.blockSize   = ldmaCtrlBlockSizeUnit1;
        uart_ctxt->descr_rx[i].xfer.reqMode     = ldmaCtrlReqModeBlock;
        uart_ctxt->descr_rx[i].xfer.doneIfs     = 1;

        uart_ctxt->descr_rx[i].xfer.size        = ldmaCtrlSizeByte;
        uart_ctxt->descr_rx[i].xfer.xferCnt     = sizeof(uart_ctxt->buf_rx[0]) - 1;

        uart_ctxt->descr_rx[i].xfer.srcInc      = ldmaCtrlSrcIncNone;
        uart_ctxt->descr_rx[i].xfer.srcAddrMode = ldmaCtrlSrcAddrModeAbs;
        uart_ctxt->descr_rx[i].xfer.srcAddr     = (uintptr_t)&(uart_ctxt->hw_regs->RXDATA);

        uart_ctxt->descr_rx[i].xfer.dstInc      = ldmaCtrlDstIncOne;
        uart_ctxt->descr_rx[i].xfer.dstAddrMode = ldmaCtrlDstAddrModeAbs;
        uart_ctxt->descr_rx[i].xfer.dstAddr     = (uintptr_t)&(uart_ctxt->buf_rx[i]);

        uart_ctxt->descr_rx[i].xfer.linkMode    = ldmaLinkModeAbs;
        uart_ctxt->descr_rx[i].xfer.link        = 1;

        next = i + 1;
        next %= ARRAY_SIZE(uart_ctxt->descr_rx);
        uart_ctxt->descr_rx[i].xfer.linkAddr = ((uintptr_t)&(uart_ctxt->descr_rx[next])) >> _LDMA_CH_LINK_LINKADDR_SHIFT;
    }

    for (i = 0; i < ARRAY_SIZE(uart_ctxt->descr_tx); i++) {
        uart_ctxt->descr_tx[i].xfer.structType  = ldmaCtrlStructTypeXfer;
        uart_ctxt->descr_tx[i].xfer.blockSize   = ldmaCtrlBlockSizeUnit1;
        uart_ctxt->descr_tx[i].xfer.reqMode     = ldmaCtrlReqModeBlock;
        uart_ctxt->descr_tx[i].xfer.doneIfs     = 1;

        uart_ctxt->descr_tx[i].xfer.size        = ldmaCtrlSizeByte;

        uart_ctxt->descr_tx[i].xfer.srcInc      = ldmaCtrlDstIncOne;
        uart_ctxt->descr_tx[i].xfer.srcAddrMode = ldmaCtrlDstAddrModeAbs;
        uart_ctxt->descr_tx[i].xfer.srcAddr     = (uintptr_t)&(uart_ctxt->buf_tx[i]);

        uart_ctxt->descr_tx[i].xfer.dstInc      = ldmaCtrlSrcIncNone;
        uart_ctxt->descr_tx[i].xfer.dstAddrMode = ldmaCtrlSrcAddrModeAbs;
        uart_ctxt->descr_tx[i].xfer.dstAddr     = (uintptr_t)&(uart_ctxt->hw_regs->TXDATA);
    }
    uart_hw_init(uart_ctxt);
    DMADRV_Init();
    DMADRV_AllocateChannel(&uart_ctxt->dma_chan_tx, NULL);
    DMADRV_AllocateChannel(&uart_ctxt->dma_chan_rx, NULL);
    DMADRV_LdmaStartTransfer(uart_ctxt->dma_chan_rx, &ldma_cfg,
                             &(uart_ctxt->descr_rx[0]),
                             uart_handle_rx_dma_complete, uart_ctxt);
}
