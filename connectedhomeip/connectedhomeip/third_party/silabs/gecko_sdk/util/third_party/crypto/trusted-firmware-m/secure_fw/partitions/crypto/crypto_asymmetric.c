/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "tfm_mbedcrypto_include.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"
#include "tfm_crypto_private.h"

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
psa_status_t tfm_crypto_sign_message(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_ASYM_SIGN_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 0, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_id_t key_id = iov->key_id;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;
    uint8_t *signature = out_vec[0].base;
    size_t signature_size = out_vec[0].len;
    mbedtls_svc_key_id_t encoded_key;
    psa_status_t status;

    status = tfm_crypto_encode_id_and_owner(key_id, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_sign_message(encoded_key, alg, input, input_length,
                            signature, signature_size, &(out_vec[0].len));
#endif /* TFM_CRYPTO_ASYM_SIGN_MODULE_DISABLED */
}

psa_status_t tfm_crypto_verify_message(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
#ifdef TFM_CRYPTO_ASYM_SIGN_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    // No output.
    (void)out_vec;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 3, out_len, 0, 0);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t key_id = iov->key_id;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;
    const uint8_t *signature = in_vec[2].base;
    size_t signature_length = in_vec[2].len;
    mbedtls_svc_key_id_t encoded_key;
    psa_status_t status;

    status = tfm_crypto_encode_id_and_owner(key_id, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_verify_message(encoded_key, alg, input, input_length,
                              signature, signature_length);
#endif /* TFM_CRYPTO_ASYM_SIGN_MODULE_DISABLED */
}

psa_status_t tfm_crypto_sign_hash(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
#ifdef TFM_CRYPTO_ASYM_SIGN_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 0, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_id_t key_id = iov->key_id;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *hash = in_vec[1].base;
    size_t hash_length = in_vec[1].len;
    uint8_t *signature = out_vec[0].base;
    size_t signature_size = out_vec[0].len;
    mbedtls_svc_key_id_t encoded_key;
    psa_status_t status;

    status = tfm_crypto_encode_id_and_owner(key_id, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_sign_hash(encoded_key, alg, hash, hash_length,
                         signature, signature_size, &(out_vec[0].len));
#endif /* TFM_CRYPTO_ASYM_SIGN_MODULE_DISABLED */
}

psa_status_t tfm_crypto_verify_hash(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_ASYM_SIGN_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    // No output.
    (void)out_vec;
    
    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 3, out_len, 0, 0);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t key_id = iov->key_id;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *hash = in_vec[1].base;
    size_t hash_length = in_vec[1].len;
    const uint8_t *signature = in_vec[2].base;
    size_t signature_length = in_vec[2].len;
    mbedtls_svc_key_id_t encoded_key;
    psa_status_t status;

    status = tfm_crypto_encode_id_and_owner(key_id, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_verify_hash(encoded_key, alg, hash, hash_length,
                           signature, signature_length);
#endif /* TFM_CRYPTO_ASYM_SIGN_MODULE_DISABLED */
}

psa_status_t tfm_crypto_asymmetric_encrypt(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
#ifdef TFM_CRYPTO_ASYM_ENCRYPT_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 3, out_len, 0, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_id_t key_id = iov->key_id;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;
    const uint8_t *salt = in_vec[2].base;
    size_t salt_length = in_vec[2].len;
    uint8_t *output = out_vec[0].base;
    size_t output_size = out_vec[0].len;
    psa_key_type_t type;
    size_t key_bits;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t encoded_key;

    status = tfm_crypto_encode_id_and_owner(key_id, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_get_key_attributes(encoded_key, &key_attributes);
    if (status != PSA_SUCCESS) {
        return status;
    }

    key_bits = psa_get_key_bits(&key_attributes);
    type = psa_get_key_type(&key_attributes);

    psa_reset_key_attributes(&key_attributes);

    /* Check that the output buffer is large enough */
    if (output_size < PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE(type, key_bits, alg)) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    return psa_asymmetric_encrypt(encoded_key, alg, input, input_length,
                                  salt, salt_length,
                                  output, output_size, &(out_vec[0].len));
#endif /* TFM_CRYPTO_ASYM_ENCRYPT_MODULE_DISABLED */
}

psa_status_t tfm_crypto_asymmetric_decrypt(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
#ifdef TFM_CRYPTO_ASYM_ENCRYPT_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 3, out_len, 0, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t key_id = iov->key_id;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;
    const uint8_t *salt = in_vec[2].base;
    size_t salt_length = in_vec[2].len;
    uint8_t *output = out_vec[0].base;
    size_t output_size = out_vec[0].len;
    psa_status_t status;
    mbedtls_svc_key_id_t encoded_key;

    status = tfm_crypto_encode_id_and_owner(key_id, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_asymmetric_decrypt(encoded_key, alg, input, input_length,
                                  salt, salt_length,
                                  output, output_size, &(out_vec[0].len));
#endif /* TFM_CRYPTO_ASYM_ENCRYPT_MODULE_DISABLED */
}
/*!@}*/
