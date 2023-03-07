/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SPM_API_H__
#define __SPM_API_H__

/* This file contains the apis exported by the SPM to tfm core */
#include "tfm_api.h"
#include "spm_partition_defs.h"
#include "tfm_secure_api.h"
#include <stdbool.h>
#ifdef TFM_PSA_API
#include "tfm_list.h"
#include "tfm_wait.h"
#include "tfm_message_queue.h"
#include "tfm_secure_api.h"
#include "tfm_thread.h"
#endif

#define SPM_INVALID_PARTITION_IDX     (~0U)

/* Privileged definitions for partition thread mode */
#define TFM_PARTITION_PRIVILEGED_MODE   1
#define TFM_PARTITION_UNPRIVILEGED_MODE 0

enum spm_err_t {
    SPM_ERR_OK = 0,
    SPM_ERR_PARTITION_DB_NOT_INIT,
    SPM_ERR_PARTITION_ALREADY_ACTIVE,
    SPM_ERR_PARTITION_NOT_AVAILABLE,
    SPM_ERR_INVALID_PARAMETER,
    SPM_ERR_INVALID_CONFIG,
};

#define SPM_PARTITION_STATE_UNINIT       0
#define SPM_PARTITION_STATE_IDLE         1
#define SPM_PARTITION_STATE_RUNNING      2
#define SPM_PARTITION_STATE_HANDLING_IRQ 3
#define SPM_PARTITION_STATE_SUSPENDED    4
#define SPM_PARTITION_STATE_BLOCKED      5
#define SPM_PARTITION_STATE_CLOSED       6

#define SPM_PART_FLAG_APP_ROT 0x01
#define SPM_PART_FLAG_PSA_ROT 0x02
#define SPM_PART_FLAG_IPC     0x04

#define TFM_HANDLE_STATUS_IDLE          0
#define TFM_HANDLE_STATUS_ACTIVE        1
#define TFM_HANDLE_STATUS_CONNECT_ERROR 2

#ifndef TFM_PSA_API
/**
 * \brief Holds the iovec parameters that are passed to a service
 *
 * \note The size of the structure is (and have to be) multiple of 8 bytes
 */
struct iovec_args_t {
    psa_invec in_vec[PSA_MAX_IOVEC];   /*!< Array of psa_invec objects */
    size_t in_len;                     /*!< Number psa_invec objects in in_vec
                                        */
    psa_outvec out_vec[PSA_MAX_IOVEC]; /*!< Array of psa_outvec objects */
    size_t out_len;                    /*!< Number psa_outvec objects in out_vec
                                        */
};

/* The size of this struct must be multiple of 4 bytes as it is stacked to an
 * uint32_t[] array
 */
struct interrupted_ctx_stack_frame_t {
    uint32_t partition_state;
};

/* The size of this struct must be multiple of 4 bytes as it is stacked to an
 * uint32_t[] array
 */
struct handler_ctx_stack_frame_t {
    uint32_t partition_state;
    uint32_t caller_partition_idx;
};
#endif /* !define(TFM_PSA_API) */

/**
 * \brief Runtime context information of a partition
 */
struct spm_partition_runtime_data_t {
#ifdef TFM_PSA_API
    uint32_t signals;                   /* Service signals had been triggered*/
    struct tfm_event_t signal_evnt;     /* Event signal                      */
    struct tfm_list_node_t service_list;/* Service list                      */
    struct tfm_core_thread_t sp_thrd;   /* Thread object                     */
    uint32_t assigned_signals;          /* All assigned signals              */
#else /* TFM_PSA_API */
    uint32_t partition_state;
    uint32_t caller_partition_idx;
    int32_t caller_client_id;
    uint32_t stack_ptr;
    uint32_t lr;
    struct iovec_args_t iovec_args;
    psa_outvec *orig_outvec;
    uint32_t *ctx_stack_ptr;
#endif /* TFM_PSA_API */
    uint32_t signal_mask;               /*
                                         * Service signal mask passed by
                                         * psa_wait()
                                         */
};

#ifdef TFM_PSA_API

#define TFM_VERSION_POLICY_RELAXED      0
#define TFM_VERSION_POLICY_STRICT       1

#define TFM_CONN_HANDLE_MAX_NUM         16

/* RoT connection handle list */
struct tfm_conn_handle_t {
    void *rhandle;                      /* Reverse handle value              */
    uint32_t status;                    /*
                                         * Status of handle, three valid
                                         * options:
                                         * TFM_HANDLE_STATUS_ACTIVE,
                                         * TFM_HANDLE_STATUS_IDLE and
                                         * TFM_HANDLE_STATUS_CONNECT_ERROR
                                         */
    int32_t client_id;                  /*
                                         * Partition ID of the sender of the
                                         * message:
                                         *  - secure partition id;
                                         *  - non secure client endpoint id.
                                         */
    struct tfm_msg_body_t internal_msg; /* Internal message for message queue */
    struct tfm_spm_service_t *service;  /* RoT service pointer               */
    struct tfm_list_node_t list;        /* list node                         */
};

/* Service database defined by manifest */
struct tfm_spm_service_db_t {
    char *name;                     /* Service name                          */
    uint32_t partition_id;          /* Partition ID which service belong to  */
    psa_signal_t signal;            /* Service signal                        */
    uint32_t sid;                   /* Service identifier                    */
    bool non_secure_client;         /* If can be called by non secure client */
    uint32_t version;               /* Service version                       */
    uint32_t version_policy;        /* Service version policy                */
};

/* RoT Service data */
struct tfm_spm_service_t {
    const struct tfm_spm_service_db_t *service_db;/* Service database pointer */
    struct spm_partition_desc_t *partition;  /*
                                              * Point to secure partition
                                              * data
                                              */
    struct tfm_list_node_t handle_list;      /* Service handle list          */
    struct tfm_msg_queue_t msg_queue;        /* Message queue                */
    struct tfm_list_node_t list;             /* For list operation           */
};
#endif /* ifdef(TFM_PSA_API) */

/*********************** common definitions ***********************/

/**
 * \brief Returns the index of the partition with the given partition ID.
 *
 * \param[in] partition_id     Partition id
 *
 * \return the partition idx if partition_id is valid,
 *         \ref SPM_INVALID_PARTITION_IDX othervise
 */
uint32_t get_partition_idx(uint32_t partition_id);

/**
 * \brief Get the id of the partition for its index from the db
 *
 * \param[in] partition_idx     Partition index
 *
 * \return Partition ID for that partition
 *
 * \note This function doesn't check if partition_idx is valid.
 */
uint32_t tfm_spm_partition_get_partition_id(uint32_t partition_idx);

/**
 * \brief Get the flags associated with a partition
 *
 * \param[in] partition_idx     Partition index
 *
 * \return Flags associated with the partition
 *
 * \note This function doesn't check if partition_idx is valid.
 */
uint32_t tfm_spm_partition_get_flags(uint32_t partition_idx);

/**
 * \brief Initialize partition database
 *
 * \return Error code \ref spm_err_t
 */
enum spm_err_t tfm_spm_db_init(void);

/**
 * \brief Change the privilege mode for partition thread mode.
 *
 * \param[in] privileged        Privileged mode,
 *                                \ref TFM_PARTITION_PRIVILEGED_MODE
 *                                and \ref TFM_PARTITION_UNPRIVILEGED_MODE
 *
 * \note Barrier instructions are not called by this function, and if
 *       it is called in thread mode, it might be necessary to call
 *       them after this function returns.
 */
void tfm_spm_partition_change_privilege(uint32_t privileged);

/**
 * \brief                   Get the current partition mode.
 *
 * \param[in] partition_flags               Flags of current partition
 *
 * \retval TFM_PARTITION_PRIVILEGED_MODE    Privileged mode
 * \retval TFM_PARTITION_UNPRIVILEGED_MODE  Unprivileged mode
 */
uint32_t tfm_spm_partition_get_privileged_mode(uint32_t partition_flags);

/**
 * \brief                   Handle an SPM request by a secure service
 * \param[in] svc_ctx       The stacked SVC context
 */
void tfm_spm_request_handler(const struct tfm_state_context_t *svc_ctx);

/*********************** library definitions ***********************/

#ifndef TFM_PSA_API
/**
 * \brief Save interrupted partition context on ctx stack
 *
 * \param[in] partition_idx  Partition index
 *
 * \note This function doesn't check if partition_idx is valid.
 * \note This function doesn't whether the ctx stack overflows.
 */
void tfm_spm_partition_push_interrupted_ctx(uint32_t partition_idx);

/**
 * \brief Restores interrupted partition context on ctx stack
 *
 * \param[in] partition_idx  Partition index
 *
 * \note This function doesn't check if partition_idx is valid.
 * \note This function doesn't whether the ctx stack underflows.
 */
void tfm_spm_partition_pop_interrupted_ctx(uint32_t partition_idx);

/**
 * \brief Save handler partition context on ctx stack
 *
 * \param[in] partition_idx  Partition index
 *
 * \note This function doesn't check if partition_idx is valid.
 * \note This function doesn't whether the ctx stack overflows.
 */
void tfm_spm_partition_push_handler_ctx(uint32_t partition_idx);

/**
 * \brief Restores handler partition context on ctx stack
 *
 * \param[in] partition_idx  Partition index
 *
 * \note This function doesn't check if partition_idx is valid.
 * \note This function doesn't whether the ctx stack underflows.
 */
void tfm_spm_partition_pop_handler_ctx(uint32_t partition_idx);

/**
 * \brief Get the current runtime data of a partition
 *
 * \param[in] partition_idx     Partition index
 *
 * \return The runtime data of the specified partition
 *
 * \note This function doesn't check if partition_idx is valid.
 */
const struct spm_partition_runtime_data_t *
             tfm_spm_partition_get_runtime_data(uint32_t partition_idx);

/**
 * \brief Returns the index of the partition that has running state
 *
 * \return The index of the partition with the running state, if there is any
 *         set. 0 otherwise.
 */
uint32_t tfm_spm_partition_get_running_partition_idx(void);

/**
 * \brief Save stack pointer and link register for partition in database
 *
 * \param[in] partition_idx  Partition index
 * \param[in] stack_ptr      Stack pointer to be stored
 * \param[in] lr             Link register to be stored
 *
 * \note This function doesn't check if partition_idx is valid.
 */
void tfm_spm_partition_store_context(uint32_t partition_idx,
        uint32_t stack_ptr, uint32_t lr);

/**
 * \brief Set the current state of a partition
 *
 * \param[in] partition_idx  Partition index
 * \param[in] state          The state to be set
 *
 * \note This function doesn't check if partition_idx is valid.
 * \note The state has to have the value set of \ref spm_part_state_t.
 */
void tfm_spm_partition_set_state(uint32_t partition_idx, uint32_t state);

/**
 * \brief Set the caller partition index for a given partition
 *
 * \param[in] partition_idx        Partition index
 * \param[in] caller_partition_idx The index of the caller partition
 *
 * \note This function doesn't check if any of the partition_idxs are valid.
 */
void tfm_spm_partition_set_caller_partition_idx(uint32_t partition_idx,
                                                uint32_t caller_partition_idx);

/**
* \brief Set the caller client ID for a given partition
*
* \param[in] partition_idx        Partition index
* \param[in] caller_client_id     The ID of the calling client
*
* \note This function doesn't check if any of the partition_idxs are valid.
*/
void tfm_spm_partition_set_caller_client_id(uint32_t partition_idx,
                                            int32_t caller_client_id);


/**
 * \brief Set the iovec parameters for the partition
 *
 * \param[in] partition_idx  Partition index
 * \param[in] args           The arguments of the secure function
 *
 * args is expected to be of type int32_t[4] where:
 *   args[0] is in_vec
 *   args[1] is in_len
 *   args[2] is out_vec
 *   args[3] is out_len
 *
 * \return Error code \ref spm_err_t
 *
 * \note This function doesn't check if partition_idx is valid.
 * \note This function assumes that the iovecs that are passed in args are
 *       valid, and does no sanity check on them at all.
 */
enum spm_err_t tfm_spm_partition_set_iovec(uint32_t partition_idx,
                                           const int32_t *args);

/**
 * \brief Execute partition init function
 *
 * \return Error code \ref spm_err_t
 */
enum spm_err_t tfm_spm_partition_init(void);

/**
 * \brief Clears the context info from the database for a partition.
 *
 * \param[in] partition_idx     Partition index
 *
 * \note This function doesn't check if partition_idx is valid.
 */
void tfm_spm_partition_cleanup_context(uint32_t partition_idx);

/**
 * \brief Set the signal mask for a given partition
 *
 * \param[in] partition_idx        Partition index
 * \param[in] signal_mask          The signal mask to be set for the partition
 *
 * \note This function doesn't check if any of the partition_idxs are valid.
 */
void tfm_spm_partition_set_signal_mask(uint32_t partition_idx,
                                       uint32_t signal_mask);

/**
 * \brief Signal that secure partition initialisation is finished
 */
void tfm_spm_secure_api_init_done(void);

/**
 * \brief Called if veneer is running in thread mode
 */
uint32_t tfm_spm_partition_request_svc_handler(
        const uint32_t *svc_args, uint32_t lr);

/**
 * \brief Called when secure service returns
 */
uint32_t tfm_spm_partition_return_handler(uint32_t lr);

/**
 * \brief Called by secure service to check if client is secure
 */
void tfm_spm_validate_secure_caller_handler(uint32_t *svc_args);

/**
 * \brief Stores caller's client id in state context
 */
void tfm_spm_get_caller_client_id_handler(uint32_t *svc_args);

/**
 * \brief Checks if a secure service's access to a memory location is permitted
 */
void tfm_spm_memory_permission_check_handler(uint32_t *svc_args);

/**
 * \brief Check whether a buffer is ok for writing to by the privileged API
 *        function.
 *
 * This function checks whether the caller partition owns the buffer, can write
 * to it, and the buffer has proper alignment.
 *
 * \param[in] partition_idx     Partition index
 * \param[in] start_addr        The start address of the buffer
 * \param[in] len               The length of the buffer
 * \param[in] alignment         The expected alignment (in bits)
 *
 * \return 1 if the check passes, 0 otherwise.
 *
 * \note For a 0 long buffer the check fails.
 */
int32_t tfm_spm_check_buffer_access(uint32_t  partition_idx,
                                    void     *start_addr,
                                    size_t    len,
                                    uint32_t  alignment);

/**
 * \brief Handle deprivileged request
 */
extern uint32_t tfm_spm_depriv_req_handler(uint32_t *svc_args,
                                           uint32_t excReturn);

/**
 * \brief Handle request to return to privileged
 */
uint32_t tfm_spm_depriv_return_handler(uint32_t *irq_svc_args, uint32_t lr);

/**
 * \brief Handle IRQ enable request
 */
void tfm_spm_enable_irq_handler(uint32_t *svc_args);

/**
 * \brief Handle IRQ disable request
 */
void tfm_spm_disable_irq_handler(uint32_t *svc_args);

/**
 * \brief Handle signal wait request
 */
void tfm_spm_psa_wait(uint32_t *svc_args);

/**
 * \brief Handle request to record IRQ processed
 */
void tfm_spm_psa_eoi(uint32_t *svc_args);
#endif /* !defined(TFM_PSA_API) */

#ifdef TFM_PSA_API
/*************************** IPC definitions **************************/

/**
 * \brief   Get the running partition ID.
 *
 * \return  Returns the partition ID
 */
uint32_t tfm_spm_partition_get_running_partition_id(void);

/******************** Service handle management functions ********************/

/**
 * \brief                   Create connection handle for client connect
 *
 * \param[in] service       Target service context pointer
 * \param[in] client_id     Partition ID of the sender of the message
 *
 * \retval PSA_NULL_HANDLE  Create failed \ref PSA_NULL_HANDLE
 * \retval >0               Service handle created, \ref psa_handle_t
 */
psa_handle_t tfm_spm_create_conn_handle(struct tfm_spm_service_t *service,
                                        int32_t client_id);

/**
 * \brief                   Validate connection handle for client connect
 *
 * \param[in] conn_handle   Handle to be validated
 * \param[in] client_id     Partition ID of the sender of the message
 *
 * \retval IPC_SUCCESS        Success
 * \retval IPC_ERROR_GENERIC  Invalid handle
 */
int32_t tfm_spm_validate_conn_handle(psa_handle_t conn_handle,
                                     int32_t client_id);

/******************** Partition management functions *************************/

/**
 * \brief                   Get current running partition context.
 *
 * \retval NULL             Failed
 * \retval "Not NULL"       Return the parttion context pointer
 *                          \ref spm_partition_desc_t structures
 */
struct spm_partition_desc_t *tfm_spm_get_running_partition(void);

/**
 * \brief                   Get the service context by service ID.
 *
 * \param[in] sid           RoT Service identity
 *
 * \retval NULL             Failed
 * \retval "Not NULL"       Target service context pointer,
 *                          \ref tfm_spm_service_t structures
 */
struct tfm_spm_service_t *tfm_spm_get_service_by_sid(uint32_t sid);

/**
 * \brief                   Get the service context by connection handle.
 *
 * \param[in] conn_handle   Connection handle created by
 *                          tfm_spm_create_conn_handle()
 *
 * \retval NULL             Failed
 * \retval "Not NULL"       Target service context pointer,
 *                          \ref tfm_spm_service_t structures
 */
struct tfm_spm_service_t *
    tfm_spm_get_service_by_handle(psa_handle_t conn_handle);

/************************ Message functions **********************************/

/**
 * \brief                   Get message context by connect handle.
 *
 * \param[in] conn_handle   Service connect handle.
 *
 * \return                  The message body context pointer
 *                          \ref msg_body_t structures
 */
struct tfm_msg_body_t *
    tfm_spm_get_msg_buffer_from_conn_handle(psa_handle_t conn_handle);

/**
 * \brief                   Fill the message for PSA client call.
 *
 * \param[in] msg           Service Message Queue buffer pointer
 * \param[in] service       Target service context pointer, which can be
 *                          obtained by partition management functions
 * \prarm[in] handle        Connect handle return by psa_connect().
 * \param[in] type          Message type, PSA_IPC_CONNECT, PSA_IPC_CALL or
 *                          PSA_IPC_DISCONNECT
 * \param[in] client_id     Partition ID of the sender of the message
 * \param[in] invec         Array of input \ref psa_invec structures
 * \param[in] in_len        Number of input \ref psa_invec structures
 * \param[in] outvec        Array of output \ref psa_outvec structures
 * \param[in] out_len       Number of output \ref psa_outvec structures
 * \param[in] caller_outvec Array of caller output \ref psa_outvec structures
 */
void tfm_spm_fill_msg(struct tfm_msg_body_t *msg,
                      struct tfm_spm_service_t *service,
                      psa_handle_t handle,
                      int32_t type, int32_t client_id,
                      psa_invec *invec, size_t in_len,
                      psa_outvec *outvec, size_t out_len,
                      psa_outvec *caller_outvec);

/**
 * \brief                   Send message and wake up the SP who is waiting on
 *                          message queue, block the current thread and
 *                          scheduler triggered
 *
 * \param[in] service       Target service context pointer, which can be
 *                          obtained by partition management functions
 * \param[in] msg           message created by tfm_spm_create_msg()
 *                          \ref tfm_msg_body_t structures
 *
 * \retval IPC_SUCCESS      Success
 * \retval IPC_ERROR_BAD_PARAMETERS Bad parameters input
 * \retval IPC_ERROR_GENERIC Failed to enqueue message to service message queue
 */
int32_t tfm_spm_send_event(struct tfm_spm_service_t *service,
                           struct tfm_msg_body_t *msg);

/**
 * \brief                   Check the client version according to
 *                          version policy
 *
 * \param[in] service       Target service context pointer, which can be get
 *                          by partition management functions
 * \param[in] version       Client support version
 *
 * \retval IPC_SUCCESS      Success
 * \retval IPC_ERROR_BAD_PARAMETERS Bad parameters input
 * \retval IPC_ERROR_VERSION Check failed
 */
int32_t tfm_spm_check_client_version(struct tfm_spm_service_t *service,
                                     uint32_t version);

/**
 * \brief                   Check the client access authorization
 *
 * \param[in] sid           Target RoT Service identity
 * \param[in] service       Target service context pointer, which can be get
 *                          by partition management functions
 * \param[in] ns_caller     Whether from NS caller
 *
 * \retval IPC_SUCCESS      Success
 * \retval IPC_ERROR_GENERIC Authorization check failed
 */
int32_t tfm_spm_check_authorization(uint32_t sid,
                                    struct tfm_spm_service_t *service,
                                    bool ns_caller);

/**
 * \brief                      Check the memory reference is valid.
 *
 * \param[in] buffer           Pointer of memory reference
 * \param[in] len              Length of memory reference in bytes
 * \param[in] ns_caller        From non-secure caller
 * \param[in] access           Type of access specified by the
 *                             \ref tfm_memory_access_e
 * \param[in] privileged       Privileged mode or unprivileged mode:
 *                             \ref TFM_PARTITION_UNPRIVILEGED_MODE
 *                             \ref TFM_PARTITION_PRIVILEGED_MODE
 *
 * \retval IPC_SUCCESS               Success
 * \retval IPC_ERROR_BAD_PARAMETERS  Bad parameters input
 * \retval IPC_ERROR_MEMORY_CHECK    Check failed
 */
int32_t tfm_memory_check(const void *buffer, size_t len, bool ns_caller,
                         enum tfm_memory_access_e access,
                         uint32_t privileged);

/*
 * PendSV specified function.
 *
 * Parameters :
 *  p_actx        -    Architecture context storage pointer
 *
 * Notes:
 *  This is a staging API. Scheduler should be called in SPM finally and
 *  this function will be obsoleted later.
 */
void tfm_pendsv_do_schedule(struct tfm_arch_ctx_t *p_actx);

/**
 * \brief                      SPM initialization implementation
 *
 * \details                    This function must be called under handler mode.
 * \retval                     This function returns an EXC_RETURN value. Other
 *                             faults would panic the execution and never
 *                             returned.
 */
uint32_t tfm_spm_init(void);

/*
 * \brief This function get the current PSA RoT lifecycle state.
 *
 * \return state                The current security lifecycle state of the PSA
 *                              RoT. The PSA state and implementation state are
 *                              encoded as follows:
 * \arg                           state[15:8] – PSA lifecycle state
 * \arg                           state[7:0] – IMPLEMENTATION DEFINED state
 */
uint32_t tfm_spm_get_lifecycle_state(void);

/* Svcall for PSA Client APIs */

/**
 * \brief SVC handler for \ref psa_framework_version.
 *
 * \return version              The version of the PSA Framework implementation
 *                              that is providing the runtime services to the
 *                              caller.
 */
uint32_t tfm_spm_psa_framework_version(void);

/**
 * \brief SVC handler for \ref psa_version.
 *
 * \param[in] args              Include all input arguments: sid.
 * \param[in] ns_caller         If 'true', call from non-secure client.
 *                              Or from secure client.
 *
 * \retval PSA_VERSION_NONE     The RoT Service is not implemented, or the
 *                              caller is not permitted to access the service.
 * \retval > 0                  The version of the implemented RoT Service.
 */
uint32_t tfm_spm_psa_version(uint32_t *args, bool ns_caller);

/**
 * \brief SVC handler for \ref psa_connect.
 *
 * \param[in] args              Include all input arguments:
 *                              sid, version.
 * \param[in] ns_caller         If 'true', call from non-secure client.
 *                              Or from secure client.
 *
 * \retval PSA_SUCCESS          Success.
 * \retval PSA_ERROR_CONNECTION_REFUSED The SPM or RoT Service has refused the
 *                              connection.
 * \retval PSA_ERROR_CONNECTION_BUSY The SPM or RoT Service cannot make the
 *                              connection at the moment.
 * \retval "Does not return"    The RoT Service ID and version are not
 *                              supported, or the caller is not permitted to
 *                              access the service.
 */
psa_status_t tfm_spm_psa_connect(uint32_t *args, bool ns_caller);

/**
 * \brief SVC handler for \ref psa_call.
 *
 * \param[in] args              Include all input arguments:
 *                              handle, in_vec, in_len, out_vec, out_len.
 * \param[in] ns_caller         If 'true', call from non-secure client.
 *                              Or from secure client.
 * \param[in] lr                EXC_RETURN value of the SVC.
 *
 * \retval >=0                  RoT Service-specific status value.
 * \retval <0                   RoT Service-specific error code.
 * \retval PSA_ERROR_PROGRAMMER_ERROR The connection has been terminated by the
 *                              RoT Service. The call is a PROGRAMMER ERROR if
 *                              one or more of the following are true:
 * \arg                           An invalid handle was passed.
 * \arg                           The connection is already handling a request.
 * \arg                           type < 0.
 * \arg                           An invalid memory reference was provided.
 * \arg                           in_len + out_len > PSA_MAX_IOVEC.
 * \arg                           The message is unrecognized by the RoT
 *                                Service or incorrectly formatted.
 */
psa_status_t tfm_spm_psa_call(uint32_t *args, bool ns_caller, uint32_t lr);

/**
 * \brief SVC handler for \ref psa_close.
 *
 * \param[in] args              Include all input arguments: handle.
 * \param[in] ns_caller         If 'true', call from non-secure client.
 *                              Or from secure client.
 *
 * \retval void                 Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           An invalid handle was provided that is not
 *                                the null handle.
 * \arg                           The connection is handling a request.
 */
void tfm_spm_psa_close(uint32_t *args, bool ns_caller);

/* Svcall for PSA Service APIs */

/**
 * \brief SVC handler for \ref psa_wait.
 *
 * \param[in] args              Include all input arguments:
 *                              signal_mask, timeout.
 *
 * \retval >0                   At least one signal is asserted.
 * \retval 0                    No signals are asserted. This is only seen when
 *                              a polling timeout is used.
 */
psa_signal_t tfm_spm_psa_wait(uint32_t *args);

/**
 * \brief SVC handler for \ref psa_get.
 *
 * \param[in] args              Include all input arguments: signal, msg.
 *
 * \retval PSA_SUCCESS          Success, *msg will contain the delivered
 *                              message.
 * \retval PSA_ERROR_DOES_NOT_EXIST Message could not be delivered.
 * \retval "Does not return"    The call is invalid because one or more of the
 *                              following are true:
 * \arg                           signal has more than a single bit set.
 * \arg                           signal does not correspond to an RoT Service.
 * \arg                           The RoT Service signal is not currently
 *                                asserted.
 * \arg                           The msg pointer provided is not a valid memory
 *                                reference.
 */
psa_status_t tfm_spm_psa_get(uint32_t *args);

/**
 * \brief SVC handler for \ref psa_set_rhandle.
 *
 * \param[in] args              Include all input arguments:
 *                              msg_handle, rhandle.
 *
 * \retval void                 Success, rhandle will be provided with all
 *                              subsequent messages delivered on this
 *                              connection.
 * \retval "Does not return"    msg_handle is invalid.
 */
void tfm_spm_psa_set_rhandle(uint32_t *args);

/**
 * \brief SVC handler for \ref psa_read.
 *
 * \param[in] args              Include all input arguments:
 *                              msg_handle, invec_idx, buffer, num_bytes.
 *
 * \retval >0                   Number of bytes copied.
 * \retval 0                    There was no remaining data in this input
 *                              vector.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           msg_handle is invalid.
 * \arg                           msg_handle does not refer to a request
 *                                message.
 * \arg                           invec_idx is equal to or greater than
 *                                \ref PSA_MAX_IOVEC.
 * \arg                           the memory reference for buffer is invalid or
 *                                not writable.
 */
size_t tfm_spm_psa_read(uint32_t *args);

/**
 * \brief SVC handler for \ref psa_skip.
 *
 * \param[in] args              Include all input arguments:
 *                              msg_handle, invec_idx, num_bytes.
 *
 * \retval >0                   Number of bytes skipped.
 * \retval 0                    There was no remaining data in this input
 *                              vector.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           msg_handle is invalid.
 * \arg                           msg_handle does not refer to a request
 *                                message.
 * \arg                           invec_idx is equal to or greater than
 *                                \ref PSA_MAX_IOVEC.
 */
size_t tfm_spm_psa_skip(uint32_t *args);

/**
 * \brief SVC handler for \ref psa_write.
 *
 * \param[in] args              Include all input arguments:
 *                              msg_handle, outvec_idx, buffer, num_bytes.
 *
 * \retval void                 Success
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           msg_handle is invalid.
 * \arg                           msg_handle does not refer to a request
 *                                message.
 * \arg                           outvec_idx is equal to or greater than
 *                                \ref PSA_MAX_IOVEC.
 * \arg                           The memory reference for buffer is invalid.
 * \arg                           The call attempts to write data past the end
 *                                of the client output vector.
 */
void tfm_spm_psa_write(uint32_t *args);

/**
 * \brief SVC handler for \ref psa_reply.
 *
 * \param[in] args              Include all input arguments:
 *                              msg_handle, status.
 *
 * \retval void                 Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                         msg_handle is invalid.
 * \arg                         An invalid status code is specified for the
 *                              type of message.
 */
void tfm_spm_psa_reply(uint32_t *args);

/**
 * \brief SVC handler for \ref psa_notify.
 *
 * \param[in] args              Include all input arguments: partition_id.
 *
 * \retval void                 Success.
 * \retval "Does not return"    partition_id does not correspond to a Secure
 *                              Partition.
 */
void tfm_spm_psa_notify(uint32_t *args);

/**
 * \brief SVC handler for \ref psa_clear.
 *
 * \retval void                 Success.
 * \retval "Does not return"    The Secure Partition's doorbell signal is not
 *                              currently asserted.
 */
void tfm_spm_psa_clear(void);

/**
 * \brief SVC handler for \ref psa_eoi.
 *
 * \param[in] args              Include all input arguments: irq_signal.
 *
 * \retval void                 Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           irq_signal is not an interrupt signal.
 * \arg                           irq_signal indicates more than one signal.
 * \arg                           irq_signal is not currently asserted.
 */
void tfm_spm_psa_eoi(uint32_t *args);

/**
 * \brief SVC hander of enabling irq_line of the specified irq_signal.
 *
 * \param[in] args              Include all input arguments: irq_signal.
 *
 * \retval void                 Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           irq_signal is not an interrupt signal.
 * \arg                           irq_signal indicates more than one signal.
 */
void tfm_spm_enable_irq(uint32_t *args);

/**
 * \brief SVC hander of disabling irq_line of the specified irq_signal.
 *
 * \param[in] args              Include all input arguments: irq_signal.
 *
 * \retval void                 Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           irq_signal is not an interrupt signal.
 * \arg                           irq_signal indicates more than one signal.
 */
void tfm_spm_disable_irq(uint32_t *args);

/**
 * \brief Validate the whether NS caller re-enter.
 *
 * \param[in] p_cur_sp          Pointer to current partition.
 * \param[in] p_ctx             Pointer to current stack context.
 * \param[in] exc_return        EXC_RETURN value.
 * \param[in] ns_caller         If 'true', call from non-secure client.
 *                              Or from secure client.
 *
 * \retval void                 Success.
 */
void tfm_spm_validate_caller(struct spm_partition_desc_t *p_cur_sp,
                             uint32_t *p_ctx, uint32_t exc_return,
                             bool ns_caller);

/**
 * \brief Terminate execution within the calling Secure Partition and will not
 *        return.
 *
 * \retval "Does not return"
 */
void tfm_spm_psa_panic(void);

#endif /* defined(TFM_PSA_API) */

#endif /*__SPM_API_H__ */
