/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa_audit_api.h"
#include "tfm_veneers.h"
#include "tfm_ns_interface.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

#define API_DISPATCH(sfn_name)                                    \
    tfm_ns_interface_dispatch((veneer_fn)tfm_##sfn_name##_veneer, \
        (uint32_t)in_vec, (uint32_t)ARRAY_SIZE(in_vec),           \
        (uint32_t)out_vec, (uint32_t)ARRAY_SIZE(out_vec))

#define API_DISPATCH_NO_INVEC(sfn_name)                           \
    tfm_ns_interface_dispatch((veneer_fn)tfm_##sfn_name##_veneer, \
        (uint32_t)NULL, 0,                                        \
        (uint32_t)out_vec, (uint32_t)ARRAY_SIZE(out_vec))

#define API_DISPATCH_NO_OUTVEC(sfn_name)                          \
    tfm_ns_interface_dispatch((veneer_fn)tfm_##sfn_name##_veneer, \
        (uint32_t)in_vec, (uint32_t)ARRAY_SIZE(in_vec),           \
        (uint32_t)NULL, 0)

psa_status_t psa_audit_retrieve_record(const uint32_t record_index,
                                       const uint32_t buffer_size,
                                       const uint8_t *token,
                                       const uint32_t token_size,
                                       uint8_t *buffer,
                                       uint32_t *record_size)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &record_index, .len = sizeof(uint32_t)},
        {.base = token, .len = token_size},
    };
    psa_outvec out_vec[] = {
        {.base = buffer, .len = buffer_size},
    };

    status = API_DISPATCH(audit_core_retrieve_record);

    *record_size = out_vec[0].len;

    return status;
}

psa_status_t psa_audit_get_info(uint32_t *num_records, uint32_t *size)
{
    psa_status_t status;
    psa_outvec out_vec[] = {
        {.base = num_records, .len = sizeof(uint32_t)},
        {.base = size, .len = sizeof(uint32_t)},
    };

    status = API_DISPATCH_NO_INVEC(audit_core_get_info);

    return status;
}

psa_status_t psa_audit_get_record_info(const uint32_t record_index,
                                       uint32_t *size)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &record_index, .len = sizeof(uint32_t)},
    };
    psa_outvec out_vec[] = {
        {.base = size, .len = sizeof(uint32_t)},
    };

    status = API_DISPATCH(audit_core_get_record_info);

    return status;
}

psa_status_t psa_audit_delete_record(const uint32_t record_index,
                                     const uint8_t *token,
                                     const uint32_t token_size)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = &record_index, .len = sizeof(uint32_t)},
        {.base = token, .len = token_size},
    };

    status = API_DISPATCH_NO_OUTVEC(audit_core_delete_record);

    return status;
}

psa_status_t psa_audit_add_record(const struct psa_audit_record *record)
{
    /* This API supports only Secure world calls. As this is the implementation
     * of the Non-Secure interface, always directly return an error without
     * routing the call to TF-M in the Secure world.
     */
    (void)record;
    return PSA_ERROR_NOT_PERMITTED;
}
