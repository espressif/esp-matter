/*
 *  Copyright (c) 2019, The OpenThread Authors.
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

/* NXP UART includes */
#include "SerialManager.h"
#include "board.h"
#include "fsl_clock.h"
#include "fsl_flexcomm.h"
#include "fsl_reset.h"
#include "fsl_usart.h"

/* Openthread general includes */
#include "fsl_debug_console.h"
#include "fsl_iocon.h"
#include "openthread-system.h"
#include "platform-k32w.h"
#include <utils/code_utils.h>
#include <openthread/platform/time.h>
#include "utils/uart.h"

/* Defines */
#ifndef gUartIsrPrio_c
#define gUartIsrPrio_c (0x40)
#endif

#if ((UART_USE_DRIVER == 1) && (UART_USE_SERIAL_MGR == 1))
#error "Serial Manager and Uart driver cannot be used at the same time. Please choose just one."
#endif

#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED)
#if ((UART_USE_DRIVER_LOG == 0) && (UART_USE_SERIAL_MGR_LOG == 0) && (UART_USE_SWO_LOG == 0))
#error "No output interface enabled for OpenThread logging. Please choose just one."
#endif

#if (((UART_USE_DRIVER_LOG == 1) || (UART_USE_SERIAL_MGR_LOG == 1)) && (UART_USE_SWO_LOG == 1))
#error "Serial Manager or Uart driver cannot be used at the same time with SWO for logging. Please choose just one."
#endif
#endif

#define US_PER_MS 1000ULL
#define FLUSH_TO 500 /* ms*/

#ifndef OT_PLAT_UART_RX_BUFFER_SIZE
/*
 * Always make sure that OT_PLAT_UART_RX_BUFFER_SIZE is smaller (or equal) than the serial manager rx ring buffer.
 * In fact there is a risk to not re-schedule the serial manager task if there is more byte in the rx ring buffer than
 * what SerialMngr_RxCbApp could read.
 */
#define OT_PLAT_UART_RX_BUFFER_SIZE gSerialMgrRxBufSize_c
#endif

#ifndef OT_PLAT_UART_BAUD_RATE
#define OT_PLAT_UART_BAUD_RATE 115200
#endif

#ifndef OT_PLAT_APP_UART_INSTANCE
#define OT_PLAT_APP_UART_INSTANCE 0
#endif

#if (UART_USE_SWO_LOG == 1)
/* For a different SWO pin, please update the necessary pin number and functionality */
#ifndef SWO_LOG_PIN
#define SWO_LOG_PIN 14U
#endif
#ifndef SWO_LOG_PIN_FUNC
#define SWO_LOG_PIN_FUNC IOCON_FUNC5
#endif
#endif

#if UART_USE_DRIVER
/* Structures */
typedef struct
{
    uint8_t buffer[OT_PLAT_UART_RX_BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    bool    isFull;
} rxRingBuffer;

/* Enums */
typedef enum
{
    UART_IDLE, /* TX idle. */
    UART_BUSY, /* TX busy. */
} K32WUartStates;

typedef void (*pSerialCallBack_t)(void *param);

/* Private functions declaration */
static void     K32WProcessReceive();
static void     K32WProcessTransmit();
static void     K32WResetRxRingBuffer(rxRingBuffer *aRxRing);
static uint8_t *K32WPopRxRingBuffer(rxRingBuffer *aRxRing);
static bool     K32WIsEmptyRxRingBuffer(rxRingBuffer *aRxRing);
static void     K32WPushRxRingBuffer(rxRingBuffer *aRxRing, uint8_t aCharacter);
static void     USART0_IRQHandler(USART_Type *base, usart_handle_t *handle);
#endif

#if UART_USE_SERIAL_MGR
static void       SerialMngr_TxCbApp(void *param);
static void       SerialMngr_RxCbApp(void *param);
OT_TOOL_WEAK void SerialProcess(void);
#endif

void txDone();

/* Private variables declaration */
static bool sIsUartInitialized; /* Is UART module initialized? */

#if UART_USE_DRIVER
static bool           sIsTransmitDone; /* Transmit done for the latest user-data buffer */
static usart_handle_t sUartHandleApp;  /* Handle to the UART module */
static rxRingBuffer   sUartRxRing0;    /* Receive Ring Buffer */
#endif

#if UART_USE_DRIVER_LOG
static usart_handle_t sUartHandleLog; /* Handle to the UART module */
#endif

volatile bool_t gTxFlush; /* tx flush ongoing */

#if UART_USE_SERIAL_MGR
uint8_t          gShellSerMgrIf;
volatile uint8_t gTxCntSerMgrIf; /* pending transmissions */

OT_TOOL_WEAK serialInterfaceType_t ifType = gSerialMgrUsart_c;
static uint8_t                     sRxBuffer[OT_PLAT_UART_RX_BUFFER_SIZE];
OT_TOOL_WEAK uint8_t               gOtAppUartInstance = OT_PLAT_APP_UART_INSTANCE;
#endif

#if UART_USE_SERIAL_MGR_LOG
uint8_t              mOtSerMgrIfLog;
OT_TOOL_WEAK uint8_t gOtLogUartInstance = 1;
#endif

void K32WUartProcess(void)
{
    if (sIsUartInitialized)
    {
#if UART_USE_DRIVER
        K32WProcessTransmit();
        K32WProcessReceive();
#endif

#if UART_USE_SERIAL_MGR
#if !USE_SDK_OSA
        SerialProcess();
#endif
#endif
    }
}

otError otPlatUartEnable(void)
{
    otError error = OT_ERROR_NONE;

    if (!sIsUartInitialized)
    {
#if UART_USE_DRIVER

        status_t       uartStatus;
        usart_config_t config;
        uint32_t       kPlatformClock = CLOCK_GetFreq(kCLOCK_Fro32M);

        /* attach clock for USART */
        CLOCK_AttachClk(kOSC32M_to_USART_CLK);

        /* reset FLEXCOMM0 for USART0 */
        RESET_PeripheralReset(kUSART0_RST_SHIFT_RSTn);

        memset(&sUartHandleApp, 0, sizeof(sUartHandleApp));
        sUartHandleApp.txState = UART_IDLE;

        BOARD_Init_UART_Pins(0);

        USART_GetDefaultConfig(&config);
        config.baudRate_Bps = OT_PLAT_UART_BAUD_RATE;
        config.enableTx     = true;
        config.enableRx     = true;
        config.rxWatermark  = kUSART_RxFifo1;

        uartStatus = USART_Init(USART0, &config, kPlatformClock);
        otEXPECT_ACTION(uartStatus == kStatus_Success, error = OT_ERROR_INVALID_ARGS);

        K32WResetRxRingBuffer(&sUartRxRing0);
        FLEXCOMM_SetIRQHandler(USART0, (flexcomm_irq_handler_t)USART0_IRQHandler, &sUartHandleApp);

        /* Enable interrupt in NVIC. */
        NVIC_SetPriority(USART0_IRQn, gUartIsrPrio_c >> (8 - __NVIC_PRIO_BITS));
        NVIC_ClearPendingIRQ(USART0_IRQn);
        EnableIRQ(USART0_IRQn);

        /* Enable RX interrupt for app */
        USART_EnableInterrupts(USART0, kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable);

#endif

#if UART_USE_SERIAL_MGR
        uint8_t status;

        Serial_InitManager();

        /* Register Serial Manager interface */
        status = Serial_InitInterface(&gShellSerMgrIf, ifType, gOtAppUartInstance);
        status += Serial_SetBaudRate(gShellSerMgrIf, OT_PLAT_UART_BAUD_RATE);

        /* Install UART RX Callback handler */
        status += Serial_SetRxCallBack(gShellSerMgrIf, SerialMngr_RxCbApp, NULL);
        otEXPECT_ACTION(status == 0, error = OT_ERROR_FAILED);
#endif
        sIsUartInitialized = true;
    }

exit:
    return error;
}

otError otPlatUartDisable(void)
{
    sIsUartInitialized = false;

#if UART_USE_DRIVER
    USART_Deinit(USART0);
    USART_Deinit(USART1);
#endif

    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    otError error = OT_ERROR_NONE;

#if UART_USE_DRIVER

    otEXPECT_ACTION(!sUartHandleApp.txData, error = OT_ERROR_BUSY);
    sUartHandleApp.txData        = (uint8_t *)aBuf;
    sUartHandleApp.txDataSize    = aBufLength;
    sUartHandleApp.txDataSizeAll = aBufLength;

    /* Enable transmitter interrupt. */
    USART_EnableInterrupts(USART0, kUSART_TxLevelInterruptEnable);

#endif

#if UART_USE_SERIAL_MGR
    /* new tx */
    gTxCntSerMgrIf++;

    uint8_t status = Serial_AsyncWrite(gShellSerMgrIf, (uint8_t *)aBuf, aBufLength, SerialMngr_TxCbApp, NULL);
    otEXPECT_ACTION(status == 0, gTxCntSerMgrIf--; error = OT_ERROR_FAILED);
#endif

exit:
    return error;
}

otError otPlatUartFlush(void)
{
    if (gTxFlush)
    {
        return OT_ERROR_NONE;
    }

    uint64_t start = otPlatTimeGet();

    gTxFlush = TRUE;

#if UART_USE_DRIVER
    /* set to NULL in USART0_IRQHandler() */
    while (sUartHandleApp.txData)

#elif UART_USE_SERIAL_MGR
    /* decremented in SerialMngr_TxCbApp() */
    while (gTxCntSerMgrIf)
#endif
    {
        K32WUartProcess();

        if ((otPlatTimeGet() - start) / US_PER_MS > FLUSH_TO)
        {
            break;
        }
    }

    gTxFlush = FALSE;

    return OT_ERROR_NONE;
}

#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED)
void K32WLogInit()
{
#if UART_USE_DRIVER_LOG
    usart_config_t config;
    uint32_t       kPlatformClock = CLOCK_GetFreq(kCLOCK_Fro32M);

    /* attach clock for USART */
    CLOCK_AttachClk(kOSC32M_to_USART_CLK);

    /* reset FLEXCOMM0 for USART1 */
    RESET_PeripheralReset(kUSART1_RST_SHIFT_RSTn);

    memset(&sUartHandleLog, 0, sizeof(sUartHandleLog));
    sUartHandleLog.txState = UART_IDLE;

    BOARD_Init_UART_Pins(1);

    USART_GetDefaultConfig(&config);
    config.enableRx = false;
    USART_Init(USART1, &config, kPlatformClock);

#endif

#if UART_USE_SERIAL_MGR_LOG
    Serial_InitManager();

    Serial_InitInterface(&mOtSerMgrIfLog, gSerialMgrUsart_c, gOtLogUartInstance);
    Serial_SetBaudRate(mOtSerMgrIfLog, OT_PLAT_UART_BAUD_RATE);
#endif

#if UART_USE_SWO_LOG
    CLOCK_EnableClock(kCLOCK_Iocon);
    IOCON_PinMuxSet(IOCON, 0U, SWO_LOG_PIN, SWO_LOG_PIN_FUNC);
    CLOCK_DisableClock(kCLOCK_Iocon);

    /* Default instance for SWO debug console is 0. This is corresponds to port 0 if using SWO Trace viewers.
     * OT_PLAT_APP_UART_INSTANCE value needs to be changed if another port is required.
     */
    int32_t status = DbgConsole_Init(OT_PLAT_APP_UART_INSTANCE, OT_PLAT_UART_BAUD_RATE, kSerialPort_Swo,
                                     CLOCK_GetFreq(kCLOCK_MainClk));
#endif
}
#endif

/**
 * Function used for blocking-write to the UART module.
 *
 * @param[in] aBuf             Pointer to the character buffer
 * @param[in] len              Length of the character buffer
 */
void K32WWriteBlocking(const uint8_t *aBuf, uint32_t len)
{
#if UART_USE_DRIVER_LOG
    otEXPECT(sUartHandleLog.txState != UART_BUSY);

    sUartHandleLog.txState = UART_BUSY;
    USART_WriteBlocking(USART1, aBuf, len);
    sUartHandleLog.txState = UART_IDLE;

exit:
    return;
#endif

#if UART_USE_SERIAL_MGR_LOG
    uint8_t status = Serial_SyncWrite(mOtSerMgrIfLog, (uint8_t *)aBuf, len);
    otEXPECT(status == 0);
exit:
    return;

#endif
}

#if UART_USE_DRIVER
/**
 * Process TX characters in process context and call the upper layer call-backs.
 */
static void K32WProcessTransmit(void)
{
    if (sIsTransmitDone)
    {
        sIsTransmitDone = false;
        txDone();
    }
}

/**
 * Process RX characters in process context and call the upper layer call-backs.
 */
static void K32WProcessReceive(void)
{
    uint8_t  rx[OT_PLAT_UART_RX_BUFFER_SIZE];
    uint16_t rxIndex = 0;
    uint8_t *pCharacter;

    while ((pCharacter = K32WPopRxRingBuffer(&sUartRxRing0)) != NULL)
    {
        rx[rxIndex] = *pCharacter;
        rxIndex++;
    }
    otPlatUartReceived(rx, rxIndex);
}

static void USART0_IRQHandler(USART_Type *base, usart_handle_t *handle)
{
    (void)base;
    (void)handle;

    bool isReceiveEnabled = true;
    bool isSendEnabled    = (sUartHandleApp.txDataSize != 0);

    /* If RX overrun. */
    if (USART0->FIFOSTAT & USART_FIFOSTAT_RXERR_MASK)
    {
        /* Clear RX error state. */
        USART0->FIFOSTAT |= USART_FIFOSTAT_RXERR_MASK;
        /* clear RX FIFO */
        USART0->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK;
    }
    while ((isReceiveEnabled && (USART0->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK)) ||
           (isSendEnabled && (USART0->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK)))
    {
        /* RX: an interrupt is fired for each received character */
        if (isReceiveEnabled && (USART0->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK))
        {
            volatile uint8_t rx_data = USART_ReadByte(USART0);

            {
                K32WPushRxRingBuffer(&sUartRxRing0, rx_data);
            }
        }

        /* There are times when the UART interrupt fires unnecessarily
         * having the TXNOTFULL and TXEMPY bits set. Disable this!
         */
        if ((!sUartHandleApp.txDataSize) && (USART0->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK) &&
            (USART0->FIFOSTAT & USART_FIFOSTAT_TXEMPTY_MASK))
        {
            USART0->FIFOINTENCLR = USART_FIFOINTENCLR_TXLVL_MASK;
        }

        /* TX: an interrupt is fired for each sent character */
        if (isSendEnabled && (USART0->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK))
        {
            USART0->FIFOWR = *sUartHandleApp.txData;
            sUartHandleApp.txDataSize--;
            sUartHandleApp.txData++;
            isSendEnabled = (sUartHandleApp.txDataSize != 0);

            if (!isSendEnabled)
            {
                USART0->FIFOINTENCLR  = USART_FIFOINTENCLR_TXLVL_MASK;
                sUartHandleApp.txData = NULL;
                sIsTransmitDone       = true;
            }
        }
    }
    otSysEventSignalPending();
}

/**
 * Function used to push a received character to the RX Ring buffer.
 * In case the ring buffer is full, the oldest address is overwritten.
 *
 * @param[in] aRxRing             Pointer to the RX Ring Buffer
 * @param[in] aCharacter          The received character
 */
static void K32WPushRxRingBuffer(rxRingBuffer *aRxRing, uint8_t aCharacter)
{
    aRxRing->buffer[aRxRing->head] = aCharacter;

    if (aRxRing->isFull)
    {
        aRxRing->tail = (aRxRing->tail + 1) % OT_PLAT_UART_RX_BUFFER_SIZE;
    }

    aRxRing->head   = (aRxRing->head + 1) % OT_PLAT_UART_RX_BUFFER_SIZE;
    aRxRing->isFull = (aRxRing->head == aRxRing->tail);
}

/**
 * Function used to pop the address of a received character from the RX Ring buffer
 * Process Context: the consumer will pop frames with the interrupts disabled
 *                  to make sure the interrupt context(ISR) doesn't push in
 *                  the middle of a pop.
 *
 * @param[in] aRxRing           Pointer to the RX Ring Buffer
 *
 * @return    tsRxFrameFormat   Pointer to a received character
 * @return    NULL              In case the RX Ring buffer is empty
 */
static uint8_t *K32WPopRxRingBuffer(rxRingBuffer *aRxRing)
{
    uint8_t *pCharacter = NULL;

    DisableIRQ(USART0_IRQn);
    if (!K32WIsEmptyRxRingBuffer(aRxRing))
    {
        pCharacter      = &(aRxRing->buffer[aRxRing->tail]);
        aRxRing->isFull = false;
        aRxRing->tail   = (aRxRing->tail + 1) % OT_PLAT_UART_RX_BUFFER_SIZE;
    }
    EnableIRQ(USART0_IRQn);

    return pCharacter;
}

/**
 * Function used to check if an RX Ring buffer is empty
 *
 * @param[in] aRxRing           Pointer to the RX Ring Buffer
 *
 * @return    TRUE              RX Ring Buffer is not empty
 * @return    FALSE             RX Ring Buffer is empty
 */
static bool K32WIsEmptyRxRingBuffer(rxRingBuffer *aRxRing)
{
    return (!aRxRing->isFull && (aRxRing->head == aRxRing->tail));
}

/**
 * Function used to init/reset an RX Ring Buffer
 *
 * @param[in] aRxRing         Pointer to an RX Ring Buffer
 */
static void K32WResetRxRingBuffer(rxRingBuffer *aRxRing)
{
    aRxRing->head   = 0;
    aRxRing->tail   = 0;
    aRxRing->isFull = false;
}
#endif /* UART_USE_DRIVER */

#if UART_USE_SERIAL_MGR
static void SerialMngr_TxCbApp(void *param)
{
    OT_UNUSED_VARIABLE(param);

    /* tx finished */
    if (gTxCntSerMgrIf)
    {
        gTxCntSerMgrIf--;
    }

    txDone();
}

static void SerialMngr_RxCbApp(void *param)
{
    uint16_t oneReadBytes = 0;
    OT_UNUSED_VARIABLE(param);

    Serial_Read(gShellSerMgrIf, sRxBuffer, OT_PLAT_UART_RX_BUFFER_SIZE, &oneReadBytes);
    otPlatUartReceived(sRxBuffer, oneReadBytes);
}

#if !USE_SDK_OSA
void SerialProcess(void)
{
    SerialManagerTask();
}
#endif
#endif /* UART_USE_SERIAL_MGR */

void txDone()
{
    if (!gTxFlush)
    {
        otPlatUartSendDone();
    }
}

/**
 * The UART driver weak functions definition.
 *
 */
OT_TOOL_WEAK void otPlatUartSendDone(void)
{
}

OT_TOOL_WEAK void otPlatUartReceived(const uint8_t *aBuf, uint16_t aBufLength)
{
    OT_UNUSED_VARIABLE(aBuf);
    OT_UNUSED_VARIABLE(aBufLength);
}
