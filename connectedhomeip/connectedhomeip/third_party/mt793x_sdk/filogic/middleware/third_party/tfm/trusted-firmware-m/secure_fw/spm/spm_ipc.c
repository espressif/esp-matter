/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <inttypes.h>
#include <stdbool.h>
#include "psa/client.h"
#include "psa/service.h"
#include "psa/lifecycle.h"
#include "tfm_thread.h"
#include "tfm_wait.h"
#include "tfm_utils.h"
#include "tfm_internal_defines.h"
#include "tfm_message_queue.h"
#include "tfm_spm_hal.h"
#include "tfm_irq_list.h"
#include "tfm_api.h"
#include "tfm_secure_api.h"
#include "tfm_memory_utils.h"
#include "spm_api.h"
#include "tfm_peripherals_def.h"
#include "spm_db.h"
#include "tfm_core_utils.h"
#include "spm_psa_client_call.h"
#include "tfm_rpc.h"
#include "tfm_internal.h"
#include "tfm_core_trustzone.h"
#include "tfm_core_mem_check.h"
#include "tfm_list.h"
#include "tfm_pools.h"
#include "region_defs.h"
#include "tfm_spm_services_api.h"

#include "secure_fw/services/tfm_service_list.inc"

/* Extern service variable */
extern struct tfm_spm_service_t service[];
extern const struct tfm_spm_service_db_t service_db[];

/* Extern SPM variable */
extern struct spm_partition_db_t g_spm_partition_db;

/* Pools */
TFM_POOL_DECLARE(conn_handle_pool, sizeof(struct tfm_conn_handle_t),
                 TFM_CONN_HANDLE_MAX_NUM);

void tfm_irq_handler(uint32_t partition_id, psa_signal_t signal,
                     IRQn_Type irq_line);

#include "tfm_secure_irq_handlers_ipc.inc"

/* Service handle management functions */
psa_handle_t tfm_spm_create_conn_handle(struct tfm_spm_service_t *service,
                                        int32_t client_id)
{
    struct tfm_conn_handle_t *p_handle;

    TFM_CORE_ASSERT(service);

    /* Get buffer for handle list structure from handle pool */
    p_handle = (struct tfm_conn_handle_t *)tfm_pool_alloc(conn_handle_pool);
    if (!p_handle) {
        return PSA_NULL_HANDLE;
    }

    p_handle->service = service;
    p_handle->status = TFM_HANDLE_STATUS_IDLE;
    p_handle->client_id = client_id;

    /* Add handle node to list for next psa functions */
    tfm_list_add_tail(&service->handle_list, &p_handle->list);

    return (psa_handle_t)p_handle;
}

int32_t tfm_spm_validate_conn_handle(psa_handle_t conn_handle,
                                     int32_t client_id)
{
    /* Check the handle address is validated */
    if (is_valid_chunk_data_in_pool(conn_handle_pool,
                                    (uint8_t *)conn_handle) != true) {
        return IPC_ERROR_GENERIC;
    }

    /* Check the handle caller is correct */
    if (((struct tfm_conn_handle_t *)conn_handle)->client_id != client_id) {
        return IPC_ERROR_GENERIC;
    }

    return IPC_SUCCESS;
}

static struct tfm_conn_handle_t *
    tfm_spm_find_conn_handle_node(struct tfm_spm_service_t *service,
                                  psa_handle_t conn_handle)
{
    TFM_CORE_ASSERT(service);

    return (struct tfm_conn_handle_t *)conn_handle;
}

/**
 * \brief                   Free connection handle which not used anymore.
 *
 * \param[in] service       Target service context pointer
 * \param[in] conn_handle   Connection handle created by
 *                          tfm_spm_create_conn_handle(), \ref psa_handle_t
 *
 * \retval IPC_SUCCESS      Success
 * \retval IPC_ERROR_BAD_PARAMETERS  Bad parameters input
 * \retval "Does not return"  Panic for not find service by handle
 */
static int32_t tfm_spm_free_conn_handle(struct tfm_spm_service_t *service,
                                        psa_handle_t conn_handle)
{
    struct tfm_conn_handle_t *p_handle;

    TFM_CORE_ASSERT(service);

    /* There are many handles for each RoT Service */
    p_handle = tfm_spm_find_conn_handle_node(service, conn_handle);
    if (!p_handle) {
        tfm_core_panic();
    }

    /* Clear magic as the handler is not used anymore */
    p_handle->internal_msg.magic = 0;

    /* Remove node from handle list */
    tfm_list_del_node(&p_handle->list);

    /* Back handle buffer to pool */
    tfm_pool_free(p_handle);
    return IPC_SUCCESS;
}

/**
 * \brief                   Set reverse handle value for connection.
 *
 * \param[in] service       Target service context pointer
 * \param[in] conn_handle   Connection handle created by
 *                          tfm_spm_create_conn_handle(), \ref psa_handle_t
 * \param[in] rhandle       rhandle need to save
 *
 * \retval IPC_SUCCESS      Success
 * \retval IPC_ERROR_BAD_PARAMETERS  Bad parameters input
 * \retval "Does not return"  Panic for not find handle node
 */
static int32_t tfm_spm_set_rhandle(struct tfm_spm_service_t *service,
                                   psa_handle_t conn_handle,
                                   void *rhandle)
{
    struct tfm_conn_handle_t *p_handle;

    TFM_CORE_ASSERT(service);
    /* Set reverse handle value only be allowed for a connected handle */
    TFM_CORE_ASSERT(conn_handle != PSA_NULL_HANDLE);

    /* There are many handles for each RoT Service */
    p_handle = tfm_spm_find_conn_handle_node(service, conn_handle);
    if (!p_handle) {
        tfm_core_panic();
    }

    p_handle->rhandle = rhandle;
    return IPC_SUCCESS;
}

/**
 * \brief                   Get reverse handle value from connection hanlde.
 *
 * \param[in] service       Target service context pointer
 * \param[in] conn_handle   Connection handle created by
 *                          tfm_spm_create_conn_handle(), \ref psa_handle_t
 *
 * \retval void *           Success
 * \retval "Does not return"  Panic for those:
 *                              service pointer are NULL
 *                              hanlde is \ref PSA_NULL_HANDLE
 *                              handle node does not be found
 */
static void *tfm_spm_get_rhandle(struct tfm_spm_service_t *service,
                                 psa_handle_t conn_handle)
{
    struct tfm_conn_handle_t *p_handle;

    TFM_CORE_ASSERT(service);
    /* Get reverse handle value only be allowed for a connected handle */
    TFM_CORE_ASSERT(conn_handle != PSA_NULL_HANDLE);

    /* There are many handles for each RoT Service */
    p_handle = tfm_spm_find_conn_handle_node(service, conn_handle);
    if (!p_handle) {
        tfm_core_panic();
    }

    return p_handle->rhandle;
}

/* Partition management functions */

/**
 * \brief                   Get the service context by signal.
 *
 * \param[in] partition     Partition context pointer
 *                          \ref spm_partition_desc_t structures
 * \param[in] signal        Signal associated with inputs to the Secure
 *                          Partition, \ref psa_signal_t
 *
 * \retval NULL             Failed
 * \retval "Not NULL"       Target service context pointer,
 *                          \ref tfm_spm_service_t structures
 */
static struct tfm_spm_service_t *
    tfm_spm_get_service_by_signal(struct spm_partition_desc_t *partition,
                                  psa_signal_t signal)
{
    struct tfm_list_node_t *node, *head;
    struct tfm_spm_service_t *service;

    TFM_CORE_ASSERT(partition);

    if (tfm_list_is_empty(&partition->runtime_data.service_list)) {
        tfm_core_panic();
    }

    head = &partition->runtime_data.service_list;
    TFM_LIST_FOR_EACH(node, head) {
        service = TFM_GET_CONTAINER_PTR(node, struct tfm_spm_service_t, list);
        if (service->service_db->signal == signal) {
            return service;
        }
    }
    return NULL;
}

struct tfm_spm_service_t *tfm_spm_get_service_by_sid(uint32_t sid)
{
    uint32_t i;
    struct tfm_list_node_t *node, *head;
    struct tfm_spm_service_t *service;
    struct spm_partition_desc_t *partition;

    for (i = 0; i < g_spm_partition_db.partition_count; i++) {
        partition = &g_spm_partition_db.partitions[i];
        /* Skip partition without IPC flag */
        if ((tfm_spm_partition_get_flags(i) & SPM_PART_FLAG_IPC) == 0) {
            continue;
        }

        if (tfm_list_is_empty(&partition->runtime_data.service_list)) {
            continue;
        }

        head = &partition->runtime_data.service_list;
        TFM_LIST_FOR_EACH(node, head) {
            service = TFM_GET_CONTAINER_PTR(node, struct tfm_spm_service_t,
                                            list);
            if (service->service_db->sid == sid) {
                return service;
            }
        }
    }
    return NULL;
}

struct tfm_spm_service_t *
    tfm_spm_get_service_by_handle(psa_handle_t conn_handle)
{
    return ((struct tfm_conn_handle_t *)conn_handle)->service;
}

/**
 * \brief                   Get the partition context by partition ID.
 *
 * \param[in] partition_id  Partition identity
 *
 * \retval NULL             Failed
 * \retval "Not NULL"       Target partition context pointer,
 *                          \ref spm_partition_desc_t structures
 */
static struct spm_partition_desc_t *
    tfm_spm_get_partition_by_id(int32_t partition_id)
{
    uint32_t idx = get_partition_idx(partition_id);

    if (idx != SPM_INVALID_PARTITION_IDX) {
        return &(g_spm_partition_db.partitions[idx]);
    }
    return NULL;
}

struct spm_partition_desc_t *tfm_spm_get_running_partition(void)
{
    uint32_t spid;

    spid = tfm_spm_partition_get_running_partition_id();

    return tfm_spm_get_partition_by_id(spid);
}

int32_t tfm_spm_check_client_version(struct tfm_spm_service_t *service,
                                     uint32_t version)
{
    TFM_CORE_ASSERT(service);

    switch (service->service_db->version_policy) {
    case TFM_VERSION_POLICY_RELAXED:
        if (version > service->service_db->version) {
            return IPC_ERROR_VERSION;
        }
        break;
    case TFM_VERSION_POLICY_STRICT:
        if (version != service->service_db->version) {
            return IPC_ERROR_VERSION;
        }
        break;
    default:
        return IPC_ERROR_VERSION;
    }
    return IPC_SUCCESS;
}

int32_t tfm_spm_check_authorization(uint32_t sid,
                                    struct tfm_spm_service_t *service,
                                    bool ns_caller)
{
    struct spm_partition_desc_t *partition = NULL;
    int32_t i;

    TFM_CORE_ASSERT(service);

    if (ns_caller) {
        if (!service->service_db->non_secure_client) {
            return IPC_ERROR_GENERIC;
        }
    } else {
        partition = tfm_spm_get_running_partition();
        if (!partition) {
            tfm_core_panic();
        }

        for (i = 0; i < partition->static_data->dependencies_num; i++) {
            if (partition->static_data->p_dependencies[i] == sid) {
                break;
            }
        }

        if (i == partition->static_data->dependencies_num) {
            return IPC_ERROR_GENERIC;
        }
    }
    return IPC_SUCCESS;
}

/* Message functions */

/**
 * \brief                   Get message context by message handle.
 *
 * \param[in] msg_handle    Message handle which is a reference generated
 *                          by the SPM to a specific message.
 *
 * \return                  The message body context pointer
 *                          \ref tfm_msg_body_t structures
 */
static struct tfm_msg_body_t *
    tfm_spm_get_msg_from_handle(psa_handle_t msg_handle)
{
    /*
     * The message handler passed by the caller is considered invalid in the
     * following cases:
     *   1. Not a valid message handle. (The address of a message is not the
     *      address of a possible handle from the pool
     *   2. Handle not belongs to the caller partition (The handle is either
     *      unused, or owned by anither partition)
     * Check the conditions above
     */
    struct tfm_conn_handle_t *connection_handle_address;
    struct tfm_msg_body_t *msg;
    uint32_t partition_id;

    msg = (struct tfm_msg_body_t *)msg_handle;

    connection_handle_address =
        TFM_GET_CONTAINER_PTR(msg, struct tfm_conn_handle_t, internal_msg);

    if (is_valid_chunk_data_in_pool(
        conn_handle_pool, (uint8_t *)connection_handle_address) != 1) {
        return NULL;
    }

    /*
     * Check that the magic number is correct. This proves that the message
     * structure contains an active message.
     */
    if (msg->magic != TFM_MSG_MAGIC) {
        return NULL;
    }

    /* Check that the running partition owns the message */
    partition_id = tfm_spm_partition_get_running_partition_id();
    if (partition_id != msg->service->partition->static_data->partition_id) {
        return NULL;
    }

    /*
     * FixMe: For condition 1 it should be checked whether the message belongs
     * to the service. Skipping this check isn't a security risk as even if the
     * message belongs to another service, the handle belongs to the calling
     * partition.
     */

    return msg;
}

struct tfm_msg_body_t *
    tfm_spm_get_msg_buffer_from_conn_handle(psa_handle_t conn_handle)
{
    TFM_CORE_ASSERT(conn_handle != PSA_NULL_HANDLE);

    return &(((struct tfm_conn_handle_t *)conn_handle)->internal_msg);
}

void tfm_spm_fill_msg(struct tfm_msg_body_t *msg,
                      struct tfm_spm_service_t *service,
                      psa_handle_t handle,
                      int32_t type, int32_t client_id,
                      psa_invec *invec, size_t in_len,
                      psa_outvec *outvec, size_t out_len,
                      psa_outvec *caller_outvec)
{
    uint32_t i;

    TFM_CORE_ASSERT(msg);
    TFM_CORE_ASSERT(service);
    TFM_CORE_ASSERT(!(invec == NULL && in_len != 0));
    TFM_CORE_ASSERT(!(outvec == NULL && out_len != 0));
    TFM_CORE_ASSERT(in_len <= PSA_MAX_IOVEC);
    TFM_CORE_ASSERT(out_len <= PSA_MAX_IOVEC);
    TFM_CORE_ASSERT(in_len + out_len <= PSA_MAX_IOVEC);

    /* Clear message buffer before using it */
    tfm_core_util_memset(msg, 0, sizeof(struct tfm_msg_body_t));

    tfm_event_init(&msg->ack_evnt);
    msg->magic = TFM_MSG_MAGIC;
    msg->service = service;
    msg->handle = handle;
    msg->caller_outvec = caller_outvec;
    msg->msg.client_id = client_id;

    /* Copy contents */
    msg->msg.type = type;

    for (i = 0; i < in_len; i++) {
        msg->msg.in_size[i] = invec[i].len;
        msg->invec[i].base = invec[i].base;
    }

    for (i = 0; i < out_len; i++) {
        msg->msg.out_size[i] = outvec[i].len;
        msg->outvec[i].base = outvec[i].base;
        /* Out len is used to record the writed number, set 0 here again */
        msg->outvec[i].len = 0;
    }

    /* Use message address as handle */
    msg->msg.handle = (psa_handle_t)msg;

    /* For connected handle, set rhandle to every message */
    if (handle != PSA_NULL_HANDLE) {
        msg->msg.rhandle = tfm_spm_get_rhandle(service, handle);
    }

    /* Set the private data of NSPE client caller in multi-core topology */
    if (TFM_CLIENT_ID_IS_NS(client_id)) {
        tfm_rpc_set_caller_data(msg, client_id);
    }
}

int32_t tfm_spm_send_event(struct tfm_spm_service_t *service,
                           struct tfm_msg_body_t *msg)
{
    struct spm_partition_runtime_data_t *p_runtime_data =
                                            &service->partition->runtime_data;

    TFM_CORE_ASSERT(service);
    TFM_CORE_ASSERT(msg);

    /* Enqueue message to service message queue */
    if (tfm_msg_enqueue(&service->msg_queue, msg) != IPC_SUCCESS) {
        return IPC_ERROR_GENERIC;
    }

    /* Messages put. Update signals */
    p_runtime_data->signals |= service->service_db->signal;

    tfm_event_wake(&p_runtime_data->signal_evnt, (p_runtime_data->signals &
                                                  p_runtime_data->signal_mask));

    /*
     * If it is a NS request via RPC, it is unnecessary to block current
     * thread.
     */
    if (!is_tfm_rpc_msg(msg)) {
        tfm_event_wait(&msg->ack_evnt);
    }

    return IPC_SUCCESS;
}

/**
 * \brief Get bottom of stack region for a partition
 *
 * \param[in] partition_idx     Partition index
 *
 * \return Stack region bottom value
 *
 * \note This function doesn't check if partition_idx is valid.
 */
static uint32_t tfm_spm_partition_get_stack_bottom(uint32_t partition_idx)
{
    return g_spm_partition_db.partitions[partition_idx].
            memory_data->stack_bottom;
}

/**
 * \brief Get top of stack region for a partition
 *
 * \param[in] partition_idx     Partition index
 *
 * \return Stack region top value
 *
 * \note This function doesn't check if partition_idx is valid.
 */
static uint32_t tfm_spm_partition_get_stack_top(uint32_t partition_idx)
{
    return g_spm_partition_db.partitions[partition_idx].memory_data->stack_top;
}

uint32_t tfm_spm_partition_get_running_partition_id(void)
{
    struct tfm_core_thread_t *pth = tfm_core_thrd_get_curr_thread();
    struct spm_partition_desc_t *partition;
    struct spm_partition_runtime_data_t *r_data;

    r_data = TFM_GET_CONTAINER_PTR(pth, struct spm_partition_runtime_data_t,
                                   sp_thrd);
    partition = TFM_GET_CONTAINER_PTR(r_data, struct spm_partition_desc_t,
                                      runtime_data);
    return partition->static_data->partition_id;
}

static struct tfm_core_thread_t *
    tfm_spm_partition_get_thread_info(uint32_t partition_idx)
{
    return &g_spm_partition_db.partitions[partition_idx].runtime_data.sp_thrd;
}

static tfm_core_thrd_entry_t
    tfm_spm_partition_get_init_func(uint32_t partition_idx)
{
    return (tfm_core_thrd_entry_t)(g_spm_partition_db.partitions[partition_idx].
                             static_data->partition_init);
}

static uint32_t tfm_spm_partition_get_priority(uint32_t partition_idx)
{
    return g_spm_partition_db.partitions[partition_idx].static_data->
                    partition_priority;
}

int32_t tfm_memory_check(const void *buffer, size_t len, bool ns_caller,
                         enum tfm_memory_access_e access,
                         uint32_t privileged)
{
    enum tfm_status_e err;

    /* If len is zero, this indicates an empty buffer and base is ignored */
    if (len == 0) {
        return IPC_SUCCESS;
    }

    if (!buffer) {
        return IPC_ERROR_BAD_PARAMETERS;
    }

    if ((uintptr_t)buffer > (UINTPTR_MAX - len)) {
        return IPC_ERROR_MEMORY_CHECK;
    }

    if (access == TFM_MEMORY_ACCESS_RW) {
        err = tfm_core_has_write_access_to_region(buffer, len, ns_caller,
                                                  privileged);
    } else {
        err = tfm_core_has_read_access_to_region(buffer, len, ns_caller,
                                                 privileged);
    }
    if (err == TFM_SUCCESS) {
        return IPC_SUCCESS;
    }

    return IPC_ERROR_MEMORY_CHECK;
}

uint32_t tfm_spm_init(void)
{
    uint32_t i, j, num;
    struct spm_partition_desc_t *partition;
    struct tfm_core_thread_t *pth, *p_ns_entry_thread = NULL;
    const struct tfm_spm_partition_platform_data_t **platform_data_p;

    tfm_pool_init(conn_handle_pool,
                  POOL_BUFFER_SIZE(conn_handle_pool),
                  sizeof(struct tfm_conn_handle_t),
                  TFM_CONN_HANDLE_MAX_NUM);

    /* Init partition first for it will be used when init service */
    for (i = 0; i < g_spm_partition_db.partition_count; i++) {
        partition = &g_spm_partition_db.partitions[i];

        /* Check if the PSA framework version matches. */
        if (partition->static_data->psa_framework_version !=
            PSA_FRAMEWORK_VERSION) {
            ERROR_MSG("Warning: PSA Framework Verison is not matched!");
            continue;
        }

        platform_data_p = partition->platform_data_list;
        if (platform_data_p != NULL) {
            while ((*platform_data_p) != NULL) {
                if (tfm_spm_hal_configure_default_isolation(i,
                            *platform_data_p) != TFM_PLAT_ERR_SUCCESS) {
                    tfm_core_panic();
                }
                ++platform_data_p;
            }
        }

        if ((tfm_spm_partition_get_flags(i) & SPM_PART_FLAG_IPC) == 0) {
            continue;
        }

        /* Add PSA_DOORBELL signal to assigned_signals */
        partition->runtime_data.assigned_signals |= PSA_DOORBELL;

        /* TODO: This can be optimized by generating the assigned signal
         *       in code generation time.
         */
        for (j = 0; j < tfm_core_irq_signals_count; ++j) {
            if (tfm_core_irq_signals[j].partition_id ==
                                        partition->static_data->partition_id) {
                partition->runtime_data.assigned_signals |=
                                        tfm_core_irq_signals[j].signal_value;
            }
        }

        tfm_event_init(&partition->runtime_data.signal_evnt);
        tfm_list_init(&partition->runtime_data.service_list);

        pth = tfm_spm_partition_get_thread_info(i);
        if (!pth) {
            tfm_core_panic();
        }

        tfm_core_thrd_init(pth,
                           tfm_spm_partition_get_init_func(i),
                           NULL,
                           (uintptr_t)tfm_spm_partition_get_stack_top(i),
                           (uintptr_t)tfm_spm_partition_get_stack_bottom(i));

        pth->prior = tfm_spm_partition_get_priority(i);

        if (partition->static_data->partition_id == TFM_SP_NON_SECURE_ID) {
            p_ns_entry_thread = pth;
            pth->param = (void *)tfm_spm_hal_get_ns_entry_point();
        }

        /* Kick off */
        if (tfm_core_thrd_start(pth) != THRD_SUCCESS) {
            tfm_core_panic();
        }
    }

    /* Init Service */
    num = sizeof(service) / sizeof(struct tfm_spm_service_t);
    for (i = 0; i < num; i++) {
        service[i].service_db = &service_db[i];
        partition =
            tfm_spm_get_partition_by_id(service[i].service_db->partition_id);
        if (!partition) {
            tfm_core_panic();
        }
        service[i].partition = partition;
        partition->runtime_data.assigned_signals |= service[i].service_db->signal;

        tfm_list_init(&service[i].handle_list);
        tfm_list_add_tail(&partition->runtime_data.service_list,
                          &service[i].list);
    }

    /*
     * All threads initialized, start the scheduler.
     *
     * NOTE:
     * It is worthy to give the thread object to scheduler if the background
     * context belongs to one of the threads. Here the background thread is the
     * initialization thread who calls SPM SVC, which re-uses the non-secure
     * entry thread's stack. After SPM initialization is done, this stack is
     * cleaned up and the background context is never going to return. Tell
     * the scheduler that the current thread is non-secure entry thread.
     */
    tfm_core_thrd_start_scheduler(p_ns_entry_thread);

    return p_ns_entry_thread->arch_ctx.lr;
}

void tfm_pendsv_do_schedule(struct tfm_arch_ctx_t *p_actx)
{
#if TFM_LVL == 2
    struct spm_partition_desc_t *p_next_partition;
    struct spm_partition_runtime_data_t *r_data;
    uint32_t is_privileged;
#endif
    struct tfm_core_thread_t *pth_next = tfm_core_thrd_get_next_thread();
    struct tfm_core_thread_t *pth_curr = tfm_core_thrd_get_curr_thread();

    if (pth_next != NULL && pth_curr != pth_next) {
#if TFM_LVL == 2
        r_data = TFM_GET_CONTAINER_PTR(pth_next,
                                       struct spm_partition_runtime_data_t,
                                       sp_thrd);
        p_next_partition = TFM_GET_CONTAINER_PTR(r_data,
                                                 struct spm_partition_desc_t,
                                                 runtime_data);

        if (p_next_partition->static_data->partition_flags &
            SPM_PART_FLAG_PSA_ROT) {
            is_privileged = TFM_PARTITION_PRIVILEGED_MODE;
        } else {
            is_privileged = TFM_PARTITION_UNPRIVILEGED_MODE;
        }

        tfm_spm_partition_change_privilege(is_privileged);
#endif

        tfm_core_thrd_switch_context(p_actx, pth_curr, pth_next);
    }

    /*
     * Handle pending mailbox message from NS in multi-core topology.
     * Empty operation on single Armv8-M platform.
     */
    tfm_rpc_client_call_handler();
}

/*********************** SPM functions for PSA Client APIs *******************/

uint32_t tfm_spm_psa_framework_version(void)
{
    return tfm_spm_client_psa_framework_version();
}

uint32_t tfm_spm_psa_version(uint32_t *args, bool ns_caller)
{
    uint32_t sid;

    TFM_CORE_ASSERT(args != NULL);
    sid = (uint32_t)args[0];

    return tfm_spm_client_psa_version(sid, ns_caller);
}

psa_status_t tfm_spm_psa_connect(uint32_t *args, bool ns_caller)
{
    uint32_t sid;
    uint32_t version;

    TFM_CORE_ASSERT(args != NULL);
    sid = (uint32_t)args[0];
    version = (uint32_t)args[1];

    return tfm_spm_client_psa_connect(sid, version, ns_caller);
}

psa_status_t tfm_spm_psa_call(uint32_t *args, bool ns_caller, uint32_t lr)
{
    psa_handle_t handle;
    psa_invec *inptr;
    psa_outvec *outptr;
    size_t in_num, out_num;
    struct spm_partition_desc_t *partition = NULL;
    uint32_t privileged;
    int32_t type;
    struct tfm_control_parameter_t ctrl_param;

    TFM_CORE_ASSERT(args != NULL);
    handle = (psa_handle_t)args[0];

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }
    privileged = tfm_spm_partition_get_privileged_mode(
        partition->static_data->partition_flags);

    /*
     * Read parameters from the arguments. It is a fatal error if the
     * memory reference for buffer is invalid or not readable.
     */
    if (tfm_memory_check((const void *)args[1],
        sizeof(struct tfm_control_parameter_t), ns_caller,
        TFM_MEMORY_ACCESS_RW, privileged) != IPC_SUCCESS) {
        tfm_core_panic();
    }

    tfm_core_util_memcpy(&ctrl_param,
                         (const void *)args[1],
                         sizeof(ctrl_param));

    type = ctrl_param.type;
    in_num = ctrl_param.in_len;
    out_num = ctrl_param.out_len;
    inptr = (psa_invec *)args[2];
    outptr = (psa_outvec *)args[3];

    /* The request type must be zero or positive. */
    if (type < 0) {
        tfm_core_panic();
    }

    return tfm_spm_client_psa_call(handle, type, inptr, in_num, outptr, out_num,
                                   ns_caller, privileged);
}

void tfm_spm_psa_close(uint32_t *args, bool ns_caller)
{
    psa_handle_t handle;

    TFM_CORE_ASSERT(args != NULL);
    handle = args[0];

    tfm_spm_client_psa_close(handle, ns_caller);
}

uint32_t tfm_spm_get_lifecycle_state(void)
{
    /*
     * FixMe: return PSA_LIFECYCLE_UNKNOWN to the caller directly. It will be
     * implemented in the future.
     */
    return PSA_LIFECYCLE_UNKNOWN;
}

/********************* SPM functions for PSA Service APIs ********************/

psa_signal_t tfm_spm_psa_wait(uint32_t *args)
{
    psa_signal_t signal_mask;
    uint32_t timeout;
    struct spm_partition_desc_t *partition = NULL;

    TFM_CORE_ASSERT(args != NULL);
    signal_mask = (psa_signal_t)args[0];
    timeout = args[1];

    /*
     * Timeout[30:0] are reserved for future use.
     * SPM must ignore the value of RES.
     */
    timeout &= PSA_TIMEOUT_MASK;

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    /*
     * It is a PROGRAMMER ERROR if the signal_mask does not include any assigned
     * signals.
     */
    if ((partition->runtime_data.assigned_signals & signal_mask) == 0) {
        tfm_core_panic();
    }

    /*
     * Expected signals are included in signal wait mask, ignored signals
     * should not be set and affect caller thread state. Save this mask for
     * further checking while signals are ready to be set.
     */
    partition->runtime_data.signal_mask = signal_mask;

    /*
     * tfm_event_wait() blocks the caller thread if no signals are available.
     * In this case, the return value of this function is temporary set into
     * runtime context. After new signal(s) are available, the return value
     * is updated with the available signal(s) and blocked thread gets to run.
     */
    if (timeout == PSA_BLOCK &&
        (partition->runtime_data.signals & signal_mask) == 0) {
        tfm_event_wait(&partition->runtime_data.signal_evnt);
    }

    return partition->runtime_data.signals & signal_mask;
}

psa_status_t tfm_spm_psa_get(uint32_t *args)
{
    psa_signal_t signal;
    psa_msg_t *msg = NULL;
    struct tfm_spm_service_t *service = NULL;
    struct tfm_msg_body_t *tmp_msg = NULL;
    struct spm_partition_desc_t *partition = NULL;
    uint32_t privileged;

    TFM_CORE_ASSERT(args != NULL);
    signal = (psa_signal_t)args[0];
    msg = (psa_msg_t *)args[1];

    /*
     * Only one message could be retrieved every time for psa_get(). It is a
     * fatal error if the input signal has more than a signal bit set.
     */
    if (!tfm_is_one_bit_set(signal)) {
        tfm_core_panic();
    }

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }
    privileged = tfm_spm_partition_get_privileged_mode(
        partition->static_data->partition_flags);

    /*
     * Write the message to the service buffer. It is a fatal error if the
     * input msg pointer is not a valid memory reference or not read-write.
     */
    if (tfm_memory_check(msg, sizeof(psa_msg_t), false, TFM_MEMORY_ACCESS_RW,
        privileged) != IPC_SUCCESS) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the caller call psa_get() when no message has
     * been set. The caller must call this function after an RoT Service signal
     * is returned by psa_wait().
     */
    if (partition->runtime_data.signals == 0) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the RoT Service signal is not currently asserted.
     */
    if ((partition->runtime_data.signals & signal) == 0) {
        tfm_core_panic();
    }

    /*
     * Get RoT service by signal from partition. It is a fatal error if getting
     * failed, which means the input signal is not correspond to an RoT service.
     */
    service = tfm_spm_get_service_by_signal(partition, signal);
    if (!service) {
        tfm_core_panic();
    }

    tmp_msg = tfm_msg_dequeue(&service->msg_queue);
    if (!tmp_msg) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    ((struct tfm_conn_handle_t *)(tmp_msg->handle))->status =
                                                       TFM_HANDLE_STATUS_ACTIVE;

    tfm_core_util_memcpy(msg, &tmp_msg->msg, sizeof(psa_msg_t));

    /*
     * There may be multiple messages for this RoT Service signal, do not clear
     * its mask until no remaining message.
     */
    if (tfm_msg_queue_is_empty(&service->msg_queue)) {
        partition->runtime_data.signals &= ~signal;
    }

    return PSA_SUCCESS;
}

void tfm_spm_psa_set_rhandle(uint32_t *args)
{
    psa_handle_t msg_handle;
    void *rhandle = NULL;
    struct tfm_msg_body_t *msg = NULL;

    TFM_CORE_ASSERT(args != NULL);
    msg_handle = (psa_handle_t)args[0];
    rhandle = (void *)args[1];

    /* It is a fatal error if message handle is invalid */
    msg = tfm_spm_get_msg_from_handle(msg_handle);
    if (!msg) {
        tfm_core_panic();
    }

    msg->msg.rhandle = rhandle;

    /* Store reverse handle for following client calls. */
    tfm_spm_set_rhandle(msg->service, msg->handle, rhandle);
}

size_t tfm_spm_psa_read(uint32_t *args)
{
    psa_handle_t msg_handle;
    uint32_t invec_idx;
    void *buffer = NULL;
    size_t num_bytes;
    size_t bytes;
    struct tfm_msg_body_t *msg = NULL;
    uint32_t privileged;
    struct spm_partition_desc_t *partition = NULL;

    TFM_CORE_ASSERT(args != NULL);
    msg_handle = (psa_handle_t)args[0];
    invec_idx = args[1];
    buffer = (void *)args[2];
    num_bytes = (size_t)args[3];

    /* It is a fatal error if message handle is invalid */
    msg = tfm_spm_get_msg_from_handle(msg_handle);
    if (!msg) {
        tfm_core_panic();
    }

    partition = msg->service->partition;
    privileged = tfm_spm_partition_get_privileged_mode(
        partition->static_data->partition_flags);

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (msg->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /* There was no remaining data in this input vector */
    if (msg->msg.in_size[invec_idx] == 0) {
        return 0;
    }

    /*
     * Copy the client data to the service buffer. It is a fatal error
     * if the memory reference for buffer is invalid or not read-write.
     */
    if (tfm_memory_check(buffer, num_bytes, false,
        TFM_MEMORY_ACCESS_RW, privileged) != IPC_SUCCESS) {
        tfm_core_panic();
    }

    bytes = num_bytes > msg->msg.in_size[invec_idx] ?
                        msg->msg.in_size[invec_idx] : num_bytes;

    tfm_core_util_memcpy(buffer, msg->invec[invec_idx].base, bytes);

    /* There maybe some remaining data */
    msg->invec[invec_idx].base = (char *)msg->invec[invec_idx].base + bytes;
    msg->msg.in_size[invec_idx] -= bytes;

    return bytes;
}

size_t tfm_spm_psa_skip(uint32_t *args)
{
    psa_handle_t msg_handle;
    uint32_t invec_idx;
    size_t num_bytes;
    struct tfm_msg_body_t *msg = NULL;

    TFM_CORE_ASSERT(args != NULL);
    msg_handle = (psa_handle_t)args[0];
    invec_idx = args[1];
    num_bytes = (size_t)args[2];

    /* It is a fatal error if message handle is invalid */
    msg = tfm_spm_get_msg_from_handle(msg_handle);
    if (!msg) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (msg->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /* There was no remaining data in this input vector */
    if (msg->msg.in_size[invec_idx] == 0) {
        return 0;
    }

    /*
     * If num_bytes is greater than the remaining size of the input vector then
     * the remaining size of the input vector is used.
     */
    if (num_bytes > msg->msg.in_size[invec_idx]) {
        num_bytes = msg->msg.in_size[invec_idx];
    }

    /* There maybe some remaining data */
    msg->invec[invec_idx].base = (char *)msg->invec[invec_idx].base +
                                 num_bytes;
    msg->msg.in_size[invec_idx] -= num_bytes;

    return num_bytes;
}

void tfm_spm_psa_write(uint32_t *args)
{
    psa_handle_t msg_handle;
    uint32_t outvec_idx;
    void *buffer = NULL;
    size_t num_bytes;
    struct tfm_msg_body_t *msg = NULL;
    uint32_t privileged;
    struct spm_partition_desc_t *partition = NULL;

    TFM_CORE_ASSERT(args != NULL);
    msg_handle = (psa_handle_t)args[0];
    outvec_idx = args[1];
    buffer = (void *)args[2];
    num_bytes = (size_t)args[3];

    /* It is a fatal error if message handle is invalid */
    msg = tfm_spm_get_msg_from_handle(msg_handle);
    if (!msg) {
        tfm_core_panic();
    }

    partition = msg->service->partition;
    privileged = tfm_spm_partition_get_privileged_mode(
        partition->static_data->partition_flags);

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (msg->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if outvec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (outvec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the call attempts to write data past the end of
     * the client output vector
     */
    if (num_bytes > msg->msg.out_size[outvec_idx] -
        msg->outvec[outvec_idx].len) {
        tfm_core_panic();
    }

    /*
     * Copy the service buffer to client outvecs. It is a fatal error
     * if the memory reference for buffer is invalid or not readable.
     */
    if (tfm_memory_check(buffer, num_bytes, false,
        TFM_MEMORY_ACCESS_RO, privileged) != IPC_SUCCESS) {
        tfm_core_panic();
    }

    tfm_core_util_memcpy((char *)msg->outvec[outvec_idx].base +
                         msg->outvec[outvec_idx].len, buffer, num_bytes);

    /* Update the write number */
    msg->outvec[outvec_idx].len += num_bytes;
}

static void update_caller_outvec_len(struct tfm_msg_body_t *msg)
{
    uint32_t i;

    /*
     * FixeMe: abstract these part into dedicated functions to avoid
     * accessing thread context in psa layer
     */
    /* If it is a NS request via RPC, the owner of this message is not set */
    if (!is_tfm_rpc_msg(msg)) {
        TFM_CORE_ASSERT(msg->ack_evnt.owner->state == THRD_STATE_BLOCK);
    }

    for (i = 0; i < PSA_MAX_IOVEC; i++) {
        if (msg->msg.out_size[i] == 0) {
            continue;
        }

        TFM_CORE_ASSERT(msg->caller_outvec[i].base == msg->outvec[i].base);

        msg->caller_outvec[i].len = msg->outvec[i].len;
    }
}

void tfm_spm_psa_reply(uint32_t *args)
{
    psa_handle_t msg_handle;
    psa_status_t status;
    struct tfm_spm_service_t *service = NULL;
    struct tfm_msg_body_t *msg = NULL;
    int32_t ret = PSA_SUCCESS;

    TFM_CORE_ASSERT(args != NULL);
    msg_handle = (psa_handle_t)args[0];
    status = (psa_status_t)args[1];

    /* It is a fatal error if message handle is invalid */
    msg = tfm_spm_get_msg_from_handle(msg_handle);
    if (!msg) {
        tfm_core_panic();
    }

    /*
     * RoT Service information is needed in this function, stored it in message
     * body structure. Only two parameters are passed in this function: handle
     * and status, so it is useful and simply to do like this.
     */
    service = msg->service;
    if (!service) {
        tfm_core_panic();
    }

    /*
     * Three type of message are passed in this function: CONNECTION, REQUEST,
     * DISCONNECTION. It needs to process differently for each type.
     */
    switch (msg->msg.type) {
    case PSA_IPC_CONNECT:
        /*
         * Reply to PSA_IPC_CONNECT message. Connect handle is returned if the
         * input status is PSA_SUCCESS. Others return values are based on the
         * input status.
         */
        if (status == PSA_SUCCESS) {
            ret = msg->handle;
        } else if (status == PSA_ERROR_CONNECTION_REFUSED) {
            /* Refuse the client connection, indicating a permanent error. */
            tfm_spm_free_conn_handle(service, msg->handle);
            ret = PSA_ERROR_CONNECTION_REFUSED;
        } else if (status == PSA_ERROR_CONNECTION_BUSY) {
            /* Fail the client connection, indicating a transient error. */
            ret = PSA_ERROR_CONNECTION_BUSY;
        } else {
            tfm_core_panic();
        }
        break;
    case PSA_IPC_DISCONNECT:
        /* Service handle is not used anymore */
        tfm_spm_free_conn_handle(service, msg->handle);

        /*
         * If the message type is PSA_IPC_DISCONNECT, then the status code is
         * ignored
         */
        break;
    default:
        if (msg->msg.type >= PSA_IPC_CALL) {
            /* Reply to a request message. Return values are based on status */
            ret = status;
            /*
             * The total number of bytes written to a single parameter must be
             * reported to the client by updating the len member of the
             * psa_outvec structure for the parameter before returning from
             * psa_call().
             */
            update_caller_outvec_len(msg);
        } else {
            tfm_core_panic();
        }
    }

    if (ret == PSA_ERROR_PROGRAMMER_ERROR) {
        /*
         * If the source of the programmer error is a Secure Partition, the SPM
         * must panic the Secure Partition in response to a PROGRAMMER ERROR.
         */
        if (TFM_CLIENT_ID_IS_NS(msg->msg.client_id)) {
            ((struct tfm_conn_handle_t *)(msg->handle))->status =
                                                TFM_HANDLE_STATUS_CONNECT_ERROR;
        } else {
            tfm_core_panic();
        }
    } else {
        ((struct tfm_conn_handle_t *)(msg->handle))->status =
                                                         TFM_HANDLE_STATUS_IDLE;
    }

    if (is_tfm_rpc_msg(msg)) {
        tfm_rpc_client_call_reply(msg, ret);
    } else {
        tfm_event_wake(&msg->ack_evnt, ret);
    }
}

/**
 * \brief   notify the partition with the signal.
 *
 * \param[in] partition_id      The ID of the partition to be notified.
 * \param[in] signal            The signal that the partition is to be notified
 *                              with.
 *
 * \retval void                 Success.
 * \retval "Does not return"    If partition_id is invalid.
 */
static void notify_with_signal(int32_t partition_id, psa_signal_t signal)
{
    struct spm_partition_desc_t *partition = NULL;

    /*
     * The value of partition_id must be greater than zero as the target of
     * notification must be a Secure Partition, providing a Non-secure
     * Partition ID is a fatal error.
     */
    if (!TFM_CLIENT_ID_IS_S(partition_id)) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if partition_id does not correspond to a Secure
     * Partition.
     */
    partition = tfm_spm_get_partition_by_id(partition_id);
    if (!partition) {
        tfm_core_panic();
    }

    partition->runtime_data.signals |= signal;

    /*
     * The target partition may be blocked with waiting for signals after
     * called psa_wait(). Set the return value with the available signals
     * before wake it up with tfm_event_signal().
     */
    tfm_event_wake(&partition->runtime_data.signal_evnt,
                   partition->runtime_data.signals &
                   partition->runtime_data.signal_mask);
}

void tfm_spm_psa_notify(uint32_t *args)
{
    int32_t partition_id;

    TFM_CORE_ASSERT(args != NULL);
    partition_id = (int32_t)args[0];

    notify_with_signal(partition_id, PSA_DOORBELL);
}

/**
 * \brief assert signal for a given IRQ line.
 *
 * \param[in] partition_id      The ID of the partition which handles this IRQ
 * \param[in] signal            The signal associated with this IRQ
 * \param[in] irq_line          The number of the IRQ line
 *
 * \retval void                 Success.
 * \retval "Does not return"    Partition ID is invalid
 */
void tfm_irq_handler(uint32_t partition_id, psa_signal_t signal,
                     IRQn_Type irq_line)
{
    tfm_spm_hal_disable_irq(irq_line);
    notify_with_signal(partition_id, signal);
}

void tfm_spm_psa_clear(void)
{
    struct spm_partition_desc_t *partition = NULL;

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the Secure Partition's doorbell signal is not
     * currently asserted.
     */
    if ((partition->runtime_data.signals & PSA_DOORBELL) == 0) {
        tfm_core_panic();
    }
    partition->runtime_data.signals &= ~PSA_DOORBELL;
}

void tfm_spm_psa_panic(void)
{
    /*
     * PSA FF recommends that the SPM causes the system to restart when a secure
     * partition panics.
     */
    tfm_spm_hal_system_reset();
}

/**
 * \brief Return the IRQ line number associated with a signal
 *
 * \param[in]      partition_id    The ID of the partition in which we look for
 *                                 the signal.
 * \param[in]      signal          The signal we do the query for.
 * \param[out]     irq_line        The irq line associated with signal
 *
 * \retval IPC_SUCCESS          Execution successful, irq_line contains a valid
 *                              value.
 * \retval IPC_ERROR_GENERIC    There was an error finding the IRQ line for the
 *                              signal. irq_line is unchanged.
 */
static int32_t get_irq_line_for_signal(int32_t partition_id,
                                       psa_signal_t signal,
                                       IRQn_Type *irq_line)
{
    size_t i;

    for (i = 0; i < tfm_core_irq_signals_count; ++i) {
        if (tfm_core_irq_signals[i].partition_id == partition_id &&
            tfm_core_irq_signals[i].signal_value == signal) {
            *irq_line = tfm_core_irq_signals[i].irq_line;
            return IPC_SUCCESS;
        }
    }
    return IPC_ERROR_GENERIC;
}

void tfm_spm_psa_eoi(uint32_t *args)
{
    psa_signal_t irq_signal;
    IRQn_Type irq_line = (IRQn_Type) 0;
    int32_t ret;
    struct spm_partition_desc_t *partition = NULL;

    TFM_CORE_ASSERT(args != NULL);
    irq_signal = (psa_signal_t)args[0];

    /* It is a fatal error if passed signal indicates more than one signals. */
    if (!tfm_is_one_bit_set(irq_signal)) {
        tfm_core_panic();
    }

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    ret = get_irq_line_for_signal(partition->static_data->partition_id,
                                  irq_signal, &irq_line);
    /* It is a fatal error if passed signal is not an interrupt signal. */
    if (ret != IPC_SUCCESS) {
        tfm_core_panic();
    }

    /* It is a fatal error if passed signal is not currently asserted */
    if ((partition->runtime_data.signals & irq_signal) == 0) {
        tfm_core_panic();
    }

    partition->runtime_data.signals &= ~irq_signal;

    tfm_spm_hal_clear_pending_irq(irq_line);
    tfm_spm_hal_enable_irq(irq_line);
}

void tfm_spm_enable_irq(uint32_t *args)
{
    struct tfm_state_context_t *svc_ctx = (struct tfm_state_context_t *)args;
    psa_signal_t irq_signal = svc_ctx->r0;
    IRQn_Type irq_line = (IRQn_Type) 0;
    int32_t ret;
    struct spm_partition_desc_t *partition = NULL;

    /* It is a fatal error if passed signal indicates more than one signals. */
    if (!tfm_is_one_bit_set(irq_signal)) {
        tfm_core_panic();
    }

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    ret = get_irq_line_for_signal(partition->static_data->partition_id,
                                  irq_signal, &irq_line);
    /* It is a fatal error if passed signal is not an interrupt signal. */
    if (ret != IPC_SUCCESS) {
        tfm_core_panic();
    }

    tfm_spm_hal_enable_irq(irq_line);
}

void tfm_spm_disable_irq(uint32_t *args)
{
    struct tfm_state_context_t *svc_ctx = (struct tfm_state_context_t *)args;
    psa_signal_t irq_signal = svc_ctx->r0;
    IRQn_Type irq_line = (IRQn_Type) 0;
    int32_t ret;
    struct spm_partition_desc_t *partition = NULL;

    /* It is a fatal error if passed signal indicates more than one signals. */
    if (!tfm_is_one_bit_set(irq_signal)) {
        tfm_core_panic();
    }

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    ret = get_irq_line_for_signal(partition->static_data->partition_id,
                                  irq_signal, &irq_line);
    /* It is a fatal error if passed signal is not an interrupt signal. */
    if (ret != IPC_SUCCESS) {
        tfm_core_panic();
    }

    tfm_spm_hal_disable_irq(irq_line);
}

void tfm_spm_validate_caller(struct spm_partition_desc_t *p_cur_sp,
                             uint32_t *p_ctx, uint32_t exc_return,
                             bool ns_caller)
{
    uintptr_t stacked_ctx_pos;

    if (ns_caller) {
        /*
         * The background IRQ can't be supported, since if SP is executing,
         * the preempted context of SP can be different with the one who
         * preempts veneer.
         */
        if (p_cur_sp->static_data->partition_id != TFM_SP_NON_SECURE_ID) {
            tfm_core_panic();
        }

        /*
         * It is non-secure caller, check if veneer stack contains
         * multiple contexts.
         */
        stacked_ctx_pos = (uintptr_t)p_ctx +
                          sizeof(struct tfm_state_context_t) +
                          TFM_VENEER_STACK_GUARD_SIZE;

        if (is_stack_alloc_fp_space(exc_return)) {
#if defined (__FPU_USED) && (__FPU_USED == 1U)
            if (FPU->FPCCR & FPU_FPCCR_TS_Msk) {
                stacked_ctx_pos += TFM_ADDTIONAL_FP_CONTEXT_WORDS *
                                   sizeof(uint32_t);
            }
#endif
            stacked_ctx_pos += TFM_BASIC_FP_CONTEXT_WORDS * sizeof(uint32_t);
        }

        if (stacked_ctx_pos != p_cur_sp->runtime_data.sp_thrd.stk_top) {
            tfm_core_panic();
        }
    } else if (p_cur_sp->static_data->partition_id <= 0) {
        tfm_core_panic();
    }
}

void tfm_spm_request_handler(const struct tfm_state_context_t *svc_ctx)
{
    uint32_t *res_ptr = (uint32_t *)&svc_ctx->r0;
    uint32_t running_partition_flags = 0;
    const struct spm_partition_desc_t *partition = NULL;

    /* Check permissions on request type basis */

    switch (svc_ctx->r0) {
    case TFM_SPM_REQUEST_RESET_VOTE:
        partition = tfm_spm_get_running_partition();
        if (!partition) {
            tfm_core_panic();
        }
        running_partition_flags = partition->static_data->partition_flags;

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
