/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform/include/tfm_spm_hal.h"
#include "psa/client.h"
#include "tfm_internal.h"
#include "tfm_nspm.h"
#include "tfm_spe_mailbox.h"
#include "tfm_utils.h"
#include "log/tfm_assert.h"
#include "log/tfm_log.h"
#include "log/tfm_assert.h"

#define DEFAULT_NS_CLIENT_ID            (-1)

int32_t tfm_nspm_get_current_client_id(void)
{
    return DEFAULT_NS_CLIENT_ID;
}

void tfm_nspm_thread_entry(void)
{
#ifdef TFM_CORE_DEBUG
    /* Boot up non-secure core */
    LOG_MSG("Enabling non-secure core...");
#endif

    tfm_spm_hal_boot_ns_cpu(tfm_spm_hal_get_ns_VTOR());
    tfm_spm_hal_wait_for_ns_cpu_ready();

    tfm_mailbox_init();

    /*
     * TODO
     * The infinite-loop can be replaced with low-power sleep and resume
     * operation. It may require privileged access to platform specific
     * hardware.
     */
    while (1) {
    }


    /* NOTREACHED */
    TFM_ASSERT(false);
}

void tfm_psa_ipc_request_handler(const uint32_t svc_args[])
{
    (void)svc_args;

    /* Should not receive any request from ns-callable in multi-core topology */
    tfm_core_panic();
}
