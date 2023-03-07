/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_DBI_FLEXIO_H_
#define _FSL_DBI_FLEXIO_H_

#include "fsl_dbi.h"
#include "fsl_flexio_mculcd.h"

#ifndef DBI_FLEXIO_USE_SMARTDMA
#define DBI_FLEXIO_USE_SMARTDMA 0
#endif

#if DBI_FLEXIO_USE_SMARTDMA

#include "fsl_flexio_mculcd_smartdma.h"

#else /* DBI_FLEXIO_USE_SMARTDMA */

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
#include "fsl_flexio_mculcd_edma.h"
#else
#include "fsl_flexio_mculcd_dma.h"
#endif

#endif /* DBI_FLEXIO_USE_SMARTDMA */

#if defined(FSL_RTOS_FREE_RTOS)
#include "FreeRTOS.h"
#include "semphr.h"
#endif

/*!
 * @addtogroup dbi_flexio
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if DBI_FLEXIO_USE_SMARTDMA

typedef flexio_mculcd_smartdma_handle_t dbi_flexio_handle_t;

#else /* DBI_FLEXIO_USE_SMARTDMA */

#if (defined(FSL_FEATURE_SOC_EDMA_COUNT) && FSL_FEATURE_SOC_EDMA_COUNT)
typedef flexio_mculcd_edma_handle_t dbi_flexio_handle_t;
typedef edma_handle_t dbi_flexio_dma_handle_t;
#else
typedef flexio_mculcd_dma_handle_t dbi_flexio_handle_t;
typedef dma_handle_t dbi_flexio_dma_handle_t;
#endif

#endif /* DBI_FLEXIO_USE_SMARTDMA */

/*! @brief FLEXIO DBI interface (MCU LCD) transfer operation. */
typedef struct _dbi_flexio_xfer_handle
{
    dbi_xfer_ops_t *xferOps;                 /*!< Transfer operations. */
    dbi_flexio_handle_t flexioHandle;        /*!< FLEXIO DMA transfer handle. */
    dbi_mem_done_callback_t memDoneCallback; /*!< The callback function when video memory access done. */
    void *userData;                          /*!< Parameter of @ref memDoneCallback */
} dbi_flexio_xfer_handle_t;

/*! @brief FLEXIO DBI interface (MCU LCD) transfer operation. */
extern const dbi_xfer_ops_t g_dbiFlexioXferOps;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

#if DBI_FLEXIO_USE_SMARTDMA

/*!
 * @brief Create FLEXIO DBI transfer handle.
 *
 * @param[out] dbiXferHandle Pointer the handle that will be created.
 * @param[in] flexioLCD Pointer FLEXIO LCD structure.
 * @return Return true if success, otherwise return error code.
 */
status_t DBI_FLEXIO_CreateXferHandle(dbi_flexio_xfer_handle_t *dbiXferHandle,
                                     FLEXIO_MCULCD_Type *flexioLCD,
                                     const flexio_mculcd_smartdma_config_t *config);

#else /* DBI_FLEXIO_USE_SMARTDMA  */

/*!
 * @brief Create FLEXIO DBI transfer handle.
 *
 * @param[out] dbiXferHandle Pointer the handle that will be created.
 * @param[in] flexioLCD Pointer FLEXIO LCD structure.
 * @param[in] txDmaHandle Pointer to the DMA TX transfer handle, if you don't
 * want to use the function @ref DBI_FLEXIO_WriteMemory, this could be NULL.
 * When this is NULL, the blocking method will be used instead of DMA method.
 * @param[in] txDmaHandle Pointer to the DMA RX transfer handle, if you don't
 * want to use the function @ref DBI_FLEXIO_ReadMemory, this could be NULL.
 * When this is NULL, the blocking method will be used instead of DMA method.
 * @return Return true if success, otherwise return error code.
 */
status_t DBI_FLEXIO_CreateXferHandle(dbi_flexio_xfer_handle_t *dbiXferHandle,
                                     FLEXIO_MCULCD_Type *flexioLCD,
                                     dbi_flexio_dma_handle_t *txDmaHandle,
                                     dbi_flexio_dma_handle_t *rxDmaHandle);

#endif /* DBI_FLEXIO_USE_SMARTDMA */

/*!
 * @brief Write command through DBI.
 *
 * @param[in] dbiXferHandle Pointer the handle that created by @ref DBI_FLEXIO_CreateXferHandle.
 * @param[in] command The command to send.
 * @return Return true if success, otherwise return error code.
 */
status_t DBI_FLEXIO_WriteCommand(void *dbiXferHandle, uint32_t command);

/*!
 * @brief Write data through DBI.
 *
 * @param[in] dbiXferHandle Pointer the handle that created by @ref DBI_FLEXIO_CreateXferHandle.
 * @param[in] data The data to send.
 * @param[in] len_byte The length of the data in bytes.
 * @return Return true if success, otherwise return error code.
 */
status_t DBI_FLEXIO_WriteData(void *dbiXferHandle, void *data, uint32_t len_byte);

/*!
 * @brief Write data to the video memory through DBI.
 *
 * This function is faster than @ref DBI_FLEXIO_WriteData because DMA is involved.
 *
 * @param[in] dbiXferHandle Pointer the handle that created by @ref DBI_FLEXIO_CreateXferHandle.
 * @param[in] command The command sent before writing data.
 * @param[in] data The data to send.
 * @param[in] len_byte The length of the data in bytes.
 * @return Return true if success, otherwise return error code.
 */
status_t DBI_FLEXIO_WriteMemory(void *dbiXferHandle, uint32_t command, const void *data, uint32_t len_byte);

/*!
 * @brief Read data from the video memory through DBI.
 *
 * @param[in] dbiXferHandle Pointer the handle that created by @ref DBI_FLEXIO_CreateXferHandle.
 * @param[in] command The command sent before reading data.
 * @param[out] data The buffer to receive the data.
 * @param[in] len_byte The length of the data in bytes.
 * @return Return true if success, otherwise return error code.
 */
status_t DBI_FLEXIO_ReadMemory(void *dbiXferHandle, uint32_t command, void *data, uint32_t len_byte);

void DBI_FLEXIO_SetMemoryDoneCallback(void *dbiXferHandle, dbi_mem_done_callback_t callback, void *userData);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_DBI_FLEXIO_H_ */
