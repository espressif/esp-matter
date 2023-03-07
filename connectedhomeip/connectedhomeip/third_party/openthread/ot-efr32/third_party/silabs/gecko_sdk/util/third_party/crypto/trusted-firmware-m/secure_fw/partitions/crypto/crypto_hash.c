/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
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
psa_status_t tfm_crypto_hash_setup(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((out_vec[0].len != sizeof(uint32_t)) ||
        (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
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
#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)
        if (status == PSA_ERROR_BAD_STATE) {
            /* Invalidate the handle and abort the operation since the PSA Hash
               setup functon never gets called to perform the proper abort operation */
            /* Release the operation context, ignore if the operation fails. */
            (void)tfm_crypto_operation_release(handle_out, true);
        }
#endif
        return status;
    }

    *handle_out = handle;

    status = psa_hash_setup(operation, alg);
    if (status != PSA_SUCCESS) {
        /* If the operation failed, the abort() function is called by the underlying crypto function
           so just indicate that the operation is invalid. */
        (void)tfm_crypto_operation_release(handle_out, false);
    }

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_update(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *operation = NULL;

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
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                         handle,
                                         (void **)&operation);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_hash_update(operation, input, input_length);
    if (status != PSA_SUCCESS) {
        /* If the operation failed, the abort() function is called by the underlying crypto function
           so just indicate that the operation is invalid. */
        (void)tfm_crypto_operation_release(handle_out, false);
    }

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_finish(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 2);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
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
    /* The abort() function is called by the underlying crypto function
       so just indicate that the operation is invalid. */
    (void)tfm_crypto_operation_release(handle_out, false);
    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_verify(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
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
    if (status == PSA_SUCCESS)
    {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out, true);
    } else {
        /* If the operation failed, the abort() function is called by the underlying crypto function
           so just indicate that the operation is invalid. */
        (void)tfm_crypto_operation_release(handle_out, false);
    }

    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_abort(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *operation = NULL;

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
        (void)tfm_crypto_operation_release(handle_out, true);
        return status;
    }

    /* The abort() function is called by the underlying crypto function
       so just invalidate the operation */
    return tfm_crypto_operation_release(handle_out, false);
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_clone(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status = PSA_SUCCESS;
    psa_hash_operation_t *source_operation = NULL;
    psa_hash_operation_t *target_operation = NULL;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
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
#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)
        if (status == PSA_ERROR_BAD_STATE) {
            /* Invalidate the handle and abort the operation since the PSA Hash
               clone functon never gets called to perform the proper abort operation */
            /* Release the operation context, ignore if the operation fails. */
            (void)tfm_crypto_operation_release(target_handle, true);
        }
#endif
        return status;
    }

    status = psa_hash_clone(source_operation, target_operation);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(target_handle, false);
    }
    return status;
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_compute(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 2, out_len, 0, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
         return PSA_ERROR_PROGRAMMER_ERROR;
    }

    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;
    uint8_t *hash = out_vec[0].base;
    size_t hash_size = out_vec[0].len;

    /* Initialize hash_length to zero */
    out_vec[0].len = 0;
    return psa_hash_compute(alg, input, input_length, hash, hash_size,
                            &out_vec[0].len);
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}

psa_status_t tfm_crypto_hash_compare(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_HASH_MODULE_DISABLED
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
    psa_algorithm_t alg = iov->alg;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;
    const uint8_t *hash = in_vec[2].base;
    size_t hash_length = in_vec[2].len;

    return psa_hash_compare(alg, input, input_length, hash, hash_length);
#endif /* TFM_CRYPTO_HASH_MODULE_DISABLED */
}
/*!@}*/
