/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <arm_cmse.h>
#include "tfm_nspm.h"
#include "secure_utilities.h"
#include "tfm_api.h"
#include "tfm_arch.h"
#include "tfm_irq_list.h"
#include "psa/service.h"
#include "tfm_core_mem_check.h"
#include "tfm_secure_api.h"
#include "tfm_spm_hal.h"
#include "spm_api.h"
#include "spm_db.h"
#include "region_defs.h"
#include "region.h"
#include "tfm_spm_services_api.h"
#include "memory_map_mt7933.h"

#define EXC_RETURN_SECURE_FUNCTION 0xFFFFFFFD
#define EXC_RETURN_SECURE_HANDLER  0xFFFFFFF1

#ifndef TFM_LVL
#error TFM_LVL is not defined!
#endif

REGION_DECLARE_T(Image$$, TFM_SECURE_STACK, $$ZI$$Base, uint32_t);
REGION_DECLARE_T(Image$$, TFM_SECURE_STACK, $$ZI$$Limit, struct iovec_args_t)[];

/*
 * This is the "Big Lock" on the secure side, to guarantee single entry
 * to SPE
 */
extern int32_t tfm_secure_lock;
static int32_t tfm_secure_api_initializing = 1;

extern struct spm_partition_db_t g_spm_partition_db;

static uint32_t *prepare_partition_iovec_ctx(
                             const struct tfm_state_context_t *svc_ctx,
                             const struct tfm_sfn_req_s *desc_ptr,
                             const struct iovec_args_t *iovec_args,
                             uint32_t *dst)
{
    /* XPSR  = as was when called, but make sure it's thread mode */
    *(--dst) = svc_ctx->xpsr & 0xFFFFFE00U;
    /* ReturnAddress = resume veneer in new context */
    *(--dst) = svc_ctx->ra;
    /* LR = sfn address */
    *(--dst) = (uint32_t)desc_ptr->sfn;
    /* R12 = don't care */
    *(--dst) = 0U;

    /* R0-R3 = sfn arguments */
    *(--dst) = iovec_args->out_len;
    *(--dst) = (uint32_t)iovec_args->out_vec;
    *(--dst) = iovec_args->in_len;
    *(--dst) = (uint32_t)iovec_args->in_vec;

    return dst;
}

/**
 * \brief Create a stack frame that sets the execution environment to thread
 *        mode on exception return.
 *
 * \param[in] svc_ctx         The stacked SVC context
 * \param[in] unpriv_handler  The unprivileged IRQ handler to be called
 * \param[in] dst             A pointer where the context is to be created. (the
 *                            pointer is considered to be a stack pointer, and
 *                            the frame is created below it)
 *
 * \return A pointer pointing at the created stack frame.
 */
static int32_t *prepare_partition_irq_ctx(
                             const struct tfm_state_context_t *svc_ctx,
                             sfn_t unpriv_handler,
                             int32_t *dst)
{
    int i;

    /* XPSR  = as was when called, but make sure it's thread mode */
    *(--dst) = svc_ctx->xpsr & 0xFFFFFE00;
    /* ReturnAddress = resume to the privileged handler code, but execute it
     * unprivileged.
     */
    *(--dst) = svc_ctx->ra;
    /* LR = start address */
    *(--dst) = (int32_t)unpriv_handler;

    /* R12, R0-R3 unused arguments */
    for (i = 0; i < 5; ++i) {
        *(--dst) = 0;
    }

    return dst;
}

static void restore_caller_ctx(const struct tfm_state_context_t *svc_ctx,
                               struct tfm_state_context_t *target_ctx)
{
    /* ReturnAddress = resume veneer after second SVC */
    target_ctx->ra = svc_ctx->ra;

    /* R0 = function return value */
    target_ctx->r0 = svc_ctx->r0;

    return;
}

/**
 * \brief Check whether the iovec parameters are valid, and the memory ranges
 *        are in the possession of the calling partition.
 *
 * \param[in] desc_ptr  The secure function request descriptor
 *
 * \return Return /ref TFM_SUCCESS if the iovec parameters are valid, error code
 *         otherwise as in /ref tfm_status_e
 */
static enum tfm_status_e tfm_core_check_sfn_parameters(
                                           const struct tfm_sfn_req_s *desc_ptr)
{
    struct psa_invec *in_vec = (psa_invec *)desc_ptr->args[0];
    size_t in_len;
    struct psa_outvec *out_vec = (psa_outvec *)desc_ptr->args[2];
    size_t out_len;
    uint32_t i;
    uint32_t privileged_mode = TFM_PARTITION_UNPRIVILEGED_MODE;

    if ((desc_ptr->args[1] < 0) || (desc_ptr->args[3] < 0)) {
        return TFM_ERROR_INVALID_PARAMETER;
    }

    in_len = (size_t)(desc_ptr->args[1]);
    out_len = (size_t)(desc_ptr->args[3]);

    /*
     * Get caller's privileged mode:
     * The privileged mode of NS Secure Service caller will be decided by the
     * tfm_core_has_xxx_access_to_region functions.
     * Secure caller can be only privileged mode because the whole SPE is
     * running under privileged mode
     */
    if (!desc_ptr->ns_caller) {
        privileged_mode = TFM_PARTITION_PRIVILEGED_MODE;
    }

    /* The number of vectors are within range. Extra checks to avoid overflow */
    if ((in_len > PSA_MAX_IOVEC) || (out_len > PSA_MAX_IOVEC) ||
        (in_len + out_len > PSA_MAX_IOVEC)) {
        return TFM_ERROR_INVALID_PARAMETER;
    }

    /* Check whether the caller partition has at write access to the iovec
     * structures themselves. Use the TT instruction for this.
     */
    if (in_len > 0) {
        if ((in_vec == NULL) ||
            (tfm_core_has_write_access_to_region(in_vec,
                            sizeof(psa_invec)*in_len, desc_ptr->ns_caller,
                            privileged_mode) != TFM_SUCCESS)) {
            return TFM_ERROR_INVALID_PARAMETER;
        }
    } else {
        if (in_vec != NULL) {
            return TFM_ERROR_INVALID_PARAMETER;
        }
    }
    if (out_len > 0) {
        if ((out_vec == NULL) ||
            (tfm_core_has_write_access_to_region(out_vec,
                            sizeof(psa_outvec)*out_len, desc_ptr->ns_caller,
                            privileged_mode) != TFM_SUCCESS)) {
            return TFM_ERROR_INVALID_PARAMETER;
        }
    } else {
        if (out_vec != NULL) {
            return TFM_ERROR_INVALID_PARAMETER;
        }
    }

    /* Check whether the caller partition has access to the data inside the
     * iovecs
     */
    for (i = 0; i < in_len; ++i) {
        if (in_vec[i].len > 0) {
            if ((in_vec[i].base == NULL) ||
                (tfm_core_has_read_access_to_region(in_vec[i].base,
                            in_vec[i].len, desc_ptr->ns_caller,
                            privileged_mode) != TFM_SUCCESS)) {
                return TFM_ERROR_INVALID_PARAMETER;
            }
        }
    }
    for (i = 0; i < out_len; ++i) {
        if (out_vec[i].len > 0) {
            if ((out_vec[i].base == NULL) ||
                (tfm_core_has_write_access_to_region(out_vec[i].base,
                            out_vec[i].len, desc_ptr->ns_caller,
                            privileged_mode) != TFM_SUCCESS)) {
                return TFM_ERROR_INVALID_PARAMETER;
            }
        }
    }

    return TFM_SUCCESS;
}

static void tfm_copy_iovec_parameters(struct iovec_args_t *target,
                                      const struct iovec_args_t *source)
{
    size_t i;

    /* The vectors have been sanity checked already, and since then the
     * interrupts have been kept disabled. So we can be sure that the
     * vectors haven't been tampered with since the check. So it is safe to pass
     * it to the called partition.
     */

    target->in_len = source->in_len;
    for (i = 0; i < source->in_len; ++i) {
        target->in_vec[i].base = source->in_vec[i].base;
        target->in_vec[i].len = source->in_vec[i].len;
    }
    target->out_len = source->out_len;
    for (i = 0; i < source->out_len; ++i) {
        target->out_vec[i].base = source->out_vec[i].base;
        target->out_vec[i].len = source->out_vec[i].len;
    }
}

static void tfm_clear_iovec_parameters(struct iovec_args_t *args)
{
    int i;

    args->in_len = 0;
    for (i = 0; i < PSA_MAX_IOVEC; ++i) {
        args->in_vec[i].base = NULL;
        args->in_vec[i].len = 0;
    }
    args->out_len = 0;
    for (i = 0; i < PSA_MAX_IOVEC; ++i) {
        args->out_vec[i].base = NULL;
        args->out_vec[i].len = 0;
    }
}

/**
 * \brief Check whether the partitions for the secure function call are in a
 *        proper state.
 *
 * \param[in] curr_partition_state    State of the partition to be called
 * \param[in] caller_partition_state  State of the caller partition
 *
 * \return \ref TFM_SUCCESS if the check passes, error otherwise.
 */
static enum tfm_status_e check_partition_state(uint32_t curr_partition_state,
                                               uint32_t caller_partition_state)
{
    if (caller_partition_state != SPM_PARTITION_STATE_RUNNING) {
        /* Calling partition from non-running state (e.g. during handling IRQ)
         * is not allowed.
         */
        return TFM_ERROR_INVALID_EXC_MODE;
    }

    if (curr_partition_state == SPM_PARTITION_STATE_RUNNING ||
        curr_partition_state == SPM_PARTITION_STATE_HANDLING_IRQ ||
        curr_partition_state == SPM_PARTITION_STATE_SUSPENDED ||
        curr_partition_state == SPM_PARTITION_STATE_BLOCKED) {
        /* Active partitions cannot be called! */
        return TFM_ERROR_PARTITION_NON_REENTRANT;
    } else if (curr_partition_state != SPM_PARTITION_STATE_IDLE) {
        /* The partition to be called is not in a proper state */
        return TFM_SECURE_LOCK_FAILED;
    }
    return TFM_SUCCESS;
}

/**
 * \brief Check whether the partitions for the secure function call of irq are
 *        in a proper state.
 *
 * \param[in] called_partition_state    State of the partition to be called
 *
 * \return \ref TFM_SUCCESS if the check passes, error otherwise.
 */
static enum tfm_status_e check_irq_partition_state(
                                                uint32_t called_partition_state)
{
    if (called_partition_state == SPM_PARTITION_STATE_IDLE ||
        called_partition_state == SPM_PARTITION_STATE_RUNNING ||
        called_partition_state == SPM_PARTITION_STATE_HANDLING_IRQ ||
        called_partition_state == SPM_PARTITION_STATE_SUSPENDED ||
        called_partition_state == SPM_PARTITION_STATE_BLOCKED) {
        return TFM_SUCCESS;
    }
    return TFM_SECURE_LOCK_FAILED;
}

/**
 * \brief Calculate the address where the iovec parameters are to be saved for
 *        the called partition.
 *
 * \param[in] partition_idx  The index of the partition to be called.
 *
 * \return The address where the iovec parameters should be saved.
 */
static struct iovec_args_t *get_iovec_args_stack_address(uint32_t partition_idx)
{
    /* Save the iovecs on the common stack. */
    return &REGION_NAME(Image$$, TFM_SECURE_STACK, $$ZI$$Limit)[-1];
}

static enum tfm_status_e tfm_start_partition(
                                           const struct tfm_sfn_req_s *desc_ptr,
                                           uint32_t excReturn)
{
    enum tfm_status_e res;
    uint32_t caller_partition_idx = desc_ptr->caller_part_idx;
    const struct spm_partition_runtime_data_t *curr_part_data;
    const struct spm_partition_runtime_data_t *caller_part_data;
    uint32_t caller_flags;
    register uint32_t partition_idx;
    uint32_t psp;
    uint32_t partition_psp, partition_psplim;
    uint32_t partition_state;
    uint32_t caller_partition_state;
    struct tfm_state_context_t *svc_ctx;
    uint32_t caller_partition_id;
    int32_t client_id;
    struct iovec_args_t *iovec_args;

    psp = __get_PSP();
    svc_ctx = (struct tfm_state_context_t *)psp;
    caller_flags = tfm_spm_partition_get_flags(caller_partition_idx);

    /* Check partition state consistency */
    if (((caller_flags & SPM_PART_FLAG_APP_ROT) != 0)
        != (!desc_ptr->ns_caller)) {
        /* Partition state inconsistency detected */
        return TFM_SECURE_LOCK_FAILED;
    }

    partition_idx = get_partition_idx(desc_ptr->sp_id);

    curr_part_data = tfm_spm_partition_get_runtime_data(partition_idx);
    caller_part_data = tfm_spm_partition_get_runtime_data(caller_partition_idx);
    partition_state = curr_part_data->partition_state;
    caller_partition_state = caller_part_data->partition_state;
    caller_partition_id = tfm_spm_partition_get_partition_id(
                                                          caller_partition_idx);

    if (!tfm_secure_api_initializing) {
        res = check_partition_state(partition_state, caller_partition_state);
        if (res != TFM_SUCCESS) {
            return res;
        }
    }

    /* Prepare switch to shared secure partition stack */
    /* In case the call is coming from the non-secure world, we save the iovecs
     * on the stop of the stack. So the memory area, that can actually be used
     * as stack by the partitions starts at a lower address
     */
    partition_psp =
        (uint32_t)&REGION_NAME(Image$$, TFM_SECURE_STACK, $$ZI$$Limit)[-1];
    partition_psplim =
        (uint32_t)&REGION_NAME(Image$$, TFM_SECURE_STACK, $$ZI$$Base);

    /* Store the context for the partition call */
    tfm_spm_partition_set_caller_partition_idx(partition_idx,
                                               caller_partition_idx);
    tfm_spm_partition_store_context(caller_partition_idx, psp, excReturn);

    if ((caller_flags & SPM_PART_FLAG_APP_ROT)) {
        tfm_spm_partition_set_caller_client_id(partition_idx,
                                               caller_partition_id);
    } else {
        client_id = tfm_nspm_get_current_client_id();
        if (client_id >= 0) {
            return TFM_SECURE_LOCK_FAILED;
        }
        tfm_spm_partition_set_caller_client_id(partition_idx, client_id);
    }

    /* In level one, only switch context and return from exception if in
     * handler mode
     */
    if ((desc_ptr->ns_caller) || (tfm_secure_api_initializing)) {
        if (tfm_spm_partition_set_iovec(partition_idx, desc_ptr->args) !=
            SPM_ERR_OK) {
            return TFM_ERROR_GENERIC;
        }
        iovec_args = get_iovec_args_stack_address(partition_idx);
        tfm_copy_iovec_parameters(iovec_args, &(curr_part_data->iovec_args));

        /* Prepare the partition context, update stack ptr */
        psp = (uint32_t)prepare_partition_iovec_ctx(svc_ctx, desc_ptr,
                                                    iovec_args,
                                                    (uint32_t *)partition_psp);
        __set_PSP(psp);
        tfm_arch_set_psplim(partition_psplim);
    }

    tfm_spm_partition_set_state(caller_partition_idx,
                                SPM_PARTITION_STATE_BLOCKED);
    tfm_spm_partition_set_state(partition_idx, SPM_PARTITION_STATE_RUNNING);
    tfm_secure_lock++;

    return TFM_SUCCESS;
}

static enum tfm_status_e tfm_start_partition_for_irq_handling(
                                            uint32_t excReturn,
                                            struct tfm_state_context_t *svc_ctx)
{
    uint32_t handler_partition_id = svc_ctx->r0;
    sfn_t unpriv_handler = (sfn_t)svc_ctx->r1;
    uint32_t irq_signal = svc_ctx->r2;
    IRQn_Type irq_line = (IRQn_Type) svc_ctx->r3;
    enum tfm_status_e res;
    uint32_t psp = __get_PSP();
    uint32_t handler_partition_psp;
    uint32_t handler_partition_state;
    uint32_t interrupted_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const struct spm_partition_runtime_data_t *handler_part_data;
    uint32_t handler_partition_idx;

    handler_partition_idx = get_partition_idx(handler_partition_id);
    handler_part_data = tfm_spm_partition_get_runtime_data(
                                                         handler_partition_idx);
    handler_partition_state = handler_part_data->partition_state;

    res = check_irq_partition_state(handler_partition_state);
    if (res != TFM_SUCCESS) {
        return res;
    }

    /* set mask for the partition */
    tfm_spm_partition_set_signal_mask(
                                   handler_partition_idx,
                                   handler_part_data->signal_mask | irq_signal);

    tfm_spm_hal_disable_irq(irq_line);

    /* save the current context of the interrupted partition */
    tfm_spm_partition_push_interrupted_ctx(interrupted_partition_idx);

    handler_partition_psp = psp;

    /* save the current context of the handler partition */
    tfm_spm_partition_push_handler_ctx(handler_partition_idx);

    /* Store caller for the partition */
    tfm_spm_partition_set_caller_partition_idx(handler_partition_idx,
                                               interrupted_partition_idx);

    psp = (uint32_t)prepare_partition_irq_ctx(svc_ctx, unpriv_handler,
                                              (int32_t *)handler_partition_psp);
    __set_PSP(psp);

    tfm_spm_partition_set_state(interrupted_partition_idx,
                                SPM_PARTITION_STATE_SUSPENDED);
    tfm_spm_partition_set_state(handler_partition_idx,
                                SPM_PARTITION_STATE_HANDLING_IRQ);

    return TFM_SUCCESS;
}

static enum tfm_status_e tfm_return_from_partition(uint32_t *excReturn)
{
    uint32_t current_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const struct spm_partition_runtime_data_t *curr_part_data, *ret_part_data;
    uint32_t return_partition_idx;
    uint32_t return_partition_flags;
    uint32_t psp = __get_PSP();
    size_t i;
    struct tfm_state_context_t *svc_ctx = (struct tfm_state_context_t *)psp;
    struct iovec_args_t *iovec_args;

    if (current_partition_idx == SPM_INVALID_PARTITION_IDX) {
        return TFM_SECURE_UNLOCK_FAILED;
    }

    curr_part_data = tfm_spm_partition_get_runtime_data(current_partition_idx);
    return_partition_idx = curr_part_data->caller_partition_idx;

    if (return_partition_idx == SPM_INVALID_PARTITION_IDX) {
        return TFM_SECURE_UNLOCK_FAILED;
    }

    ret_part_data = tfm_spm_partition_get_runtime_data(return_partition_idx);

    return_partition_flags = tfm_spm_partition_get_flags(return_partition_idx);

    tfm_secure_lock--;

    if (!(return_partition_flags & SPM_PART_FLAG_APP_ROT) ||
        (tfm_secure_api_initializing)) {
        /* In TFM level 1 context restore is only done when
         * returning to NS or after initialization
         */
        /* Restore caller context */
        restore_caller_ctx(svc_ctx,
            (struct tfm_state_context_t *)ret_part_data->stack_ptr);
        *excReturn = ret_part_data->lr;
        __set_PSP(ret_part_data->stack_ptr);
        REGION_DECLARE_T(Image$$, ARM_LIB_STACK, $$ZI$$Base, uint32_t)[];
        uint32_t psp_stack_bottom =
            (uint32_t)REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Base);
        tfm_arch_set_psplim(psp_stack_bottom);

        iovec_args = &REGION_NAME(Image$$, TFM_SECURE_STACK, $$ZI$$Limit)[-1];

        for (i = 0; i < curr_part_data->iovec_args.out_len; ++i) {
            curr_part_data->orig_outvec[i].len = iovec_args->out_vec[i].len;
        }
        tfm_clear_iovec_parameters(iovec_args);
    }

    tfm_spm_partition_cleanup_context(current_partition_idx);

    tfm_spm_partition_set_state(current_partition_idx,
                                SPM_PARTITION_STATE_IDLE);
    tfm_spm_partition_set_state(return_partition_idx,
                                SPM_PARTITION_STATE_RUNNING);

    return TFM_SUCCESS;
}

static enum tfm_status_e tfm_return_from_partition_irq_handling(
                                                            uint32_t *excReturn)
{
    uint32_t handler_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const struct spm_partition_runtime_data_t *handler_part_data;
    uint32_t interrupted_partition_idx;
    uint32_t psp = __get_PSP();
    struct tfm_state_context_t *svc_ctx = (struct tfm_state_context_t *)psp;

    if (handler_partition_idx == SPM_INVALID_PARTITION_IDX) {
        return TFM_SECURE_UNLOCK_FAILED;
    }

    handler_part_data = tfm_spm_partition_get_runtime_data(
                                                         handler_partition_idx);
    interrupted_partition_idx = handler_part_data->caller_partition_idx;

    if (interrupted_partition_idx == SPM_INVALID_PARTITION_IDX) {
        return TFM_SECURE_UNLOCK_FAILED;
    }

    /* For level 1, modify PSP, so that the SVC stack frame disappears,
     * and return to the privileged handler using the stack frame still on the
     * MSP stack.
     */
    *excReturn = svc_ctx->ra;
    psp += sizeof(struct tfm_state_context_t);

    tfm_spm_partition_pop_handler_ctx(handler_partition_idx);
    tfm_spm_partition_pop_interrupted_ctx(interrupted_partition_idx);

    __set_PSP(psp);

    return TFM_SUCCESS;
}

static enum tfm_status_e tfm_check_sfn_req_integrity(
                                           const struct tfm_sfn_req_s *desc_ptr)
{
    if ((desc_ptr == NULL) ||
        (desc_ptr->sp_id == 0) ||
        (desc_ptr->sfn == NULL)) {
        /* invalid parameter */
        return TFM_ERROR_INVALID_PARAMETER;
    }
    return TFM_SUCCESS;
}

static enum tfm_status_e tfm_core_check_sfn_req_rules(
        const struct tfm_sfn_req_s *desc_ptr)
{
    /* Check partition idx validity */
    if (desc_ptr->caller_part_idx == SPM_INVALID_PARTITION_IDX) {
        return TFM_ERROR_NO_ACTIVE_PARTITION;
    }

    if ((desc_ptr->ns_caller) && (tfm_secure_lock != 0)) {
        /* Secure domain is already locked!
         * This should only happen if caller is secure partition!
         */
        /* This scenario is a potential security breach.
         * Error is handled in caller.
         */
        return TFM_ERROR_SECURE_DOMAIN_LOCKED;
    }

    if (tfm_secure_api_initializing) {
        int32_t id =
            tfm_spm_partition_get_partition_id(desc_ptr->caller_part_idx);

        if ((id != TFM_SP_CORE_ID) || (tfm_secure_lock != 0)) {
            /* Invalid request during system initialization */
            ERROR_MSG("Invalid service request during initialization!");
            return TFM_ERROR_NOT_INITIALIZED;
        }
    }

    return TFM_SUCCESS;
}

void tfm_spm_secure_api_init_done(void)
{
    tfm_secure_api_initializing = 0;
}

enum tfm_status_e tfm_spm_sfn_request_handler(
                             struct tfm_sfn_req_s *desc_ptr, uint32_t excReturn)
{
    enum tfm_status_e res;

    res = tfm_check_sfn_req_integrity(desc_ptr);
    if (res != TFM_SUCCESS) {
        ERROR_MSG("Invalid service request!");
        tfm_secure_api_error_handler();
    }

    __disable_irq();

    desc_ptr->caller_part_idx = tfm_spm_partition_get_running_partition_idx();

    res = tfm_core_check_sfn_parameters(desc_ptr);
    if (res != TFM_SUCCESS) {
        /* The sanity check of iovecs failed. */
        __enable_irq();
        tfm_secure_api_error_handler();
    }

    res = tfm_core_check_sfn_req_rules(desc_ptr);
    if (res != TFM_SUCCESS) {
        /* FixMe: error compartmentalization TBD */
        tfm_spm_partition_set_state(
            desc_ptr->caller_part_idx, SPM_PARTITION_STATE_CLOSED);
        __enable_irq();
        ERROR_MSG("Unauthorized service request!");
        tfm_secure_api_error_handler();
    }

    res = tfm_start_partition(desc_ptr, excReturn);
    if (res != TFM_SUCCESS) {
        /* FixMe: consider possible fault scenarios */
        __enable_irq();
        ERROR_MSG("Failed to process service request!");
        tfm_secure_api_error_handler();
    }

    __enable_irq();

    return res;
}

int32_t tfm_spm_sfn_request_thread_mode(struct tfm_sfn_req_s *desc_ptr)
{
    enum tfm_status_e res;
    int32_t *args;
    int32_t retVal;

    res = tfm_core_check_sfn_parameters(desc_ptr);
    if (res != TFM_SUCCESS) {
        /* The sanity check of iovecs failed. */
        return (int32_t)res;
    }

    /* No excReturn value is needed as no exception handling is used */
    res = tfm_spm_sfn_request_handler(desc_ptr, 0);

    if (res != TFM_SUCCESS) {
        tfm_secure_api_error_handler();
    }

    /* Secure partition to secure partition call in TFM level 1 */
    args = desc_ptr->args;
    retVal = desc_ptr->sfn(args[0], args[1], args[2], args[3]);

    /* return handler should restore original exc_return value... */
    res = tfm_return_from_partition(NULL);
    if (res == TFM_SUCCESS) {
        /* If unlock successful, pass SS return value to caller */
        return retVal;
    } else {
        /* Unlock errors indicate ctx database corruption or unknown
         * anomalies. Halt execution
         */
        ERROR_MSG("Secure API error during unlock!");
        tfm_secure_api_error_handler();
    }
    return (int32_t)res;
}

void tfm_spm_validate_secure_caller_handler(uint32_t *svc_args)
{

    enum tfm_status_e res = TFM_ERROR_GENERIC;
    uint32_t running_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const struct spm_partition_runtime_data_t *curr_part_data =
            tfm_spm_partition_get_runtime_data(running_partition_idx);
    uint32_t running_partition_flags =
            tfm_spm_partition_get_flags(running_partition_idx);
    uint32_t caller_partition_flags =
            tfm_spm_partition_get_flags(curr_part_data->caller_partition_idx);

    if (!(running_partition_flags & SPM_PART_FLAG_APP_ROT) ||
        curr_part_data->partition_state == SPM_PARTITION_STATE_HANDLING_IRQ ||
        curr_part_data->partition_state == SPM_PARTITION_STATE_SUSPENDED) {
        /* This handler shouldn't be called from outside partition context.
         * Also if the current partition is handling IRQ, the caller partition
         * index might not be valid;
         * Partitions are only allowed to run while S domain is locked.
         */
        svc_args[0] = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    /* Store return value in r0 */
    if (caller_partition_flags & SPM_PART_FLAG_APP_ROT) {
        res = TFM_SUCCESS;
    }
    svc_args[0] = (uint32_t)res;
}

int32_t tfm_spm_check_buffer_access(uint32_t  partition_idx,
                                    void     *start_addr,
                                    size_t    len,
                                    uint32_t  alignment)
{
    uintptr_t start_addr_value = (uintptr_t)start_addr;
    uintptr_t end_addr_value = (uintptr_t)start_addr + len;
    uintptr_t alignment_mask;

    alignment_mask = (((uintptr_t)1) << alignment) - 1;

    /* Check that the pointer is aligned properly */
    if (start_addr_value & alignment_mask) {
        /* not aligned, return error */
        return 0;
    }

    /* Protect against overflow (and zero len) */
    if (end_addr_value <= start_addr_value) {
        return 0;
    }

    /* For privileged partition execution, all secure data memory and stack
     * is accessible
     */

    if ((start_addr_value >= S_DATA_START &&
        end_addr_value <= (S_DATA_START + S_DATA_SIZE)) ||
        (start_addr_value >= TCM_TFM_BASE &&
        end_addr_value <= (TCM_TFM_BASE + TCM_TFM_LENGTH)) ) {
        return 1;
    }

    return 0;
}

void tfm_spm_get_caller_client_id_handler(uint32_t *svc_args)
{
    uintptr_t result_ptr_value = svc_args[0];
    uint32_t running_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
    const uint32_t running_partition_flags =
            tfm_spm_partition_get_flags(running_partition_idx);
    const struct spm_partition_runtime_data_t *curr_part_data =
            tfm_spm_partition_get_runtime_data(running_partition_idx);
    int res = 0;

    if (!(running_partition_flags & SPM_PART_FLAG_APP_ROT) ||
        curr_part_data->partition_state == SPM_PARTITION_STATE_HANDLING_IRQ ||
        curr_part_data->partition_state == SPM_PARTITION_STATE_SUSPENDED) {
        /* This handler shouldn't be called from outside partition context.
         * Also if the current partition is handling IRQ, the caller partition
         * index might not be valid;
         * Partitions are only allowed to run while S domain is locked.
         */
        svc_args[0] = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    /* Make sure that the output pointer points to a memory area that is owned
     * by the partition
     */
    res = tfm_spm_check_buffer_access(running_partition_idx,
                                      (void *)result_ptr_value,
                                      sizeof(curr_part_data->caller_client_id),
                                      2);
    if (!res) {
        /* Not in accessible range, return error */
        svc_args[0] = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
        return;
    }

    *((int32_t *)result_ptr_value) = curr_part_data->caller_client_id;

    /* Store return value in r0 */
    svc_args[0] = (uint32_t)TFM_SUCCESS;
}

/* This SVC handler is called if veneer is running in thread mode */
uint32_t tfm_spm_partition_request_svc_handler(
        const uint32_t *svc_ctx, uint32_t excReturn)
{
    struct tfm_sfn_req_s *desc_ptr;

    if (!(excReturn & EXC_RETURN_STACK_PROCESS)) {
        /* Service request SVC called with MSP active.
         * Either invalid configuration for Thread mode or SVC called
         * from Handler mode, which is not supported.
         * FixMe: error severity TBD
         */
        ERROR_MSG("Service request SVC called with MSP active!");
        tfm_secure_api_error_handler();
    }

    desc_ptr = (struct tfm_sfn_req_s *)svc_ctx[0];

    if (tfm_spm_sfn_request_handler(desc_ptr, excReturn) != TFM_SUCCESS) {
        tfm_secure_api_error_handler();
    }

    return EXC_RETURN_SECURE_FUNCTION;
}

/* This SVC handler is called, if a thread mode execution environment is to
 * be set up, to run an unprivileged IRQ handler
 */
uint32_t tfm_spm_depriv_req_handler(uint32_t *svc_args, uint32_t excReturn)
{
    struct tfm_state_context_t *svc_ctx =
                                        (struct tfm_state_context_t *)svc_args;

    enum tfm_status_e res;

    if (excReturn & EXC_RETURN_STACK_PROCESS) {
        /* FixMe: error severity TBD */
        ERROR_MSG("Partition request SVC called with PSP active!");
        tfm_secure_api_error_handler();
    }

    res = tfm_start_partition_for_irq_handling(excReturn, svc_ctx);
    if (res != TFM_SUCCESS) {
        /* The partition is in an invalid state (UNINIT or CLOSED), so none of
         * its code can be run
         */
        /* FixMe: For now this case is handled with TF-M panic, however it would
         * be possible to skip the execution of the interrupt handler, and
         * resume the execution of the interrupted code.
         */
        tfm_secure_api_error_handler();
    }
    return EXC_RETURN_SECURE_FUNCTION;
}

/* This SVC handler is called when sfn returns */
uint32_t tfm_spm_partition_return_handler(uint32_t lr)
{
    enum tfm_status_e res;

    if (!(lr & EXC_RETURN_STACK_PROCESS)) {
        /* Partition return SVC called with MSP active.
         * This should not happen!
         */
        ERROR_MSG("Partition return SVC called with MSP active!");
        tfm_secure_api_error_handler();
    }

    res = tfm_return_from_partition(&lr);
    if (res != TFM_SUCCESS) {
        /* Unlock errors indicate ctx database corruption or unknown anomalies
         * Halt execution
         */
        ERROR_MSG("Secure API error during unlock!");
        tfm_secure_api_error_handler();
    }

    return lr;
}

/* This SVC handler is called if a deprivileged IRQ handler was executed, and
 * the execution environment is to be set back for the privileged handler mode
 */
uint32_t tfm_spm_depriv_return_handler(uint32_t *irq_svc_args, uint32_t lr)
{
    enum tfm_status_e res;
    struct tfm_state_context_t *irq_svc_ctx =
                                    (struct tfm_state_context_t *)irq_svc_args;

    if (!(lr & EXC_RETURN_STACK_PROCESS)) {
        /* Partition request SVC called with MSP active.
         * FixMe: error severity TBD
         */
        ERROR_MSG("Partition request SVC called with MSP active!");
        tfm_secure_api_error_handler();
    }

    res = tfm_return_from_partition_irq_handling(&lr);
    if (res != TFM_SUCCESS) {
        /* Unlock errors indicate ctx database corruption or unknown anomalies
         * Halt execution
         */
        ERROR_MSG("Secure API error during unlock!");
        tfm_secure_api_error_handler();
    }

    irq_svc_ctx->ra = lr;

    return EXC_RETURN_SECURE_HANDLER;
}

/* FIXME: get_irq_line_for_signal is also implemented in the ipc folder. */
/**
 * \brief Return the IRQ line number associated with a signal
 *
 * \param[in] partition_id    The ID of the partition in which we look for the
 *                            signal
 * \param[in] signal          The signal we do the query for
 *
 * \retval >=0     The IRQ line number associated with a signal in the partition
 * \retval <0      error
 */
static IRQn_Type get_irq_line_for_signal(int32_t partition_id,
                                       psa_signal_t signal)
{
    size_t i;

    for (i = 0; i < tfm_core_irq_signals_count; ++i) {
        if (tfm_core_irq_signals[i].partition_id == partition_id &&
            tfm_core_irq_signals[i].signal_value == signal) {
            return tfm_core_irq_signals[i].irq_line;
        }
    }
    return (IRQn_Type) -1;
}

void tfm_spm_enable_irq_handler(uint32_t *svc_args)
{
    struct tfm_state_context_t *svc_ctx =
                                        (struct tfm_state_context_t *)svc_args;
    psa_signal_t irq_signal = svc_ctx->r0;
    uint32_t running_partition_idx =
                      tfm_spm_partition_get_running_partition_idx();
    uint32_t running_partition_id =
                      tfm_spm_partition_get_partition_id(running_partition_idx);
    IRQn_Type irq_line;

    /* Only a single signal is allowed */
    if (!tfm_is_one_bit_set(irq_signal)) {
        /* FixMe: error severity TBD */
        tfm_secure_api_error_handler();
    }

    irq_line = get_irq_line_for_signal(running_partition_id, irq_signal);

    if (irq_line < 0) {
        /* FixMe: error severity TBD */
        tfm_secure_api_error_handler();
    }

    tfm_spm_hal_enable_irq(irq_line);
}

void tfm_spm_disable_irq_handler(uint32_t *svc_args)
{
    struct tfm_state_context_t *svc_ctx =
                                        (struct tfm_state_context_t *)svc_args;
    psa_signal_t irq_signal = svc_ctx->r0;
    uint32_t running_partition_idx =
                      tfm_spm_partition_get_running_partition_idx();
    uint32_t running_partition_id =
                      tfm_spm_partition_get_partition_id(running_partition_idx);
    IRQn_Type irq_line;

    /* Only a single signal is allowed */
    if (!tfm_is_one_bit_set(irq_signal)) {
        /* FixMe: error severity TBD */
        tfm_secure_api_error_handler();
    }

    irq_line = get_irq_line_for_signal(running_partition_id, irq_signal);

    if (irq_line < 0) {
        /* FixMe: error severity TBD */
        tfm_secure_api_error_handler();
    }

    tfm_spm_hal_disable_irq(irq_line);
}

void tfm_spm_psa_wait(uint32_t *svc_args)
{
    /* Look for partition that is ready for run */
    struct tfm_state_context_t *svc_ctx =
                                        (struct tfm_state_context_t *)svc_args;
    uint32_t running_partition_idx;
    const struct spm_partition_runtime_data_t *curr_part_data;

    psa_signal_t signal_mask = svc_ctx->r0;
    uint32_t timeout = svc_ctx->r1;

    /*
     * Timeout[30:0] are reserved for future use.
     * SPM must ignore the value of RES.
     */
    timeout &= PSA_TIMEOUT_MASK;

    running_partition_idx = tfm_spm_partition_get_running_partition_idx();
    curr_part_data = tfm_spm_partition_get_runtime_data(running_partition_idx);

    if (timeout == PSA_BLOCK) {
        /* FIXME: Scheduling is not available in library model, and busy wait is
         * also not possible as this code is running in SVC context, and it
         * cannot be pre-empted by interrupts. So do nothing here for now
         */
        (void) signal_mask;
    }

    svc_ctx->r0 = curr_part_data->signal_mask;
}

void tfm_spm_psa_eoi(uint32_t *svc_args)
{
    struct tfm_state_context_t *svc_ctx =
                                        (struct tfm_state_context_t *)svc_args;
    psa_signal_t irq_signal = svc_ctx->r0;
    uint32_t signal_mask;
    uint32_t running_partition_idx;
    uint32_t running_partition_id;
    const struct spm_partition_runtime_data_t *curr_part_data;
    IRQn_Type irq_line;

    running_partition_idx = tfm_spm_partition_get_running_partition_idx();
    running_partition_id =
                      tfm_spm_partition_get_partition_id(running_partition_idx);
    curr_part_data = tfm_spm_partition_get_runtime_data(running_partition_idx);

    /* Only a single signal is allowed */
    if (!tfm_is_one_bit_set(irq_signal)) {
        tfm_secure_api_error_handler();
    }

    irq_line = get_irq_line_for_signal(running_partition_id, irq_signal);

    if (irq_line < 0) {
        /* FixMe: error severity TBD */
        tfm_secure_api_error_handler();
    }

    tfm_spm_hal_clear_pending_irq(irq_line);
    tfm_spm_hal_enable_irq(irq_line);

    signal_mask = curr_part_data->signal_mask & ~irq_signal;
    tfm_spm_partition_set_signal_mask(running_partition_idx, signal_mask);
}

/*
 * This function is called when a secure partition causes an error.
 * In case of an error in the error handling, a non-zero value have to be
 * returned.
 */
static void tfm_spm_partition_err_handler(
    const struct spm_partition_desc_t *partition,
    int32_t err_code)
{
    (void)err_code;

    tfm_spm_partition_set_state(partition->static_data->partition_id,
                                SPM_PARTITION_STATE_CLOSED);
}

enum spm_err_t tfm_spm_partition_init(void)
{
    struct spm_partition_desc_t *part;
    struct tfm_sfn_req_s desc;
    int32_t args[4] = {0};
    int32_t fail_cnt = 0;
    uint32_t idx;
    const struct tfm_spm_partition_platform_data_t **platform_data_p;

    /* Call the init function for each partition */
    for (idx = 0; idx < g_spm_partition_db.partition_count; ++idx) {
        part = &g_spm_partition_db.partitions[idx];
        platform_data_p = part->platform_data_list;
        if (platform_data_p != NULL) {
            while ((*platform_data_p) != NULL) {
                if (tfm_spm_hal_configure_default_isolation(idx,
                            *platform_data_p) != TFM_PLAT_ERR_SUCCESS) {
                    fail_cnt++;
                }
                ++platform_data_p;
            }
        }
        if (part->static_data->partition_init == NULL) {
            tfm_spm_partition_set_state(idx, SPM_PARTITION_STATE_IDLE);
            tfm_spm_partition_set_caller_partition_idx(idx,
                                                    SPM_INVALID_PARTITION_IDX);
        } else {
            int32_t res;

            desc.args = args;
            desc.ns_caller = false;
            desc.sfn = (sfn_t)part->static_data->partition_init;
            desc.sp_id = part->static_data->partition_id;
            res = tfm_core_sfn_request(&desc);
            if (res == TFM_SUCCESS) {
                tfm_spm_partition_set_state(idx, SPM_PARTITION_STATE_IDLE);
            } else {
                tfm_spm_partition_err_handler(part, res);
                fail_cnt++;
            }
        }
    }

    tfm_spm_secure_api_init_done();

    if (fail_cnt == 0) {
        return SPM_ERR_OK;
    } else {
        return SPM_ERR_PARTITION_NOT_AVAILABLE;
    }
}

void tfm_spm_partition_push_interrupted_ctx(uint32_t partition_idx)
{
    struct spm_partition_runtime_data_t *runtime_data =
        &g_spm_partition_db.partitions[partition_idx].runtime_data;
    struct interrupted_ctx_stack_frame_t *stack_frame =
        (struct interrupted_ctx_stack_frame_t *)runtime_data->ctx_stack_ptr;

    stack_frame->partition_state = runtime_data->partition_state;

    runtime_data->ctx_stack_ptr +=
        sizeof(struct interrupted_ctx_stack_frame_t) / sizeof(uint32_t);
}

void tfm_spm_partition_pop_interrupted_ctx(uint32_t partition_idx)
{
    struct spm_partition_runtime_data_t *runtime_data =
        &g_spm_partition_db.partitions[partition_idx].runtime_data;
    struct interrupted_ctx_stack_frame_t *stack_frame;

    runtime_data->ctx_stack_ptr -=
        sizeof(struct interrupted_ctx_stack_frame_t) / sizeof(uint32_t);

    stack_frame = (struct interrupted_ctx_stack_frame_t *)
                      runtime_data->ctx_stack_ptr;
    tfm_spm_partition_set_state(partition_idx, stack_frame->partition_state);
    stack_frame->partition_state = 0;
}

void tfm_spm_partition_push_handler_ctx(uint32_t partition_idx)
{
    struct spm_partition_runtime_data_t *runtime_data =
        &g_spm_partition_db.partitions[partition_idx].runtime_data;
    struct handler_ctx_stack_frame_t *stack_frame =
        (struct handler_ctx_stack_frame_t *)
            runtime_data->ctx_stack_ptr;

    stack_frame->partition_state = runtime_data->partition_state;
    stack_frame->caller_partition_idx = runtime_data->caller_partition_idx;

    runtime_data->ctx_stack_ptr +=
        sizeof(struct handler_ctx_stack_frame_t) / sizeof(uint32_t);
}

void tfm_spm_partition_pop_handler_ctx(uint32_t partition_idx)
{
    struct spm_partition_runtime_data_t *runtime_data =
        &g_spm_partition_db.partitions[partition_idx].runtime_data;
    struct handler_ctx_stack_frame_t *stack_frame;

    runtime_data->ctx_stack_ptr -=
        sizeof(struct handler_ctx_stack_frame_t) / sizeof(uint32_t);

    stack_frame = (struct handler_ctx_stack_frame_t *)
                      runtime_data->ctx_stack_ptr;

    tfm_spm_partition_set_state(partition_idx, stack_frame->partition_state);
    stack_frame->partition_state = 0;
    tfm_spm_partition_set_caller_partition_idx(
        partition_idx, stack_frame->caller_partition_idx);
    stack_frame->caller_partition_idx = 0;
}

void tfm_spm_partition_store_context(uint32_t partition_idx,
        uint32_t stack_ptr, uint32_t lr)
{
    g_spm_partition_db.partitions[partition_idx].
            runtime_data.stack_ptr = stack_ptr;
    g_spm_partition_db.partitions[partition_idx].
            runtime_data.lr = lr;
}

const struct spm_partition_runtime_data_t *
            tfm_spm_partition_get_runtime_data(uint32_t partition_idx)
{
    return &(g_spm_partition_db.partitions[partition_idx].runtime_data);
}

void tfm_spm_partition_set_state(uint32_t partition_idx, uint32_t state)
{
    g_spm_partition_db.partitions[partition_idx].runtime_data.partition_state =
            state;
    if (state == SPM_PARTITION_STATE_RUNNING ||
        state == SPM_PARTITION_STATE_HANDLING_IRQ) {
        g_spm_partition_db.running_partition_idx = partition_idx;
    }
}

void tfm_spm_partition_set_caller_partition_idx(uint32_t partition_idx,
                                                uint32_t caller_partition_idx)
{
    g_spm_partition_db.partitions[partition_idx].runtime_data.
            caller_partition_idx = caller_partition_idx;
}

void tfm_spm_partition_set_signal_mask(uint32_t partition_idx,
                                       uint32_t signal_mask)
{
    g_spm_partition_db.partitions[partition_idx].runtime_data.
            signal_mask = signal_mask;
}

void tfm_spm_partition_set_caller_client_id(uint32_t partition_idx,
                                            int32_t caller_client_id)
{
    g_spm_partition_db.partitions[partition_idx].runtime_data.
            caller_client_id = caller_client_id;
}

enum spm_err_t tfm_spm_partition_set_iovec(uint32_t partition_idx,
                                           const int32_t *args)
{
    struct spm_partition_runtime_data_t *runtime_data =
            &g_spm_partition_db.partitions[partition_idx].runtime_data;
    size_t i;

    if ((args[1] < 0) || (args[3] < 0)) {
        return SPM_ERR_INVALID_PARAMETER;
    }

    runtime_data->iovec_args.in_len = (size_t)args[1];
    for (i = 0U; i < runtime_data->iovec_args.in_len; ++i) {
        runtime_data->iovec_args.in_vec[i].base =
                                                 ((psa_invec *)args[0])[i].base;
        runtime_data->iovec_args.in_vec[i].len = ((psa_invec *)args[0])[i].len;
    }
    runtime_data->iovec_args.out_len = (size_t)args[3];
    for (i = 0U; i < runtime_data->iovec_args.out_len; ++i) {
        runtime_data->iovec_args.out_vec[i].base =
                                                ((psa_outvec *)args[2])[i].base;
        runtime_data->iovec_args.out_vec[i].len =
                                                 ((psa_outvec *)args[2])[i].len;
    }
    runtime_data->orig_outvec = (psa_outvec *)args[2];

    return SPM_ERR_OK;
}

uint32_t tfm_spm_partition_get_running_partition_idx(void)
{
    return g_spm_partition_db.running_partition_idx;
}

void tfm_spm_partition_cleanup_context(uint32_t partition_idx)
{
    struct spm_partition_desc_t *partition =
            &(g_spm_partition_db.partitions[partition_idx]);
    int32_t i;

    partition->runtime_data.caller_partition_idx = SPM_INVALID_PARTITION_IDX;
    partition->runtime_data.iovec_args.in_len = 0;
    for (i = 0; i < PSA_MAX_IOVEC; ++i) {
        partition->runtime_data.iovec_args.in_vec[i].base = 0;
        partition->runtime_data.iovec_args.in_vec[i].len = 0;
    }
    partition->runtime_data.iovec_args.out_len = 0;
    for (i = 0; i < PSA_MAX_IOVEC; ++i) {
        partition->runtime_data.iovec_args.out_vec[i].base = 0;
        partition->runtime_data.iovec_args.out_vec[i].len = 0;
    }
    partition->runtime_data.orig_outvec = 0;
}

void tfm_spm_request_handler(const struct tfm_state_context_t *svc_ctx)
{
    uint32_t *res_ptr = (uint32_t *)&svc_ctx->r0;
    uint32_t running_partition_flags = 0;
    uint32_t running_partition_idx;

    /* Check permissions on request type basis */

    switch (svc_ctx->r0) {
    case TFM_SPM_REQUEST_RESET_VOTE:
        running_partition_idx =
            tfm_spm_partition_get_running_partition_idx();
        running_partition_flags = tfm_spm_partition_get_flags(
                                                         running_partition_idx);

        /* Currently only PSA Root of Trust services are allowed to make Reset
         * vote request
         */
        if ((running_partition_flags & SPM_PART_FLAG_PSA_ROT) == 0) {
            *res_ptr = (uint32_t)TFM_ERROR_GENERIC;
        }

        /* FixMe: this is a placeholder for checks to be performed before
         * allowing execution of reset
         */
        *res_ptr = (uint32_t)TFM_SUCCESS;

        break;
    default:
        *res_ptr = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
    }
}
