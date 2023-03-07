/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifdef TFM_PSA_API
#include "psa/service.h"
#include "psa_manifest/tfm_sst_test_service.h"
#else
#include "psa/client.h"
#endif

#include "secure_fw/services/secure_storage/sst_object_system.h"

psa_status_t tfm_sst_test_prepare(psa_invec *in_vec, size_t in_len,
                                  psa_outvec *out_vec, size_t out_len)
{
    (void)in_vec;
    (void)in_len;
    (void)out_vec;
    (void)out_len;

    return sst_system_prepare();
}

psa_status_t tfm_sst_test_init(void)
{
#ifdef TFM_PSA_API
    psa_msg_t msg;

    while (1) {
        (void)psa_wait(TFM_SST_TEST_PREPARE_SIGNAL, PSA_BLOCK);
        (void)psa_get(TFM_SST_TEST_PREPARE_SIGNAL, &msg);
        switch (msg.type) {
        case PSA_IPC_CONNECT:
        case PSA_IPC_DISCONNECT:
            psa_reply(msg.handle, PSA_SUCCESS);
            break;
        case PSA_IPC_CALL:
            psa_reply(msg.handle, sst_system_prepare());
            break;
        }
    }
#else
    return PSA_SUCCESS;
#endif
}
