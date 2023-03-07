/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <arm_cmse.h>

#include "core/tfm_core_svc.h"
#include "tfm_secure_api.h"
#include "tfm_internal.h"
#include "secure_fw/include/tfm_spm_services_api.h"
#include "spm_api.h"
#include "psa/service.h"

#ifndef TFM_PSA_API
nsfptr_t ns_entry;

void jump_to_ns_code(void)
{
    /* Calls the non-secure Reset_Handler to jump to the non-secure binary */
    ns_entry();
}

__attribute__((naked))
int32_t tfm_core_get_caller_client_id(int32_t *caller_client_id)
{
    __ASM volatile(
        "SVC %0\n"
        "BX LR\n"
        : : "I" (TFM_SVC_GET_CALLER_CLIENT_ID));
}

__attribute__((naked))
int32_t tfm_core_validate_secure_caller(void)
{
    __ASM volatile(
        "SVC    %0\n"
        "BX     lr\n"
        : : "I" (TFM_SVC_VALIDATE_SECURE_CALLER));
}

#endif

__attribute__((naked))
int32_t tfm_spm_request(void)
{
    __ASM volatile(
        "SVC    %0\n"
        "BX     lr\n"
        : : "I" (TFM_SVC_SPM_REQUEST));
}

__attribute__((naked))
int32_t tfm_spm_request_reset_vote(void)
{
    __ASM volatile(
        "MOVS   R0, %0\n"
        "B      tfm_spm_request\n"
        : : "I" (TFM_SPM_REQUEST_RESET_VOTE));
}

__attribute__((naked))
int32_t tfm_core_get_boot_data(uint8_t major_type,
                               struct tfm_boot_data *boot_status,
                               uint32_t len)
{
    __ASM volatile(
        "SVC    %0\n"
        "BX     lr\n"
        : : "I" (TFM_SVC_GET_BOOT_DATA));
}

__attribute__((naked))
void tfm_enable_irq(psa_signal_t irq_signal)
{
    __ASM("SVC %0\n"
          "BX LR\n"
          : : "I" (TFM_SVC_ENABLE_IRQ));
}

__attribute__((naked))
void tfm_disable_irq(psa_signal_t irq_signal)
{
    __ASM("SVC %0\n"
          "BX LR\n"
          : : "I" (TFM_SVC_DISABLE_IRQ));
}

#ifndef TFM_PSA_API

__attribute__((naked))
static psa_signal_t psa_wait_internal(psa_signal_t signal_mask,
                                      uint32_t timeout)
{
    __ASM("SVC %0\n"
          "BX LR\n"
          : : "I" (TFM_SVC_PSA_WAIT));
}

psa_signal_t psa_wait(psa_signal_t signal_mask, uint32_t timeout)
{
    /* FIXME: By using the 'WFI' instruction this function blocks until an
     * interrupt happens. It is necessary to do this here as tfm_core_psa_wait
     * runs with the priority of the SVC, so it cannot be interrupted, so
     * waiting in it for the required interrupt to happen is not an option.
     */
    psa_signal_t actual_signal_mask;

    while (1) {
        actual_signal_mask = psa_wait_internal(signal_mask, timeout);
        if ((actual_signal_mask & signal_mask) != 0) {
            return actual_signal_mask;
        }
        __WFI();
    }
}

__attribute__((naked))
void psa_eoi(psa_signal_t irq_signal)
{
    __ASM("SVC %0\n"
          "BX LR\n"
          : : "I" (TFM_SVC_PSA_EOI));
}

#endif
