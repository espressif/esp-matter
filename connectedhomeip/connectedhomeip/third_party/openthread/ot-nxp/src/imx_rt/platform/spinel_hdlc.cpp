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

#include "spinel_hdlc.hpp"
#include "board.h"
#include "fsl_component_serial_manager.h"
#include "fsl_os_abstraction.h"

#include <openthread/tasklet.h>
#include <openthread/platform/alarm-milli.h>

#ifndef SPINEL_UART_INSTANCE
#define SPINEL_UART_INSTANCE 8
#endif
#ifndef SPINEL_ENABLE_RX_RTS
#define SPINEL_ENABLE_RX_RTS 0
#endif
#ifndef SPINEL_ENABLE_TX_RTS
#define SPINEL_ENABLE_TX_RTS 0
#endif

#ifndef SPINEL_UART_BAUD_RATE
#define SPINEL_UART_BAUD_RATE 115200
#endif

#ifndef SPINEL_UART_CLOCK_RATE
#define SPINEL_UART_CLOCK_RATE BOARD_BT_UART_CLK_FREQ
#endif

#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U) && (SPINEL_ENABLE_TX_RTS > 0U) && \
     (SPINEL_ENABLE_RX_RTS > 0U))
#error "DMA on UART with flow control not required"
#endif

#define LPUART_TX_DMA_CHANNEL 0U
#define LPUART_RX_DMA_CHANNEL 1U

#define SPINEL_HDLC_MALLOC pvPortMalloc
#define SPINEL_HDLC_FREE vPortFree

static void Uart_RxCallBackStatic(void *                             pData,
                                  serial_manager_callback_message_t *message,
                                  serial_manager_status_t            status)
{
    /* notify the main loop that a RX buffer is available */
#if (defined(SERIAL_MANAGER_TASK_HANDLE_RX_AVAILABLE_NOTIFY) && (SERIAL_MANAGER_TASK_HANDLE_RX_AVAILABLE_NOTIFY > 0U))
    otTaskletsSignalPending(NULL);
#else
    otSysEventSignalPending();
#endif
}

static void Uart_TxCallBackStatic(void *                             pData,
                                  serial_manager_callback_message_t *message,
                                  serial_manager_status_t            status)
{
    /* Close the write handle */
    SerialManager_CloseWriteHandle((serial_write_handle_t)pData);
    /* Free the buffer */
    SPINEL_HDLC_FREE(pData);
}

namespace ot {

namespace RT {

HdlcInterface::HdlcInterface(ot::Spinel::SpinelInterface::ReceiveFrameCallback aCallback,
                             void *                                            aCallbackContext,
                             ot::Spinel::SpinelInterface::RxFrameBuffer &      aFrameBuffer)
    : mReceiveFrameCallback(aCallback)
    , mReceiveFrameContext(aCallbackContext)
    , mReceiveFrameBuffer(aFrameBuffer)
    , encoderBuffer()
    , mHdlcEncoder(encoderBuffer)
    , mHdlcDecoder(aFrameBuffer, HandleHdlcFrame, this)
{
}

HdlcInterface::~HdlcInterface(void)
{
}

void HdlcInterface::Init(void)
{
    InitUart();
}

void HdlcInterface::Deinit(void)
{
    DeinitUart();
}

otError HdlcInterface::SendFrame(const uint8_t *aFrame, uint16_t aLength)
{
    otError error = OT_ERROR_NONE;
    assert(encoderBuffer.IsEmpty());

    SuccessOrExit(error = mHdlcEncoder.BeginFrame());
    SuccessOrExit(error = mHdlcEncoder.Encode(aFrame, aLength));
    SuccessOrExit(error = mHdlcEncoder.EndFrame());
    OT_PLAT_DBG("frame len to send = %d/%d", encoderBuffer.GetLength(), aLength);
    SuccessOrExit(error = Write(encoderBuffer.GetFrame(), encoderBuffer.GetLength()));

exit:
    if (error != OT_ERROR_NONE)
    {
        OT_PLAT_ERR("error = 0x%x", error);
    }
    return error;
}

void HdlcInterface::Process(const otInstance &aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    TryReadAndDecode(false);
}

otError HdlcInterface::Write(const uint8_t *aFrame, uint16_t aLength)
{
    // OT_PLAT_DBG("Send tx frame len = %d", aLength);
    // otDump(OT_LOG_LEVEL_DEBG, OT_LOG_REGION_PLATFORM, "-PLAT---", aFrame, aLength);

    otError                 otResult = OT_ERROR_FAILED;
    serial_manager_status_t serialManagerStatus;
    uint8_t *pWriteHandleAndFrame = (uint8_t *)SPINEL_HDLC_MALLOC(SERIAL_MANAGER_WRITE_HANDLE_SIZE + aLength);
    uint8_t *pNewFrameBuffer      = NULL;
    do
    {
        if (pWriteHandleAndFrame == NULL)
            break;
        pNewFrameBuffer = pWriteHandleAndFrame + SERIAL_MANAGER_WRITE_HANDLE_SIZE;
        memcpy(pNewFrameBuffer, aFrame, aLength);
        serialManagerStatus = SerialManager_OpenWriteHandle((serial_handle_t)otTransceiverSerialHandle,
                                                            (serial_write_handle_t)pWriteHandleAndFrame);
        if (serialManagerStatus != kStatus_SerialManager_Success)
            break;
        serialManagerStatus = SerialManager_InstallTxCallback((serial_write_handle_t)pWriteHandleAndFrame,
                                                              Uart_TxCallBackStatic, pWriteHandleAndFrame);
        if (serialManagerStatus != kStatus_SerialManager_Success)
            break;
        serialManagerStatus =
            SerialManager_WriteNonBlocking((serial_write_handle_t)pWriteHandleAndFrame, pNewFrameBuffer, aLength);
        if (serialManagerStatus != kStatus_SerialManager_Success)
            break;
        otResult = OT_ERROR_NONE;
    } while (0);

    if (otResult != OT_ERROR_NONE && pWriteHandleAndFrame != NULL)
    {
        SPINEL_HDLC_FREE(pWriteHandleAndFrame);
    }
    /* Always clear the encoder */
    encoderBuffer.Clear();
    return otResult;
}

uint32_t HdlcInterface::TryReadAndDecode(bool fullRead)
{
    uint32_t totalBytesRead = 0U;
    uint32_t bytesRead      = 0;
    uint8_t  mUartRxBuffer[256];
    do
    {
        OSA_InterruptDisable();
        SerialManager_TryRead((serial_read_handle_t)otTransceiverSerialReadHandle, mUartRxBuffer, sizeof(mUartRxBuffer),
                              &bytesRead);
        OSA_InterruptEnable();
        if (bytesRead != 0)
        {
            // OT_PLAT_DBG("bytesRead = %d", bytesRead);
            mHdlcDecoder.Decode(mUartRxBuffer, bytesRead);
            totalBytesRead += bytesRead;
            if (!fullRead)
                break;
        }
    } while (bytesRead != 0);

    return totalBytesRead;
}

otError HdlcInterface::WaitForFrame(uint64_t aTimeoutUs)
{
    uint32_t now   = otPlatAlarmMilliGetNow();
    uint32_t start = now;
    otError  error = OT_ERROR_RESPONSE_TIMEOUT;
    do
    {
        /* Poll the frame receive flag */
        if (TryReadAndDecode(true) != 0)
        {
            error = OT_ERROR_NONE;
            break;
        }
        OSA_TimeDelay(200);
        now = otPlatAlarmMilliGetNow();
    } while ((now - start) < aTimeoutUs / 1000);
    return error;
}

void HdlcInterface::HandleHdlcFrame(void *aContext, otError aError)
{
    static_cast<HdlcInterface *>(aContext)->HandleHdlcFrame(aError);
}

void HdlcInterface::HandleHdlcFrame(otError aError)
{
    if (aError == OT_ERROR_NONE)
    {
        OT_PLAT_DBG_NO_FUNC("Frame correctly received");
        mReceiveFrameCallback(mReceiveFrameContext);
    }
    else
    {
        OT_PLAT_DBG_NO_FUNC("Frame will be discarded error = 0x%x", aError);
        mReceiveFrameBuffer.DiscardFrame();
    }
}

void HdlcInterface::InitUart(void)
{
    serial_manager_status_t status;
#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U))
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && (FSL_FEATURE_SOC_DMAMUX_COUNT > 0U))
    dma_mux_configure_t dma_mux;
    dma_mux.dma_dmamux_configure.dma_mux_instance = 0;
    dma_mux.dma_dmamux_configure.rx_request       = kDmaRequestMuxLPUART8Rx;
    dma_mux.dma_dmamux_configure.tx_request       = kDmaRequestMuxLPUART8Tx;
#endif
#endif
#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U))
#if (defined(FSL_FEATURE_EDMA_HAS_CHANNEL_MUX) && (FSL_FEATURE_EDMA_HAS_CHANNEL_MUX > 0U))
    dma_channel_mux_configure_t dma_channel_mux;
    dma_channel_mux.dma_dmamux_configure.dma_mux_instance   = 0;
    dma_channel_mux.dma_dmamux_configure.dma_tx_channel_mux = kDmaRequestMuxLPUART8Rx;
    dma_channel_mux.dma_dmamux_configure.dma_rx_channel_mux = kDmaRequestMuxLPUART8Tx;
#endif
#endif
#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U))
    serial_port_uart_dma_config_t uartConfig =
#else
    serial_port_uart_config_t uartConfig =
#endif
    {.clockRate       = SPINEL_UART_CLOCK_RATE,
     .baudRate        = SPINEL_UART_BAUD_RATE,
     .parityMode      = kSerialManager_UartParityDisabled,
     .stopBitCount    = kSerialManager_UartOneStopBit,
     .enableRx        = 1,
     .enableTx        = 1,
     .enableRxRTS     = SPINEL_ENABLE_RX_RTS,
     .enableTxCTS     = SPINEL_ENABLE_TX_RTS,
     .instance        = SPINEL_UART_INSTANCE,
     .mode            = kSerialManager_UartNonBlockMode,
     .txFifoWatermark = 0,
     .rxFifoWatermark = 0,
#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U))
     .dma_instance = 0,
     .rx_channel   = LPUART_RX_DMA_CHANNEL,
     .tx_channel   = LPUART_TX_DMA_CHANNEL,
#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && (FSL_FEATURE_SOC_DMAMUX_COUNT > 0U))
     .dma_mux_configure = &dma_mux,
#else
     .dma_mux_configure         = NULL,
#endif
#if (defined(FSL_FEATURE_EDMA_HAS_CHANNEL_MUX) && (FSL_FEATURE_EDMA_HAS_CHANNEL_MUX > 0U))
     .dma_channel_mux_configure = &dma_channel_mux,
#else
     .dma_channel_mux_configure = NULL,
#endif
#endif
    };

    serial_manager_config_t serialManagerConfig = {
        .ringBuffer     = &s_ringBuffer[0],
        .ringBufferSize = sizeof(s_ringBuffer),
#if (defined(HAL_UART_DMA_ENABLE) && (HAL_UART_DMA_ENABLE > 0U))
        .type = kSerialPort_UartDma,
#else
        .type = kSerialPort_Uart,
#endif
        .blockType  = kSerialManager_NonBlocking,
        .portConfig = (serial_port_uart_config_t *)&uartConfig,
    };

    status = SerialManager_Init((serial_handle_t)otTransceiverSerialHandle, &serialManagerConfig);
    assert(status == kStatus_SerialManager_Success);
    status = SerialManager_OpenReadHandle((serial_handle_t)otTransceiverSerialHandle,
                                          (serial_read_handle_t)otTransceiverSerialReadHandle);
    assert(status == kStatus_SerialManager_Success);
    status = SerialManager_InstallRxCallback((serial_read_handle_t)otTransceiverSerialReadHandle, Uart_RxCallBackStatic,
                                             NULL);
    assert(status == kStatus_SerialManager_Success);
    OT_UNUSED_VARIABLE(status);
}

void HdlcInterface::DeinitUart(void)
{
    serial_manager_status_t status;
    status = SerialManager_CloseReadHandle((serial_read_handle_t)otTransceiverSerialReadHandle);
    assert(status == kStatus_SerialManager_Success);
    status = SerialManager_Deinit((serial_handle_t)otTransceiverSerialHandle);
    assert(status == kStatus_SerialManager_Success);
    OT_UNUSED_VARIABLE(status);
}

} // namespace RT

} // namespace ot
