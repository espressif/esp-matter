/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test_sst_nv_counters.h"

#include <limits.h>
#include "secure_fw/services/secure_storage/nv_counters/sst_nv_counters.h"
#include "secure_fw/services/secure_storage/sst_utils.h"

#define DISABLE_INCREMENT 0
#define ENABLE_INCREMENT  1

#define TOTAL_SST_NV_COUNTERS  3
#define INIT_NV_COUNTERS_VALUE 42

static uint8_t nv_increment_status = ENABLE_INCREMENT;
static uint32_t test_nv_counters[TOTAL_SST_NV_COUNTERS] = {
                                                [0] = INIT_NV_COUNTERS_VALUE,
                                                [1] = INIT_NV_COUNTERS_VALUE,
                                                [2] = INIT_NV_COUNTERS_VALUE
                };

static uint32_t get_nv_counter_position(enum tfm_nv_counter_t counter_id)
{
    switch (counter_id) {
    case TFM_SST_NV_COUNTER_1:
        return 0;
    case TFM_SST_NV_COUNTER_2:
        return 1;
    case TFM_SST_NV_COUNTER_3:
        return 2;
    default:
        return TOTAL_SST_NV_COUNTERS;
    }
}

psa_status_t sst_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                 uint32_t *val)
{
    uint32_t nv_pos;

    nv_pos = get_nv_counter_position(counter_id);
    if (nv_pos >= TOTAL_SST_NV_COUNTERS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Reads counter value */
    *val = test_nv_counters[nv_pos];

    return PSA_SUCCESS;
}

psa_status_t sst_increment_nv_counter(enum tfm_nv_counter_t counter_id)
{
    uint32_t nv_pos;

    if (nv_increment_status == DISABLE_INCREMENT) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    nv_pos = get_nv_counter_position(counter_id);
    if (nv_pos >= TOTAL_SST_NV_COUNTERS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if (test_nv_counters[nv_pos] == UINT32_MAX) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Increments counter value */
    test_nv_counters[nv_pos]++;

    return PSA_SUCCESS;
}

/* Implementation of SST NV counter interfaces defined by
 * test_sst_nv_counters.h
 */
void test_sst_disable_increment_nv_counter(void)
{
    nv_increment_status = DISABLE_INCREMENT;
}

void test_sst_enable_increment_nv_counter(void)
{
    nv_increment_status = ENABLE_INCREMENT;
}

psa_status_t test_sst_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                      uint32_t *val)
{
    return sst_read_nv_counter(counter_id, val);
}

psa_status_t test_sst_increment_nv_counter(enum tfm_nv_counter_t counter_id)
{
    return sst_increment_nv_counter(counter_id);
}

psa_status_t test_sst_decrement_nv_counter(enum tfm_nv_counter_t counter_id)
{
    uint32_t nv_pos;

    nv_pos = get_nv_counter_position(counter_id);
    if (nv_pos >= TOTAL_SST_NV_COUNTERS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if (test_nv_counters[nv_pos] == 0) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Decrements counter value */
    test_nv_counters[nv_pos]--;

    return PSA_SUCCESS;
}

psa_status_t test_sst_set_nv_counter(enum tfm_nv_counter_t counter_id,
                                     uint32_t value)
{
    uint32_t nv_pos;

    nv_pos = get_nv_counter_position(counter_id);
    if (nv_pos >= TOTAL_SST_NV_COUNTERS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    /* Sets counter value */
    test_nv_counters[nv_pos] = value;

    return PSA_SUCCESS;
}
