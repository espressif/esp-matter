/**
 * Copyright (c) 2016-2021 Bouffalolab Co., Ltd.
 *
 * Contact information:
 * web site:    https://www.bouffalolab.com/
 */

#include <FreeRTOS.h>
#include <task.h>

#include <stdio.h>
#include <cli.h>
#include <hosal_uart.h>
#include <blog.h>

/**
 *  Define a UART device,
 *  TX pin : 4/10
 *  RX pin : 3/5
 *  baud : 2000000
 */
#ifdef CONF_USER_BL702
HOSAL_UART_DEV_DECL(uart_dev_abr, 1, 10, 5, 2000000);
#elif CONF_USER_BL602
HOSAL_UART_DEV_DECL(uart_dev_abr, 1, 4, 3, 2000000);
#endif


/**
 * hal uart interrupt mode demo
 */
void demo_hosal_uart_abr(int uart_id)
{
    uart_dev_abr.config.uart_id = uart_id;
    uint8_t wData[2] = {0x4f, 0x4b};
    int i = 0;

    while (1) {
        if (i % 2) {
            hosal_uart_abr_get(&uart_dev_abr, HOSAL_UART_AUTOBAUD_0X55);
        } else {
            hosal_uart_abr_get(&uart_dev_abr, HOSAL_UART_AUTOBAUD_STARTBIT);
        }

        i++;

        /* Uart init device */
        hosal_uart_init(&uart_dev_abr);
        blog_info("Detected baudrate baudrate is %d.\r\n", uart_dev_abr.config.baud_rate);
        /* send "OK"*/
        hosal_uart_send(&uart_dev_abr, wData, 2);
        
        vTaskDelay(100);
    }
}
