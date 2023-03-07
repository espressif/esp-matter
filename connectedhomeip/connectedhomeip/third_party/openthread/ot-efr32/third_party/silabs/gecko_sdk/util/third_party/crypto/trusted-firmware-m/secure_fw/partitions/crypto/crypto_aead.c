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
psa_status_t tfm_crypto_aead_encrypt(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 3, out_len, 0, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    const struct tfm_crypto_aead_pack_input *aead_pack_input = &iov->aead_in;
    psa_key_id_t key_id = iov->key_id;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *nonce = aead_pack_input->nonce;
    size_t nonce_length = aead_pack_input->nonce_length;
    const uint8_t *plaintext = in_vec[1].base;
    size_t plaintext_length = in_vec[1].len;
    uint8_t *ciphertext = out_vec[0].base;
    size_t ciphertext_size = out_vec[0].len;
    const uint8_t *additional_data = in_vec[2].base;
    size_t additional_data_length = in_vec[2].len;
    mbedtls_svc_key_id_t encoded_key;

    /* Initialise ciphertext_length to zero. */
    out_vec[0].len = 0;

    status = tfm_crypto_encode_id_and_owner(key_id, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_aead_encrypt(encoded_key, alg, nonce, nonce_length,
                            additional_data, additional_data_length,
                            plaintext, plaintext_length,
                            ciphertext, ciphertext_size, &out_vec[0].len);
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_decrypt(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 3, out_len, 0, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    const struct tfm_crypto_aead_pack_input *aead_pack_input = &iov->aead_in;
    psa_key_id_t key_id = iov->key_id;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *nonce = aead_pack_input->nonce;
    size_t nonce_length = aead_pack_input->nonce_length;
    const uint8_t *ciphertext = in_vec[1].base;
    size_t ciphertext_length = in_vec[1].len;
    uint8_t *plaintext = out_vec[0].base;
    size_t plaintext_size = out_vec[0].len;
    const uint8_t *additional_data = in_vec[2].base;
    size_t additional_data_length = in_vec[2].len;
    mbedtls_svc_key_id_t encoded_key;

    /* Initialise plaintext_length to zero. */
    out_vec[0].len = 0;

    status = tfm_crypto_encode_id_and_owner(key_id, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_aead_decrypt(encoded_key, alg, nonce, nonce_length,
                            additional_data, additional_data_length,
                            ciphertext, ciphertext_length,
                            plaintext, plaintext_size, &out_vec[0].len);
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_encrypt_setup(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_aead_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((out_vec[0].len != sizeof(uint32_t)) ||
        (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    psa_key_id_t key_id = iov->key_id;
    psa_algorithm_t alg = iov->alg;

    /* Allocate the operation context in the secure world */
    status = tfm_crypto_operation_alloc(TFM_CRYPTO_AEAD_OPERATION,
                                        &handle,
                                        (void **)&operation);
    if (status != PSA_SUCCESS) {
#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)
        if (status == PSA_ERROR_BAD_STATE) {
            /* Invalidate the handle and abort the operation since the setup functon
               never gets called to perform the proper abort operation */
            /* Release the operation context, ignore if the operation fails. */
            (void)tfm_crypto_operation_release(handle_out, true);
        }
#endif
        return status;
    }
    *handle_out = handle;

    status = psa_aead_encrypt_setup(operation, key_id, alg);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    return status;

exit:
    /* Release the operation context, ignore if the operation fails. */
    (void)tfm_crypto_operation_release(handle_out, true);
    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_decrypt_setup(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_aead_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((out_vec[0].len != sizeof(uint32_t)) ||
        (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    psa_key_id_t key_id = iov->key_id;
    psa_algorithm_t alg = iov->alg;

    /* Allocate the operation context in the secure world */
    status = tfm_crypto_operation_alloc(TFM_CRYPTO_AEAD_OPERATION,
                                        &handle,
                                        (void **)&operation);
    if (status != PSA_SUCCESS) {
#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)
        if (status == PSA_ERROR_BAD_STATE) {
            /* Invalidate the handle and abort the operation since the setup functon
               never gets called to perform the proper abort operation */
            /* Release the operation context, ignore if the operation fails. */
            (void)tfm_crypto_operation_release(handle_out, true);
        }
#endif
        return status;
    }
    *handle_out = handle;

    status = psa_aead_decrypt_setup(operation, key_id, alg);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    return status;

exit:
    /* Release the operation context, ignore if the operation fails. */
    (void)tfm_crypto_operation_release(handle_out, true);
    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_abort(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_aead_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_AEAD_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        /* Operation does not exist, so abort has no effect */
        return PSA_SUCCESS;
    }

    status = psa_aead_abort(operation);

    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out, true);
        return status;
    }

    /* The abort() function is called by the underlying crypto function
       so just invalidate the operation */
    return tfm_crypto_operation_release(handle_out, false);
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_finish(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_aead_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 3);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    uint8_t *ciphertext = out_vec[1].base;
    size_t ciphertext_size = out_vec[1].len;
    uint8_t *tag = out_vec[2].base;
    size_t tag_size = out_vec[2].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Initialise the ciphertext_length and tag_length to zero */
    out_vec[1].len = 0;
    out_vec[2].len = 0;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_AEAD_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_aead_finish(operation,
                             ciphertext, ciphertext_size, &out_vec[1].len,
                             tag, tag_size, &out_vec[2].len);
    /* The abort() function is called by the underlying crypto function
       so just indicate that the operation is invalid. */
    (void)tfm_crypto_operation_release(handle_out, false);

    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_generate_nonce(psa_invec in_vec[],
                                            size_t in_len,
                                            psa_outvec out_vec[],
                                            size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_aead_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 2);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    uint8_t *nonce = out_vec[1].base;
    size_t nonce_size = out_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_AEAD_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *handle_out = handle;

    status = psa_aead_generate_nonce(operation, nonce, nonce_size, &out_vec[1].len);
    if (status != PSA_SUCCESS) {
        /* If the operation failed, the abort() function is called by the underlying crypto function
           so just indicate that the operation is invalid. */
        (void)tfm_crypto_operation_release(handle_out, false);
    }
    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_set_nonce(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_aead_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    const uint8_t *nonce = in_vec[1].base;
    size_t nonce_length = in_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_AEAD_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_aead_set_nonce(operation, nonce, nonce_length);
    if (status != PSA_SUCCESS) {
        /* If the operation failed, the abort() function is called by the underlying crypto function
           so just indicate that the operation is invalid. */
        (void)tfm_crypto_operation_release(handle_out, false);
    }
    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_set_lengths(psa_invec in_vec[],
                                         size_t in_len,
                                         psa_outvec out_vec[],
                                         size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_aead_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 3, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    const size_t *ad_length = in_vec[1].base;
    const size_t *plaintext_length = in_vec[2].base;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_AEAD_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_aead_set_lengths(operation, *ad_length, *plaintext_length);
    if (status != PSA_SUCCESS) {
        /* If the operation failed, the abort() function is called by the underlying crypto function
           so just indicate that the operation is invalid. */
        (void)tfm_crypto_operation_release(handle_out, false);
    }
    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_update(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_aead_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 1, 2);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;
    uint8_t *output = out_vec[1].base;
    size_t output_size = out_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Initialise the output_length to zero */
    out_vec[1].len = 0;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_AEAD_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_aead_update(operation, input, input_length,
                             output, output_size, &out_vec[1].len);
    if (status != PSA_SUCCESS) {
        /* If the operation failed, the abort() function is called by the underlying crypto function
           so just indicate that the operation is invalid. */
        (void)tfm_crypto_operation_release(handle_out, false);
    }

    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_update_ad(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_aead_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_AEAD_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_aead_update_ad(operation, input, input_length);
    if (status != PSA_SUCCESS) {
        /* If the operation failed, the abort() function is called by the underlying crypto function
           so just indicate that the operation is invalid. */
        (void)tfm_crypto_operation_release(handle_out, false);
    }

    return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}

psa_status_t tfm_crypto_aead_verify(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_AEAD_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_aead_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 1, 2);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    const uint8_t *tag = in_vec[1].base;
    size_t tag_length = in_vec[1].len;
    uint8_t *plaintext = out_vec[1].base;
    size_t plaintext_size = out_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Initialise the output_length to zero */
    out_vec[1].len = 0;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_AEAD_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_aead_verify(operation,
                             plaintext, plaintext_size, &out_vec[1].len,
                             tag, tag_length);

    /* The abort() function is called by the underlying crypto function
       so just indicate that the operation is invalid. */
    (void)tfm_crypto_operation_release(handle_out, false);

     return status;
#endif /* TFM_CRYPTO_AEAD_MODULE_DISABLED */
}
/*!@}*/
