/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_secure_client_service_api.h"
#ifdef TFM_PSA_API
#include "psa/client.h"
#include "tfm_api.h"
#include "psa_manifest/sid.h"
#else /* TFM_PSA_API */
#include "tfm_veneers.h"
#endif /* TFM_PSA_API */
#include "tfm_ns_interface.h"

int32_t tfm_secure_client_run_tests(void)
{
#ifdef TFM_PSA_API
    psa_handle_t handle;
    psa_status_t status;

    handle = psa_connect(TFM_SECURE_CLIENT_SFN_RUN_TESTS_SID,
                         TFM_SECURE_CLIENT_SFN_RUN_TESTS_VERSION);
    if (handle <= 0) {
        return TFM_ERROR_GENERIC;
    }

    status = psa_call(handle, PSA_IPC_CALL, NULL, 0, NULL, 0);
    psa_close(handle);

    if (status != PSA_SUCCESS) {
        return TFM_ERROR_GENERIC;
    }
#else
    tfm_ns_interface_dispatch(
    (veneer_fn)tfm_tfm_secure_client_service_sfn_run_tests_veneer,
    (uint32_t)NULL, 0,
    (uint32_t)NULL, 0);
#endif

    return 0;
}
