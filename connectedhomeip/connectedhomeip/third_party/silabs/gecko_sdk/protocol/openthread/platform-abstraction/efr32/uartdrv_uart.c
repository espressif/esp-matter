/*
 *  Copyright (c) 2017, The OpenThread Authors.
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


#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#include <stddef.h>
#include <string.h>
#include "openthread-system.h"
#include <openthread-core-config.h>
#include "utils/code_utils.h"
#include "utils/uart.h"
#include "em_core.h"
#include "uartdrv.h"
#include "sl_uartdrv_instances.h"
#include "sl_uartdrv_usart_vcom_config.h"

#include "ecode.h"
#include "sl_sleeptimer.h"
#include "sl_status.h"
#include "platform-efr32.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"

#ifdef SL_CATALOG_KERNEL_PRESENT

static unsigned int         sGpioIntContext = 0;

static void gpioSerialWakeupCallback(uint8_t interrupt_no, void *context)
{
  unsigned int *pin = (unsigned int *)context;

  (void)interrupt_no;

  if (*pin == SL_UARTDRV_USART_VCOM_RX_PIN) {
      otSysEventSignalPending();
  }
}
#endif // SL_CATALOG_KERNEL_PRESENT

enum
{
    kReceiveFifoSize = 128,
};

// In order to reduce the probability of data loss due to disabled interrupts, we use
// two duplicate receive buffers so we can always have one "active" receive request.
#define RECEIVE_BUFFER_SIZE 128
static uint8_t              sReceiveBuffer1[RECEIVE_BUFFER_SIZE];
static uint8_t              sReceiveBuffer2[RECEIVE_BUFFER_SIZE];
static uint8_t              lastCount = 0;
static volatile bool        sTxComplete = false;

typedef struct ReceiveFifo_t
{
    // The data buffer
    uint8_t mBuffer[kReceiveFifoSize];
    // The offset of the first item written to the list.
    volatile uint16_t mHead;
    // The offset of the next item to be written to the list.
    volatile uint16_t mTail;
} ReceiveFifo_t;

static ReceiveFifo_t sReceiveFifo;

static void processReceive(void);
static void processTransmit(void);

static void receiveDone(UARTDRV_Handle_t aHandle, Ecode_t aStatus, uint8_t *aData, UARTDRV_Count_t aCount)
{
    OT_UNUSED_VARIABLE(aStatus);

    // We can only write if incrementing mTail doesn't equal mHead
    if (sReceiveFifo.mHead != (sReceiveFifo.mTail + aCount - lastCount) % kReceiveFifoSize)
    {
        memcpy(sReceiveFifo.mBuffer + sReceiveFifo.mTail, aData + lastCount, aCount - lastCount);
        sReceiveFifo.mTail = (sReceiveFifo.mTail + aCount - lastCount) % kReceiveFifoSize;
        lastCount = 0;
    }

    UARTDRV_Receive(aHandle, aData, aCount, receiveDone);
    otSysEventSignalPending();
}

static void transmitDone(UARTDRV_Handle_t aHandle, Ecode_t aStatus, uint8_t *aData, UARTDRV_Count_t aCount)
{
    OT_UNUSED_VARIABLE(aHandle);
    OT_UNUSED_VARIABLE(aStatus);
    OT_UNUSED_VARIABLE(aData);
    OT_UNUSED_VARIABLE(aCount);

    // This value will be used later in processTransmit() to call otPlatUartSendDone()
    sTxComplete = true;
    otSysEventSignalPending();
}

static void processReceive(void)
{
    uint8_t *aData;
    UARTDRV_Count_t aCount, remaining;
    CORE_ATOMIC_SECTION(
      UARTDRV_GetReceiveStatus(sl_uartdrv_usart_vcom_handle, &aData, &aCount, &remaining);
      if (aCount > lastCount)
      {
          memcpy(sReceiveFifo.mBuffer + sReceiveFifo.mTail, aData + lastCount, aCount - lastCount);
          sReceiveFifo.mTail = (sReceiveFifo.mTail + aCount - lastCount) % kReceiveFifoSize;
          lastCount = aCount;
      }
    )

    // Copy tail to prevent multiple reads
    uint16_t tail = sReceiveFifo.mTail;

    // If the data wraps around, process the first part
    if (sReceiveFifo.mHead > tail)
    {
        otPlatUartReceived(sReceiveFifo.mBuffer + sReceiveFifo.mHead, kReceiveFifoSize - sReceiveFifo.mHead);

        // Reset the buffer mHead back to zero.
        sReceiveFifo.mHead = 0;
    }

    // For any data remaining, process it
    if (sReceiveFifo.mHead != tail)
    {
        otPlatUartReceived(sReceiveFifo.mBuffer + sReceiveFifo.mHead, tail - sReceiveFifo.mHead);

        // Set mHead to the local tail we have cached
        sReceiveFifo.mHead = tail;
    }
}

static void processTransmit(void)
{
    // NOTE: This check needs to be done in here and cannot be done in transmitDone because the transmit may not be
    // fully complete when the transmitDone callback is called.
    if (!sTxComplete)
    {
        return;
    }
    sTxComplete = false;
    otPlatUartSendDone();
}

void efr32UartProcess(void)
{
    processReceive();
    processTransmit();
}

otError otPlatUartEnable(void)
{
    otError error  = OT_ERROR_NONE;

#ifdef SL_CATALOG_KERNEL_PRESENT
    unsigned int intNo;

    GPIOINT_Init();

    sGpioIntContext = SL_UARTDRV_USART_VCOM_RX_PIN;
    intNo = GPIOINT_CallbackRegisterExt(SL_UARTDRV_USART_VCOM_RX_PIN, gpioSerialWakeupCallback, &sGpioIntContext);


    otEXPECT_ACTION(intNo != INTERRUPT_UNAVAILABLE, error = OT_ERROR_FAILED);

    GPIO_ExtIntConfig(SL_UARTDRV_USART_VCOM_RX_PORT,
                      SL_UARTDRV_USART_VCOM_RX_PIN,
                      intNo, false, true, true);
#endif

    sReceiveFifo.mHead = 0;
    sReceiveFifo.mTail = 0;

    // When one receive request is completed, the other buffer is used for a separate receive request, issued immediately.
    UARTDRV_Receive(sl_uartdrv_usart_vcom_handle, sReceiveBuffer1, RECEIVE_BUFFER_SIZE, receiveDone);
    UARTDRV_Receive(sl_uartdrv_usart_vcom_handle, sReceiveBuffer2, RECEIVE_BUFFER_SIZE, receiveDone);

#ifdef SL_CATALOG_KERNEL_PRESENT
exit:
#endif
    return error;
}

otError otPlatUartDisable(void)
{
    return OT_ERROR_NOT_IMPLEMENTED;
}

static void flushTimeoutAlarmCallback(sl_sleeptimer_timer_handle_t *aHandle, void *aData)
{
    OT_UNUSED_VARIABLE(aHandle);
    *(bool*)aData      = true;
}

otError otPlatUartFlush(void)
{
    otError                      error         = OT_ERROR_NONE;
    sl_status_t                  status        = SL_STATUS_OK;
    volatile bool                flushTimedOut = false;
    sl_sleeptimer_timer_handle_t flushTimer;

    // Start flush timeout timer
    status = sl_sleeptimer_start_timer_ms(&flushTimer, OPENTHREAD_CONFIG_EFR32_UART_TX_FLUSH_TIMEOUT_MS,
                                          flushTimeoutAlarmCallback, (void*)&flushTimedOut, 0,
                                          SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
    otEXPECT_ACTION(status == SL_STATUS_OK, error = OT_ERROR_FAILED);

    // Block until DMA has finished transmitting every buffer in sUartTxQueue and becomes idle
    uint8_t transmitQueueDepth = 0;
    bool    uartFullyFlushed   = false;
    bool    uartIdle           = false;

    do
    {
        // Check both peripheral status and queue depth
        transmitQueueDepth = UARTDRV_GetTransmitDepth(sl_uartdrv_usart_vcom_handle);
        uartIdle           = (UARTDRV_GetPeripheralStatus(sl_uartdrv_usart_vcom_handle)
                              & (UARTDRV_STATUS_TXIDLE | UARTDRV_STATUS_TXC));
        uartFullyFlushed   = uartIdle && (transmitQueueDepth == 0);
    } while (!uartFullyFlushed && !flushTimedOut);

    sl_sleeptimer_stop_timer(&flushTimer);

    if (flushTimedOut)
    {
        // Abort all transmits
        UARTDRV_Abort(sl_uartdrv_usart_vcom_handle, uartdrvAbortTransmit);
    }
    sTxComplete = false;
exit:
    return error;
}

OT_TOOL_WEAK void otPlatUartReceived(const uint8_t *aBuf, uint16_t aBufLength)
{
    OT_UNUSED_VARIABLE(aBuf);
    OT_UNUSED_VARIABLE(aBufLength);
    // do nothing
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    otError error  = OT_ERROR_NONE;
    Ecode_t status = ECODE_EMDRV_UARTDRV_OK;

    status = UARTDRV_Transmit(sl_uartdrv_usart_vcom_handle, (uint8_t *)aBuf, aBufLength, transmitDone);
    otEXPECT_ACTION(status == ECODE_EMDRV_UARTDRV_OK, error = OT_ERROR_FAILED);

exit:
    return error;
}

OT_TOOL_WEAK void otPlatUartSendDone(void)
{
    // do nothing
}
