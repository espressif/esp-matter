/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_nv_counters.h"
#include "tfm_platform_api.h"

psa_status_t sst_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                 uint32_t *val)
{
    enum tfm_platform_err_t err;

    err = tfm_platform_nv_counter_read(counter_id, SST_NV_COUNTER_SIZE,
                                       (uint8_t *)val);
    if (err != TFM_PLATFORM_ERR_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    return PSA_SUCCESS;
}

psa_status_t sst_increment_nv_counter(enum tfm_nv_counter_t counter_id)
{
    enum tfm_platform_err_t err;

    /* NOTE: tfm_plat_increment_nv_counter returns TFM_PLAT_ERR_MAX_VALUE when
     *       the counter reaches its maximum value. The current SST
     *       implementation treats this condition as an error as, from that
     *       moment onwards, the rollback protection can not be achieved based
     *       on the NV counters.
     */
    err = tfm_platform_nv_counter_increment(counter_id);
    if (err != TFM_PLATFORM_ERR_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    return PSA_SUCCESS;
}
