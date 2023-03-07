/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis_compiler.h"

#include "psa/error.h"
#include "tfm_core_utils.h"
#include "tfm_utils.h"
#include "tfm_spe_mailbox.h"
#include "tfm_rpc.h"

#define NS_CALLER_FLAG          (true)

static struct secure_mailbox_queue_t spe_mailbox_queue;

static int32_t tfm_mailbox_dispatch(uint32_t call_type,
                                    const struct psa_client_params_t *params,
                                    int32_t client_id, int32_t *psa_ret)
{
    struct client_call_params_t spm_params = {0};

    TFM_CORE_ASSERT(params != NULL);
    TFM_CORE_ASSERT(psa_ret != NULL);

    (void)client_id;

    switch (call_type) {
    case MAILBOX_PSA_FRAMEWORK_VERSION:
        *psa_ret = tfm_rpc_psa_framework_version();
        return MAILBOX_SUCCESS;
    case MAILBOX_PSA_VERSION:
        spm_params.sid = params->psa_version_params.sid;
        *psa_ret = tfm_rpc_psa_version(&spm_params, NS_CALLER_FLAG);
        return MAILBOX_SUCCESS;
    case MAILBOX_PSA_CONNECT:
        spm_params.sid = params->psa_connect_params.sid;
        spm_params.version = params->psa_connect_params.version;
        *psa_ret = (uint32_t)tfm_rpc_psa_connect(&spm_params, NS_CALLER_FLAG);
        return MAILBOX_SUCCESS;
    case MAILBOX_PSA_CALL:
        spm_params.handle = params->psa_call_params.handle;
        spm_params.type = params->psa_call_params.type;
        spm_params.in_vec = params->psa_call_params.in_vec;
        spm_params.in_len = params->psa_call_params.in_len;
        spm_params.out_vec = params->psa_call_params.out_vec;
        spm_params.out_len = params->psa_call_params.out_len;
        *psa_ret = (uint32_t)tfm_rpc_psa_call(&spm_params, NS_CALLER_FLAG);
        return MAILBOX_SUCCESS;
    case MAILBOX_PSA_CLOSE:
        spm_params.handle = params->psa_close_params.handle;
        tfm_rpc_psa_close(&spm_params, NS_CALLER_FLAG);
        return MAILBOX_SUCCESS;
    default:
        return MAILBOX_INVAL_PARAMS;
    }
}

__STATIC_INLINE void set_spe_queue_empty_status(uint8_t idx)
{
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        spe_mailbox_queue.empty_slots |= (1 << idx);
    }
}

__STATIC_INLINE void clear_spe_queue_empty_status(uint8_t idx)
{
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        spe_mailbox_queue.empty_slots &= ~(1 << idx);
    }
}

__STATIC_INLINE bool get_spe_queue_empty_status(uint8_t idx)
{
    if ((idx < NUM_MAILBOX_QUEUE_SLOT) &&
        (spe_mailbox_queue.empty_slots & (1 << idx))) {
        return true;
    }

    return false;
}

__STATIC_INLINE mailbox_queue_status_t get_nspe_queue_pend_status(
                                    const struct ns_mailbox_queue_t *ns_queue)
{
    return ns_queue->pend_slots;
}

__STATIC_INLINE void set_nspe_queue_replied_status(
                                            struct ns_mailbox_queue_t *ns_queue,
                                            mailbox_queue_status_t mask)
{
    ns_queue->replied_slots |= mask;
}

__STATIC_INLINE void clear_nspe_queue_pend_status(
                                            struct ns_mailbox_queue_t *ns_queue,
                                            mailbox_queue_status_t mask)
{
    ns_queue->pend_slots &= ~mask;
}

__STATIC_INLINE int32_t get_spe_mailbox_msg_handle(uint8_t idx,
                                                   mailbox_msg_handle_t *handle)
{
    if ((idx >= NUM_MAILBOX_QUEUE_SLOT) || !handle) {
        return MAILBOX_INVAL_PARAMS;
    }

    *handle = (mailbox_msg_handle_t)(idx + 1);

    return MAILBOX_SUCCESS;
}

__STATIC_INLINE int32_t get_spe_mailbox_msg_idx(mailbox_msg_handle_t handle,
                                                uint8_t *idx)
{
    if ((handle == MAILBOX_MSG_NULL_HANDLE) || !idx) {
        return MAILBOX_INVAL_PARAMS;
    }

    *idx = (uint8_t)(handle - 1);

    return MAILBOX_SUCCESS;
}

static void mailbox_clean_queue_slot(uint8_t idx)
{
    if (idx >= NUM_MAILBOX_QUEUE_SLOT) {
        return;
    }

    tfm_core_util_memset(&spe_mailbox_queue.queue[idx], 0,
                         sizeof(spe_mailbox_queue.queue[idx]));
    set_spe_queue_empty_status(idx);
}

__STATIC_INLINE struct mailbox_reply_t *get_nspe_reply_addr(uint8_t idx)
{
    uint8_t ns_slot_idx;

    if (idx >= NUM_MAILBOX_QUEUE_SLOT) {
        return NULL;
    }

    ns_slot_idx = spe_mailbox_queue.queue[idx].ns_slot_idx;

    return &spe_mailbox_queue.ns_queue->queue[ns_slot_idx].reply;
}

static void mailbox_direct_reply(uint8_t idx, uint32_t result)
{
    struct mailbox_reply_t *reply_ptr;
    uint32_t ret_result = result;

    /* Get reply address */
    reply_ptr = get_nspe_reply_addr(idx);
    tfm_core_util_memcpy(&reply_ptr->return_val, &ret_result,
                         sizeof(reply_ptr->return_val));

    mailbox_clean_queue_slot(idx);

    /*
     * Skip NSPE queue status update after single reply.
     * Update NSPE queue status after all the mailbox messages are completed
     */
}

__STATIC_INLINE int32_t check_mailbox_msg(const struct mailbox_msg_t *msg)
{
    /*
     * TODO
     * Comprehensive check of mailbox msessage content can be implemented here.
     */
    (void)msg;
    return MAILBOX_SUCCESS;
}

int32_t tfm_mailbox_handle_msg(void)
{
    uint8_t idx;
    int32_t result;
    int32_t psa_ret = PSA_ERROR_GENERIC_ERROR;
    mailbox_queue_status_t mask_bits, pend_slots, reply_slots = 0;
    struct ns_mailbox_queue_t *ns_queue = spe_mailbox_queue.ns_queue;
    struct mailbox_msg_t *msg_ptr;

    TFM_CORE_ASSERT(ns_queue != NULL);

    tfm_mailbox_hal_enter_critical();

    /* Check if NSPE mailbox did assert a PSA client call request */
    if (!ns_queue->pend_slots) {
        tfm_mailbox_hal_exit_critical();
        return MAILBOX_NO_PEND_EVENT;
    }

    pend_slots = get_nspe_queue_pend_status(ns_queue);

    tfm_mailbox_hal_exit_critical();

    for (idx = 0; idx < NUM_MAILBOX_QUEUE_SLOT; idx++) {
        mask_bits = (1 << idx);
        /* Check if current NSPE mailbox queue slot is pending for handling */
        if (!(pend_slots & mask_bits)) {
            continue;
        }

        /*
         * TODO
         * The operations are simplified here. Use the SPE mailbox queue
         * slot with the same idx as that of the NSPE mailbox queue slot.
         * A more general implementation should dynamically search and
         * select an empty SPE mailbox queue slot.
         */
        clear_spe_queue_empty_status(idx);
        spe_mailbox_queue.queue[idx].ns_slot_idx = idx;

        msg_ptr = &spe_mailbox_queue.queue[idx].msg;
        tfm_core_util_memcpy(msg_ptr, &ns_queue->queue[idx].msg,
                             sizeof(*msg_ptr));

        if (check_mailbox_msg(msg_ptr) != MAILBOX_SUCCESS) {
            mailbox_clean_queue_slot(idx);
            continue;
        }

        get_spe_mailbox_msg_handle(idx,
                                   &spe_mailbox_queue.queue[idx].msg_handle);

        /*
         * Set the current slot index under processing.
         * The value is used in mailbox_get_caller_data() to identify the
         * mailbox queue slot.
         */
        spe_mailbox_queue.cur_proc_slot_idx = idx;

        result = tfm_mailbox_dispatch(msg_ptr->call_type, &msg_ptr->params,
                                      msg_ptr->client_id, &psa_ret);
        if (result != MAILBOX_SUCCESS) {
            mailbox_clean_queue_slot(idx);
            continue;
        }

        /* Clean up the current slot index under processing */
        spe_mailbox_queue.cur_proc_slot_idx = NUM_MAILBOX_QUEUE_SLOT;

        if ((msg_ptr->call_type == MAILBOX_PSA_FRAMEWORK_VERSION) ||
            (msg_ptr->call_type == MAILBOX_PSA_VERSION)) {
            /*
             * Directly write the result to NSPE for psa_framework_version() and
             * psa_version().
             */
            reply_slots |= (1 << idx);

            mailbox_direct_reply(idx, psa_ret);
        } else if ((msg_ptr->call_type == MAILBOX_PSA_CONNECT) ||
                   (msg_ptr->call_type == MAILBOX_PSA_CALL)) {
            /*
             * If it failed to deliver psa_connect() or psa_call() request to
             * TF-M IPC SPM, the failure result should be returned immediately.
             */
            if (psa_ret != PSA_SUCCESS) {
                reply_slots |= (1 << idx);
                mailbox_direct_reply(idx, psa_ret);
            }
        }
        /*
         * Skip checking psa_call() since it neither returns immediately nor
         * has return value.
         */
    }

    tfm_mailbox_hal_enter_critical();

    /* Clean the NSPE mailbox pending status. */
    clear_nspe_queue_pend_status(ns_queue, pend_slots);

    /* Set the NSPE mailbox replied status */
    set_nspe_queue_replied_status(ns_queue, reply_slots);

    tfm_mailbox_hal_exit_critical();

    if (reply_slots) {
        tfm_mailbox_hal_notify_peer();
    }

    return MAILBOX_SUCCESS;
}

int32_t tfm_mailbox_reply_msg(mailbox_msg_handle_t handle, int32_t reply)
{
    uint8_t idx;
    int32_t ret;
    struct ns_mailbox_queue_t *ns_queue = spe_mailbox_queue.ns_queue;

    TFM_CORE_ASSERT(ns_queue != NULL);

    /*
     * If handle == MAILBOX_MSG_NULL_HANDLE, reply to the mailbox message
     * in the first slot.
     * When multiple ongoing PSA client calls from NSPE are supported,
     * additional check might be necessary to avoid spoofing the first slot.
     */
    if (handle == MAILBOX_MSG_NULL_HANDLE) {
        idx = 0;
    } else {
        ret = get_spe_mailbox_msg_idx(handle, &idx);
        if (ret != MAILBOX_SUCCESS) {
            return ret;
        }
    }

    if (get_spe_queue_empty_status(idx)) {
        return MAILBOX_NO_PEND_EVENT;
    }

    mailbox_direct_reply(idx, (uint32_t)reply);

    tfm_mailbox_hal_enter_critical();

    /* Set the NSPE mailbox replied status */
    set_nspe_queue_replied_status(ns_queue, (1 << idx));

    tfm_mailbox_hal_exit_critical();

    tfm_mailbox_hal_notify_peer();

    return MAILBOX_SUCCESS;
}

/* RPC handle_req() callback */
static void mailbox_handle_req(void)
{
    (void)tfm_mailbox_handle_msg();
}

/* RPC reply() callback */
static void mailbox_reply(const void *owner, int32_t ret)
{
    mailbox_msg_handle_t handle = MAILBOX_MSG_NULL_HANDLE;

    /* If the owner is specified */
    if (owner) {
        handle = *((mailbox_msg_handle_t *)owner);
    }

    (void)tfm_mailbox_reply_msg(handle, ret);
}

/* RPC get_caller_data() callback */
static const void *mailbox_get_caller_data(int32_t client_id)
{
    uint8_t idx;

    (void)client_id;

    idx = spe_mailbox_queue.cur_proc_slot_idx;
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        return (const void *)&spe_mailbox_queue.queue[idx].msg_handle;
    }

    return NULL;
}

/* Mailbox specific operations callback for TF-M RPC */
static const struct tfm_rpc_ops_t mailbox_rpc_ops = {
    .handle_req = mailbox_handle_req,
    .reply      = mailbox_reply,
    .get_caller_data = mailbox_get_caller_data,
};

int32_t tfm_mailbox_init(void)
{
    int32_t ret;

    tfm_core_util_memset(&spe_mailbox_queue, 0, sizeof(spe_mailbox_queue));

    spe_mailbox_queue.empty_slots =
            (mailbox_queue_status_t)((1UL << (NUM_MAILBOX_QUEUE_SLOT - 1)) - 1);
    spe_mailbox_queue.empty_slots +=
            (mailbox_queue_status_t)(1UL << (NUM_MAILBOX_QUEUE_SLOT - 1));

    /* Register RPC callbacks */
    ret = tfm_rpc_register_ops(&mailbox_rpc_ops);
    if (ret != TFM_RPC_SUCCESS) {
        return MAILBOX_CALLBACK_REG_ERROR;
    }

    /*
     * Platform specific initialization.
     * Initialize Inter-Processor Communication and achieve the base address of
     * NSPE mailbox queue
     */
    ret = tfm_mailbox_hal_init(&spe_mailbox_queue);
    if (ret != MAILBOX_SUCCESS) {
        tfm_rpc_unregister_ops();

        return ret;
    }

    return MAILBOX_SUCCESS;
}
