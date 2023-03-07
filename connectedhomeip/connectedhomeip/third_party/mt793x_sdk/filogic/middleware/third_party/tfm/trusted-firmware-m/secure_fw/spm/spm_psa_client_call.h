/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PSA_CLIENT_CALL_H__
#define __TFM_PSA_CLIENT_CALL_H__

#include <stdint.h>
#include "psa/client.h"

/* Common handlers for PSA client calls */

/**
 * \brief handler for \ref psa_framework_version.
 *
 * \return version              The version of the PSA Framework implementation
 *                              that is providing the runtime services.
 */
uint32_t tfm_spm_client_psa_framework_version(void);

/**
 * \brief handler for \ref psa_version.
 *
 * \param[in] sid               RoT Service identity.
 * \param[in] ns_caller         If 'true', call from non-secure client.
 *                              Otherwise from secure client.
 *
 * \retval PSA_VERSION_NONE     The RoT Service is not implemented, or the
 *                              caller is not permitted to access the service.
 * \retval > 0                  The version of the implemented RoT Service.
 */
uint32_t tfm_spm_client_psa_version(uint32_t sid, bool ns_caller);

/**
 * \brief handler for \ref psa_connect.
 *
 * \param[in] sid               RoT Service identity.
 * \param[in] version           The version of the RoT Service.
 * \param[in] ns_caller         If 'true', call from non-secure client.
 *                              Otherwise from secure client.
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
psa_status_t tfm_spm_client_psa_connect(uint32_t sid, uint32_t version,
                                        bool ns_caller);

/**
 * \brief handler for \ref psa_call.
 *
 * \param[in] handle            Service handle to the established connection,
 *                              \ref psa_handle_t
 * \param[in] type              The request type.
 *                              Must be zero( \ref PSA_IPC_CALL) or positive.
 * \param[in] inptr             Array of input psa_invec structures.
 *                              \ref psa_invec
 * \param[in] in_num            Number of input psa_invec structures.
 *                              \ref psa_invec
 * \param[in] outptr            Array of output psa_outvec structures.
 *                              \ref psa_outvec
 * \param[in] out_num           Number of outut psa_outvec structures.
 *                              \ref psa_outvec
 * \param[in] ns_caller         If 'true', call from non-secure client.
 *                              Otherwise from secure client.
 * \param[in] privileged        Privileged mode or unprivileged mode:
 *                              \ref TFM_PARTITION_UNPRIVILEGED_MODE
 *                              \ref TFM_PARTITION_PRIVILEGED_MODE
 *
 * \retval PSA_SUCCESS          Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           An invalid handle was passed.
 * \arg                           The connection is already handling a request.
 * \arg                           An invalid memory reference was provided.
 * \arg                           in_num + out_num > PSA_MAX_IOVEC.
 * \arg                           The message is unrecognized by the RoT
 *                                Service or incorrectly formatted.
 */
psa_status_t tfm_spm_client_psa_call(psa_handle_t handle, int32_t type,
                                     const psa_invec *inptr, size_t in_num,
                                     psa_outvec *outptr, size_t out_num,
                                     bool ns_caller, uint32_t privileged);

/**
 * \brief handler for \ref psa_close.
 *
 * \param[in] handle            Service handle to the connection to be closed,
 *                              \ref psa_handle_t
 * \param[in] ns_caller         If 'true', call from non-secure client.
 *                              Otherwise from secure client.
 *
 * \retval void                 Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           An invalid handle was provided that is not
 *                                the null handle.
 * \arg                           The connection is handling a request.
 */
void tfm_spm_client_psa_close(psa_handle_t handle, bool ns_caller);

#endif
