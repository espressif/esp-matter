/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
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
psa_status_t tfm_crypto_hash_setup(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *operation = NULL;

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

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Allocate the operation context in the secure world */
    status = tfm_crypto_operation_alloc(TFM_CRYPTO_HASH_OPERATION,
                                        &handle,
                                        (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *handle_out = handle;

    status = psa_hash_setup(operation, alg);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out);
        return status;
    }

    return PSA_SUCCESS;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_update(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *operation = NULL;

    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_hash_update(operation, input, input_length);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out);
        return status;
    }

    return PSA_SUCCESS;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_finish(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *operation = NULL;

    if ((in_len != 1) || (out_len != 2)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    uint8_t *hash = out_vec[1].base;
    size_t hash_size = out_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Initialise hash_length to zero */
    out_vec[1].len = 0;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_hash_finish(operation, hash, hash_size, &out_vec[1].len);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out);
        return status;
    }

    status = tfm_crypto_operation_release(handle_out);

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_verify(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *operation = NULL;

    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    const uint8_t *hash = in_vec[1].base;
    size_t hash_length = in_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_hash_verify(operation, hash, hash_length);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out);
        return status;
    }

    status = tfm_crypto_operation_release(handle_out);

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_abort(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *operation = NULL;

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

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        /* Operation does not exist, so abort has no effect */
        return PSA_SUCCESS;
    }

    status = psa_hash_abort(operation);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out);
        return status;
    }

    status = tfm_crypto_operation_release(handle_out);

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_clone(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *source_operation = NULL;
    psa_hash_operation_t *target_operation = NULL;

    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t source_handle = iov->op_handle;
    uint32_t *target_handle = out_vec[0].base;

    /* Look up the corresponding source operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                         source_handle,
                                         (void **)&source_operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Allocate the target operation context in the secure world */
    status = tfm_crypto_operation_alloc(TFM_CRYPTO_HASH_OPERATION,
                                        target_handle,
                                        (void **)&target_operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_hash_clone(source_operation, target_operation);
    if (status != PSA_SUCCESS) {
        /* Release the target operation context, ignore if it fails. */
        (void)tfm_crypto_operation_release(target_handle);
        return status;
    }

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_compare(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_hash_compute(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
}
/*!@}*/
