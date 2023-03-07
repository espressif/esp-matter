/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CORE_MEM_CHECK_H__
#define __TFM_CORE_MEM_CHECK_H__

#include "tfm_api.h"

/**
 * \brief Check whether the current partition has read access to a memory range
 *
 * This function assumes, that the current MPU configuration is set for the
 * partition to be checked.
 *
 * \param[in] p                The start address of the range to check
 * \param[in] s                The size of the range to check
 * \param[in] ns_caller        Whether the current partition is non-secure
 * \param[in] privileged       Privileged mode or unprivileged mode:
 *                             \ref TFM_PARTITION_UNPRIVILEGED_MODE
 *                             \ref TFM_PARTITION_PRIVILEGED_MODE
 *
 * \return TFM_SUCCESS if the partition has access to the memory range,
 *         TFM_ERROR_GENERIC otherwise.
 */
enum tfm_status_e tfm_core_has_read_access_to_region(const void *p, size_t s,
                                                     bool ns_caller,
                                                     uint32_t privileged);

/**
 * \brief Check whether the current partition has write access to a memory range
 *
 * This function assumes, that the current MPU configuration is set for the
 * partition to be checked.
 *
 * \param[in] p                The start address of the range to check
 * \param[in] s                The size of the range to check
 * \param[in] ns_caller        Whether the current partition is non-secure
 * \param[in] privileged       Privileged mode or unprivileged mode:
 *                             \ref TFM_PARTITION_UNPRIVILEGED_MODE
 *                             \ref TFM_PARTITION_PRIVILEGED_MODE
 *
 * \return TFM_SUCCESS if the partition has access to the memory range,
 *         TFM_ERROR_GENERIC otherwise.
 */
enum tfm_status_e tfm_core_has_write_access_to_region(const void *p, size_t s,
                                                      bool ns_caller,
                                                      uint32_t privileged);

#endif /* __TFM_CORE_MEM_CHECK_H__ */
