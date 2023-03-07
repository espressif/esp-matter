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
#include <em_usart.h>
#include <em_ldma.h>
#include <em_cmu.h>
#include <em_gpio.h>
#include <dmadrv.h>

#include "sl_wsrcp.h"
#include "sl_wsrcp_log.h"
#include "sl_wsrcp_uart.h"
#include "sl_wsrcp_uart_config.h"
#include "sl_wsrcp_utils.h"

#ifndef USART_ROUTEPEN_TXPEN
#error Not supported
#endif

static struct sl_wsrcp_uart *g_uart_ctxt;

void uart_hw_init(struct sl_wsrcp_uart *uart_ctxt)
{
    USART_InitAsync_TypeDef uart_cfg = USART_INITASYNC_DEFAULT;

    g_uart_ctxt = uart_ctxt;
    //CORE_SetNvicRamTableHandler(UART_RX_IRQ, uart_rx_irq);
    NVIC_ClearPendingIRQ(UART_RX_IRQ);
    NVIC_EnableIRQ(UART_RX_IRQ);
#if defined(_CMU_HFPERCLKEN0_MASK)
    CMU_ClockEnable(cmuClock_HFPER, true);
#endif
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(UART_CLOCK, true);

    GPIO_PinModeSet(UART_PORT_TX, UART_PIN_TX, gpioModePushPull, 1);
    GPIO_PinModeSet(UART_PORT_RX, UART_PIN_RX, gpioModeInputPull, 1);

    uart_cfg.enable = usartDisable;
    USART_InitAsync(uart_ctxt->hw_regs, &uart_cfg);

    uart_ctxt->hw_regs->ROUTELOC0 &= ~(_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK);
    uart_ctxt->hw_regs->ROUTELOC0 |= UART_LOC_TX << _USART_ROUTELOC0_TXLOC_SHIFT;
    uart_ctxt->hw_regs->ROUTELOC0 |= UART_LOC_RX << _USART_ROUTELOC0_RXLOC_SHIFT;
    uart_ctxt->hw_regs->ROUTEPEN = USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN;
    uart_ctxt->hw_regs->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
    uart_ctxt->hw_regs->TIMECMP1 = USART_TIMECMP1_TSTOP_RXACT | \
                                  USART_TIMECMP1_TSTART_RXEOF | \
                                  USART_TIMECMP1_RESTARTEN | \
                                  (0xff << _USART_TIMECMP1_TCMPVAL_SHIFT);

    USART_IntClear(uart_ctxt->hw_regs, 0xFFFFFFFF);
    //USART_IntEnable(uart_ctxt->hw_regs, USART_IF_RXDATAV);
    USART_IntEnable(uart_ctxt->hw_regs, USART_IF_RXOF);
    USART_IntEnable(uart_ctxt->hw_regs, USART_IF_TCMP1);
    USART_Enable(uart_ctxt->hw_regs, usartEnable);
}

void USART0_RX_IRQHandler(void)
{
    struct sl_wsrcp_uart *uart_ctxt = g_uart_ctxt;

    BUG_ON(!uart_ctxt);
    if (uart_ctxt->hw_regs->IF & USART_IF_TCMP1) {
        uart_handle_rx_dma_timeout(uart_ctxt);
        uart_ctxt->hw_regs->TIMECMP1 &= ~_USART_TIMECMP1_TSTART_MASK;
        uart_ctxt->hw_regs->TIMECMP1 |= USART_TIMECMP1_TSTART_RXEOF;
        USART_IntClear(uart_ctxt->hw_regs, USART_IF_TCMP1);
        return;
    }
    if (uart_ctxt->hw_regs->IF & USART_IF_RXOF) {
        uart_handle_rx_overflow(uart_ctxt);
        USART_IntClear(uart_ctxt->hw_regs, USART_IF_RXOF);
        return;
    }
    WARN("unexpected IRQ");
}
