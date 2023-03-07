/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_UTILS_H__
#define __SST_UTILS_H__

#include <stdint.h>

#include "psa/error.h"
#include "psa/protected_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SST_INVALID_FID  0
#define SST_DEFAULT_EMPTY_BUFF_VAL 0

/**
 * \brief Macro to check, at compilation time, if data fits in data buffer
 *
 * \param[in] err_msg        Error message which will be displayed in first
 *                           instance if the error is triggered
 * \param[in] data_size      Data size to check if it fits
 * \param[in] data_buf_size  Size of the data buffer
 *
 * \return  Triggers a compilation error if data_size is bigger than
 *          data_buf_size. The compilation error should be
 *          "... error: 'err_msg' declared as an array with a negative size"
 */
#define SST_UTILS_BOUND_CHECK(err_msg, data_size, data_buf_size) \
typedef char err_msg[(data_size <= data_buf_size)*2 - 1]

/**
 * \brief Evaluates to the minimum of the two parameters.
 */
#define SST_UTILS_MIN(x, y) (((x) < (y)) ? (x) : (y))

/**
 * \brief Checks if a subset region is fully contained within a superset region.
 *
 * \param[in] superset_size  Size of superset region
 * \param[in] subset_offset  Offset of start of subset region from start of
 *                           superset region
 * \param[in] subset_size    Size of subset region
 *
 * \return Returns error code as specified in \ref psa_status_t
 *
 * \retval PSA_SUCCESS                  The subset is contained within the
 *                                      superset
 * \retval PSA_ERROR_INVALID_ARGUMENT   The subset offset is greater than the
 *                                      size of the superset or when
 *                                      the subset offset is valid, but the
 *                                      subset offset + size is greater than the
 *                                      size of the superset
 */
psa_status_t sst_utils_check_contained_in(uint32_t superset_size,
                                          uint32_t subset_offset,
                                          uint32_t subset_size);

#ifdef __cplusplus
}
#endif

#endif /* __SST_UTILS_H__ */
