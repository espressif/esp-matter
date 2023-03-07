/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for UART communication.
 *
 */

#include <stdio.h>

#include <openthread/instance.h>
#if OPENTHREAD_API_VERSION >= 85
#include <utils/uart.h>
#else
#include <openthread/platform/uart.h>
#endif
#include <wiced_hal_puart.h>

#include <wiced_platform.h>
#include <wiced_rtos.h>

#define UART_RX_BUFFER_SIZE (512)

#ifdef CLI_COMMAND_SEPARATOR

#define UART_RX_CLI_STATE_CHAR_CARRIAGE_RETURN '\r'

typedef enum uart_rx_cli_start_state
{
    UART_RX_CLI_START_STATE_IDLE    = 0,
    UART_RX_CLI_START_STATE_STARTED = 1, // receive '\r'
} uart_rx_cli_start_state_t;

#endif // CLI_COMMAND_SEPARATOR

typedef struct uart_cb
{
    bool     initialized;
    uint32_t event_code;

    struct
    {
        wiced_mutex_t *p_mutex;
        uint16_t       index_start;
        uint16_t       index_end;
        uint16_t       len;
        uint8_t        data[UART_RX_BUFFER_SIZE];

#ifdef CLI_COMMAND_SEPARATOR

        struct
        {
            uart_rx_cli_start_state_t start;
        } cli_state;

#endif // CLI_COMMAND_SEPARATOR
    } rx;
} uart_cb_t;

/* Declaration of static functions. */
static void    uart_rx_data_handler(void);
static uint8_t uart_rx_data_get(void);
static void    uart_rx_data_put(uint8_t data);
static void    uart_rx_interrupt(void *arg);
static void    uart_write(const uint8_t *aBuf, uint16_t aBufLength);

/* Declaration of static variables. */
static uart_cb_t uart_cb = {0};

void otPlatUartInit(void)
{
    /* Get/Register the application thread event code. */
    if (!wiced_platform_application_thread_event_register(&uart_cb.event_code, uart_rx_data_handler))
    {
        printf("%s: Fail to get event code.\n", __FUNCTION__);
        return;
    }

    /* Create rx data mutex. */
    uart_cb.rx.p_mutex = wiced_rtos_create_mutex();

    if (uart_cb.rx.p_mutex == NULL)
    {
        printf("%s: Fail to create mutex.\n", __FUNCTION__);
        return;
    }

    /* Initialize the rx data mutex. */
    if (wiced_rtos_init_mutex(uart_cb.rx.p_mutex) != WICED_SUCCESS)
    {
        printf("%s: Fail to init. mutex.\n", __FUNCTION__);
        return;
    }

    uart_cb.initialized = true;
}

otError otPlatUartEnable(void)
{
    wiced_platform_application_thread_check();

    if (!uart_cb.initialized)
    {
        return OT_ERROR_INVALID_STATE;
    }

    wiced_platform_puart_init(uart_rx_interrupt);

    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    wiced_platform_application_thread_check();
    if (!uart_cb.initialized)
    {
        return OT_ERROR_INVALID_STATE;
    }
    uart_write(aBuf, aBufLength);
    otPlatUartSendDone();

    return OT_ERROR_NONE;
}

otError otPlatUartFlush(void)
{
    return OT_ERROR_NOT_IMPLEMENTED;
}

//------------------------------ Static Function --------------------------------------------------

static void uart_rx_interrupt(void *arg)
{
    uint8_t data;

    OT_UNUSED_VARIABLE(arg);

    wiced_rtos_lock_mutex(uart_cb.rx.p_mutex);

    while (wiced_hal_puart_read(&data))
    {
        uart_rx_data_put(data);
    }

    wiced_platform_application_thread_event_set(uart_cb.event_code);

    wiced_rtos_unlock_mutex(uart_cb.rx.p_mutex);

    wiced_hal_puart_reset_puart_interrupt();
}

static void uart_rx_data_handler(void)
{
    uint8_t data;

    while (uart_cb.rx.len)
    {
        wiced_rtos_lock_mutex(uart_cb.rx.p_mutex);

        data = uart_rx_data_get();

        wiced_rtos_unlock_mutex(uart_cb.rx.p_mutex);

#ifdef CLI_COMMAND_SEPARATOR
        /* Check cli start state. */
        switch (uart_cb.rx.cli_state.start)
        {
        case UART_RX_CLI_START_STATE_IDLE:
            /* Check the data. */
            if (data == UART_RX_CLI_STATE_CHAR_CARRIAGE_RETURN)
            {
                /* Set cli start state. */
                uart_cb.rx.cli_state.start = UART_RX_CLI_START_STATE_STARTED;

                /* Deliver data to upper layer. */
                otPlatUartReceived(&data, sizeof(data));
            }
            break;

        case UART_RX_CLI_START_STATE_STARTED:
            /* Check the data. */
            if (data == UART_RX_CLI_STATE_CHAR_CARRIAGE_RETURN)
            {
                /* Set cli start state. */
                uart_cb.rx.cli_state.start = UART_RX_CLI_START_STATE_IDLE;
            }

            /* Deliver data to upper layer. */
            otPlatUartReceived(&data, sizeof(data));
            break;

        default:
            printf("Err: CLI in invalid start state\n");
            break;
        }
#else  // CLI_COMMAND_SEPARATOR
        otPlatUartReceived(&data, sizeof(data));
#endif // !CLI_COMMAND_SEPARATOR
    }
}

static void uart_rx_data_put(uint8_t data)
{
    if (uart_cb.rx.len == UART_RX_BUFFER_SIZE)
    {
        printf("%s: Error: Rx data full.\n", __FUNCTION__);
        return;
    }

    uart_cb.rx.data[uart_cb.rx.index_end++] = data;

    uart_cb.rx.len++;
    if (uart_cb.rx.index_end == UART_RX_BUFFER_SIZE)
    {
        uart_cb.rx.index_end = 0;
    }
}

static uint8_t uart_rx_data_get(void)
{
    uint8_t data;

    if (uart_cb.rx.len == 0)
    {
        printf("%s: Error: Rx data empty.\n", __FUNCTION__);
        return 0;
    }

    data = uart_cb.rx.data[uart_cb.rx.index_start++];

    uart_cb.rx.len--;
    if (uart_cb.rx.index_start == UART_RX_BUFFER_SIZE)
    {
        uart_cb.rx.index_start = 0;
    }

    return data;
}

static void uart_write(const uint8_t *aBuf, uint16_t aBufLength)
{
    while (aBufLength > 0)
    {
        wiced_hal_puart_write((UINT8)*aBuf);
        aBuf++;
        aBufLength--;
    }
}
