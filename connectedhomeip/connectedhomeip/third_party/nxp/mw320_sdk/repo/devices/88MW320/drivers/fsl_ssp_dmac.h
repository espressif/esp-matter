/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _FSL_SSP_DMAC_H_
#define _FSL_SSP_DMAC_H_

#include "fsl_dmac.h"
#include "fsl_ssp.h"

/*!
 * @addtogroup ssp_dma_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_SSP_DMAC_DRIVER_VERSION (MAKE_VERSION(2, 0, 0)) /*!< Version 2.0.0 */
/*@}*/

typedef struct _ssp_dmac_handle ssp_dmac_handle_t;

/*! @brief Define SSP DMAC callback */
typedef void (*ssp_dmac_callback_t)(ssp_dmac_handle_t *handle, status_t transferStatus, void *userData);

/*! @brief SSP DMAC transfer handle, users should not touch the content of the handle.
 * A ssp_dmac_handle object can used to represent SSP TX or RX.
 */
struct _ssp_dmac_handle
{
    SSP_Type *base;            /*!< SSP base address */
    dmac_handle_t *dmacHandle; /*!< DMAC handler for SSP send */

    uint32_t bitWidth;                       /*!< bit width */
    dmac_channel_burst_length_t burstLength; /*!< dmac channel burst length */

    uint32_t state;               /*!< SSP DMAC transfer internal state */
    ssp_dmac_callback_t callback; /*!< Callback for users while transfer finish or error occurred */
    void *userData;               /*!< User callback parameter */
};

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif
/*!
 * @name DMAC Transactional
 * @{
 */

/*!
 * @brief Initializes the SSP master DMAC handle.
 *
 * This function initializes the SSP master DMAC handle, which can be used for other SSP master transactional APIs.
 * Usually, for a specified SSP instance, call this API once to get the initialized handle.
 *
 * @param base SSP base pointer.
 * @param handle SSP DMAC handle pointer.
 * @param base SSP peripheral base address.
 * @param callback Pointer to user callback function.
 * @param userData User parameter passed to the callback function.
 * @param dmacHandle DMAC handle pointer, this handle shall be static allocated by users.
 */
void SSP_TransferSendCreateHandleDMAC(
    SSP_Type *base, ssp_dmac_handle_t *handle, ssp_dmac_callback_t callback, void *userData, dmac_handle_t *dmacHandle);

/*!
 * @brief Initializes the SSP receive DMAC handle.
 *
 * This function initializes the SSP receive DMAC handle, which can be used for other SSP master transactional APIs.
 * Usually, for a specified SSP instance, call this API once to get the initialized handle.
 *
 * @param base SSP base pointer.
 * @param handle SSP DMAC handle pointer.
 * @param base SSP peripheral base address.
 * @param callback Pointer to user callback function.
 * @param userData User parameter passed to the callback function.
 * @param dmacHandle DMAC handle pointer, this handle shall be static allocated by users.
 */
void SSP_TransferReceiveCreateHandleDMAC(
    SSP_Type *base, ssp_dmac_handle_t *handle, ssp_dmac_callback_t callback, void *userData, dmac_handle_t *dmacHandle);

/*!
 * @brief Performs a non-blocking SSP transfer using DMAC.
 *
 * @note This interface returns immediately after the transfer initiates.
 *
 * @param handle SSP DMAC handle pointer.
 * @param xfer Pointer to DMAC transfer structure.
 * @retval kStatus_Success Successfully start the data receive.
 * @retval kStatus_SSP_TxBusy Previous receive still not finished.
 * @retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t SSP_TransferSendDMAC(ssp_dmac_handle_t *handle, ssp_transfer_t *xfer);

/*!
 * @brief Performs a non-blocking SSP transfer using DMAC.
 *
 * @note This interface returns immediately after transfer initiates.
 *
 * @param handle SSP DMAC handle pointer.
 * @param xfer Pointer to DMAC transfer structure.
 * @retval kStatus_Success Successfully start the data receive.
 * @retval kStatus_SSP_RxBusy Previous receive still not finished.
 * @retval kStatus_InvalidArgument The input parameter is invalid.
 */
status_t SSP_TransferReceiveDMAC(ssp_dmac_handle_t *handle, ssp_transfer_t *xfer);

/*!
 * @brief Aborts a SSP transfer using DMAC.
 *
 * @param handle SSP DMAC handle pointer.
 */
void SSP_TransferAbortSendDMAC(ssp_dmac_handle_t *handle);

/*!
 * @brief Aborts a SSP transfer using DMAC.
 *
 * @param handle SSP DMAC handle pointer.
 */
void SSP_TransferAbortReceiveDMAC(ssp_dmac_handle_t *handle);
/*! @} */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif
