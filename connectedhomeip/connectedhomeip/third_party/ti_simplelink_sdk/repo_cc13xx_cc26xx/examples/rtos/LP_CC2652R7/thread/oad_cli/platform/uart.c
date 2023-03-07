/******************************************************************************

 @file uart.c

 @brief TIRTOS platform specific uart functions for OpenThread

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#include <openthread/config.h>

#include <stddef.h>

#include <utils/code_utils.h>
#include <openthread/platform/uart.h>

#include <ti/drivers/UART2.h>

#include "ti_drivers_config.h"
#include "system.h"

/**
 * Configure the UART core for 115200 baud 8-N-1, no HW flow control.
 *
 * @note Make sure that data being passed to @ref otPlatUartSend is in
 *       persistent readable memory.
 */

/**
 * Event identifiers passed back to the stack task callback.
 */
#define PLATFORM_UART_EVENT_TX_DONE  (1U << 0)
#define PLATFORM_UART_EVENT_RX_DONE  (1U << 1)

/**
 * Size of the statically allocated buffer to pass data from the callback to
 * the processing loop.
 */
#define PLATFORM_UART_RECV_BUF_LEN 32

/**
 * Statically allocated data buffer.
 */
static uint8_t PlatformUart_receiveBuffer[PLATFORM_UART_RECV_BUF_LEN];

/**
 * Number of bytes in the receive buffer.
 */
static size_t PlatformUart_receiveLen;

/**
 * Uart driver handle.
 */
static UART2_Handle PlatformUart_uartHandle;

/**
 * Callback for when the UART driver finishes reading.
 *
 * This is triggered when the buffer is full, or when the UART hardware times
 * out.
 */
static void uartReadCallback(UART2_Handle aHandle, void *aBuf, size_t aLen,
                             void *userArg, int_fast16_t status)
{
    (void)aHandle;
    (void)aBuf;
    (void)userArg;
    (void)status;
    PlatformUart_receiveLen = aLen;
    platformUartSignal(PLATFORM_UART_EVENT_RX_DONE);
}

/**
 * Callback for when the UART driver finishes writing a buffer.
 */
static void uartWriteCallback(UART2_Handle aHandle, void *aBuf, size_t aLen,
                              void *userArg, int_fast16_t status)
{
    (void)aHandle;
    (void)aBuf;
    (void)aLen;
    (void)userArg;
    (void)status;
    platformUartSignal(PLATFORM_UART_EVENT_TX_DONE);
}

/**
 * Function documented in platform/uart.h
 */
otError otPlatUartEnable(void)
{
    UART2_Params params;

    UART2_Params_init(&params);

    params.readMode         = UART2_Mode_CALLBACK;
    params.writeMode        = UART2_Mode_CALLBACK;
    params.readCallback     = uartReadCallback;
    params.writeCallback    = uartWriteCallback;
    params.readReturnMode   = UART2_ReadReturnMode_PARTIAL;
    params.baudRate         = 115200;
    params.dataLength       = UART2_DataLen_8;
    params.stopBits         = UART2_StopBits_1;
    params.parityType       = UART2_Parity_NONE;

    PlatformUart_uartHandle = UART2_open(CONFIG_DISPLAY_UART, &params);

    /* begin reading from the uart */
    UART2_read(PlatformUart_uartHandle, PlatformUart_receiveBuffer,
              sizeof(PlatformUart_receiveBuffer), NULL);

    return OT_ERROR_NONE;
}

/**
 * Function documented in platform/uart.h
 */
otError otPlatUartDisable(void)
{
    UART2_close(PlatformUart_uartHandle);

    return OT_ERROR_NONE;
}

/**
 * Function documented in platform/uart.h
 */
otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    UART2_write(PlatformUart_uartHandle, aBuf, aBufLength, NULL);

    return OT_ERROR_NONE;
}


/**
 * Function documented in system.h
 */
void platformUartProcess(uintptr_t arg)
{
    if (arg & PLATFORM_UART_EVENT_TX_DONE)
    {
        otPlatUartFlush();
        otPlatUartSendDone();
    }

    if (arg & PLATFORM_UART_EVENT_RX_DONE)
    {
        otPlatUartReceived(PlatformUart_receiveBuffer, PlatformUart_receiveLen);
        PlatformUart_receiveLen = 0;
        UART2_read(PlatformUart_uartHandle, PlatformUart_receiveBuffer,
                  sizeof(PlatformUart_receiveBuffer), NULL);
    }
}

/**
 * Function documented in system.h
 */
otError otPlatUartFlush(void)
{
    return OT_ERROR_NOT_IMPLEMENTED;
}

