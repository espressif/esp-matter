/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_flexio_mculcd_dma.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.flexio_mculcd_dma"
#endif

#define DMA_MAX_XFER_COUNT ((DMA_CHANNEL_XFERCFG_XFERCOUNT_MASK >> DMA_CHANNEL_XFERCFG_XFERCOUNT_SHIFT) + 1U)

enum _MCULCD_transfer_state
{
    kFLEXIO_MCULCD_StateIdle,           /*!< No transfer in progress. */
    kFLEXIO_MCULCD_StateReadArray,      /*!< Reading array in progress. */
    kFLEXIO_MCULCD_StateWriteArray,     /*!< Writing array in progress. */
    kFLEXIO_MCULCD_StateWriteSameValue, /*!< Writing the same value in progress.
                                         */
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief DMA callback function for FLEXIO MCULCD TX.
 *
 * For details, see @ref dma_callback.
 */
static void FLEXIO_MCULCD_TxDMACallback(dma_handle_t *dmaHandle, void *param, bool transferDone, uint32_t intMode);

/*!
 * @brief DMA callback function for FLEXIO MCULCD RX.
 *
 * For details, see @ref dma_callback.
 */
static void FLEXIO_MCULCD_RxDMACallback(dma_handle_t *dmaHandle, void *param, bool transferDone, uint32_t intMode);

/*!
 * @brief Set DMA config for FLEXIO MCULCD transfer.
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle pointer to flexio_mculcd_dma_handle_t structure to store the
 * transfer state.
 */
static void FLEXIO_MCULCD_DMAConfig(FLEXIO_MCULCD_Type *base, flexio_mculcd_dma_handle_t *handle);

/*!
 * @brief Get the DMA trigger burst.
 *
 * In the driver, one burst reads or writes all the assigned FlexIO shifters.
 * This function calculates the trigger burst based on assigned FlexIO shifters number,
 *
 * @param shifterNum The number of assigned FlexIO shifters.
 * @return If there is valid trigger burst value, returns true, otherwise returns false.
 */
static bool FLEXIO_MCULCD_GetTrigerBurst(uint32_t shifterNum, dma_trigger_burst_t *triggerBurst);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

static void FLEXIO_MCULCD_TxDMACallback(dma_handle_t *dmaHandle, void *param, bool transferDone, uint32_t intMode)
{
    intMode                                        = intMode;
    flexio_mculcd_dma_handle_t *flexioLcdMcuHandle = (flexio_mculcd_dma_handle_t *)param;
    FLEXIO_MCULCD_Type *flexioLcdMcuBase           = flexioLcdMcuHandle->base;

    if (transferDone)
    {
        if (flexioLcdMcuHandle->remainingCount >= flexioLcdMcuHandle->burstBytes)
        {
            FLEXIO_MCULCD_DMAConfig(flexioLcdMcuBase, flexioLcdMcuHandle);
            DMA_StartTransfer(flexioLcdMcuHandle->txDmaHandle);
        }
        else
        {
            FLEXIO_MCULCD_EnableTxDMA(flexioLcdMcuBase, false);

            /* Now the data are in shifter, wait for the data send out from the shifter. */
            FLEXIO_MCULCD_WaitTransmitComplete();

            /* Disable the TX shifter and the timer. */
            FLEXIO_MCULCD_ClearMultiBeatsWriteConfig(flexioLcdMcuBase);

            /* Send the remaining data. */
            if (flexioLcdMcuHandle->remainingCount)
            {
                if (kFLEXIO_MCULCD_StateWriteSameValue == flexioLcdMcuHandle->state)
                {
                    FLEXIO_MCULCD_WriteSameValueBlocking(flexioLcdMcuBase, flexioLcdMcuHandle->dataAddrOrSameValue,
                                                         flexioLcdMcuHandle->remainingCount);
                }
                else
                {
                    FLEXIO_MCULCD_WriteDataArrayBlocking(flexioLcdMcuBase,
                                                         (void *)flexioLcdMcuHandle->dataAddrOrSameValue,
                                                         flexioLcdMcuHandle->remainingCount);
                }
            }

            /* De-assert nCS. */
            FLEXIO_MCULCD_StopTransfer(flexioLcdMcuBase);

            /* Change the state. */
            flexioLcdMcuHandle->state          = kFLEXIO_MCULCD_StateIdle;
            flexioLcdMcuHandle->dataCount      = 0;
            flexioLcdMcuHandle->remainingCount = 0;

            /* Callback to inform upper layer. */
            if (flexioLcdMcuHandle->completionCallback)
            {
                flexioLcdMcuHandle->completionCallback(flexioLcdMcuBase, flexioLcdMcuHandle, kStatus_FLEXIO_MCULCD_Idle,
                                                       flexioLcdMcuHandle->userData);
            }
        }
    }
}

static void FLEXIO_MCULCD_RxDMACallback(dma_handle_t *dmaHandle, void *param, bool transferDone, uint32_t tcds)
{
    tcds = tcds;
    uint32_t i;
    uint32_t rxBufAddr;
    flexio_mculcd_dma_handle_t *flexioLcdMcuHandle = (flexio_mculcd_dma_handle_t *)param;
    FLEXIO_MCULCD_Type *flexioLcdMcuBase           = flexioLcdMcuHandle->base;
    FLEXIO_Type *flexioBase                        = flexioLcdMcuBase->flexioBase;

    if (transferDone)
    {
        if (flexioLcdMcuHandle->remainingCount >= (2 * flexioLcdMcuHandle->burstBytes))
        {
            FLEXIO_MCULCD_DMAConfig(flexioLcdMcuBase, flexioLcdMcuHandle);
            DMA_StartTransfer(flexioLcdMcuHandle->rxDmaHandle);
        }
        else
        {
            FLEXIO_MCULCD_EnableRxDMA(flexioLcdMcuBase, false);

            /* Wait the data saved to the shifter buffer. */
            while (!((1U << flexioLcdMcuBase->rxShifterEndIndex) & FLEXIO_GetShifterStatusFlags(flexioBase)))
            {
            }

            /* Disable the RX shifter and the timer. */
            FLEXIO_MCULCD_ClearMultiBeatsReadConfig(flexioLcdMcuBase);

            rxBufAddr = FLEXIO_MCULCD_GetRxDataRegisterAddress(flexioLcdMcuBase);

/* Read out the data. */
#if (defined(__CORTEX_M) && (__CORTEX_M == 0))
            /* Cortex M0 and M0+ only support aligned access. */
            for (i = 0; i < flexioLcdMcuHandle->rxShifterNum * 4; i++)
            {
                ((uint8_t *)(flexioLcdMcuHandle->dataAddrOrSameValue))[i] = ((volatile uint8_t *)rxBufAddr)[i];
            }
#else
            for (i = 0; i < flexioLcdMcuHandle->rxShifterNum; i++)
            {
                ((uint32_t *)(flexioLcdMcuHandle->dataAddrOrSameValue))[i] = ((volatile uint32_t *)rxBufAddr)[i];
            }
#endif
            flexioLcdMcuHandle->remainingCount -= flexioLcdMcuHandle->burstBytes;

            if (flexioLcdMcuHandle->remainingCount)
            {
                FLEXIO_MCULCD_ReadDataArrayBlocking(
                    flexioLcdMcuBase,
                    (void *)(flexioLcdMcuHandle->dataAddrOrSameValue + flexioLcdMcuHandle->burstBytes),
                    flexioLcdMcuHandle->remainingCount);
            }

            /* De-assert nCS. */
            FLEXIO_MCULCD_StopTransfer(flexioLcdMcuBase);

            /* Change the state. */
            flexioLcdMcuHandle->state          = kFLEXIO_MCULCD_StateIdle;
            flexioLcdMcuHandle->dataCount      = 0;
            flexioLcdMcuHandle->remainingCount = 0;

            /* Callback to inform upper layer. */
            if (flexioLcdMcuHandle->completionCallback)
            {
                flexioLcdMcuHandle->completionCallback(flexioLcdMcuBase, flexioLcdMcuHandle, kStatus_FLEXIO_MCULCD_Idle,
                                                       flexioLcdMcuHandle->userData);
            }
        }
    }
}

static void FLEXIO_MCULCD_DMAConfig(FLEXIO_MCULCD_Type *base, flexio_mculcd_dma_handle_t *handle)
{
    uint32_t xferCount;
    uint32_t transferCount;
    uint32_t burstCounts;
    dma_channel_config_t channelConfig   = {0};
    dma_channel_trigger_t channelTrigger = {kDMA_NoTrigger};

    burstCounts = handle->remainingCount / handle->burstBytes;

    /* For reading, the last burst data is not tranfered by DMA. */
    if (kFLEXIO_MCULCD_StateReadArray == handle->state)
    {
        burstCounts--;
    }

    xferCount = burstCounts * (handle->burstBytes / 4);

    if (xferCount > DMA_MAX_XFER_COUNT)
    {
        xferCount = DMA_MAX_XFER_COUNT;
    }

    transferCount = xferCount * 4;

    channelConfig.trigger  = &channelTrigger;
    channelConfig.isPeriph = true;
    channelConfig.nextDesc = NULL;
    channelTrigger.type    = kDMA_RisingEdgeTrigger;

    if (kFLEXIO_MCULCD_StateReadArray == handle->state)
    {
        channelConfig.srcStartAddr = (void *)FLEXIO_MCULCD_GetRxDataRegisterAddress(base);
        channelConfig.dstStartAddr = (void *)(handle->dataAddrOrSameValue);
        channelConfig.xferCfg      = DMA_CHANNEL_XFER(false, true, true, false, 4, kDMA_AddressInterleave1xWidth,
                                                 kDMA_AddressInterleave1xWidth, transferCount);

        channelTrigger.wrap  = kDMA_SrcWrap;
        channelTrigger.burst = handle->rxTriggerBurst;

        handle->remainingCount -= transferCount;
        handle->dataAddrOrSameValue += transferCount;
        DMA_SubmitChannelTransfer(handle->rxDmaHandle, &channelConfig);
    }
    else
    {
        if (kFLEXIO_MCULCD_StateWriteArray == handle->state)
        {
            channelConfig.srcStartAddr = (void *)handle->dataAddrOrSameValue;
            channelConfig.xferCfg      = DMA_CHANNEL_XFER(false, true, true, false, 4, kDMA_AddressInterleave1xWidth,
                                                     kDMA_AddressInterleave1xWidth, transferCount);
            handle->dataAddrOrSameValue += transferCount;
        }
        else
        {
            channelConfig.srcStartAddr = (&(handle->dataAddrOrSameValue));
            channelConfig.xferCfg      = DMA_CHANNEL_XFER(false, true, true, false, 4, kDMA_AddressInterleave0xWidth,
                                                     kDMA_AddressInterleave1xWidth, transferCount);
        }

        channelConfig.dstStartAddr = (void *)FLEXIO_MCULCD_GetTxDataRegisterAddress(base);
        channelTrigger.wrap        = kDMA_DstWrap;
        channelTrigger.burst       = handle->txTriggerBurst;
        handle->remainingCount -= transferCount;
        DMA_SubmitChannelTransfer(handle->txDmaHandle, &channelConfig);
    }
}

static bool FLEXIO_MCULCD_GetTrigerBurst(uint32_t shifterNum, dma_trigger_burst_t *triggerBurst)
{
    bool ret = true;

    switch (shifterNum)
    {
        case 1U:
            *triggerBurst = kDMA_EdgeBurstTransfer1;
            break;
        case 2U:
            *triggerBurst = kDMA_EdgeBurstTransfer2;
            break;
        case 4U:
            *triggerBurst = kDMA_EdgeBurstTransfer4;
            break;
        case 8U:
            *triggerBurst = kDMA_EdgeBurstTransfer8;
            break;
        case 16U:
            *triggerBurst = kDMA_EdgeBurstTransfer16;
            break;
        case 32U:
            *triggerBurst = kDMA_EdgeBurstTransfer32;
            break;
        default:
            ret = false;
            break;
    }

    return ret;
}

/*!
 * brief Initializes the FLEXO MCULCD master DMA handle.
 *
 * This function initializes the FLEXO MCULCD master DMA handle which can be
 * used for other FLEXO MCULCD transactional APIs. For a specified FLEXO MCULCD
 * instance, call this API once to get the initialized handle.
 *
 * param base Pointer to FLEXIO_MCULCD_Type structure.
 * param handle Pointer to flexio_mculcd_dma_handle_t structure to store the
 * transfer state.
 * param callback MCULCD transfer complete callback, NULL means no callback.
 * param userData callback function parameter.
 * param txDmaHandle User requested DMA handle for FlexIO MCULCD DMA TX,
 * the DMA request source of this handle should be the first of TX shifters.
 * param rxDmaHandle User requested DMA handle for FlexIO MCULCD DMA RX,
 * the DMA request source of this handle should be the last of RX shifters.
 * retval kStatus_Success Successfully create the handle.
 */
status_t FLEXIO_MCULCD_TransferCreateHandleDMA(FLEXIO_MCULCD_Type *base,
                                               flexio_mculcd_dma_handle_t *handle,
                                               flexio_mculcd_dma_transfer_callback_t callback,
                                               void *userData,
                                               dma_handle_t *txDmaHandle,
                                               dma_handle_t *rxDmaHandle)
{
    assert(handle);

    /* Zero the handle. */
    memset(handle, 0, sizeof(*handle));

    /* Initialize the state. */
    handle->state = kFLEXIO_MCULCD_StateIdle;

    /* Register callback and userData. */
    handle->completionCallback = callback;
    handle->userData           = userData;

    handle->base         = base;
    handle->txShifterNum = base->txShifterEndIndex - base->txShifterStartIndex + 1U;
    handle->rxShifterNum = base->rxShifterEndIndex - base->rxShifterStartIndex + 1U;

    if (rxDmaHandle)
    {
        if (!FLEXIO_MCULCD_GetTrigerBurst(handle->rxShifterNum, &handle->rxTriggerBurst))
        {
            return kStatus_InvalidArgument;
        }

        handle->rxDmaHandle = rxDmaHandle;
        DMA_SetCallback(rxDmaHandle, FLEXIO_MCULCD_RxDMACallback, handle);
    }

    if (txDmaHandle)
    {
        if (!FLEXIO_MCULCD_GetTrigerBurst(handle->txShifterNum, &handle->txTriggerBurst))
        {
            return kStatus_InvalidArgument;
        }

        handle->txDmaHandle = txDmaHandle;
        DMA_SetCallback(txDmaHandle, FLEXIO_MCULCD_TxDMACallback, handle);
    }

    return kStatus_Success;
}

/*!
 * brief Performs a non-blocking FlexIO MCULCD transfer using DMA.
 *
 * This function returns immediately after transfer initiates. To check whether
 * the transfer is completed, user could:
 * 1. Use the transfer completed callback;
 * 2. Polling function ref FLEXIO_MCULCD_GetTransferCountDMA
 *
 * param base pointer to FLEXIO_MCULCD_Type structure.
 * param handle pointer to flexio_mculcd_dma_handle_t structure to store the
 * transfer state.
 * param xfer Pointer to FlexIO MCULCD transfer structure.
 * retval kStatus_Success Successfully start a transfer.
 * retval kStatus_InvalidArgument Input argument is invalid.
 * retval kStatus_FLEXIO_MCULCD_Busy FlexIO MCULCD is not idle, it is running another
 * transfer.
 */
status_t FLEXIO_MCULCD_TransferDMA(FLEXIO_MCULCD_Type *base,
                                   flexio_mculcd_dma_handle_t *handle,
                                   flexio_mculcd_transfer_t *xfer)
{
    assert(handle);
    assert(xfer);

    /*
     * The data transfer mechanism:
     *
     * Read:
     * Assume the data length is Lr = (n1 * burstBytes + n2), where
     * n2 < burstBytes.
     * If (n1 <= 1), then all data are sent using blocking method.
     * If (n1 > 1), then the beginning ((n1-1) * burstBytes) are read
     * using DMA, the left (burstBytes + n2) are read using blocking method.
     *
     * Write:
     * Assume the data length is Lw = (n1 * burstBytes + n2), where
     * n2 < burstBytes.
     * If (n1 = 0), then all data are sent using blocking method.
     * If (n1 >= 1), then the beginning (n1 * burstBytes) are sent
     * using DMA, the left n2 are sent using blocking method.
     */

    /* Check if the device is busy. */
    if (kFLEXIO_MCULCD_StateIdle != handle->state)
    {
        return kStatus_FLEXIO_MCULCD_Busy;
    }

    /* Set the state in handle. */
    if (kFLEXIO_MCULCD_ReadArray == xfer->mode)
    {
        handle->state      = kFLEXIO_MCULCD_StateReadArray;
        handle->burstBytes = handle->rxShifterNum * 4U;
    }
    else
    {
        handle->burstBytes = handle->txShifterNum * 4U;

        if (kFLEXIO_MCULCD_WriteArray == xfer->mode)
        {
            handle->state = kFLEXIO_MCULCD_StateWriteArray;
        }
        else
        {
            handle->state = kFLEXIO_MCULCD_StateWriteSameValue;
        }
    }

    /*
     * For TX, if data is less than one burst, then use polling method.
     * For RX, if data is less than two burst, then use polling method.
     */
    if ((xfer->dataSize < handle->burstBytes) ||
        ((kFLEXIO_MCULCD_ReadArray == xfer->mode) && (xfer->dataSize < 2 * (handle->burstBytes))))
    {
        FLEXIO_MCULCD_TransferBlocking(base, xfer);

        handle->state = kFLEXIO_MCULCD_StateIdle;

        /* Callback to inform upper layer. */
        if (handle->completionCallback)
        {
            handle->completionCallback(base, handle, kStatus_FLEXIO_MCULCD_Idle, handle->userData);
        }
    }
    else
    {
        handle->dataCount      = xfer->dataSize;
        handle->remainingCount = xfer->dataSize;

        /*
         * In the driver, DMA transfer size is set to 4 bytes, so if sending the
         * same data, the source value should be extended to 4 bytes.
         */
        if (kFLEXIO_MCULCD_WriteSameValue == xfer->mode)
        {
#if (8 == FLEXIO_MCULCD_DATA_BUS_WIDTH)
            handle->dataAddrOrSameValue = ((uint32_t)((uint8_t)xfer->dataAddrOrSameValue)) * 0x01010101U;
#else
            handle->dataAddrOrSameValue = ((uint32_t)((uint16_t)xfer->dataAddrOrSameValue)) * 0x00010001U;
#endif
        }
        else
        {
            handle->dataAddrOrSameValue = xfer->dataAddrOrSameValue;
        }

        /* Setup DMA to transfer data.  */
        /* Assert the nCS. */
        FLEXIO_MCULCD_StartTransfer(base);
        /* Send the command. */
        FLEXIO_MCULCD_WriteCommandBlocking(base, xfer->command);

        /* Setup the DMA configuration. */
        FLEXIO_MCULCD_DMAConfig(base, handle);

        /* Start the transfer. */
        if (kFLEXIO_MCULCD_ReadArray == xfer->mode)
        {
            /* For 6800, assert the RDWR pin. */
            if (kFLEXIO_MCULCD_6800 == base->busType)
            {
                base->setRDWRPin(true);
            }
            FLEXIO_MCULCD_SetMultiBeatsReadConfig(base);
            FLEXIO_MCULCD_EnableRxDMA(base, true);
            DMA_StartTransfer(handle->rxDmaHandle);
        }
        else
        {
            /* For 6800, de-assert the RDWR pin. */
            if (kFLEXIO_MCULCD_6800 == base->busType)
            {
                base->setRDWRPin(false);
            }
            FLEXIO_MCULCD_SetMultiBeatsWriteConfig(base);
            FLEXIO_MCULCD_EnableTxDMA(base, true);
            DMA_StartTransfer(handle->txDmaHandle);
        }
    }

    return kStatus_Success;
}

/*!
 * brief Aborts a FlexIO MCULCD transfer using DMA.
 *
 * param base pointer to FLEXIO_MCULCD_Type structure.
 * param handle FlexIO MCULCD DMA handle pointer.
 */
void FLEXIO_MCULCD_TransferAbortDMA(FLEXIO_MCULCD_Type *base, flexio_mculcd_dma_handle_t *handle)
{
    assert(handle);

    /* Disable dma. */
    if (handle->txDmaHandle)
    {
        DMA_AbortTransfer(handle->txDmaHandle);
    }
    if (handle->rxDmaHandle)
    {
        DMA_AbortTransfer(handle->rxDmaHandle);
    }

    /* Disable DMA enable bit. */
    FLEXIO_MCULCD_EnableTxDMA(handle->base, false);
    FLEXIO_MCULCD_EnableRxDMA(handle->base, false);

    /* Set the handle state. */
    handle->state     = kFLEXIO_MCULCD_StateIdle;
    handle->dataCount = 0;
}

/*!
 * brief Gets the remaining bytes for FlexIO MCULCD DMA transfer.
 *
 * param base pointer to FLEXIO_MCULCD_Type structure.
 * param handle FlexIO MCULCD DMA handle pointer.
 * param count Number of count transferred so far by the DMA transaction.
 * retval kStatus_Success Get the transferred count Successfully.
 * retval kStatus_NoTransferInProgress No transfer in process.
 */
status_t FLEXIO_MCULCD_TransferGetCountDMA(FLEXIO_MCULCD_Type *base, flexio_mculcd_dma_handle_t *handle, size_t *count)
{
    assert(handle);
    assert(count);
    uint32_t state = handle->state;

    if (kFLEXIO_MCULCD_StateIdle == state)
    {
        return kStatus_NoTransferInProgress;
    }
    else
    {
        *count = handle->dataCount - handle->remainingCount;

        if (kFLEXIO_MCULCD_StateReadArray == state)
        {
            *count -= DMA_GetRemainingBytes(handle->rxDmaHandle->base, handle->rxDmaHandle->channel);
        }
        else
        {
            *count -= DMA_GetRemainingBytes(handle->txDmaHandle->base, handle->txDmaHandle->channel);
        }
    }
    return kStatus_Success;
}
