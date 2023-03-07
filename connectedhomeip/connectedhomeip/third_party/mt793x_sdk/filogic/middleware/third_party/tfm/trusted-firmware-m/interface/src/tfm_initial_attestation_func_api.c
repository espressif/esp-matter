/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/initial_attestation.h"
#include "tfm_veneers.h"
#include "tfm_ns_interface.h"
#include "psa/client.h"
#include "psa/crypto_types.h"

#define IOVEC_LEN(x) (sizeof(x)/sizeof(x[0]))

psa_status_t
psa_initial_attest_get_token(const uint8_t *auth_challenge,
                             size_t         challenge_size,
                             uint8_t       *token_buf,
                             size_t         token_buf_size,
                             size_t        *token_size)
{

    int32_t res;

    psa_invec in_vec[] = {
        {auth_challenge, challenge_size}
    };
    psa_outvec out_vec[] = {
        {token_buf, token_buf_size}
    };

    res = tfm_ns_interface_dispatch(
                               (veneer_fn)tfm_initial_attest_get_token_veneer,
                               (uint32_t)in_vec,  IOVEC_LEN(in_vec),
                               (uint32_t)out_vec, IOVEC_LEN(out_vec));

    if (res == (int32_t)PSA_SUCCESS) {
        *token_size = out_vec[0].len;
    }

    return res;
}

psa_status_t
psa_initial_attest_get_token_size(size_t  challenge_size,
                                  size_t *token_size)
{
    psa_invec in_vec[] = {
        {&challenge_size, sizeof(challenge_size)}
    };
    psa_outvec out_vec[] = {
        {token_size, sizeof(size_t)}
    };

    return tfm_ns_interface_dispatch(
                            (veneer_fn)tfm_initial_attest_get_token_size_veneer,
                            (uint32_t)in_vec,  IOVEC_LEN(in_vec),
                            (uint32_t)out_vec, IOVEC_LEN(out_vec));
}

psa_status_t
tfm_initial_attest_get_public_key(uint8_t         *public_key,
                                  size_t           public_key_buf_size,
                                  size_t          *public_key_len,
                                  psa_ecc_curve_t *elliptic_curve_type)
{
    int32_t res;

    psa_outvec out_vec[] = {
        {.base = public_key,          .len = public_key_buf_size},
        {.base = elliptic_curve_type, .len = sizeof(*elliptic_curve_type)},
        {.base = public_key_len,      .len = sizeof(*public_key_len)}
    };

    res = tfm_ns_interface_dispatch(
                        (veneer_fn)tfm_initial_attest_get_public_key_veneer,
                        (uint32_t)NULL,  0,
                        (uint32_t)out_vec, IOVEC_LEN(out_vec));

    return (psa_status_t) res;
}
