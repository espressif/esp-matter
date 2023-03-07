/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/internal_trusted_storage.h"
#include "tfm_api.h"

#ifdef TFM_PSA_API
#include "psa/client.h"
#include "psa_manifest/sid.h"
#else
#include "tfm_veneers.h"
#endif

#define IOVEC_LEN(x) (sizeof(x)/sizeof(x[0]))

__attribute__((section("SFN")))
psa_status_t psa_its_set(psa_storage_uid_t uid,
                         size_t data_length,
                         const void *p_data,
                         psa_storage_create_flags_t create_flags)
{
    psa_status_t status;
#ifdef TFM_PSA_API
    psa_handle_t handle;
#endif

    psa_invec in_vec[] = {
        { .base = &uid, .len = sizeof(uid) },
        { .base = p_data, .len = data_length },
        { .base = &create_flags, .len = sizeof(create_flags) }
    };

#ifdef TFM_PSA_API
    handle = psa_connect(TFM_ITS_SET_SID, TFM_ITS_SET_VERSION);
    if (!PSA_HANDLE_IS_VALID(handle)) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    status = psa_call(handle, PSA_IPC_CALL, in_vec, IOVEC_LEN(in_vec), NULL, 0);

    psa_close(handle);
#else
    status = tfm_tfm_its_set_req_veneer(in_vec, IOVEC_LEN(in_vec), NULL, 0);
#endif

    /* A parameter with a buffer pointer where its data length is longer than
     * maximum permitted, it is treated as a secure violation.
     * TF-M framework rejects the request with TFM_ERROR_INVALID_PARAMETER.
     * The ITS secure PSA implementation returns PSA_ERROR_INVALID_ARGUMENT in
     * that case.
     */
    if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_its_get(psa_storage_uid_t uid,
                         size_t data_offset,
                         size_t data_size,
                         void *p_data,
                         size_t *p_data_length)
{
    psa_status_t status;
#ifdef TFM_PSA_API
    psa_handle_t handle;
#endif

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

#ifdef TFM_PSA_API
    handle = psa_connect(TFM_ITS_GET_SID, TFM_ITS_GET_VERSION);
    if (!PSA_HANDLE_IS_VALID(handle)) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    status = psa_call(handle, PSA_IPC_CALL, in_vec, IOVEC_LEN(in_vec), out_vec,
                      IOVEC_LEN(out_vec));

    psa_close(handle);
#else
    status = tfm_tfm_its_get_req_veneer(in_vec, IOVEC_LEN(in_vec),
                                        out_vec, IOVEC_LEN(out_vec));
#endif

    /* A parameter with a buffer pointer where its data length is longer than
     * maximum permitted or a parameter with a null pointer, it is treated as a
     * secure violation.
     * TF-M framework rejects the request with TFM_ERROR_INVALID_PARAMETER.
     * The ITS secure PSA implementation returns PSA_ERROR_INVALID_ARGUMENT in
     * that case.
     */
    if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *p_data_length = out_vec[0].len;

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_its_get_info(psa_storage_uid_t uid,
                              struct psa_storage_info_t *p_info)
{
    psa_status_t status;
#ifdef TFM_PSA_API
    psa_handle_t handle;
#endif

    psa_invec in_vec[] = {
        { .base = &uid, .len = sizeof(uid) }
    };

    psa_outvec out_vec[] = {
        { .base = p_info, .len = sizeof(*p_info) }
    };

#ifdef TFM_PSA_API
    handle = psa_connect(TFM_ITS_GET_INFO_SID, TFM_ITS_GET_INFO_VERSION);
    if (!PSA_HANDLE_IS_VALID(handle)) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    status = psa_call(handle, PSA_IPC_CALL, in_vec, IOVEC_LEN(in_vec), out_vec,
                      IOVEC_LEN(out_vec));

    psa_close(handle);
#else
    status = tfm_tfm_its_get_info_req_veneer(in_vec, IOVEC_LEN(in_vec),
                                             out_vec, IOVEC_LEN(out_vec));
#endif

    /* A parameter with a null pointer is treated as a secure violation.
     * TF-M framework rejects the request with TFM_ERROR_INVALID_PARAMETER.
     * The secure PSA ITS implementation returns PSA_ERROR_INVALID_ARGUMENT
     * in that case.
     */
    if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return status;
}

__attribute__((section("SFN")))
psa_status_t psa_its_remove(psa_storage_uid_t uid)
{
    psa_status_t status;
#ifdef TFM_PSA_API
    psa_handle_t handle;
#endif

    psa_invec in_vec[] = {
        { .base = &uid, .len = sizeof(uid) }
    };

#ifdef TFM_PSA_API
    handle = psa_connect(TFM_ITS_REMOVE_SID, TFM_ITS_REMOVE_VERSION);
    if (!PSA_HANDLE_IS_VALID(handle)) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    status = psa_call(handle, PSA_IPC_CALL, in_vec, IOVEC_LEN(in_vec), NULL, 0);

    psa_close(handle);

#else
    status = tfm_tfm_its_remove_req_veneer(in_vec, IOVEC_LEN(in_vec), NULL, 0);
#endif

    return status;
}
