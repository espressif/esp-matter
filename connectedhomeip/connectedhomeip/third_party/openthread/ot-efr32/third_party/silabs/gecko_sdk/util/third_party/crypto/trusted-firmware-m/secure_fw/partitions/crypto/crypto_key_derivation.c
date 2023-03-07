/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "tfm_mbedcrypto_include.h"

#if !defined(TFM_CONFIG_SL_SECURE_LIBRARY)
/* Required for mbedtls_calloc in tfm_crypto_huk_derivation_input_bytes */
#include "mbedtls/platform.h"
#endif

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"
#include "tfm_memory_utils.h"

#if !defined(TFM_CONFIG_SL_SECURE_LIBRARY)
#include "tfm_plat_crypto_keys.h"
#endif
#include "tfm_crypto_private.h"

#ifdef TFM_PARTITION_TEST_PS
#include "psa_manifest/pid.h"
#endif /* TFM_PARTITION_TEST_PS */

#ifndef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED

#if !defined(TFM_CONFIG_SL_SECURE_LIBRARY)
static psa_status_t tfm_crypto_huk_derivation_setup(
                                      psa_key_derivation_operation_t *operation,
                                      psa_algorithm_t alg)
{
    (void) alg;
    operation->MBEDTLS_PRIVATE(alg) = TFM_CRYPTO_ALG_HUK_DERIVATION;
    return PSA_SUCCESS;
}

static psa_status_t tfm_crypto_huk_derivation_input_bytes(
                                      psa_key_derivation_operation_t *operation,
                                      psa_key_derivation_step_t step,
                                      const uint8_t *data,
                                      size_t data_length)
{
    psa_status_t status;
    int32_t partition_id;
    psa_tls12_prf_key_derivation_t *tls12_prf;

    if (step != PSA_KEY_DERIVATION_INPUT_LABEL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Concatenate the caller's partition ID with the supplied label to prevent
     * two different partitions from deriving the same key.
     */
    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

#ifdef TFM_PARTITION_TEST_PS
    /* The PS tests run some operations under the wrong partition ID - this
     * causes the key derivation to change.
     */
    if (partition_id == TFM_SP_PS_TEST) {
        partition_id = TFM_SP_PS;
    }
#endif /* TFM_PARTITION_TEST_PS */

    /* Put the label in the tls12_prf ctx to make it available in the output key
     * step.
     */
    tls12_prf = &(operation->MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(tls12_prf));
    tls12_prf->MBEDTLS_PRIVATE(label) =
                 mbedtls_calloc(1, sizeof(partition_id) + data_length);
    if (tls12_prf->MBEDTLS_PRIVATE(label) == NULL) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }
    (void)tfm_memcpy(tls12_prf->MBEDTLS_PRIVATE(label), &partition_id,
                     sizeof(partition_id));
    (void)tfm_memcpy(tls12_prf->MBEDTLS_PRIVATE(label) + sizeof(partition_id),
                     data, data_length);
    tls12_prf->MBEDTLS_PRIVATE(label_length) = sizeof(partition_id) +
                                               data_length;

    return PSA_SUCCESS;
}

static psa_status_t tfm_crypto_huk_derivation_output_key(
                                      const psa_key_attributes_t *attributes,
                                      psa_key_derivation_operation_t *operation,
                                      mbedtls_svc_key_id_t *key_id)
{
    enum tfm_plat_err_t err;
    size_t bytes = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));
    psa_tls12_prf_key_derivation_t *tls12_prf =
                &(operation->MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(tls12_prf));

    if (sizeof(tls12_prf->MBEDTLS_PRIVATE(output_block)) < bytes) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    /* Derive key material from the HUK and output it to the operation buffer */
    err = tfm_plat_get_huk_derived_key(tls12_prf->MBEDTLS_PRIVATE(label),
                                       tls12_prf->MBEDTLS_PRIVATE(label_length),
                                       NULL, 0,
                                       tls12_prf->MBEDTLS_PRIVATE(output_block),
                                       bytes);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    return psa_import_key(attributes, tls12_prf->MBEDTLS_PRIVATE(output_block),
                          bytes, key_id);
}

static psa_status_t tfm_crypto_huk_derivation_abort(
                                      psa_key_derivation_operation_t *operation)
{
    psa_tls12_prf_key_derivation_t *tls12_prf =
                &(operation->MBEDTLS_PRIVATE(ctx).MBEDTLS_PRIVATE(tls12_prf));

    if (tls12_prf->MBEDTLS_PRIVATE(label) != NULL) {
        (void)tfm_memset(tls12_prf->MBEDTLS_PRIVATE(label), 0,
                         tls12_prf->MBEDTLS_PRIVATE(label_length));
        mbedtls_free(tls12_prf->MBEDTLS_PRIVATE(label));
    }

    (void)tfm_memset(operation, 0, sizeof(*operation));

    return PSA_SUCCESS;
}

#endif /* !defined(TFM_CONFIG_SL_SECURE_LIBRARY) */

#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
psa_status_t tfm_crypto_key_derivation_setup(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_key_derivation_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((out_vec[0].len != sizeof(uint32_t)) ||
        (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    psa_algorithm_t alg = iov->alg;

    /* Allocate the operation context in the secure world */
    status = tfm_crypto_operation_alloc(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                        &handle,
                                        (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *handle_out = handle;

#if !defined(TFM_CONFIG_SL_SECURE_LIBRARY)
    if (alg == TFM_CRYPTO_ALG_HUK_DERIVATION) {
        status = tfm_crypto_huk_derivation_setup(operation, alg);
    } else
#endif
    {
        status = psa_key_derivation_setup(operation, alg);
    }
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out, true);
        return status;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_get_capacity(psa_invec in_vec[],
                                                    size_t in_len,
                                                    psa_outvec out_vec[],
                                                    size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(size_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    size_t *capacity = out_vec[0].base;
    psa_key_derivation_operation_t *operation = NULL;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        *capacity = 0;
        return status;
    }

    return psa_key_derivation_get_capacity(operation, capacity);
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_set_capacity(psa_invec in_vec[],
                                                    size_t in_len,
                                                    psa_outvec out_vec[],
                                                    size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;
    psa_status_t status;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 0);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    size_t capacity = iov->capacity;
    psa_key_derivation_operation_t *operation = NULL;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_key_derivation_set_capacity(operation, capacity);
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_input_bytes(psa_invec in_vec[],
                                                   size_t in_len,
                                                   psa_outvec out_vec[],
                                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;
    psa_status_t status;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 0, 0);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    psa_key_derivation_step_t step = iov->step;
    const uint8_t *data = in_vec[1].base;
    size_t data_length = in_vec[1].len;
    psa_key_derivation_operation_t *operation = NULL;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

#if !defined(TFM_CONFIG_SL_SECURE_LIBRARY)
    if (operation->MBEDTLS_PRIVATE(alg) == TFM_CRYPTO_ALG_HUK_DERIVATION) {
        return tfm_crypto_huk_derivation_input_bytes(operation, step, data,
                                                     data_length);
    } else
#endif
    {
        return psa_key_derivation_input_bytes(operation, step, data,
                                              data_length);
    }
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_output_bytes(psa_invec in_vec[],
                                                    size_t in_len,
                                                    psa_outvec out_vec[],
                                                    size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    uint8_t *output = out_vec[0].base;
    size_t output_length = out_vec[0].len;
    psa_key_derivation_operation_t *operation = NULL;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_key_derivation_output_bytes(operation, output, output_length);
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_input_key(psa_invec in_vec[],
                                                 size_t in_len,
                                                 psa_outvec out_vec[],
                                                 size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;
    psa_status_t status;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 0);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    psa_key_id_t key_id = iov->key_id;
    psa_key_derivation_step_t step = iov->step;
    psa_key_derivation_operation_t *operation = NULL;
    mbedtls_svc_key_id_t encoded_key;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_encode_id_and_owner(key_id, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_key_derivation_input_key(operation, step, encoded_key);
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_output_key(psa_invec in_vec[],
                                                  size_t in_len,
                                                  psa_outvec out_vec[],
                                                  size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 2, 2, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(struct psa_client_key_attributes_s)) ||
        (out_vec[0].len != sizeof(psa_key_id_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    const struct psa_client_key_attributes_s *client_key_attr = in_vec[1].base;
    psa_key_derivation_operation_t *operation = NULL;
    psa_key_id_t *key_handle = out_vec[0].base;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    int32_t partition_id;
    mbedtls_svc_key_id_t encoded_key;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_key_attributes_from_client(client_key_attr,
                                                   partition_id,
                                                   &key_attributes);
    if (status != PSA_SUCCESS) {
        return status;
    }

#if !defined(TFM_CONFIG_SL_SECURE_LIBRARY)
    if (operation->MBEDTLS_PRIVATE(alg) == TFM_CRYPTO_ALG_HUK_DERIVATION) {
        status = tfm_crypto_huk_derivation_output_key(&key_attributes,
                                                      operation, &encoded_key);
    } else
#endif
    {
        status = psa_key_derivation_output_key(&key_attributes, operation,
                                               &encoded_key);
    }

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    *key_handle = encoded_key.MBEDTLS_PRIVATE(key_id);
#else
    *key_handle = encoded_key;
#endif
    return status;
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_abort(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    psa_key_derivation_operation_t *operation = NULL;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        /* Operation does not exist, so abort has no effect */
        return PSA_SUCCESS;
    }

    *handle_out = handle;

#if !defined(TFM_CONFIG_SL_SECURE_LIBRARY)
    if (operation->MBEDTLS_PRIVATE(alg) == TFM_CRYPTO_ALG_HUK_DERIVATION) {
        status = tfm_crypto_huk_derivation_abort(operation);
    } else
#endif
    {
        status = psa_key_derivation_abort(operation);
    }
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out, true);
        return status;
    }

    return tfm_crypto_operation_release(handle_out, false);
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_key_agreement(psa_invec in_vec[],
                                                     size_t in_len,
                                                     psa_outvec out_vec[],
                                                     size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;
    psa_status_t status;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 0, 0);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    psa_key_id_t private_key = iov->key_id;
    const uint8_t *peer_key = in_vec[1].base;
    size_t peer_key_length = in_vec[1].len;
    psa_key_derivation_operation_t *operation = NULL;
    psa_key_derivation_step_t step = iov->step;
    mbedtls_svc_key_id_t encoded_key;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_encode_id_and_owner(private_key, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_key_derivation_key_agreement(operation, step,
                                            encoded_key,
                                            peer_key,
                                            peer_key_length);
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_raw_key_agreement(psa_invec in_vec[],
                                          size_t in_len,
                                          psa_outvec out_vec[],
                                          size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 0, 1);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint8_t *output = out_vec[0].base;
    size_t output_size = out_vec[0].len;
    psa_algorithm_t alg = iov->alg;
    psa_key_id_t private_key = iov->key_id;
    const uint8_t *peer_key = in_vec[1].base;
    size_t peer_key_length = in_vec[1].len;
    mbedtls_svc_key_id_t encoded_key;
    psa_status_t status;

    status = tfm_crypto_encode_id_and_owner(private_key, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_raw_key_agreement(alg, encoded_key, peer_key, peer_key_length,
                                 output, output_size, &out_vec[0].len);
#endif
}

psa_status_t tfm_sl_psa_key_derivation_single_shot(psa_invec in_vec[],
                                                   size_t in_len,
                                                   psa_outvec out_vec[],
                                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 5, 5, out_len, 1, 1);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_algorithm_t alg = iov->alg;
    psa_key_id_t key_in = iov->key_id;
    const uint8_t *info = in_vec[1].base;
    size_t info_length  = in_vec[1].len;
    const uint8_t *salt = in_vec[2].base;
    size_t salt_length  = in_vec[2].len;
    const size_t *iterations  = in_vec[3].base;
    if (in_vec[3].len != sizeof(size_t)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct psa_client_key_attributes_s *client_key_attr = in_vec[4].base;
    if (in_vec[4].len != sizeof(struct psa_client_key_attributes_s)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    psa_key_id_t *key_out = out_vec[0].base;
    if (out_vec[0].len != sizeof(psa_key_id_t)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    mbedtls_svc_key_id_t encoded_key;
    psa_key_attributes_t key_out_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_status_t status;
    int32_t partition_id;

    status = tfm_crypto_encode_id_and_owner(key_in, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_key_attributes_from_client(client_key_attr,
                                                   partition_id,
                                                   &key_out_attributes);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return sl_psa_key_derivation_single_shot(alg, encoded_key,
                                             info, info_length,
                                             salt, salt_length,
                                             *iterations,
                                             &key_out_attributes, key_out);
#endif
}
/*!@}*/
