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
#include <em_eusart.h>
#include <em_ldma.h>
#include <em_cmu.h>
#include <em_gpio.h>
#include <dmadrv.h>
#include <cmsis_os2.h>

#include "sl_wsrcp.h"
#include "sl_wsrcp_log.h"
#include "sl_wsrcp_uart.h"
#include "sl_wsrcp_uart_config.h"
#include "sl_wsrcp_utils.h"

static struct sl_wsrcp_uart *g_uart_ctxt;

void uart_hw_init(struct sl_wsrcp_uart *uart_ctxt)
{
    EUSART_UartInit_TypeDef uart_cfg = EUSART_UART_INIT_DEFAULT_HF;

    g_uart_ctxt = uart_ctxt;
    //CORE_SetNvicRamTableHandler(UART_RX_IRQ, uart_rx_irq);
    NVIC_ClearPendingIRQ(UART_RX_IRQ);
    NVIC_EnableIRQ(UART_RX_IRQ);
#if defined(_CMU_HFPERCLKEN0_MASK)
    CMU_ClockEnable(cmuClock_HFPER, true);
#endif
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_PRS, true);
    CMU_ClockEnable(UART_CLOCK, true);
    CMU_ClockSelectSet(cmuClock_EM01GRPCCLK, cmuSelect_HFRCODPLL);
    if (UART_CLOCK == cmuClock_EUSART0) {
        CMU_ClockSelectSet(cmuClock_EUSART0CLK, cmuSelect_EM01GRPCCLK);
    }

    GPIO_PinModeSet(UART_PORT_TX, UART_PIN_TX, gpioModePushPull, 1);
    GPIO_PinModeSet(UART_PORT_RX, UART_PIN_RX, gpioModeInputPull, 1);
    GPIO->EUSARTROUTE[EUSART_NUM(UART_PERIPHERAL)].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN | GPIO_EUSART_ROUTEEN_RXPEN;
    GPIO->EUSARTROUTE[EUSART_NUM(UART_PERIPHERAL)].TXROUTE = (UART_PORT_TX << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
                                 | (UART_PIN_TX << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
    GPIO->EUSARTROUTE[EUSART_NUM(UART_PERIPHERAL)].RXROUTE = (UART_PORT_RX << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
                                 | (UART_PIN_RX << _GPIO_EUSART_RXROUTE_PIN_SHIFT);

    uart_cfg.enable = eusartDisable;
    EUSART_UartInitHf(uart_ctxt->hw_regs, &uart_cfg);

    uart_ctxt->hw_regs->CFG1_SET = EUSART_CFG1_RXTIMEOUT_SEVENFRAMES;
    EUSART_IntClear(uart_ctxt->hw_regs, 0xFFFFFFFF);
    // EUSART_IntEnable(uart_ctxt->hw_regs, EUSART_IEN_RXFL);
    EUSART_IntEnable(uart_ctxt->hw_regs, EUSART_IEN_RXOF);
    EUSART_IntEnable(uart_ctxt->hw_regs, EUSART_IEN_RXTO);
    EUSART_Enable(uart_ctxt->hw_regs, eusartEnable);
}

void EUSART0_RX_IRQHandler(void)
{
    struct sl_wsrcp_uart *uart_ctxt = g_uart_ctxt;

    BUG_ON(!uart_ctxt);
    if (uart_ctxt->hw_regs->IF & EUSART_IF_RXTO) {
        uart_handle_rx_dma_timeout(uart_ctxt);
        EUSART_IntClear(uart_ctxt->hw_regs, EUSART_IF_RXTO);
        return;
    }
    if (uart_ctxt->hw_regs->IF & EUSART_IF_RXOF) {
        uart_handle_rx_overflow(uart_ctxt);
        EUSART_IntClear(uart_ctxt->hw_regs, EUSART_IF_RXOF);
        return;
    }
    WARN("unexpected IRQ");
}
