/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "platform/include/tfm_spm_hal.h"
#include "psa/error.h"
#include "tfm_nspm.h"
#include "tfm_utils.h"
#include "tfm_internal.h"
#include "log/tfm_assert.h"
#include "log/tfm_log.h"

#define DEFAULT_NS_CLIENT_ID ((int32_t)-1)

typedef uint32_t TZ_ModuleId_t;
typedef uint32_t TZ_MemoryId_t;

int32_t tfm_nspm_get_current_client_id(void)
{
    return DEFAULT_NS_CLIENT_ID;
}

/* TF-M implementation of the CMSIS TZ RTOS thread context management API */

/// Initialize secure context memory system
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__tfm_nspm_secure_gateway_attributes__
uint32_t TZ_InitContextSystem_S(void)
{
    return 1U;
}

/// Allocate context memory for calling secure software modules in TrustZone
/// \param[in]  module   identifies software modules called from non-secure mode
/// \return value != 0 id TrustZone memory slot identifier
/// \return value 0    no memory available or internal error
/* This veneer is TF-M internal, not a secure service */
__tfm_nspm_secure_gateway_attributes__
TZ_MemoryId_t TZ_AllocModuleContext_S (TZ_ModuleId_t module)
{
    /* add attribute 'noinline' to avoid a build error. */
    (void)module;
    return 1U;
}

/// Free context memory that was previously allocated with \ref TZ_AllocModuleContext_S
/// \param[in]  id  TrustZone memory slot identifier
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__tfm_nspm_secure_gateway_attributes__
uint32_t TZ_FreeModuleContext_S (TZ_MemoryId_t id)
{
    (void)id;
    return 1U;
}

/// Load secure context (called on RTOS thread context switch)
/// \param[in]  id  TrustZone memory slot identifier
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__tfm_nspm_secure_gateway_attributes__
uint32_t TZ_LoadContext_S (TZ_MemoryId_t id)
{
    (void)id;
    return 1U;
}

/// Store secure context (called on RTOS thread context switch)
/// \param[in]  id  TrustZone memory slot identifier
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__tfm_nspm_secure_gateway_attributes__
uint32_t TZ_StoreContext_S (TZ_MemoryId_t id)
{
    (void)id;
    return 1U;
}

/*
 * 'r0' impliedly holds the address of non-secure entry,
 * given during non-secure partition initialization.
 */
__attribute__((naked, section("SFN")))
void tfm_nspm_thread_entry(void)
{
    __ASM volatile(
#ifndef __ICCARM__
        ".syntax unified         \n"
#endif
        "mov      r4, r0         \n"
        "movs     r2, #1         \n" /* Clear Bit[0] for S to NS transition */
        "bics     r4, r2         \n"
        "mov      r0, r4         \n"
        "mov      r1, r4         \n"
        "mov      r2, r4         \n"
        "mov      r3, r4         \n"
        "mov      r5, r4         \n"
        "mov      r6, r4         \n"
        "mov      r7, r4         \n"
        "mov      r8, r4         \n"
        "mov      r9, r4         \n"
        "mov      r10, r4        \n"
        "mov      r11, r4        \n"
        "mov      r12, r4        \n"
        "push     {r0, r1}       \n"
        "bxns     r0             \n"
    );
}

void configure_ns_code(void)
{
    /* SCB_NS.VTOR points to the Non-secure vector table base address */
    SCB_NS->VTOR = tfm_spm_hal_get_ns_VTOR();

    /* Setups Main stack pointer of the non-secure code */
    uint32_t ns_msp = tfm_spm_hal_get_ns_MSP();

    __TZ_set_MSP_NS(ns_msp);
}
