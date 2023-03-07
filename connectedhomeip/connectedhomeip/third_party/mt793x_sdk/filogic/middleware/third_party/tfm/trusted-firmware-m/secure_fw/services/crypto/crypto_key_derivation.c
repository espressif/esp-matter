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

/* Required for mbedtls_calloc in tfm_crypto_huk_derivation_input_bytes */
#include "mbedtls/platform.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"
#include "tfm_memory_utils.h"

#include "platform/include/tfm_plat_crypto_keys.h"

#ifdef TFM_PARTITION_TEST_SST
#include "psa_manifest/pid.h"
#endif /* TFM_PARTITION_TEST_SST */

#ifndef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
static psa_status_t tfm_crypto_huk_derivation_setup(
                                      psa_key_derivation_operation_t *operation,
                                      psa_algorithm_t alg)
{
    operation->alg = TFM_CRYPTO_ALG_HUK_DERIVATION;
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

#ifdef TFM_PARTITION_TEST_SST
    /* The SST tests run some operations under the wrong partition ID - this
     * causes the key derivation to change.
     */
    if (partition_id == TFM_SP_SST_TEST) {
        partition_id = TFM_SP_STORAGE;
    }
#endif /* TFM_PARTITION_TEST_SST */

    /* Put the label in the tls12_prf ctx to make it available in the output key
     * step.
     */
    operation->ctx.tls12_prf.label = mbedtls_calloc(1, sizeof(partition_id)
                                                       + data_length);
    if (operation->ctx.tls12_prf.label == NULL) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }
    (void)tfm_memcpy(operation->ctx.tls12_prf.label, &partition_id,
                     sizeof(partition_id));
    (void)tfm_memcpy(operation->ctx.tls12_prf.label + sizeof(partition_id),
                     data, data_length);
    operation->ctx.tls12_prf.label_length = sizeof(partition_id) + data_length;

    return PSA_SUCCESS;
}

static psa_status_t tfm_crypto_huk_derivation_output_key(
                                      const psa_key_attributes_t *attributes,
                                      psa_key_derivation_operation_t *operation,
                                      psa_key_handle_t *handle)
{
    enum tfm_plat_err_t err;
    size_t bytes = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));

    if (sizeof(operation->ctx.tls12_prf.output_block) < bytes) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    /* Derive key material from the HUK and output it to the operation buffer */
    err = tfm_plat_get_huk_derived_key(operation->ctx.tls12_prf.label,
                                       operation->ctx.tls12_prf.label_length,
                                       NULL, 0,
                                       operation->ctx.tls12_prf.output_block,
                                       bytes);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    return psa_import_key(attributes, operation->ctx.tls12_prf.output_block,
                          bytes, handle);
}

static psa_status_t tfm_crypto_huk_derivation_abort(
                                      psa_key_derivation_operation_t *operation)
{
    if (operation->ctx.tls12_prf.label != NULL) {
        (void)tfm_memset(operation->ctx.tls12_prf.label, 0,
                         operation->ctx.tls12_prf.label_length);
        mbedtls_free(operation->ctx.tls12_prf.label);
    }

    (void)tfm_memset(operation, 0, sizeof(*operation));

    return PSA_SUCCESS;
}
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
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_key_derivation_operation_t *operation = NULL;

    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(uint32_t)) ||
        (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_CONNECTION_REFUSED;
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

    if (alg == TFM_CRYPTO_ALG_HUK_DERIVATION) {
        status = tfm_crypto_huk_derivation_setup(operation, alg);
    } else {
        status = psa_key_derivation_setup(operation, alg);
    }
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out);
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
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(size_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
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
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 1) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_CONNECTION_REFUSED;
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
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 2) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_CONNECTION_REFUSED;
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

    if (operation->alg == TFM_CRYPTO_ALG_HUK_DERIVATION) {
        return tfm_crypto_huk_derivation_input_bytes(operation, step, data,
                                                     data_length);
    } else {
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
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_CONNECTION_REFUSED;
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
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 1) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    psa_key_handle_t key_handle = iov->key_handle;
    psa_key_derivation_step_t step = iov->step;
    psa_key_derivation_operation_t *operation = NULL;

    status = tfm_crypto_check_handle_owner(key_handle, NULL);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_key_derivation_input_key(operation, step, key_handle);
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_output_key(psa_invec in_vec[],
                                                  size_t in_len,
                                                  psa_outvec out_vec[],
                                                  size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_client_key_attributes_t)) ||
        (out_vec[0].len != sizeof(psa_key_handle_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    const psa_client_key_attributes_t *client_key_attr = in_vec[1].base;
    psa_key_derivation_operation_t *operation = NULL;
    psa_key_handle_t *key_handle = out_vec[0].base;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    int32_t partition_id;
    uint32_t index;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_check_key_storage(&index);
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

    if (operation->alg == TFM_CRYPTO_ALG_HUK_DERIVATION) {
        status = tfm_crypto_huk_derivation_output_key(&key_attributes,
                                                      operation, key_handle);
    } else {
        status = psa_key_derivation_output_key(&key_attributes, operation,
                                               key_handle);
    }
    if (status == PSA_SUCCESS) {
        status = tfm_crypto_set_key_storage(index, *key_handle);
    }

    return status;
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_abort(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
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

    if (operation->alg == TFM_CRYPTO_ALG_HUK_DERIVATION) {
        status = tfm_crypto_huk_derivation_abort(operation);
    } else {
        status = psa_key_derivation_abort(operation);
    }
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out);
        return status;
    }

    status = tfm_crypto_operation_release(handle_out);

    return status;
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation_key_agreement(psa_invec in_vec[],
                                                     size_t in_len,
                                                     psa_outvec out_vec[],
                                                     size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 2) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    psa_key_handle_t private_key = iov->key_handle;
    const uint8_t *peer_key = in_vec[1].base;
    size_t peer_key_length = in_vec[1].len;
    psa_key_derivation_operation_t *operation = NULL;
    psa_key_derivation_step_t step = iov->step;

    status = tfm_crypto_check_handle_owner(private_key, NULL);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_KEY_DERIVATION_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_key_derivation_key_agreement(operation, step,
                                            private_key,
                                            peer_key,
                                            peer_key_length);
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_generate_random(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    uint8_t *output = out_vec[0].base;
    size_t output_size = out_vec[0].len;

    return psa_generate_random(output, output_size);
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}

psa_status_t tfm_crypto_raw_key_agreement(psa_invec in_vec[],
                                          size_t in_len,
                                          psa_outvec out_vec[],
                                          size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint8_t *output = out_vec[0].base;
    size_t output_size = out_vec[0].len;
    psa_algorithm_t alg = iov->alg;
    psa_key_handle_t private_key = iov->key_handle;
    const uint8_t *peer_key = in_vec[1].base;
    size_t peer_key_length = in_vec[1].len;

    return psa_raw_key_agreement(alg, private_key, peer_key, peer_key_length,
                                 output, output_size, &out_vec[0].len);
#endif /* TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED */
}
/*!@}*/
