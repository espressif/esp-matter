/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "secure_utilities.h"
#include "tfm_secure_api.h"

/* This is the "Big Lock" on the secure side, to guarantee single entry
 * to SPE
 */
int32_t tfm_secure_lock;

bool tfm_is_one_bit_set(uint32_t n)
{
    return ((n && !(n & (n-1))) ? true : false);
}

enum tfm_status_e check_address_range(const void *p, size_t s,
                                      uintptr_t region_start,
                                      uintptr_t region_limit)
{
    int32_t range_in_region;

    /* Check for overflow in the range parameters */
    if ((uintptr_t)p > UINTPTR_MAX - s) {
        return TFM_ERROR_GENERIC;
    }

    /* We trust the region parameters, and don't check for overflow */

    /* Calculate the result */
    range_in_region = ((uintptr_t)p >= region_start) &&
                      ((uintptr_t)((char *) p + s - 1) <= region_limit);
    if (range_in_region) {
        return TFM_SUCCESS;
    } else {
        return TFM_ERROR_GENERIC;
    }
}

void tfm_secure_api_error_handler(void)
{
    ERROR_MSG("Security violation when calling secure API");
    tfm_core_panic();
}

#ifndef TFM_PSA_API
int32_t tfm_core_partition_request(uint32_t id, bool is_ns, void *fn,
            int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    int32_t args[4] = {arg1, arg2, arg3, arg4};
    struct tfm_sfn_req_s desc, *desc_ptr = &desc;

    desc.sp_id = id;
    desc.sfn = (sfn_t) fn;
    desc.args = args;
    desc.ns_caller = is_ns;

    if (__get_active_exc_num() != EXC_NUM_THREAD_MODE) {
        /* The veneer of a secure service had been called from Handler mode.
         * This violates TF-M's programming model, and is considered an
         * unrecoverable error.
         */
        tfm_core_panic();
    } else {
        if (desc.ns_caller) {
            return tfm_core_sfn_request(desc_ptr);
        } else {
            return tfm_spm_sfn_request_thread_mode(desc_ptr);
        }
    }
    return TFM_ERROR_GENERIC;
}
#endif
