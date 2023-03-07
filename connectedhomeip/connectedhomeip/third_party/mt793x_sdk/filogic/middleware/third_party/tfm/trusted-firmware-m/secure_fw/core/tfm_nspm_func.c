/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "cmsis_compiler.h"
#include "platform/include/tfm_spm_hal.h"
#include "tfm_arch.h"
#include "tfm_api.h"
#include "tfm_internal.h"
#include "tfm_nspm.h"

#ifndef TFM_MAX_NS_THREAD_COUNT
#define TFM_MAX_NS_THREAD_COUNT 8
#endif
#define INVALID_CLIENT_ID 0

#define DEFAULT_NS_CLIENT_ID ((int32_t)-1)

#define INVALID_NS_CLIENT_IDX (-1)
#define DEFAULT_NS_CLIENT_IDX   0

typedef uint32_t TZ_ModuleId_t;
typedef uint32_t TZ_MemoryId_t;

#ifdef TFM_NS_CLIENT_IDENTIFICATION
static struct ns_client_list_t {
    int32_t ns_client_id;
    int32_t next_free_index;
} NsClientIdList[TFM_MAX_NS_THREAD_COUNT];

static int32_t free_index = 0U;
static int32_t active_ns_client_idx = INVALID_NS_CLIENT_IDX;

static int get_next_ns_client_id()
{
    static int32_t next_ns_client_id = DEFAULT_NS_CLIENT_ID;

    if (next_ns_client_id > 0) {
        next_ns_client_id = DEFAULT_NS_CLIENT_ID;
    }
    return next_ns_client_id--;
}
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

void tfm_nspm_configure_clients(void)
{
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    int32_t i;

    /* Default to one NS client */
    free_index = 1;
    NsClientIdList[0].ns_client_id = get_next_ns_client_id();
    for (i = 1; i < TFM_MAX_NS_THREAD_COUNT; ++i) {
        NsClientIdList[i].ns_client_id = INVALID_CLIENT_ID;
    }
    active_ns_client_idx = DEFAULT_NS_CLIENT_IDX;
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
}

int32_t tfm_nspm_get_current_client_id(void)
{
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    if (active_ns_client_idx == INVALID_NS_CLIENT_IDX) {
        return 0;
    } else {
        return NsClientIdList[active_ns_client_idx].ns_client_id;
    }
#else /* TFM_NS_CLIENT_IDENTIFICATION */
    return DEFAULT_NS_CLIENT_ID;
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
}

/*
 * TF-M implementation of the CMSIS TZ RTOS thread context management API
 * Currently the context management only contains the NS ID identification
 */

/// Initialize secure context memory system
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__tfm_nspm_secure_gateway_attributes__
uint32_t TZ_InitContextSystem_S(void)
{
#ifdef CONFIG_TFM_ENABLE_CTX_MGMT
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    int32_t i;

    if (__get_active_exc_num() == EXC_NUM_THREAD_MODE) {
        /* This veneer should only be called by NS RTOS in handler mode */
        return 0U;
    }

    /* NS RTOS supports TZ context management, override defaults */
    for (i = 1; i < TFM_MAX_NS_THREAD_COUNT; ++i) {
        NsClientIdList[i].ns_client_id = INVALID_CLIENT_ID;
        NsClientIdList[i].next_free_index = i + 1;
    }

    /* Terminate list */
    NsClientIdList[i - 1].next_free_index = INVALID_NS_CLIENT_IDX;
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
#endif /* CONFIG_TFM_ENABLE_CTX_MGMT */

    /* Success */
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
    TZ_MemoryId_t tz_id = 1;
    (void) module; /* Currently unused */

#ifdef CONFIG_TFM_ENABLE_CTX_MGMT
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    if (__get_active_exc_num() == EXC_NUM_THREAD_MODE) {
        /* This veneer should only be called by NS RTOS in handler mode */
        return 0U;
    }

    if (free_index < 0) {
        /* No more free slots */
        return 0U;
    }

    /* TZ_MemoryId_t must be a positive integer */
    tz_id = (TZ_MemoryId_t)free_index + 1;
    NsClientIdList[free_index].ns_client_id = get_next_ns_client_id();
    free_index = NsClientIdList[free_index].next_free_index;
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
#endif /* CONFIG_TFM_ENABLE_CTX_MGMT */

    return tz_id;
}

/// Free context memory that was previously allocated with \ref TZ_AllocModuleContext_S
/// \param[in]  id  TrustZone memory slot identifier
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__tfm_nspm_secure_gateway_attributes__
uint32_t TZ_FreeModuleContext_S (TZ_MemoryId_t id)
{
#ifdef CONFIG_TFM_ENABLE_CTX_MGMT
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    uint32_t index;

    if (__get_active_exc_num() == EXC_NUM_THREAD_MODE) {
        /* This veneer should only be called by NS RTOS in handler mode */
        return 0U;
    }

    if ((id == 0U) || (id > TFM_MAX_NS_THREAD_COUNT)) {
        /* Invalid TZ_MemoryId_t */
        return 0U;
    }

    index = id - 1;

    if (NsClientIdList[index].ns_client_id == INVALID_CLIENT_ID) {
        /* Non-existent client */
        return 0U;
    }

    if (active_ns_client_idx == index) {
        active_ns_client_idx = DEFAULT_NS_CLIENT_IDX;
    }
    NsClientIdList[index].ns_client_id = INVALID_CLIENT_ID;
    NsClientIdList[index].next_free_index = free_index;

    free_index = index;
#else /* TFM_NS_CLIENT_IDENTIFICATION */
    (void)id;
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
#else /* CONFIG_TFM_ENABLE_CTX_MGMT */
    (void)id;
#endif /* CONFIG_TFM_ENABLE_CTX_MGMT */

    return 1U;    // Success
}

/// Load secure context (called on RTOS thread context switch)
/// \param[in]  id  TrustZone memory slot identifier
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__tfm_nspm_secure_gateway_attributes__
uint32_t TZ_LoadContext_S (TZ_MemoryId_t id)
{
#ifdef CONFIG_TFM_ENABLE_CTX_MGMT
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    uint32_t index;

    if (__get_active_exc_num() == EXC_NUM_THREAD_MODE) {
        /* This veneer should only be called by NS RTOS in handler mode */
        return 0U;
    }

    if ((id == 0U) || (id > TFM_MAX_NS_THREAD_COUNT)) {
        /* Invalid TZ_MemoryId_t */
        return 0U;
    }

    index = id - 1;

    if (NsClientIdList[index].ns_client_id == INVALID_CLIENT_ID) {
        /* Non-existent client */
        return 0U;
    }

    active_ns_client_idx = index;
#else /* TFM_NS_CLIENT_IDENTIFICATION */
    (void)id;
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
#else /* CONFIG_TFM_ENABLE_CTX_MGMT */
    (void)id;
#endif /* CONFIG_TFM_ENABLE_CTX_MGMT */

    return 1U;    // Success
}

/// Store secure context (called on RTOS thread context switch)
/// \param[in]  id  TrustZone memory slot identifier
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__tfm_nspm_secure_gateway_attributes__
uint32_t TZ_StoreContext_S (TZ_MemoryId_t id)
{
#ifdef CONFIG_TFM_ENABLE_CTX_MGMT
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    uint32_t index;

    if (__get_active_exc_num() == EXC_NUM_THREAD_MODE) {
        /* This veneer should only be called by NS RTOS in handler mode */
        return 0U;
    }

    /* id corresponds to context being swapped out on NS side */
    if ((id == 0U) || (id > TFM_MAX_NS_THREAD_COUNT)) {
        /* Invalid TZ_MemoryId_t */
        return 0U;
    }

    index = id - 1;

    if (NsClientIdList[index].ns_client_id == INVALID_CLIENT_ID) {
        /* Non-existent client */
        return 0U;
    }

    if (active_ns_client_idx != index) {
        return 0U;
    }

    active_ns_client_idx = DEFAULT_NS_CLIENT_IDX;
#else /* TFM_NS_CLIENT_IDENTIFICATION */
    (void)id;
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
#else /* CONFIG_TFM_ENABLE_CTX_MGMT */
    (void)id;
#endif /* CONFIG_TFM_ENABLE_CTX_MGMT */

    return 1U;    // Success
}

#ifdef TFM_NS_CLIENT_IDENTIFICATION
__tfm_nspm_secure_gateway_attributes__
enum tfm_status_e tfm_register_client_id (int32_t ns_client_id)
{
    int current_client_id;

    if (__get_active_exc_num() == EXC_NUM_THREAD_MODE) {
        /* This veneer should only be called by NS RTOS in handler mode */
        return TFM_ERROR_NS_THREAD_MODE_CALL;
    }

    if (ns_client_id >= 0) {
        /* The client ID is invalid */
        return TFM_ERROR_INVALID_PARAMETER;
    }

    if (active_ns_client_idx < 0) {
        /* No client is active */
        return TFM_ERROR_GENERIC;
    }

    current_client_id = NsClientIdList[active_ns_client_idx].ns_client_id;
    if (current_client_id >= 0 ) {
        /* The client ID is invalid */
        return TFM_ERROR_INVALID_PARAMETER;
    }

    NsClientIdList[active_ns_client_idx].ns_client_id = ns_client_id;

    return TFM_SUCCESS;
}
#endif

void configure_ns_code(void)
{
    /* SCB_NS.VTOR points to the Non-secure vector table base address */
    SCB_NS->VTOR = tfm_spm_hal_get_ns_VTOR();

    /* Setups Main stack pointer of the non-secure code */
    uint32_t ns_msp = tfm_spm_hal_get_ns_MSP();

    __TZ_set_MSP_NS(ns_msp);

    /* Get the address of non-secure code entry point to jump there */
    uint32_t entry_ptr = tfm_spm_hal_get_ns_entry_point();

    /* Clears LSB of the function address to indicate the function-call
     * will perform the switch from secure to non-secure
     */
    ns_entry = (nsfptr_t) cmse_nsfptr_create(entry_ptr);
}
