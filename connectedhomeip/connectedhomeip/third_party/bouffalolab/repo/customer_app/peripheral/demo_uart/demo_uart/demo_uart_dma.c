/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <stdio.h>
#include <cli.h>
#include <hosal_uart.h>
#include <blog.h>

static uint8_t g_tx_buf[] = "Please input 16 bytes\r\n";
static uint8_t g_rx_buf[16];

/**
 *  Define a UART device,
 *  TX pin : 16/14
 *  RX pin : 7/15
 *  baud : 2000000
 */
     
#ifdef CONF_USER_BL702
HOSAL_UART_DEV_DECL(uart_dev_dma, 0, 14, 15, 2000000);
#elif CONF_USER_BL602
HOSAL_UART_DEV_DECL(uart_dev_dma, 0, 16, 7, 2000000);
#endif

/**
 * hal uart DMA RX interrupt callback
 */
static int __uart_rx_dma_callback(void *p_arg)
{
    /**
     * If RX transmission is completed
     * g_rx_buf is received data
     */
    blog_info("%s\r\n", g_rx_buf);
    return 0;
}

/**
 * hal uart DMA TX interrupt callback
 */
static int __uart_tx_dma_callback(void *p_arg)
{
    /* If TX transmission is completed */
    return 0;
}

/**
 * hal uart DMA mode demo
 */
void demo_hosal_uart_dma(int uart_id)
{
    hosal_uart_dma_cfg_t txdam_cfg = {
        .dma_buf = g_tx_buf,
        .dma_buf_size = sizeof(g_tx_buf),
    };
    hosal_uart_dma_cfg_t rxdam_cfg = {
        .dma_buf = g_rx_buf,
        .dma_buf_size = sizeof(g_rx_buf),
    };
    
    uart_dev_dma.config.uart_id = uart_id;

    /* uart init device */
    hosal_uart_init(&uart_dev_dma);

    /* Set DMA TX RX transmission complete interrupt callback */
    hosal_uart_callback_set(&uart_dev_dma, HOSAL_UART_TX_DMA_CALLBACK,
                          __uart_tx_dma_callback, &uart_dev_dma);
    hosal_uart_callback_set(&uart_dev_dma, HOSAL_UART_RX_DMA_CALLBACK,
                          __uart_rx_dma_callback, &uart_dev_dma);

    /* Start a UART TX DMA transfer */
    hosal_uart_ioctl(&uart_dev_dma, HOSAL_UART_DMA_TX_START, &txdam_cfg);

    /* Start a UART RX DMA transfer */
    hosal_uart_ioctl(&uart_dev_dma, HOSAL_UART_DMA_RX_START, &rxdam_cfg);

    while (1) {
        /* Do not let the test return */
    }
}
