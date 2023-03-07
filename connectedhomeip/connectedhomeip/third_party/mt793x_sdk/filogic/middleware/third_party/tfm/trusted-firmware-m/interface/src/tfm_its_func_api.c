/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/internal_trusted_storage.h"
#include "tfm_api.h"

#include "tfm_ns_interface.h"
#include "tfm_veneers.h"

#define IOVEC_LEN(x) (sizeof(x)/sizeof(x[0]))

psa_status_t psa_its_set(psa_storage_uid_t uid,
                         size_t data_length,
                         const void *p_data,
                         psa_storage_create_flags_t create_flags)
{
    psa_status_t status;

    psa_invec in_vec[] = {
        { .base = &uid, .len = sizeof(uid) },
        { .base = p_data, .len = data_length },
        { .base = &create_flags, .len = sizeof(create_flags) }
    };

    status = tfm_ns_interface_dispatch((veneer_fn)tfm_tfm_its_set_req_veneer,
                                       (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                       (uint32_t)NULL, 0);

    if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return status;
}

psa_status_t psa_its_get(psa_storage_uid_t uid,
                         size_t data_offset,
                         size_t data_size,
                         void *p_data,
                         size_t *p_data_length)
{
    psa_status_t status;

    psa_invec in_vec[] = {
        { .base = &uid, .len = sizeof(uid) },
        { .base = &data_offset, .len = sizeof(data_offset) }
    };

    psa_outvec out_vec[] = {
        { .base = p_data, .len = data_size }
    };

    if (p_data_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = tfm_ns_interface_dispatch((veneer_fn)tfm_tfm_its_get_req_veneer,
                                       (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                       (uint32_t)out_vec, IOVEC_LEN(out_vec));

    if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *p_data_length = out_vec[0].len;

    return status;
}

psa_status_t psa_its_get_info(psa_storage_uid_t uid,
                              struct psa_storage_info_t *p_info)
{
    psa_status_t status;

    psa_invec in_vec[] = {
        { .base = &uid, .len = sizeof(uid) }
    };

    psa_outvec out_vec[] = {
        { .base = p_info, .len = sizeof(*p_info) }
    };

    status = tfm_ns_interface_dispatch(
                                     (veneer_fn)tfm_tfm_its_get_info_req_veneer,
                                     (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                     (uint32_t)out_vec, IOVEC_LEN(out_vec));

    if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return status;
}

psa_status_t psa_its_remove(psa_storage_uid_t uid)
{
    psa_invec in_vec[] = {
        { .base = &uid, .len = sizeof(uid) }
    };

    return tfm_ns_interface_dispatch((veneer_fn)tfm_tfm_its_remove_req_veneer,
                                     (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                     (uint32_t)NULL, 0);
}
