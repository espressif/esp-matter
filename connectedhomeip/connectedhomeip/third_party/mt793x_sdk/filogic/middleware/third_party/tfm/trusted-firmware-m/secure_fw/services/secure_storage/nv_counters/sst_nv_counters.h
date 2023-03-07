/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_NV_COUNTERS_H__
#define __SST_NV_COUNTERS_H__

/* NOTE: This API abstracts SST NV counters operations. This API detaches the
 *       use of NV counters from the TF-M NV counters implementation, provided
 *       by the platform, and provides a mechanism to compile in a different
 *       API implementation for test purposes. A SST test suite may provide
 *       its own custom implementation to be able to test different SST service
 *       use cases.
 */

#include <stdint.h>
#include "psa/protected_storage.h"
#include "platform/include/tfm_plat_nv_counters.h"

#define TFM_SST_NV_COUNTER_1        PLAT_NV_COUNTER_0
#define TFM_SST_NV_COUNTER_2        PLAT_NV_COUNTER_1
#define TFM_SST_NV_COUNTER_3        PLAT_NV_COUNTER_2

#define SST_NV_COUNTER_SIZE         4 /* In bytes */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Reads the given non-volatile (NV) counter.
 *
 * \param[in]  counter_id  NV counter ID.
 * \param[out] val         Pointer to store the current NV counter value.
 *
 * \return  PSA_SUCCESS if the value is read correctly, otherwise
 *          PSA_ERROR_GENERIC_ERROR
 */
psa_status_t sst_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                 uint32_t *val);

/**
 * \brief Increments the given non-volatile (NV) counter.
 *
 * \param[in] counter_id  NV counter ID.
 *
 * \return  If the counter is incremented correctly, it returns
 *          PSA_SUCCESS. Otherwise, PSA_ERROR_GENERIC_ERROR.
 */
psa_status_t sst_increment_nv_counter(enum tfm_nv_counter_t counter_id);

#ifdef __cplusplus
}
#endif

#endif /* __SST_NV_COUNTERS_H__ */
