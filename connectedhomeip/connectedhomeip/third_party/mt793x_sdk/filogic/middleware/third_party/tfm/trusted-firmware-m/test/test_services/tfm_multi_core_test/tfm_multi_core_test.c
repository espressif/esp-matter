/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/client.h"
#include "psa/service.h"
#include "psa_manifest/tfm_multi_core_test.h"

static uint32_t nr_psa_call;

/*
 * Fixme: Temporarily implement abort as infinite loop,
 * will replace it later.
 */
static void tfm_abort(void)
{
    while (1)
        ;
}

static void multi_core_multi_client_call_test(uint32_t signal)
{
    psa_msg_t msg;
    psa_status_t status;

    status = psa_get(signal, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    switch(msg.type) {
    case PSA_IPC_CONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_CALL:
        nr_psa_call++;
        /* Write current number of calls to outvec. */
        psa_write(msg.handle, 0, &nr_psa_call, sizeof(nr_psa_call));
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_DISCONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        /* Unsupported operations */
        tfm_abort();
    }
}

/* Test thread */
void multi_core_test_main(void *param)
{
    uint32_t signals = 0;

    (void)param;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

        if (signals & MULTI_CORE_MULTI_CLIENT_CALL_TEST_0_SIGNAL) {
            multi_core_multi_client_call_test(
                                    MULTI_CORE_MULTI_CLIENT_CALL_TEST_0_SIGNAL);
        } else if (signals & MULTI_CORE_MULTI_CLIENT_CALL_TEST_1_SIGNAL) {
            multi_core_multi_client_call_test(
                                    MULTI_CORE_MULTI_CLIENT_CALL_TEST_1_SIGNAL);
        } else {
            /* Should not come here */
            tfm_abort();
        }
    }
}
