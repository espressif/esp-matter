/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dbi.h"
#include "fsl_dbi_flexio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief FLEXIO DBI bus transfer complete callback function.
 */
static void DBI_FLEXIO_TransferCompletedCallback(FLEXIO_MCULCD_Type *base,
                                                 dbi_flexio_handle_t *handle,
                                                 status_t status,
                                                 void *userData);

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)

/* Wrapper for DMA and eDMA transfer. */
#define _DBI_FLEXIO_TransferDMA FLEXIO_MCULCD_TransferEDMA
#define _DBI_FLEXIO_TransferCreateHandleDMA FLEXIO_MCULCD_TransferCreateHandleEDMA

#else

/* Wrapper for DMA and eDMA transfer. */
#define _DBI_FLEXIO_TransferDMA FLEXIO_MCULCD_TransferDMA
#define _DBI_FLEXIO_TransferCreateHandleDMA FLEXIO_MCULCD_TransferCreateHandleDMA

#endif

/*******************************************************************************
 * Variables
 ******************************************************************************/
const dbi_xfer_ops_t g_dbiFlexioXferOps = {
    .writeCommand          = DBI_FLEXIO_WriteCommand,
    .writeData             = DBI_FLEXIO_WriteData,
    .writeMemory           = DBI_FLEXIO_WriteMemory,
    .readMemory            = DBI_FLEXIO_ReadMemory,
    .setMemoryDoneCallback = DBI_FLEXIO_SetMemoryDoneCallback,
};

/*******************************************************************************
 * Code
 ******************************************************************************/
static void DBI_FLEXIO_TransferCompletedCallback(FLEXIO_MCULCD_Type *base,
                                                 dbi_flexio_handle_t *handle,
                                                 status_t status,
                                                 void *userData)
{
    dbi_flexio_xfer_handle_t *xferHandle = (dbi_flexio_xfer_handle_t *)userData;

    if (kStatus_FLEXIO_MCULCD_Idle == status)
    {
        status = kStatus_Success;
    }

    xferHandle->memDoneCallback(status, xferHandle->userData);
}

void DBI_FLEXIO_SetMemoryDoneCallback(void *dbiXferHandle, dbi_mem_done_callback_t callback, void *userData)
{
    dbi_flexio_xfer_handle_t *xferHandle = (dbi_flexio_xfer_handle_t *)dbiXferHandle;

    xferHandle->memDoneCallback = callback;
    xferHandle->userData        = userData;
}

#if !DBI_FLEXIO_USE_SMARTDMA
status_t DBI_FLEXIO_CreateXferHandle(dbi_flexio_xfer_handle_t *dbiXferHandle,
                                     FLEXIO_MCULCD_Type *flexioLCD,
                                     dbi_flexio_dma_handle_t *txDmaHandle,
                                     dbi_flexio_dma_handle_t *rxDmaHandle)
#else
status_t DBI_FLEXIO_CreateXferHandle(dbi_flexio_xfer_handle_t *dbiXferHandle,
                                     FLEXIO_MCULCD_Type *flexioLCD,
                                     const flexio_mculcd_smartdma_config_t *config)

#endif
{
    status_t status;

    dbi_flexio_xfer_handle_t *xferHandle = (dbi_flexio_xfer_handle_t *)dbiXferHandle;

    memset(xferHandle, 0, sizeof(dbi_flexio_xfer_handle_t));

#if DBI_FLEXIO_USE_SMARTDMA
    status = FLEXIO_MCULCD_TransferCreateHandleSMARTDMA(flexioLCD, &xferHandle->flexioHandle, config,
                                                        DBI_FLEXIO_TransferCompletedCallback, xferHandle);
#else  /* DBI_FLEXIO_USE_SMARTDMA */
    status =
        _DBI_FLEXIO_TransferCreateHandleDMA(flexioLCD, &xferHandle->flexioHandle, DBI_FLEXIO_TransferCompletedCallback,
                                            xferHandle, txDmaHandle, rxDmaHandle);
#endif /* DBI_FLEXIO_USE_SMARTDMA */

    return status;
}

status_t DBI_FLEXIO_WriteCommand(void *dbiXferHandle, uint32_t command)
{
    dbi_flexio_xfer_handle_t *xferHandle = (dbi_flexio_xfer_handle_t *)dbiXferHandle;

    FLEXIO_MCULCD_Type *flexioLCD = xferHandle->flexioHandle.base;

    FLEXIO_MCULCD_StartTransfer(flexioLCD);
    FLEXIO_MCULCD_WriteCommandBlocking(flexioLCD, command);
    FLEXIO_MCULCD_StopTransfer(flexioLCD);

    return kStatus_Success;
}

status_t DBI_FLEXIO_WriteData(void *dbiXferHandle, void *data, uint32_t len_byte)
{
    dbi_flexio_xfer_handle_t *xferHandle = (dbi_flexio_xfer_handle_t *)dbiXferHandle;

    FLEXIO_MCULCD_Type *flexioLCD = xferHandle->flexioHandle.base;

    FLEXIO_MCULCD_StartTransfer(flexioLCD);
    FLEXIO_MCULCD_WriteDataArrayBlocking(flexioLCD, data, len_byte);
    FLEXIO_MCULCD_StopTransfer(flexioLCD);

    return kStatus_Success;
}

status_t DBI_FLEXIO_WriteMemory(void *dbiXferHandle, uint32_t command, const void *data, uint32_t len_byte)
{
    flexio_mculcd_transfer_t xfer;

    dbi_flexio_xfer_handle_t *xferHandle = (dbi_flexio_xfer_handle_t *)dbiXferHandle;

    /* Callback is necessary to notify user. */
    assert(NULL != xferHandle->memDoneCallback);

    FLEXIO_MCULCD_Type *flexioLCD = xferHandle->flexioHandle.base;

#if !DBI_FLEXIO_USE_SMARTDMA
    if (NULL != xferHandle->flexioHandle.txDmaHandle)
    {
        xfer.command             = command;
        xfer.mode                = kFLEXIO_MCULCD_WriteArray;
        xfer.dataAddrOrSameValue = (uint32_t)data;
        xfer.dataSize            = len_byte;

        _DBI_FLEXIO_TransferDMA(flexioLCD, &xferHandle->flexioHandle, &xfer);
    }
    else
    {
        /* If DMA not enabled, use the blocking method. */
        FLEXIO_MCULCD_StartTransfer(flexioLCD);
        FLEXIO_MCULCD_WriteCommandBlocking(flexioLCD, command);
        FLEXIO_MCULCD_WriteDataArrayBlocking(flexioLCD, (void *)data, (size_t)len_byte);
        FLEXIO_MCULCD_StopTransfer(flexioLCD);

        xferHandle->memDoneCallback(kStatus_Success, xferHandle->userData);
    }

#else /* DBI_FLEXIO_USE_SMARTDMA */

    xfer.command             = command;
    xfer.mode                = kFLEXIO_MCULCD_WriteArray;
    xfer.dataAddrOrSameValue = (uint32_t)data;
    xfer.dataSize            = len_byte;

    FLEXIO_MCULCD_TransferSMARTDMA(flexioLCD, &xferHandle->flexioHandle, &xfer);

#endif /* DBI_FLEXIO_USE_SMARTDMA */

    return kStatus_Success;
}

status_t DBI_FLEXIO_ReadMemory(void *dbiXferHandle, uint32_t command, void *data, uint32_t len_byte)
{
#if DBI_FLEXIO_USE_SMARTDMA

    /* FlexIO MCULCD SMARTDMA does not support read */
    return kStatus_Fail;
#else

    flexio_mculcd_transfer_t xfer;

    dbi_flexio_xfer_handle_t *xferHandle = (dbi_flexio_xfer_handle_t *)dbiXferHandle;

    /* Callback is necessary to notify user. */
    assert(NULL != xferHandle->memDoneCallback);

    FLEXIO_MCULCD_Type *flexioLCD = xferHandle->flexioHandle.base;

    if (NULL != xferHandle->flexioHandle.rxDmaHandle)
    {
        xfer.command             = command;
        xfer.mode                = kFLEXIO_MCULCD_ReadArray;
        xfer.dataAddrOrSameValue = (uint32_t)data;
        xfer.dataSize            = len_byte;

        _DBI_FLEXIO_TransferDMA(flexioLCD, &xferHandle->flexioHandle, &xfer);
    }
    else
    {
        /* If DMA not enabled, use the blocking method. */
        FLEXIO_MCULCD_StartTransfer(flexioLCD);
        FLEXIO_MCULCD_WriteCommandBlocking(flexioLCD, command);
        FLEXIO_MCULCD_ReadDataArrayBlocking(flexioLCD, data, (size_t)len_byte);
        FLEXIO_MCULCD_StopTransfer(flexioLCD);

        xferHandle->memDoneCallback(kStatus_Success, xferHandle->userData);
    }

    return kStatus_Success;
#endif
}
