/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_ssp_dmac.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.ssp_dmac"
#endif

/*! @brief Mapping of dmac burst length */
#define DMAC_BURST_LENGTH_MAPPING(fifoWatermark)                                                                \
    ((fifoWatermark) < 4U ? kDMAC_ChannelBurstLength1DataWidth :                                                \
                            (fifoWatermark) < 8U ? kDMAC_ChannelBurstLength4DataWidth :                         \
                                                   (fifoWatermark) < 16U ? kDMAC_ChannelBurstLength8DataWidth : \
                                                                           kDMAC_ChannelBurstLength16DataWidth)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief SSP EDMAC callback for send.
 *
 * @param handle pointer to ssp_dmac_handle_t structure which stores the transfer state.
 * @param userData Parameter for user callback.
 * @param transferStatus dmac transfer status
 */
static void SSP_TxDMACallback(dmac_handle_t *handle, void *userData, uint32_t transferStatus);

/*!
 * @brief SSP EDMAC callback for receive.
 *
 * @param handle pointer to ssp_dmac_handle_t structure which stores the transfer state.
 * @param userData Parameter for user callback.
 * @param transferStatus dmac transfer status
 */
static void SSP_RxDMACallback(dmac_handle_t *handle, void *userData, uint32_t transferStatus);

/*******************************************************************************
 * Code
 ******************************************************************************/
static void SSP_TxDMACallback(dmac_handle_t *handle, void *userData, uint32_t transferStatus)
{
    ssp_dmac_handle_t *sspHandle = (ssp_dmac_handle_t *)userData;

    if (transferStatus == (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete)
    {
        /* Call callback function */
        if (sspHandle->callback != NULL)
        {
            (sspHandle->callback)(sspHandle, kSSP_TransferStatusTxIdle, sspHandle->userData);
        }

        SSP_TransferAbortSendDMAC(sspHandle);
    }
}

static void SSP_RxDMACallback(dmac_handle_t *handle, void *userData, uint32_t transferStatus)
{
    ssp_dmac_handle_t *sspHandle = (ssp_dmac_handle_t *)userData;

    if (transferStatus == (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete)
    {
        /* Call callback function */
        if (sspHandle->callback != NULL)
        {
            (sspHandle->callback)(sspHandle, kSSP_TransferStatusRxIdle, sspHandle->userData);
        }

        SSP_TransferAbortReceiveDMAC(sspHandle);
    }
}

/*!
 * brief Initializes the SSP send DMAC handle.
 *
 * This function initializes the SSP send DMAC handle, must be called after the SSP mode configuration done.
 *
 * param base SSP base pointer.
 * param handle SSP DMAC handle pointer.
 * param base SSP peripheral base address.
 * param callback Pointer to user callback function.
 * param userData User parameter passed to the callback function.
 * param dmacHandle DMAC handle pointer, this handle shall be static allocated by users.
 */
void SSP_TransferSendCreateHandleDMAC(
    SSP_Type *base, ssp_dmac_handle_t *handle, ssp_dmac_callback_t callback, void *userData, dmac_handle_t *dmacHandle)
{
    assert((handle != NULL) && (dmacHandle != NULL));

    uint32_t fifoWatermark = ((base->SSP_SSCR1 & SSP_SSP_SSCR1_TFT_MASK) >> SSP_SSP_SSCR1_TFT_SHIFT) + 1U;
    uint32_t regBitWidth   = base->SSP_SSCR0 & (SSP_SSP_SSCR0_DSS_MASK | SSP_SSP_SSCR0_EDSS_MASK);

    /* Zero the handle */
    (void)memset(handle, 0, sizeof(*handle));

    /* Set ssp base to handle */
    handle->dmacHandle  = dmacHandle;
    handle->callback    = callback;
    handle->userData    = userData;
    handle->base        = base;
    handle->burstLength = DMAC_BURST_LENGTH_MAPPING(fifoWatermark);
    handle->bitWidth =
        regBitWidth == (uint32_t)kSSP_DataSize8Bit ? 1U : regBitWidth == (uint32_t)kSSP_DataSize16Bit ? 2U : 4U;

    /* Set SSP state to idle */
    handle->state = (uint32_t)kSSP_TransferStatusTxIdle;

    /* Install callback for Tx dmac channel */
    DMAC_TransferSetCallback(dmacHandle, SSP_TxDMACallback, handle);
}

/*!
 * brief Initializes the SSP receive DMAC handle.
 *
 * This function initializes the SSP receive DMAC handle, must be called after the SSP mode configuration done.
 *
 * param base SSP base pointer.
 * param handle SSP DMAC handle pointer.
 * param base SSP peripheral base address.
 * param callback Pointer to user callback function.
 * param userData User parameter passed to the callback function.
 * param dmacHandle DMAC handle pointer, this handle shall be static allocated by users.
 */
void SSP_TransferReceiveCreateHandleDMAC(
    SSP_Type *base, ssp_dmac_handle_t *handle, ssp_dmac_callback_t callback, void *userData, dmac_handle_t *dmacHandle)
{
    assert((handle != NULL) && (dmacHandle != NULL));

    uint32_t fifoWatermark = ((base->SSP_SSCR1 & SSP_SSP_SSCR1_RFT_MASK) >> SSP_SSP_SSCR1_RFT_SHIFT) + 1U;
    uint32_t regBitWidth   = base->SSP_SSCR0 & (SSP_SSP_SSCR0_DSS_MASK | SSP_SSP_SSCR0_EDSS_MASK);

    /* Zero the handle */
    (void)memset(handle, 0, sizeof(*handle));

    /* Set ssp base to handle */
    handle->dmacHandle  = dmacHandle;
    handle->callback    = callback;
    handle->userData    = userData;
    handle->base        = base;
    handle->burstLength = DMAC_BURST_LENGTH_MAPPING(fifoWatermark);
    handle->bitWidth =
        regBitWidth == (uint32_t)kSSP_DataSize8Bit ? 1U : regBitWidth == (uint32_t)kSSP_DataSize16Bit ? 2U : 4U;

    /* Set SSP state to idle */
    handle->state = (uint32_t)kSSP_TransferStatusTxIdle;

    /* Install callback for Tx dmac channel */
    DMAC_TransferSetCallback(dmacHandle, SSP_RxDMACallback, handle);
}

/*!
 * brief Performs a non-blocking SSP transfer using DMAC.
 *
 * note This interface returns immediately after the transfer initiates.
 *
 * param base SSP base pointer.
 * param handle SSP DMAC handle pointer.
 * param xfer Pointer to DMAC transfer structure.
 * retval kStatus_Success Successfully start the data receive.
 * retval kStatus_SSP_TxBusy Previous receive still not finished.
 * retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t SSP_TransferSendDMAC(ssp_dmac_handle_t *handle, ssp_transfer_t *xfer)
{
    status_t error = kStatus_Success;
    dmac_channel_transfer_config_t transferConfig;

    /* Check if input parameter invalid */
    if ((handle == NULL) || (xfer->dataBuffer == NULL) || (xfer->dataSize == 0U))
    {
        error = kStatus_InvalidArgument;
    }
    else
    {
        if (handle->state == (uint32_t)kSSP_TransferStatusBusy)
        {
            error = kSSP_TransferStatusBusy;
        }
        else
        {
            DMAC_GetChannelDefaultTransferConfig(&transferConfig, xfer->dataBuffer,
                                                 (uint32_t *)SSP_GetFifoAddress(handle->base),
                                                 (dmac_channel_transfer_width_t)handle->bitWidth, handle->burstLength,
                                                 xfer->dataSize, kDMAC_ChannelTransferMemoryToPeripheral);
            transferConfig.interruptMask = (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete;
            if (DMAC_TransferSubmit(handle->dmacHandle, &transferConfig) != kStatus_Success)
            {
                error = kSSP_TransferStatusBusy;
            }
            else
            {
                /* Change the state of handle */
                handle->state = (uint32_t)kSSP_TransferStatusBusy;

                /* Start DMAC transfer */
                DMAC_TransferStart(handle->dmacHandle);

                /* Enable DMAC request and start SSP */
                SSP_EnableSendDMARequest(handle->base, true);
                /* Enable ssp port */
                SSP_Enable(handle->base, true);
            }
        }
    }

    return error;
}

/*!
 * brief Performs a non-blocking SSP transfer using DMAC.
 *
 * note This interface returns immediately after transfer initiates.
 *
 * param base SSP base pointer
 * param handle SSP DMAC handle pointer.
 * param xfer Pointer to DMAC transfer structure.
 * retval kStatus_Success Successfully start the data receive.
 * retval kStatus_SSP_RxBusy Previous receive still not finished.
 * retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t SSP_TransferReceiveDMAC(ssp_dmac_handle_t *handle, ssp_transfer_t *xfer)
{
    status_t error = kStatus_Success;
    dmac_channel_transfer_config_t transferConfig;

    /* Check if input parameter invalid */
    if ((handle == NULL) || (xfer->dataBuffer == NULL) || (xfer->dataSize == 0U))
    {
        error = kStatus_InvalidArgument;
    }
    else
    {
        if (handle->state == (uint32_t)kSSP_TransferStatusBusy)
        {
            error = kSSP_TransferStatusBusy;
        }
        else
        {
            DMAC_GetChannelDefaultTransferConfig(&transferConfig, (uint32_t *)SSP_GetFifoAddress(handle->base),
                                                 xfer->dataBuffer, (dmac_channel_transfer_width_t)handle->bitWidth,
                                                 handle->burstLength, xfer->dataSize,
                                                 kDMAC_ChannelTransferPeripheralToMemory);
            transferConfig.interruptMask = (uint32_t)kDMAC_ChannelInterruptFlagTransferComplete;
            if (DMAC_TransferSubmit(handle->dmacHandle, &transferConfig) != kStatus_Success)
            {
                error = kSSP_TransferStatusBusy;
            }
            else
            {
                /* Change the state of handle */
                handle->state = (uint32_t)kSSP_TransferStatusBusy;
                /* Start DMAC transfer */
                DMAC_TransferStart(handle->dmacHandle);
                /* Enable DMAC request and start SSP */
                SSP_EnableReceiveDMARequest(handle->base, true);
                /* Enable ssp port */
                SSP_Enable(handle->base, true);
            }
        }
    }

    return error;
}

/*!
 * brief Aborts a SSP transfer using DMAC.
 *
 * param base SSP base pointer.
 * param handle SSP DMAC handle pointer.
 */
void SSP_TransferAbortSendDMAC(ssp_dmac_handle_t *handle)
{
    assert(handle != NULL);

    /* Disable dmac */
    DMAC_TransferStop(handle->dmacHandle);

    /* Disable DMAC enable bit */
    SSP_EnableSendDMARequest(handle->base, false);

    /* Set the handle state */
    handle->state = (uint32_t)kSSP_TransferStatusTxIdle;
}

/*!
 * brief Aborts a SSP transfer using DMAC.
 *
 * param base SSP base pointer.
 * param handle SSP DMAC handle pointer.
 */
void SSP_TransferAbortReceiveDMAC(ssp_dmac_handle_t *handle)
{
    assert(handle != NULL);

    /* Disable dmac */
    DMAC_TransferStop(handle->dmacHandle);

    /* Disable DMAC enable bit */
    SSP_EnableReceiveDMARequest(handle->base, false);

    /* Set the handle state */
    handle->state = (uint32_t)kSSP_TransferStatusRxIdle;
}
