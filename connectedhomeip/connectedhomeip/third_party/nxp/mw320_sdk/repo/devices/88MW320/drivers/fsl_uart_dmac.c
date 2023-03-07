/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_uart_dmac.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.uart_dmac"
#endif

/* UART DMAC transfer state. */
enum
{
    kUART_TxIdle, /* TX idle. */
    kUART_TxBusy, /* TX busy. */
    kUART_RxIdle, /* RX idle. */
    kUART_RxBusy  /* RX busy. */
};

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief UART DMAC send finished callback function.
 *
 * This function is called when UART DMAC send finished. It disables the UART
 * TX DMAC request and sends @ref kStatus_UART_TxIdle to UART callback.
 *
 * @param handle The DMAC handle.
 * @param param Callback function parameter.
 * @param status The mask of channel status.
 */
static void UART_TransferSendDMACCallback(dmac_handle_t *handle, void *param, uint32_t status);

/*!
 * @brief UART DMAC receive finished callback function.
 *
 * This function is called when UART DMAC receive finished. It disables the UART
 * RX DMAC request and sends @ref kStatus_UART_RxIdle to UART callback.
 *
 * @param handle The DMAC handle.
 * @param param Callback function parameter.
 * @param status The mask of channel status.
 */
static void UART_TransferReceiveDMACCallback(dmac_handle_t *handle, void *param, uint32_t status);

/*******************************************************************************
 * Code
 ******************************************************************************/
static void UART_TransferSendDMACCallback(dmac_handle_t *handle, void *param, uint32_t status)
{
    assert((handle != NULL) && (param != NULL));

    uart_dmac_handle_t *uartHandle = (uart_dmac_handle_t *)param;

    /* Disable interrupt. */
    DMAC_DisableChannelInterrupts(handle->base, handle->channel, kDMAC_ChannelInterruptFlagTransferComplete);

    uartHandle->txState = (uint8_t)kUART_TxIdle;

    /* Ensure all the data in the transmit buffer are sent out to bus. */
    while (0U == (uartHandle->base->LSR & UART_LSR_TEMT_MASK))
    {
    }

    if (uartHandle->callback != NULL)
    {
        uartHandle->callback(uartHandle->base, uartHandle, kStatus_UART_TxIdle, uartHandle->userData);
    }
}

static void UART_TransferReceiveDMACCallback(dmac_handle_t *handle, void *param, uint32_t status)
{
    assert((handle != NULL) && (param != NULL));

    uart_dmac_handle_t *uartHandle = (uart_dmac_handle_t *)param;

    /* Disable interrupt. */
    DMAC_DisableChannelInterrupts(handle->base, handle->channel, kDMAC_ChannelInterruptFlagTransferComplete);

    uartHandle->rxState = (uint8_t)kUART_RxIdle;

    if (uartHandle->callback != NULL)
    {
        uartHandle->callback(uartHandle->base, uartHandle, kStatus_UART_RxIdle, uartHandle->userData);
    }
}

/*!
 * brief Initializes the UART handle which is used in transactional functions and sets the callback.
 *
 * param base UART peripheral base address.
 * param handle Pointer to the uart_dmac_handle_t structure.
 * param callback UART callback, NULL means no callback.
 * param userData User callback function data.
 * param rxDmaHandle User requested DMAC handle for the RX DMAC transfer.
 * param txDmaHandle User requested DMAC handle for the TX DMAC transfer.
 */
void UART_TransferCreateHandleDMAC(UART_Type *base,
                                   uart_dmac_handle_t *handle,
                                   uart_dmac_transfer_callback_t callback,
                                   void *userData,
                                   dmac_handle_t *txDmaHandle,
                                   dmac_handle_t *rxDmaHandle)
{
    assert((handle != NULL) && (txDmaHandle != NULL) && (rxDmaHandle != NULL));

    (void)memset(handle, 0, sizeof(*handle));

    handle->rxState = (uint8_t)kUART_RxIdle;
    handle->txState = (uint8_t)kUART_TxIdle;

    handle->callback = callback;
    handle->userData = userData;

    handle->rxDmaHandle = rxDmaHandle;
    handle->txDmaHandle = txDmaHandle;

    /* Configure TX DMAC callback. */
    if (txDmaHandle != NULL)
    {
        DMAC_TransferSetCallback(handle->txDmaHandle, UART_TransferSendDMACCallback, handle);
    }

    /* Configure RX DMAC callback. */
    if (rxDmaHandle != NULL)
    {
        DMAC_TransferSetCallback(handle->rxDmaHandle, UART_TransferReceiveDMACCallback, handle);
    }
}

/*!
 * brief Sends data using DMAC.
 *
 * This function sends data using DMAC. This is non-blocking function, which returns
 * right away. When all data is sent, the send callback function is called.
 *
 * param base UART peripheral base address.
 * param handle UART handle pointer.
 * param xfer UART DMAC transfer structure. See #uart_transfer_t.
 * retval kStatus_Success if succeeded; otherwise failed.
 * retval kStatus_UART_TxBusy Previous transfer ongoing.
 * retval kStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferSendDMAC(UART_Type *base, uart_dmac_handle_t *handle, uart_transfer_t *xfer)
{
    assert((handle != NULL) && (handle->txDmaHandle != NULL));
    assert((xfer != NULL) && (xfer->data != NULL) && (xfer->dataSize != 0U));

    status_t status;

    /* If previous TX not finished. */
    if ((uint8_t)kUART_TxBusy == handle->txState)
    {
        status = kStatus_UART_TxBusy;
    }
    else
    {
        dmac_channel_transfer_config_t dmacConfig;

        handle->txState = (uint8_t)kUART_TxBusy;

        /* Prepare transfer. */
        DMAC_GetChannelDefaultTransferConfig(&dmacConfig, (void *)xfer->data,
                                             (uint32_t *)UART_GetTxDataRegisterAddress(base),
                                             kDMAC_ChannelTransferWidth8Bits, kDMAC_ChannelBurstLength1DataWidth,
                                             xfer->dataSize, kDMAC_ChannelTransferMemoryToPeripheral);
        dmacConfig.interruptMask = kDMAC_ChannelInterruptFlagTransferComplete;
        DMAC_TransferSubmit(handle->txDmaHandle, &dmacConfig);
        DMAC_TransferStart(handle->txDmaHandle);
        UART_Enable(base, true);
        UART_EnableDMA(base, true);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Receives data using DMAC.
 *
 * This function receives data using DMAC. This is non-blocking function, which returns
 * right away. When all data is received, the receive callback function is called.
 *
 * param base UART peripheral base address.
 * param handle Pointer to the uart_dmac_handle_t structure.
 * param xfer UART DMAC transfer structure. See #uart_transfer_t.
 * retval kStatus_Success if succeeded; otherwise failed.
 * retval kStatus_UART_RxBusy Previous transfer on going.
 * retval kStatus_InvalidArgument Invalid argument.
 */
status_t UART_TransferReceiveDMAC(UART_Type *base, uart_dmac_handle_t *handle, uart_transfer_t *xfer)
{
    assert((handle != NULL) && (handle->rxDmaHandle != NULL));
    assert((xfer != NULL) && (xfer->data != NULL) && (xfer->dataSize != 0U));

    status_t status;

    /* If previous RX not finished. */
    if ((uint8_t)kUART_RxBusy == handle->rxState)
    {
        status = kStatus_UART_RxBusy;
    }
    else
    {
        dmac_channel_transfer_config_t dmacConfig;

        handle->rxState = (uint8_t)kUART_RxBusy;

        /* Prepare transfer. */
        DMAC_GetChannelDefaultTransferConfig(&dmacConfig, (uint32_t *)UART_GetRxDataRegisterAddress(base),
                                             (void *)xfer->data, kDMAC_ChannelTransferWidth8Bits,
                                             kDMAC_ChannelBurstLength1DataWidth, xfer->dataSize,
                                             kDMAC_ChannelTransferPeripheralToMemory);
        dmacConfig.interruptMask = kDMAC_ChannelInterruptFlagTransferComplete;
        DMAC_TransferSubmit(handle->rxDmaHandle, &dmacConfig);
        DMAC_TransferStart(handle->rxDmaHandle);
        UART_Enable(base, true);
        UART_EnableDMA(base, true);

        status = kStatus_Success;
    }

    return status;
}

/*!
 * brief Aborts the send data using DMAC.
 *
 * This function aborts the sent data using DMAC.
 *
 * note This function only aborts the tx DMAC transfer not disable the DMA function, since disable DMA function will
 * disable tx and rx together. If user wants to abort both tx/rx DMAC transfer, use UART_EnableDMA(base, false).
 *
 * param base UART peripheral base address.
 * param handle Pointer to uart_dmac_handle_t structure.
 */
void UART_TransferAbortSendDMAC(UART_Type *base, uart_dmac_handle_t *handle)
{
    assert((handle != NULL) && (handle->txDmaHandle != NULL));

    /* Stop transfer. */
    DMAC_TransferStop(handle->txDmaHandle);

    handle->txState = (uint8_t)kUART_TxIdle;
}

/*!
 * brief Aborts the received data using DMAC.
 *
 * This function abort receive data which using DMAC.
 *
 * note This function only aborts the rx DMAC transfer not disable the DMA function, since disable DMA function will
 * disable tx and rx together. If user wants to abort both tx/rx DMAC transfer, use UART_EnableDMA(base, false).
 *
 * param base UART peripheral base address.
 * param handle Pointer to uart_dmac_handle_t structure.
 */
void UART_TransferAbortReceiveDMAC(UART_Type *base, uart_dmac_handle_t *handle)
{
    assert((handle != NULL) && (handle->rxDmaHandle != NULL));

    /* Stop transfer. */
    DMAC_TransferStop(handle->rxDmaHandle);

    handle->rxState = (uint8_t)kUART_RxIdle;
}
