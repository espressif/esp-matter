/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/protected_storage.h"

#include "tfm_ns_interface.h"
#include "tfm_veneers.h"

#define IOVEC_LEN(x) (uint32_t)(sizeof(x)/sizeof(x[0]))

psa_status_t psa_ps_set(psa_storage_uid_t uid,
                        size_t data_length,
                        const void *p_data,
                        psa_storage_create_flags_t create_flags)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        { .base = &uid,   .len = sizeof(uid) },
        { .base = p_data, .len = data_length },
        { .base = &create_flags, .len = sizeof(create_flags) }
    };

    status = tfm_ns_interface_dispatch(
                                  (veneer_fn)tfm_tfm_sst_set_req_veneer,
                                  (uint32_t)in_vec,  IOVEC_LEN(in_vec),
                                  (uint32_t)NULL, 0);

    /* A parameter with a buffer pointer pointer that has data length longer
     * than maximum permitted is treated as a secure violation.
     * TF-M framework rejects the request with TFM_ERROR_INVALID_PARAMETER.
     */
    if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return status;
}

psa_status_t psa_ps_get(psa_storage_uid_t uid,
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

    status = tfm_ns_interface_dispatch(
                                  (veneer_fn)tfm_tfm_sst_get_req_veneer,
                                  (uint32_t)in_vec,  IOVEC_LEN(in_vec),
                                  (uint32_t)out_vec, IOVEC_LEN(out_vec));

    *p_data_length = out_vec[0].len;

    return status;
}

psa_status_t psa_ps_get_info(psa_storage_uid_t uid,
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
                                  (veneer_fn)tfm_tfm_sst_get_info_req_veneer,
                                  (uint32_t)in_vec,  IOVEC_LEN(in_vec),
                                  (uint32_t)out_vec, IOVEC_LEN(out_vec));

    return status;
}

psa_status_t psa_ps_remove(psa_storage_uid_t uid)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        { .base = &uid, .len = sizeof(uid) }
    };

    status = tfm_ns_interface_dispatch(
                                  (veneer_fn)tfm_tfm_sst_remove_req_veneer,
                                  (uint32_t)in_vec,  IOVEC_LEN(in_vec),
                                  (uint32_t)NULL, 0);

    return status;
}

psa_status_t psa_ps_create(psa_storage_uid_t uid,
                           size_t capacity,
                           psa_storage_create_flags_t create_flags)
{
    (void)uid;
    (void)capacity;
    (void)create_flags;

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_ps_set_extended(psa_storage_uid_t uid, size_t data_offset,
                                 size_t data_length, const void *p_data)
{
    (void)uid;
    (void)data_offset;
    (void)data_length;
    (void)p_data;

    return PSA_ERROR_NOT_SUPPORTED;
}

uint32_t psa_ps_get_support(void)
{
    /* Initialise support_flags to a sensible default, to avoid returning an
     * uninitialised value in case the secure function fails.
     */
    uint32_t support_flags = 0;

    psa_outvec out_vec[] = {
        { .base = &support_flags, .len = sizeof(support_flags) }
    };

    (void)tfm_ns_interface_dispatch(
                               (veneer_fn)tfm_tfm_sst_get_support_req_veneer,
                               (uint32_t)NULL,  0,
                               (uint32_t)out_vec, IOVEC_LEN(out_vec));

    return support_flags;
}
