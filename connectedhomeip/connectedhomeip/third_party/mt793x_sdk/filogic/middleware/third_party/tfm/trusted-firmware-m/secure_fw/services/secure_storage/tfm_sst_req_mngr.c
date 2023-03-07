/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_sst_req_mngr.h"

#include <stdbool.h>
#include <stdint.h>

#include "psa/protected_storage.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "tfm_protected_storage.h"
#ifdef TFM_PSA_API
#include "psa/service.h"
#include "psa_manifest/tfm_secure_storage.h"
#include "flash_layout.h"
#endif

#ifndef TFM_PSA_API
#include "tfm_memory_utils.h"

static void *p_data;

/*
 * \brief Indicates whether SST has been initialised.
 */
static bool sst_is_init = false;

/*
 * \brief Initialises SST, if not already initialised.
 *
 * \note In library mode, initialisation is delayed until the first secure
 *       function call, as calls to the Crypto service are required for
 *       initialisation.
 *
 * \return PSA_SUCCESS if SST is initialised, PSA_ERROR_GENERIC_ERROR
 *         otherwise.
 */
static psa_status_t sst_check_init(void)
{
    if (!sst_is_init) {
        if (tfm_sst_init() != PSA_SUCCESS) {
            return PSA_ERROR_GENERIC_ERROR;
        }
        sst_is_init = true;
    }

    return PSA_SUCCESS;
}

psa_status_t tfm_sst_set_req(psa_invec *in_vec, size_t in_len,
                             psa_outvec *out_vec, size_t out_len)
{
    psa_storage_uid_t uid;
    uint32_t data_length;
    int32_t client_id;
    int32_t status;
    psa_storage_create_flags_t create_flags;

    if (sst_check_init() != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if ((in_len != 3) || (out_len != 0)) {
        /* The number of arguments are incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[0].len != sizeof(psa_storage_uid_t)) {
        /* The input argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[0].base);

    p_data = (void *)in_vec[1].base;
    data_length = in_vec[1].len;

    if (in_vec[2].len != sizeof(psa_storage_create_flags_t)) {
        /* The input argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    create_flags = *(psa_storage_create_flags_t *)in_vec[2].base;

    /* Get the caller's client ID */
    status = tfm_core_get_caller_client_id(&client_id);
    if (status != (int32_t)TFM_SUCCESS) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_sst_set(client_id, uid, data_length, create_flags);
}

psa_status_t tfm_sst_get_req(psa_invec *in_vec, size_t in_len,
                             psa_outvec *out_vec, size_t out_len)
{
    uint32_t data_offset;
    uint32_t data_size;
    int32_t client_id;
    psa_storage_uid_t uid;
    int32_t status;
    size_t *p_data_length;

    if (sst_check_init() != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if ((in_len != 2) || (out_len != 1)) {
        /* The number of arguments are incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[0].len != sizeof(psa_storage_uid_t)) {
        /* The input argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[0].base);

    if (in_vec[1].len != sizeof(data_offset)) {
        /* The input argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    data_offset = *(uint32_t *)in_vec[1].base;

    p_data = (void *)out_vec[0].base;
    data_size = out_vec[0].len;
    p_data_length = &out_vec[0].len;

    /* Get the caller's client ID */
    status = tfm_core_get_caller_client_id(&client_id);
    if (status != (int32_t)TFM_SUCCESS) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_sst_get(client_id, uid, data_offset, data_size, p_data_length);

}

psa_status_t tfm_sst_get_info_req(psa_invec *in_vec, size_t in_len,
                                  psa_outvec *out_vec, size_t out_len)
{
    int32_t client_id;
    psa_storage_uid_t uid;
    struct psa_storage_info_t *p_info;
    int32_t tfm_status;

    if (sst_check_init() != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if ((in_len != 1) || (out_len != 1)) {
        /* The number of arguments are incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_vec[0].len != sizeof(psa_storage_uid_t)) {
        /* The input argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[0].base);

    if (out_vec[0].len != sizeof(struct psa_storage_info_t)) {
        /* The output argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    p_info = (struct psa_storage_info_t *)out_vec[0].base;

    /* Get the caller's client ID */
    tfm_status = tfm_core_get_caller_client_id(&client_id);
    if (tfm_status != (int32_t)TFM_SUCCESS) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_sst_get_info(client_id, uid, p_info);
}

psa_status_t tfm_sst_remove_req(psa_invec *in_vec, size_t in_len,
                                psa_outvec *out_vec, size_t out_len)
{
    int32_t client_id;
    psa_storage_uid_t uid;
    int32_t tfm_status;

    if (sst_check_init() != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if ((in_len != 1) ||
        (in_vec[0].len != sizeof(psa_storage_uid_t)) ||
        (out_len != 0)) {
        /* The number of arguments/output argument size are incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    uid = *((psa_storage_uid_t *)in_vec[0].base);

    /* Get the caller's client ID */
    tfm_status = tfm_core_get_caller_client_id(&client_id);
    if (tfm_status != (int32_t)TFM_SUCCESS) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_sst_remove(client_id, uid);;
}

psa_status_t tfm_sst_get_support_req(psa_invec *in_vec, size_t in_len,
                                     psa_outvec *out_vec, size_t out_len)
{
    uint32_t *support_flags;

    (void)in_vec;

    if (sst_check_init() != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    if ((in_len != 0) || (out_len != 1)) {
        /* The number of arguments are incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (out_vec[0].len != sizeof(*support_flags)) {
        /* The output argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    support_flags = (uint32_t *)out_vec[0].base;

    *support_flags = tfm_sst_get_support();

    return PSA_SUCCESS;
}

#else /* !defined(TFM_PSA_API) */
typedef psa_status_t (*sst_func_t)(void);
static psa_msg_t msg;

static psa_status_t tfm_sst_set_ipc(void)
{
    psa_storage_uid_t uid;
    int32_t client_id;
    psa_storage_create_flags_t create_flags;
    size_t num = 0;

    client_id = msg.client_id;

    if (msg.in_size[0] != sizeof(psa_storage_uid_t) ||
        msg.in_size[2] != sizeof(psa_storage_create_flags_t)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg.handle, 0, &uid, msg.in_size[0]);
    if (num != msg.in_size[0]) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg.handle, 2, &create_flags, msg.in_size[2]);
    if (num != msg.in_size[2]) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_sst_set(client_id, uid, msg.in_size[1], create_flags);
}

static psa_status_t tfm_sst_get_ipc(void)
{
    psa_storage_uid_t uid;
    uint32_t data_offset;
    size_t num = 0;
    size_t p_data_length;

    if (msg.in_size[0] != sizeof(psa_storage_uid_t) ||
        msg.in_size[1] != sizeof(uint32_t)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg.handle, 0, &uid,  msg.in_size[0]);
    if (num != msg.in_size[0]) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg.handle, 1, &data_offset, msg.in_size[1]);
    if (num !=  msg.in_size[1]) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_sst_get(msg.client_id, uid, data_offset,  msg.out_size[0],
                       &p_data_length);
}

static psa_status_t tfm_sst_get_info_ipc(void)
{
    psa_storage_uid_t uid;

    struct psa_storage_info_t info;
    size_t num = 0;
    psa_status_t status;

    if (msg.in_size[0] != sizeof(psa_storage_uid_t) ||
        msg.out_size[0] != sizeof(struct psa_storage_info_t)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg.handle, 0, &uid, msg.in_size[0]);
    if (num != msg.in_size[0]) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    status = tfm_sst_get_info(msg.client_id, uid, &info);

    if (status == PSA_SUCCESS) {
        psa_write(msg.handle, 0, &info, msg.out_size[0]);
    }
    return status;
}

static psa_status_t tfm_sst_remove_ipc(void)
{
    psa_storage_uid_t uid;
    size_t num = 0;

    if (msg.in_size[0] != sizeof(psa_storage_uid_t)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    num = psa_read(msg.handle, 0, &uid, msg.in_size[0]);
    if (num != msg.in_size[0]) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    return tfm_sst_remove(msg.client_id, uid);
}

static psa_status_t tfm_sst_get_support_ipc(void)
{
    size_t out_size;
    uint32_t support_flags;

    out_size = msg.out_size[0];
    if (out_size != sizeof(support_flags)) {
        /* The output argument size is incorrect */
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    support_flags = tfm_sst_get_support();
    psa_write(msg.handle, 0, &support_flags, out_size);
    return PSA_SUCCESS;
}

/*
 * Fixme: Temporarily implement abort as infinite loop,
 * will replace it later.
 */
static void tfm_abort(void)
{
    while (1)
        ;
}

static void ps_signal_handle(psa_signal_t signal, sst_func_t pfn)
{
    psa_status_t status;

    status = psa_get(signal, &msg);
    switch (msg.type) {
    case PSA_IPC_CONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_CALL:
        status = pfn();
        psa_reply(msg.handle, status);
        break;
    case PSA_IPC_DISCONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        tfm_abort();
    }
}
#endif /* !defined(TFM_PSA_API) */

psa_status_t tfm_sst_req_mngr_init(void)
{
#ifdef TFM_PSA_API
    psa_signal_t signals = 0;

    if (tfm_sst_init() != PSA_SUCCESS) {
        tfm_abort();
    }

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & TFM_SST_SET_SIGNAL) {
            ps_signal_handle(TFM_SST_SET_SIGNAL, tfm_sst_set_ipc);
        } else if (signals & TFM_SST_GET_SIGNAL) {
            ps_signal_handle(TFM_SST_GET_SIGNAL, tfm_sst_get_ipc);
        } else if (signals & TFM_SST_GET_INFO_SIGNAL) {
            ps_signal_handle(TFM_SST_GET_INFO_SIGNAL, tfm_sst_get_info_ipc);
        } else if (signals & TFM_SST_REMOVE_SIGNAL) {
            ps_signal_handle(TFM_SST_REMOVE_SIGNAL, tfm_sst_remove_ipc);
        } else if (signals & TFM_SST_GET_SUPPORT_SIGNAL) {
            ps_signal_handle(TFM_SST_GET_SUPPORT_SIGNAL,
                             tfm_sst_get_support_ipc);
        } else {
            tfm_abort();
        }
    }
#else
    /* In library mode, initialisation is delayed until the first secure
     * function call, as calls to the Crypto service are required for
     * initialisation.
     */
    return PSA_SUCCESS;
#endif
}

psa_status_t sst_req_mngr_read_asset_data(uint8_t *out_data, uint32_t size)
{
#ifdef TFM_PSA_API
  size_t num = 0;
  num = psa_read(msg.handle, 1, out_data, size);
  if (num != size) {
      return PSA_ERROR_PROGRAMMER_ERROR;
  }
#else /* TFM_PSA_API */
  (void)tfm_memcpy(out_data, p_data, size);
#endif
  return PSA_SUCCESS;
}

void sst_req_mngr_write_asset_data(const uint8_t *in_data,
                                   uint32_t size)
{
#ifdef TFM_PSA_API
  psa_write(msg.handle, 0, in_data, size);
#else /* TFM_PSA_API */
  (void)tfm_memcpy(p_data, in_data, size);
#endif
}
