/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_sst_test_service_api.h"

#ifdef TFM_PSA_API
#include "psa/client.h"
#include "psa_manifest/sid.h"
#else
#include "tfm_veneers.h"
#endif

__attribute__((section("SFN")))
psa_status_t tfm_sst_test_system_prepare(void)
{
#ifdef TFM_PSA_API
    psa_handle_t handle;
    psa_status_t status;

    handle = psa_connect(TFM_SST_TEST_PREPARE_SID,
                         TFM_SST_TEST_PREPARE_VERSION);
    if (!PSA_HANDLE_IS_VALID(handle)) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    status = psa_call(handle, PSA_IPC_CALL, NULL, 0, NULL, 0);
    psa_close(handle);

    return status;
#else
    return tfm_tfm_sst_test_prepare_veneer(NULL, 0, NULL, 0);
#endif
}
