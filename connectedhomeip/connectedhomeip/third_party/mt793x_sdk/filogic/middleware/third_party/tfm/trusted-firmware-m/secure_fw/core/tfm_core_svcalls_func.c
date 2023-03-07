/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>

#include "secure_utilities.h"
#include "core/tfm_core_svc.h"
#include "tfm_secure_api.h"
#include "region_defs.h"
#include "spm_api.h"
#include "spm_partition_defs.h"
#include "tfm_api.h"
#include "tfm_internal.h"
#include "tfm_memory_utils.h"
#include "tfm_arch.h"
#include "tfm_peripherals_def.h"
#include "tfm_irq_list.h"
#include "spm_api.h"

#ifdef PLATFORM_SVC_HANDLERS
extern int32_t platform_svc_handlers(tfm_svc_number_t svc_num,
                                     uint32_t *svc_args, uint32_t lr);
#endif

/* Include the definitions of the privileged IRQ handlers in case of library
 * model
 */
#include "tfm_secure_irq_handlers.inc"

uint32_t tfm_core_svc_handler(uint32_t *svc_args, uint32_t lr, uint32_t *msp)
{
    uint8_t svc_number = 0;
    /*
     * Stack contains:
     * r0, r1, r2, r3, r12, r14 (lr), the return address and xPSR
     * First argument (r0) is svc_args[0]
     */
    if (is_return_secure_stack(lr)) {
        /* SV called directly from secure context. Check instruction for
         * svc_number
         */
        svc_number = ((uint8_t *)svc_args[6])[-2];
    } else {
        /* Secure SV executing with NS return.
         * NS cannot directly trigger S SVC so this should not happen. This is
         * an unrecoverable error.
         */
        tfm_core_panic();
    }
    switch (svc_number) {
    case TFM_SVC_SFN_REQUEST:
        lr = tfm_spm_partition_request_svc_handler(svc_args, lr);
        break;
    case TFM_SVC_SFN_RETURN:
        lr = tfm_spm_partition_return_handler(lr);
        break;
    case TFM_SVC_VALIDATE_SECURE_CALLER:
        tfm_spm_validate_secure_caller_handler(svc_args);
        break;
    case TFM_SVC_GET_CALLER_CLIENT_ID:
        tfm_spm_get_caller_client_id_handler(svc_args);
        break;
    case TFM_SVC_SPM_REQUEST:
        tfm_spm_request_handler((struct tfm_state_context_t *)svc_args);
        break;
    case TFM_SVC_DEPRIV_REQ:
        lr = tfm_spm_depriv_req_handler(svc_args, lr);
        break;
    case TFM_SVC_DEPRIV_RET:
        lr = tfm_spm_depriv_return_handler(msp, lr);
        break;
    case TFM_SVC_PSA_WAIT:
        tfm_spm_psa_wait(svc_args);
        break;
    case TFM_SVC_PSA_EOI:
        tfm_spm_psa_eoi(svc_args);
        break;
    case TFM_SVC_ENABLE_IRQ:
        tfm_spm_enable_irq_handler(svc_args);
        break;
    case TFM_SVC_DISABLE_IRQ:
        tfm_spm_disable_irq_handler(svc_args);
        break;
    case TFM_SVC_GET_BOOT_DATA:
        tfm_core_get_boot_data_handler(svc_args);
        break;
    default:
#ifdef PLATFORM_SVC_HANDLERS
        svc_args[0] = platform_svc_handlers(svc_num, svc_args, lr);
#endif
        break;
    }

    return lr;
}

void tfm_access_violation_handler(void)
{
    while (1) {
        ;
    }
}
