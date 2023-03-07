/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "region.h"
#include "spm_api.h"
#include "spm_db.h"
#include "tfm_api.h"
#include "tfm_arch.h"
#include "tfm_core_trustzone.h"
#include "tfm_internal.h"
#include "tfm_svcalls.h"
#include "tfm_utils.h"
#include "core/tfm_core_svc.h"

/* The section names come from the scatter file */
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);

#ifdef PLATFORM_SVC_HANDLERS
extern int32_t platform_svc_handlers(tfm_svc_number_t svc_num,
                                     uint32_t *ctx, uint32_t lr);
#endif

static int32_t SVC_Handler_IPC(tfm_svc_number_t svc_num, uint32_t *ctx,
                               uint32_t lr)
{
    bool ns_caller = false;
    struct spm_partition_desc_t *partition = NULL;
    uint32_t veneer_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
    uint32_t veneer_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);

    /*
     * The caller security attribute detection bases on LR of state context.
     * However, if SP calls PSA APIs based on its customized SVC, the LR may be
     * occupied by general purpose value while calling SVC.
     * Check if caller comes from non-secure: return address (ctx[6]) is belongs
     * to veneer section, and the bit0 of LR (ctx[5]) is zero.
     */
    if (ctx[6] >= veneer_base && ctx[6] < veneer_limit &&
        !(ctx[5] & TFM_VENEER_LR_BIT0_MASK)) {
        ns_caller = true;
    }

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    tfm_spm_validate_caller(partition, ctx, lr, ns_caller);

    switch (svc_num) {
    case TFM_SVC_PSA_FRAMEWORK_VERSION:
        return tfm_spm_psa_framework_version();
    case TFM_SVC_PSA_VERSION:
        return tfm_spm_psa_version(ctx, ns_caller);
    case TFM_SVC_PSA_CONNECT:
        return tfm_spm_psa_connect(ctx, ns_caller);
    case TFM_SVC_PSA_CALL:
        return tfm_spm_psa_call(ctx, ns_caller, lr);
    case TFM_SVC_PSA_CLOSE:
        tfm_spm_psa_close(ctx, ns_caller);
        break;
    case TFM_SVC_PSA_WAIT:
        return tfm_spm_psa_wait(ctx);
    case TFM_SVC_PSA_GET:
        return tfm_spm_psa_get(ctx);
    case TFM_SVC_PSA_SET_RHANDLE:
        tfm_spm_psa_set_rhandle(ctx);
        break;
    case TFM_SVC_PSA_READ:
        return tfm_spm_psa_read(ctx);
    case TFM_SVC_PSA_SKIP:
        return tfm_spm_psa_skip(ctx);
    case TFM_SVC_PSA_WRITE:
        tfm_spm_psa_write(ctx);
        break;
    case TFM_SVC_PSA_REPLY:
        tfm_spm_psa_reply(ctx);
        break;
    case TFM_SVC_PSA_NOTIFY:
        tfm_spm_psa_notify(ctx);
        break;
    case TFM_SVC_PSA_CLEAR:
        tfm_spm_psa_clear();
        break;
    case TFM_SVC_PSA_EOI:
        tfm_spm_psa_eoi(ctx);
        break;
    case TFM_SVC_ENABLE_IRQ:
        tfm_spm_enable_irq(ctx);
        break;
    case TFM_SVC_DISABLE_IRQ:
        tfm_spm_disable_irq(ctx);
        break;
    case TFM_SVC_PSA_PANIC:
        tfm_spm_psa_panic();
        break;
    case TFM_SVC_SPM_REQUEST:
        tfm_spm_request_handler((const struct tfm_state_context_t *)ctx);
        break;
    case TFM_SVC_PSA_LIFECYCLE:
        return tfm_spm_get_lifecycle_state();
    default:
#ifdef PLATFORM_SVC_HANDLERS
        return (platform_svc_handlers(svc_num, ctx, lr));
#else
        ERROR_MSG("Unknown SVC number requested!");
        return PSA_ERROR_GENERIC_ERROR;
#endif
    }
    return PSA_SUCCESS;
}

uint32_t tfm_core_svc_handler(uint32_t *svc_args, uint32_t exc_return)
{
    tfm_svc_number_t svc_number = TFM_SVC_SFN_REQUEST;
    /*
     * Stack contains:
     * r0, r1, r2, r3, r12, r14 (lr), the return address and xPSR
     * First argument (r0) is svc_args[0]
     */
    if (is_return_secure_stack(exc_return)) {
        /* SV called directly from secure context. Check instruction for
         * svc_number
         */
        svc_number = ((tfm_svc_number_t *)svc_args[6])[-2];
    } else {
        /* Secure SV executing with NS return.
         * NS cannot directly trigger S SVC so this should not happen. This is
         * an unrecoverable error.
         */
        tfm_core_panic();
    }
    switch (svc_number) {
    case TFM_SVC_HANDLER_MODE:
        tfm_arch_clear_fp_status();
        exc_return = tfm_spm_init();
        break;
    case TFM_SVC_GET_BOOT_DATA:
        tfm_core_get_boot_data_handler(svc_args);
        break;
    default:
        svc_args[0] = SVC_Handler_IPC(svc_number, svc_args, exc_return);
        break;
    }

    return exc_return;
}

__attribute__ ((naked)) void tfm_core_handler_mode(void)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_HANDLER_MODE));
}

void tfm_access_violation_handler(void)
{
    while (1) {
        ;
    }
}
