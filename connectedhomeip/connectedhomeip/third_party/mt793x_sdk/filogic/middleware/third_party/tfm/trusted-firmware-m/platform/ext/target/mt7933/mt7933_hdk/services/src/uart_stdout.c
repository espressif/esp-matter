/*
 * Copyright (c) 2017-2019 ARM Limited
 *
 * Licensed under the Apace License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apace.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "uart_stdout.h"
#include "hal_uart.h"

extern uint32_t hal_uart_send_polling(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size);

int stdio_output_string(const unsigned char *str, uint32_t len)
{
    int left;
    int bytes_written;
    int send_cr = 0;
    // workaround, stupid but works most of case
    if( (str[len-1] == '\n' && str[len-2] != '\r') ||
        (str[len-1] == '\n' && str[0] != '\r')) {
        send_cr = 1;
    }

    left = len;
    while(left > 0) {
        bytes_written = hal_uart_send_polling(HAL_UART_0, (const uint8_t *)str, left);
        left -= bytes_written;
        str += bytes_written;
    }

    if(send_cr) {
        hal_uart_send_polling(HAL_UART_0, (const uint8_t *)"\r", 1);
    }
    return len;
}

/* Redirects printf to TFM_DRIVER_STDIO in case of ARMCLANG*/
#if defined(__ARMCC_VERSION)
/* Struct FILE is implemented in stdio.h. Used to redirect printf to
 * TFM_DRIVER_STDIO
 */
FILE __stdout;
/* __ARMCC_VERSION is only defined starting from Arm compiler version 6 */
int fputc(int ch, FILE *f)
{
    (void)f;

    /* Send byte to USART */
    (void)stdio_output_string((const unsigned char *)&ch, 1);

    /* Return character written */
    return ch;
}
#elif defined(__GNUC__)
/* Redirects printf to TFM_DRIVER_STDIO in case of GNUARM */
int _write(int fd, char *str, int len)
{
    (void)fd;

    /* Send string and return the number of characters written */
    return stdio_output_string((const unsigned char *)str, (uint32_t)len);
}
#elif defined(__ICCARM__)
int putchar(int ch)
{
    /* Send byte to USART */
    (void)stdio_output_string((const unsigned char *)&ch, 1);

    /* Return character written */
    return ch;
}
#endif

void stdio_init(void)
{
    ;
}

void stdio_uninit(void)
{
    ;
}
