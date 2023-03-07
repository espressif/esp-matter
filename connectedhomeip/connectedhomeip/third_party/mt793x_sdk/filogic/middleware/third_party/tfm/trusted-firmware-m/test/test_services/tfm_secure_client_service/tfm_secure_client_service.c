/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_secure_client_service.h"
#include "test/framework/test_framework_integ_test.h"
#ifdef TFM_PSA_API
#include "psa/client.h"
#include "psa/service.h"
#include "psa_manifest/tfm_test_client_service.h"
#endif

/**
 * \brief Service initialisation function. No special initialisation is
 *        required.
 *
 * \return Returns 0 on success
 */
int32_t tfm_secure_client_service_init(void)
{
#ifdef TFM_PSA_API
    psa_msg_t msg;

    while (1) {
        psa_wait(TFM_SECURE_CLIENT_SFN_RUN_TESTS_SIGNAL, PSA_BLOCK);
        psa_get(TFM_SECURE_CLIENT_SFN_RUN_TESTS_SIGNAL, &msg);
        switch (msg.type) {
        case PSA_IPC_CONNECT:
            psa_reply(msg.handle, PSA_SUCCESS);
            break;
        case PSA_IPC_CALL:
            psa_reply(msg.handle, tfm_secure_client_service_sfn_run_tests());
            break;
        case PSA_IPC_DISCONNECT:
            psa_reply(msg.handle, PSA_SUCCESS);
            break;
        default:
            /* cannot get here? [broken SPM]. TODO*/
            break;
        }
    }
#else
    return 0;
#endif
}

int32_t tfm_secure_client_service_sfn_run_tests(void)
{
    start_integ_test();
    return 0;
}
