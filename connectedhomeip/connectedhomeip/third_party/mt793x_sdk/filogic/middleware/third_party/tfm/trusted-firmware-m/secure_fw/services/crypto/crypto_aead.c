/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

/* FixMe: Use PSA_ERROR_CONNECTION_REFUSED when performing parameter
 *        integrity checks but this will have to be revised
 *        when the full set of error codes mandated by PSA FF
 *        is available.
 */
#include "tfm_mbedcrypto_include.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
psa_status_t tfm_crypto_aead_encrypt(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;

    if ( !((in_len == 2) || (in_len == 3)) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    const struct tfm_crypto_aead_pack_input *aead_pack_input = &iov->aead_in;
    psa_key_handle_t key_handle = iov->key_handle;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *nonce = aead_pack_input->nonce;
    size_t nonce_length = aead_pack_input->nonce_length;
    const uint8_t *plaintext = in_vec[1].base;
    size_t plaintext_length = in_vec[1].len;
    uint8_t *ciphertext = out_vec[0].base;
    size_t ciphertext_size = out_vec[0].len;
    const uint8_t *additional_data = NULL;
    size_t additional_data_length = 0;

    /* Check if additional data has been passed and initialise it */
    if (in_len == 3) {
        additional_data = in_vec[2].base;
        additional_data_length = in_vec[2].len;
    }

    /* Initialise ciphertext_length to zero */
    out_vec[0].len = 0;

    status = tfm_crypto_check_handle_owner(key_handle, NULL);
    if (status == PSA_SUCCESS) {

        status = psa_aead_encrypt(key_handle, alg, nonce, nonce_length,
                                  additional_data, additional_data_length,
                                  plaintext, plaintext_length,
                                  ciphertext, ciphertext_size, &out_vec[0].len);
    }

    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_decrypt(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;

    if ( !((in_len == 2) || (in_len == 3)) || (out_len > 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    const struct tfm_crypto_aead_pack_input *aead_pack_input = &iov->aead_in;
    psa_key_handle_t key_handle = iov->key_handle;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *nonce = aead_pack_input->nonce;
    size_t nonce_length = aead_pack_input->nonce_length;
    const uint8_t *ciphertext = in_vec[1].base;
    size_t ciphertext_length = in_vec[1].len;
    uint8_t *plaintext = out_vec[0].base;
    size_t plaintext_size = out_vec[0].len;
    const uint8_t *additional_data = NULL;
    size_t additional_data_length = 0;

    /* Check if additional data has been passed and initialise it */
    if (in_len == 3) {
        additional_data = in_vec[2].base;
        additional_data_length = in_vec[2].len;
    }

    /* Initialise plaintext_length to zero */
    out_vec[0].len = 0;

    status = tfm_crypto_check_handle_owner(key_handle, NULL);
    if (status == PSA_SUCCESS) {

        status = psa_aead_decrypt(key_handle, alg, nonce, nonce_length,
                                  additional_data, additional_data_length,
                                  ciphertext, ciphertext_length,
                                  plaintext, plaintext_size, &out_vec[0].len);
    }

    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_encrypt_setup(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_aead_decrypt_setup(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_aead_abort(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_aead_finish(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_aead_generate_nonce(psa_invec in_vec[],
                                            size_t in_len,
                                            psa_outvec out_vec[],
                                            size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_aead_set_nonce(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_aead_set_lengths(psa_invec in_vec[],
                                         size_t in_len,
                                         psa_outvec out_vec[],
                                         size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_aead_update(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_aead_update_ad(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_aead_verify(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}
/*!@}*/
