/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FLEXIO_MCULCD_DMA_H_
#define _FSL_FLEXIO_MCULCD_DMA_H_

#include "fsl_dma.h"
#include "fsl_flexio_mculcd.h"

/*!
 * @addtogroup flexio_dma_mculcd
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*@{*/
/*! @brief FlexIO MCULCD DMA driver version 2.0.0. */
#define FSL_FLEXIO_MCULCD_DMA_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief  typedef for flexio_mculcd_dma_handle_t in advance. */
typedef struct _flexio_mculcd_dma_handle flexio_mculcd_dma_handle_t;

/*! @brief FlexIO MCULCD master callback for transfer complete.
 *
 * When transfer finished, the callback function is called and returns the
 * @p status as kStatus_FLEXIO_MCULCD_Idle.
 */
typedef void (*flexio_mculcd_dma_transfer_callback_t)(FLEXIO_MCULCD_Type *base,
                                                      flexio_mculcd_dma_handle_t *handle,
                                                      status_t status,
                                                      void *userData);

/*! @brief FlexIO MCULCD DMA transfer handle, users should not touch the
 * content of the handle.*/
struct _flexio_mculcd_dma_handle
{
    FLEXIO_MCULCD_Type *base;                                 /*!< Pointer to the FLEXIO_MCULCD_Type. */
    uint8_t txShifterNum;                                     /*!< Number of shifters used for TX. */
    uint8_t rxShifterNum;                                     /*!< Number of shifters used for RX. */
    dma_trigger_burst_t txTriggerBurst;                       /*!< Trigger burst setting for TX. */
    dma_trigger_burst_t rxTriggerBurst;                       /*!< Trigger burst setting for RX. */
    uint32_t burstBytes;                                      /*!< DMA transfer bytes per burst. */
    uint32_t dataAddrOrSameValue;                             /*!< When sending the same value for many times,
                                                                   this is the value to send. When writing or
                                                                   reading array, this is the address of the
                                                                   data array. */
    size_t dataCount;                                         /*!< Total count to be transferred. */
    volatile size_t remainingCount;                           /*!< Remaining count still not transfered. */
    volatile uint32_t state;                                  /*!< FlexIO MCULCD driver internal state. */
    dma_handle_t *txDmaHandle;                                /*!< DMA handle for MCULCD TX */
    dma_handle_t *rxDmaHandle;                                /*!< DMA handle for MCULCD RX */
    flexio_mculcd_dma_transfer_callback_t completionCallback; /*!< Callback for MCULCD DMA transfer */
    void *userData;                                           /*!< User Data for MCULCD DMA callback */
};

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name DMA Transactional
 * @{
 */

/*!
 * @brief Initializes the FLEXO MCULCD master DMA handle.
 *
 * This function initializes the FLEXO MCULCD master DMA handle which can be
 * used for other FLEXO MCULCD transactional APIs. For a specified FLEXO MCULCD
 * instance, call this API once to get the initialized handle.
 *
 * @param base Pointer to FLEXIO_MCULCD_Type structure.
 * @param handle Pointer to flexio_mculcd_dma_handle_t structure to store the
 * transfer state.
 * @param callback MCULCD transfer complete callback, NULL means no callback.
 * @param userData callback function parameter.
 * @param txDmaHandle User requested DMA handle for FlexIO MCULCD DMA TX,
 * the DMA request source of this handle should be the first of TX shifters.
 * @param rxDmaHandle User requested DMA handle for FlexIO MCULCD DMA RX,
 * the DMA request source of this handle should be the last of RX shifters.
 * @retval kStatus_Success Successfully create the handle.
 */
status_t FLEXIO_MCULCD_TransferCreateHandleDMA(FLEXIO_MCULCD_Type *base,
                                               flexio_mculcd_dma_handle_t *handle,
                                               flexio_mculcd_dma_transfer_callback_t callback,
                                               void *userData,
                                               dma_handle_t *txDmaHandle,
                                               dma_handle_t *rxDmaHandle);

/*!
 * @brief Performs a non-blocking FlexIO MCULCD transfer using DMA.
 *
 * This function returns immediately after transfer initiates. To check whether
 * the transfer is completed, user could:
 * 1. Use the transfer completed callback;
 * 2. Polling function @ref FLEXIO_MCULCD_GetTransferCountDMA
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle pointer to flexio_mculcd_dma_handle_t structure to store the
 * transfer state.
 * @param xfer Pointer to FlexIO MCULCD transfer structure.
 * @retval kStatus_Success Successfully start a transfer.
 * @retval kStatus_InvalidArgument Input argument is invalid.
 * @retval kStatus_FLEXIO_MCULCD_Busy FlexIO MCULCD is not idle, it is running another
 * transfer.
 */
status_t FLEXIO_MCULCD_TransferDMA(FLEXIO_MCULCD_Type *base,
                                   flexio_mculcd_dma_handle_t *handle,
                                   flexio_mculcd_transfer_t *xfer);

/*!
 * @brief Aborts a FlexIO MCULCD transfer using DMA.
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle FlexIO MCULCD DMA handle pointer.
 */
void FLEXIO_MCULCD_TransferAbortDMA(FLEXIO_MCULCD_Type *base, flexio_mculcd_dma_handle_t *handle);

/*!
 * @brief Gets the remaining bytes for FlexIO MCULCD DMA transfer.
 *
 * @param base pointer to FLEXIO_MCULCD_Type structure.
 * @param handle FlexIO MCULCD DMA handle pointer.
 * @param count Number of count transferred so far by the DMA transaction.
 * @retval kStatus_Success Get the transferred count Successfully.
 * @retval kStatus_NoTransferInProgress No transfer in process.
 */
status_t FLEXIO_MCULCD_TransferGetCountDMA(FLEXIO_MCULCD_Type *base, flexio_mculcd_dma_handle_t *handle, size_t *count);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _FSL_FLEXIO_MCULCD_DMA_H_ */
