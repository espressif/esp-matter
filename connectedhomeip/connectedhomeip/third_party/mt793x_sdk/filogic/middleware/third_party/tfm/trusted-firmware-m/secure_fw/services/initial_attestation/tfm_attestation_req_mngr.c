/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "psa/initial_attestation.h"
#include "psa/crypto.h"
#include "attestation.h"

#ifdef TFM_PSA_API
#include "psa_manifest/tfm_initial_attestation.h"
#include "tfm_client.h"
#include "psa/service.h"
#include "region_defs.h"

#define ECC_P256_PUBLIC_KEY_SIZE PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(256)
#define IOVEC_LEN(x) (sizeof(x)/sizeof(x[0]))

typedef psa_status_t (*attest_func_t)(const psa_msg_t *msg);

int32_t g_attest_caller_id;

static psa_status_t psa_attest_get_token(const psa_msg_t *msg)
{
    psa_status_t status = PSA_SUCCESS;
    uint8_t challenge_buff[PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64];
    uint8_t token_buff[PSA_INITIAL_ATTEST_TOKEN_MAX_SIZE];
    uint32_t bytes_read = 0;
    size_t challenge_size = msg->in_size[0];
    size_t token_size = msg->out_size[0];
    psa_invec in_vec[] = {
        {challenge_buff, challenge_size}
    };
    psa_outvec out_vec[] = {
        {token_buff, token_size}
    };

    if (challenge_size > PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* store the client ID here for later use in service */
    g_attest_caller_id = msg->client_id;

    bytes_read = psa_read(msg->handle, 0,
                          challenge_buff, challenge_size);
    if (bytes_read != challenge_size) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    token_size = (token_size < PSA_INITIAL_ATTEST_TOKEN_MAX_SIZE) ?
                  token_size : PSA_INITIAL_ATTEST_TOKEN_MAX_SIZE;

    status = initial_attest_get_token(in_vec, IOVEC_LEN(in_vec),
                                      out_vec, IOVEC_LEN(out_vec));
    if (status == PSA_SUCCESS) {
        psa_write(msg->handle, 0, out_vec[0].base, out_vec[0].len);
    }

    return status;
}

static psa_status_t psa_attest_get_token_size(const psa_msg_t *msg)
{
    psa_status_t status = PSA_SUCCESS;
    size_t challenge_size;
    size_t token_size;
    size_t bytes_read = 0;
    psa_invec in_vec[] = {
        {&challenge_size, msg->in_size[0]}
    };
    psa_outvec out_vec[] = {
        {&token_size, msg->out_size[0]}
    };

    if (msg->in_size[0] != sizeof(challenge_size)
        || msg->out_size[0] != sizeof(token_size)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* store the client ID here for later use in service */
    g_attest_caller_id = msg->client_id;

    bytes_read = psa_read(msg->handle, 0,
                          &challenge_size, msg->in_size[0]);
    if (bytes_read != msg->in_size[0]) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = initial_attest_get_token_size(in_vec, IOVEC_LEN(in_vec),
                                           out_vec, IOVEC_LEN(out_vec));
    if (status == PSA_SUCCESS) {
        psa_write(msg->handle, 0, out_vec[0].base, out_vec[0].len);
    }

    return status;
}

static psa_status_t tfm_attest_get_public_key(const psa_msg_t *msg)
{
    psa_status_t status = PSA_SUCCESS;
    uint8_t key_buf[ECC_P256_PUBLIC_KEY_SIZE];
    size_t key_len;
    psa_ecc_curve_t curve_type;

    psa_outvec out_vec[] = {
        {.base = key_buf,     .len = sizeof(key_buf)},
        {.base = &curve_type, .len = sizeof(curve_type)},
        {.base = &key_len,    .len = sizeof(key_len)}
    };

    if (msg->out_size[1] != out_vec[1].len ||
        msg->out_size[2] != out_vec[2].len) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Store the client ID here for later use in service. */
    g_attest_caller_id = msg->client_id;

    status = initial_attest_get_public_key(NULL, 0,
                                           out_vec, IOVEC_LEN(out_vec));

    if (msg->out_size[0] < key_len) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (status == PSA_SUCCESS) {
        psa_write(msg->handle, 0, key_buf, key_len);
        psa_write(msg->handle, 1, &curve_type, out_vec[1].len);
        psa_write(msg->handle, 2, &key_len, out_vec[2].len);
    }

    return status;
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

static void attest_signal_handle(psa_signal_t signal, attest_func_t pfn)
{
    psa_msg_t msg;
    psa_status_t status;

    status = psa_get(signal, &msg);
    switch (msg.type) {
    case PSA_IPC_CONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_CALL:
        status = (psa_status_t)pfn(&msg);
        psa_reply(msg.handle, status);
        break;
    case PSA_IPC_DISCONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        tfm_abort();
    }
}
#endif

psa_status_t attest_partition_init(void)
{
    psa_status_t err = attest_init();
#ifdef TFM_PSA_API
    psa_signal_t signals;

    if (err != PSA_SUCCESS) {
        tfm_abort();
    }

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & TFM_ATTEST_GET_TOKEN_SIGNAL) {
            attest_signal_handle(TFM_ATTEST_GET_TOKEN_SIGNAL,
                                 psa_attest_get_token);
        } else if (signals & TFM_ATTEST_GET_TOKEN_SIZE_SIGNAL) {
            attest_signal_handle(TFM_ATTEST_GET_TOKEN_SIZE_SIGNAL,
                                 psa_attest_get_token_size);
        } else if (signals & TFM_ATTEST_GET_PUBLIC_KEY_SIGNAL) {
            attest_signal_handle(TFM_ATTEST_GET_PUBLIC_KEY_SIGNAL,
                                 tfm_attest_get_public_key);
        } else {
            tfm_abort();
        }
    }
#else
    return err;
#endif
}
