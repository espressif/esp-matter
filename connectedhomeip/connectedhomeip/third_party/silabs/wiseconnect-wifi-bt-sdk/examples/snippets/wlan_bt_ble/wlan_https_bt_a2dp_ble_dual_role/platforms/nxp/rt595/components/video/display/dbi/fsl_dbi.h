/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_DBI_H_
#define _FSL_DBI_H_

#include "fsl_common.h"

/*!
 * @addtogroup dbi
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!
 * @brief Callback function when the writeMemory or readMemory finished.
 *
 * If transfer done successfully, the @p status is kStatus_Success.
 */
typedef void (*dbi_mem_done_callback_t)(status_t status, void *userData);

/*!
 * @brief DBI interface (MCU LCD) transfer operation.
 *
 * The API @ref writeCommand and @ref writeData are blocking method, they returns
 * only when transfer finished. They are usually used to transfer small data, for
 * example, sending the cofigurations.
 *
 * The API @ref writeMemory and @ref readMemory are non-blocking method, they are
 * used to write or read the LCD contoller video memory. These APIs start transfer
 * and return directly, upper layer could be notified by callback when transfer
 * done. The callback function is set by @ref setMemoryDoneCallback.
 */
typedef struct _dbi_xfer_ops
{
    status_t (*writeCommand)(void *dbiXferHandle, uint32_t command);           /*!< Write command. */
    status_t (*writeData)(void *dbiXferHandle, void *data, uint32_t len_byte); /*!< Write data. */
    status_t (*writeMemory)(void *dbiXferHandle,
                            uint32_t command,
                            const void *data,
                            uint32_t len_byte); /*!< Write to the memory. */
    status_t (*readMemory)(void *dbiXferHandle,
                           uint32_t command,
                           void *data,
                           uint32_t len_byte); /*!< Read from the memory. */
    void (*setMemoryDoneCallback)(void *dbiXferHandle,
                                  dbi_mem_done_callback_t callback,
                                  void *userData); /*!< Set the memory access done callback. */
} dbi_xfer_ops_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_DBI_H_ */
