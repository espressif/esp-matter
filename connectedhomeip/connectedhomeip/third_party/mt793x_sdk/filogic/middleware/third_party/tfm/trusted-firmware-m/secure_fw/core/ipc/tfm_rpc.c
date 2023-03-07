/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "spm_api.h"
#include "spm_psa_client_call.h"
#include "tfm_rpc.h"
#include "tfm_utils.h"

static void default_handle_req(void)
{
}

static void default_mailbox_reply(const void *owner, int32_t ret)
{
    (void)owner;
    (void)ret;
}

static const void *default_get_caller_data(int32_t client_id)
{
    (void)client_id;

    return NULL;
}

static struct tfm_rpc_ops_t rpc_ops = {
    .handle_req = default_handle_req,
    .reply      = default_mailbox_reply,
    .get_caller_data = default_get_caller_data,
};

uint32_t tfm_rpc_psa_framework_version(void)
{
    return tfm_spm_client_psa_framework_version();
}

uint32_t tfm_rpc_psa_version(const struct client_call_params_t *params,
                             bool ns_caller)
{
    TFM_CORE_ASSERT(params != NULL);

    return tfm_spm_client_psa_version(params->sid, ns_caller);
}

psa_status_t tfm_rpc_psa_connect(const struct client_call_params_t *params,
                                 bool ns_caller)
{
    TFM_CORE_ASSERT(params != NULL);

    return tfm_spm_client_psa_connect(params->sid, params->version, ns_caller);
}

psa_status_t tfm_rpc_psa_call(const struct client_call_params_t *params,
                              bool ns_caller)
{
    TFM_CORE_ASSERT(params != NULL);

    return tfm_spm_client_psa_call(params->handle, params->type,
                                   params->in_vec, params->in_len,
                                   params->out_vec, params->out_len, ns_caller,
                                   TFM_PARTITION_UNPRIVILEGED_MODE);
}

void tfm_rpc_psa_close(const struct client_call_params_t *params,
                       bool ns_caller)
{
    TFM_CORE_ASSERT(params != NULL);

    tfm_spm_client_psa_close(params->handle, ns_caller);
}

int32_t tfm_rpc_register_ops(const struct tfm_rpc_ops_t *ops_ptr)
{
    if (!ops_ptr) {
        return TFM_RPC_INVAL_PARAM;
    }

    if (!ops_ptr->handle_req || !ops_ptr->reply || !ops_ptr->get_caller_data) {
        return TFM_RPC_INVAL_PARAM;
    }

    /* Currently, one and only one mailbox implementation is supported. */
    if ((rpc_ops.handle_req != default_handle_req) ||
        (rpc_ops.reply != default_mailbox_reply) || \
        (rpc_ops.get_caller_data != default_get_caller_data)) {
        return TFM_RPC_CONFLICT_CALLBACK;
    }

    rpc_ops.handle_req = ops_ptr->handle_req;
    rpc_ops.reply = ops_ptr->reply;
    rpc_ops.get_caller_data = ops_ptr->get_caller_data;

    return TFM_RPC_SUCCESS;
}

void tfm_rpc_unregister_ops(void)
{
    rpc_ops.handle_req = default_handle_req;
    rpc_ops.reply = default_mailbox_reply;
    rpc_ops.get_caller_data = default_get_caller_data;
}

void tfm_rpc_client_call_handler(void)
{
    rpc_ops.handle_req();
}

void tfm_rpc_client_call_reply(const void *owner, int32_t ret)
{
    const struct tfm_msg_body_t *msg = (const struct tfm_msg_body_t *)owner;

    rpc_ops.reply(msg->caller_data, ret);
}

void tfm_rpc_set_caller_data(struct tfm_msg_body_t *msg, int32_t client_id)
{
    msg->caller_data = rpc_ops.get_caller_data(client_id);
}
