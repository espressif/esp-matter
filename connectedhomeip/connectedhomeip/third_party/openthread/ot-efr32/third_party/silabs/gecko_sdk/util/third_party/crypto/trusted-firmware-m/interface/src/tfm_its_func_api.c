/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)

#include "psa/internal_trusted_storage.h"
#include "sli_tz_service_its.h"
#include "tfm_ns_interface.h"
#include "sli_tz_s_interface.h"

#else

#include "psa/client.h"
#include "psa/internal_trusted_storage.h"
#include "tfm_api.h"
#include "tfm_ns_interface.h"
#include "tfm_veneers.h"

#endif

psa_status_t psa_its_set(psa_storage_uid_t uid,
                         uint32_t data_length,
                         const void *p_data,
                         psa_storage_create_flags_t create_flags)
{
    psa_status_t status;

    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_ITS_SET_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id)},
        { .base = &uid, .len = sizeof(uid) },
        { .base = p_data, .len = data_length },
        { .base = &create_flags, .len = sizeof(create_flags) }
    };

    status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_its,
                                       (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                       (uint32_t)NULL, 0);

    if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return status;
}

psa_status_t psa_its_get(psa_storage_uid_t uid,
                         uint32_t data_offset,
                         uint32_t data_size,
                         void *p_data,
                         size_t *p_data_length)
{
    psa_status_t status;

    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_ITS_GET_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id)},
        { .base = &uid, .len = sizeof(uid) },
        { .base = &data_offset, .len = sizeof(data_offset) }
    };

    psa_outvec out_vec[] = {
        { .base = p_data, .len = data_size }
    };

    if (p_data_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_its,
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

    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_ITS_GETINFO_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id)},
        { .base = &uid, .len = sizeof(uid) }
    };

    psa_outvec out_vec[] = {
        { .base = p_info, .len = sizeof(*p_info) }
    };

    status = tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_its,
                                       (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                       (uint32_t)out_vec, IOVEC_LEN(out_vec));

    if (status == (psa_status_t)TFM_ERROR_INVALID_PARAMETER) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return status;
}

psa_status_t psa_its_remove(psa_storage_uid_t uid)
{
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_ITS_REMOVE_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id)},
        { .base = &uid, .len = sizeof(uid) }
    };

    return tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_its,
                                     (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                     (uint32_t)NULL, 0);
}

#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)
psa_status_t sli_psa_its_encrypted(void)
{
    sli_tz_fn_id fn_id = SLI_TZ_SERVICE_ITS_ENCRYPTED_SID;
    psa_invec in_vec[] = {
        { &fn_id, sizeof(sli_tz_fn_id) }
    };

    return tfm_ns_interface_dispatch((veneer_fn)sli_tz_s_interface_dispatch_its,
                                     (uint32_t)in_vec, IOVEC_LEN(in_vec),
                                     (uint32_t)NULL, 0);
}
#endif
