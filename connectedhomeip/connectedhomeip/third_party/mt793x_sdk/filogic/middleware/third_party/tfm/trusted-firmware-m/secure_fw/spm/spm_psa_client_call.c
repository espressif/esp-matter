/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/service.h"
#include "spm_api.h"
#include "tfm_core_utils.h"
#include "tfm_internal_defines.h"
#include "tfm_memory_utils.h"
#include "tfm_message_queue.h"
#include "spm_psa_client_call.h"
#include "tfm_utils.h"
#include "tfm_wait.h"
#include "tfm_nspm.h"

uint32_t tfm_spm_client_psa_framework_version(void)
{
    return PSA_FRAMEWORK_VERSION;
}

uint32_t tfm_spm_client_psa_version(uint32_t sid, bool ns_caller)
{
    struct tfm_spm_service_t *service;

    /*
     * It should return PSA_VERSION_NONE if the RoT Service is not
     * implemented.
     */
    service = tfm_spm_get_service_by_sid(sid);
    if (!service) {
        return PSA_VERSION_NONE;
    }

    /*
     * It should return PSA_VERSION_NONE if the caller is not authorized
     * to access the RoT Service.
     */
    if (tfm_spm_check_authorization(sid, service, ns_caller) != IPC_SUCCESS) {
        return PSA_VERSION_NONE;
    }

    return service->service_db->version;
}

psa_status_t tfm_spm_client_psa_connect(uint32_t sid, uint32_t version,
                                        bool ns_caller)
{
    struct tfm_spm_service_t *service;
    struct tfm_msg_body_t *msg;
    psa_handle_t connect_handle;
    int32_t client_id;

    /* It is a fatal error if the RoT Service does not exist on the platform */
    service = tfm_spm_get_service_by_sid(sid);
    if (!service) {
        tfm_core_panic();
    }

    if (ns_caller) {
        client_id = tfm_nspm_get_current_client_id();
    } else {
        client_id = tfm_spm_partition_get_running_partition_id();
    }

    /*
     * It is a fatal error if the caller is not authorized to access the RoT
     * Service.
     */
    if (tfm_spm_check_authorization(sid, service, ns_caller) != IPC_SUCCESS) {
        tfm_core_panic();
    }

    /*
     * Create connection handle here since it is possible to return the error
     * code to client when creation fails.
     */
    connect_handle = tfm_spm_create_conn_handle(service, client_id);
    if (connect_handle == PSA_NULL_HANDLE) {
        return PSA_ERROR_CONNECTION_BUSY;
    }

    /*
     * It is a fatal error if the version of the RoT Service requested is not
     * supported on the platform.
     */
    if (tfm_spm_check_client_version(service, version) != IPC_SUCCESS) {
        tfm_core_panic();
    }

    msg = tfm_spm_get_msg_buffer_from_conn_handle(connect_handle);
    if (!msg) {
        /* Have no enough resource to create message */
        return PSA_ERROR_CONNECTION_BUSY;
    }

    /* No input or output needed for connect message */
    tfm_spm_fill_msg(msg, service, connect_handle, PSA_IPC_CONNECT,
                     client_id, NULL, 0, NULL, 0, NULL);

    /*
     * Send message and wake up the SP who is waiting on message queue,
     * and scheduler triggered
     */
    tfm_spm_send_event(service, msg);

    return PSA_SUCCESS;
}

psa_status_t tfm_spm_client_psa_call(psa_handle_t handle, int32_t type,
                                     const psa_invec *inptr, size_t in_num,
                                     psa_outvec *outptr, size_t out_num,
                                     bool ns_caller, uint32_t privileged)
{
    psa_invec invecs[PSA_MAX_IOVEC];
    psa_outvec outvecs[PSA_MAX_IOVEC];
    struct tfm_spm_service_t *service;
    struct tfm_msg_body_t *msg;
    int i, j;
    int32_t client_id;

    /* It is a fatal error if in_len + out_len > PSA_MAX_IOVEC. */
    if ((in_num > PSA_MAX_IOVEC) ||
        (out_num > PSA_MAX_IOVEC) ||
        (in_num + out_num > PSA_MAX_IOVEC)) {
        tfm_core_panic();
    }

    if (ns_caller) {
        client_id = tfm_nspm_get_current_client_id();
    } else {
        client_id = tfm_spm_partition_get_running_partition_id();
    }

    /* It is a fatal error if an invalid handle was passed. */
    if (tfm_spm_validate_conn_handle(handle, client_id) != IPC_SUCCESS) {
        tfm_core_panic();
    }
    service = tfm_spm_get_service_by_handle(handle);
    if (!service) {
        /* FixMe: Need to implement one mechanism to resolve this failure. */
        tfm_core_panic();
    }

    /* It is a fatal error if the connection is currently handling a request. */
    if (((struct tfm_conn_handle_t *)handle)->status ==
                                                     TFM_HANDLE_STATUS_ACTIVE) {
        tfm_core_panic();
    }

    /*
     * Return PSA_ERROR_PROGRAMMER_ERROR immediately for the connection
     * has been terminated by the RoT Service.
     */
    if (((struct tfm_conn_handle_t *)handle)->status ==
                                              TFM_HANDLE_STATUS_CONNECT_ERROR) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /*
     * Read client invecs from the wrap input vector. It is a fatal error
     * if the memory reference for the wrap input vector is invalid or not
     * readable.
     */
    if (tfm_memory_check(inptr, in_num * sizeof(psa_invec), ns_caller,
        TFM_MEMORY_ACCESS_RO, privileged) != IPC_SUCCESS) {
        tfm_core_panic();
    }

    /*
     * Read client outvecs from the wrap output vector and will update the
     * actual length later. It is a fatal error if the memory reference for
     * the wrap output vector is invalid or not read-write.
     */
    if (tfm_memory_check(outptr, out_num * sizeof(psa_outvec), ns_caller,
        TFM_MEMORY_ACCESS_RW, privileged) != IPC_SUCCESS) {
        tfm_core_panic();
    }

    tfm_core_util_memset(invecs, 0, sizeof(invecs));
    tfm_core_util_memset(outvecs, 0, sizeof(outvecs));

    /* Copy the address out to avoid TOCTOU attacks. */
    tfm_core_util_memcpy(invecs, inptr, in_num * sizeof(psa_invec));
    tfm_core_util_memcpy(outvecs, outptr, out_num * sizeof(psa_outvec));

    /*
     * For client input vector, it is a fatal error if the provided payload
     * memory reference was invalid or not readable.
     */
    for (i = 0; i < in_num; i++) {
        if (tfm_memory_check(invecs[i].base, invecs[i].len, ns_caller,
            TFM_MEMORY_ACCESS_RO, privileged) != IPC_SUCCESS) {
            tfm_core_panic();
        }
    }

    /*
     * Clients must never overlap input parameters because of the risk of a
     * double-fetch inconsistency.
     * Overflow is checked in tfm_memory_check functions.
     */
    for (i = 0; i + 1 < in_num; i++) {
        for (j = i+1; j < in_num; j++) {
            if (!((char *) invecs[j].base + invecs[j].len <=
                  (char *) invecs[i].base ||
                  (char *) invecs[j].base >=
                  (char *) invecs[i].base + invecs[i].len)) {
                tfm_core_panic();
            }
        }
    }

    /*
     * For client output vector, it is a fatal error if the provided payload
     * memory reference was invalid or not read-write.
     */
    for (i = 0; i < out_num; i++) {
        if (tfm_memory_check(outvecs[i].base, outvecs[i].len,
            ns_caller, TFM_MEMORY_ACCESS_RW, privileged) != IPC_SUCCESS) {
            tfm_core_panic();
        }
    }

    /*
     * FixMe: Need to check if the message is unrecognized by the RoT
     * Service or incorrectly formatted.
     */
    msg = tfm_spm_get_msg_buffer_from_conn_handle(handle);
    if (!msg) {
        /* FixMe: Need to implement one mechanism to resolve this failure. */
        tfm_core_panic();
    }

    tfm_spm_fill_msg(msg, service, handle, type, client_id, invecs,
                     in_num, outvecs, out_num, outptr);

    /*
     * Send message and wake up the SP who is waiting on message queue,
     * and scheduler triggered
     */
    if (tfm_spm_send_event(service, msg) != IPC_SUCCESS) {
        /* FixMe: Need to refine failure process here. */
        tfm_core_panic();
    }
    return PSA_SUCCESS;
}

void tfm_spm_client_psa_close(psa_handle_t handle, bool ns_caller)
{
    struct tfm_spm_service_t *service;
    struct tfm_msg_body_t *msg;
    int32_t client_id;

    /* It will have no effect if called with the NULL handle */
    if (handle == PSA_NULL_HANDLE) {
        return;
    }

    if (ns_caller) {
        client_id = tfm_nspm_get_current_client_id();
    } else {
        client_id = tfm_spm_partition_get_running_partition_id();
    }

    /*
     * It is a fatal error if an invalid handle was provided that is not the
     * null handle.
     */
    if (tfm_spm_validate_conn_handle(handle, client_id) != IPC_SUCCESS) {
        tfm_core_panic();
    }
    service = tfm_spm_get_service_by_handle(handle);
    if (!service) {
        /* FixMe: Need to implement one mechanism to resolve this failure. */
        tfm_core_panic();
    }

    msg = tfm_spm_get_msg_buffer_from_conn_handle(handle);
    if (!msg) {
        /* FixMe: Need to implement one mechanism to resolve this failure. */
        tfm_core_panic();
    }

    /* It is a fatal error if the connection is currently handling a request. */
    if (((struct tfm_conn_handle_t *)handle)->status ==
                                                     TFM_HANDLE_STATUS_ACTIVE) {
        tfm_core_panic();
    }

    /* No input or output needed for close message */
    tfm_spm_fill_msg(msg, service, handle, PSA_IPC_DISCONNECT, client_id,
                     NULL, 0, NULL, 0, NULL);

    /*
     * Send message and wake up the SP who is waiting on message queue,
     * and scheduler triggered
     */
    tfm_spm_send_event(service, msg);
}
