/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "tfm_mbedcrypto_include.h"
#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"
#include "tfm_crypto_private.h"

psa_status_t tfm_crypto_import_key(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 2, 3, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(struct psa_client_key_attributes_s)) ||
        (out_vec[0].len != sizeof(psa_key_id_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct psa_client_key_attributes_s *client_key_attr = in_vec[1].base;
    const uint8_t *data = in_vec[2].base;
    size_t data_length = in_vec[2].len;
    psa_key_id_t *psa_key = out_vec[0].base;

    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t encoded_key;
    int32_t partition_id = 0;

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

    status = psa_import_key(&key_attributes, data, data_length, &encoded_key);

    /* Update the imported key id */
#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    *psa_key = encoded_key.MBEDTLS_PRIVATE(key_id);
#else
    *psa_key = encoded_key;
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_open_key(psa_invec in_vec[],
                                 size_t in_len,
                                 psa_outvec out_vec[],
                                 size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 2, 2, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_key_id_t)) ||
        (out_vec[0].len != sizeof(psa_key_id_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    psa_key_id_t client_key_id = *((psa_key_id_t *)in_vec[1].base);
    psa_key_id_t *key = out_vec[0].base;
    psa_status_t status;
    mbedtls_svc_key_id_t encoded_key;
    int32_t partition_id = 0;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Use the client key id as the key_id and its partition id as the owner */
    encoded_key = mbedtls_svc_key_id_make(partition_id, client_key_id);

    status = psa_open_key(encoded_key, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    *key = encoded_key.MBEDTLS_PRIVATE(key_id);
#else
    *key = encoded_key;
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_close_key(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 0);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t key = iov->key_id;
    mbedtls_svc_key_id_t encoded_key;
    int32_t partition_id = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key = mbedtls_svc_key_id_make(partition_id, key);

    return psa_close_key(encoded_key);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_destroy_key(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 0);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_id_t key = iov->key_id;
    mbedtls_svc_key_id_t encoded_key;
    int32_t partition_id = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key = mbedtls_svc_key_id_make(partition_id, key);

    return psa_destroy_key(encoded_key);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_get_key_attributes(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(struct psa_client_key_attributes_s))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t key = iov->key_id;
    struct psa_client_key_attributes_s *client_key_attr = out_vec[0].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t encoded_key;
    int32_t partition_id = 0;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key = mbedtls_svc_key_id_make(partition_id, key);

    status = psa_get_key_attributes(encoded_key, &key_attributes);
    if (status == PSA_SUCCESS) {
        status = tfm_crypto_key_attributes_to_client(&key_attributes,
                                                     client_key_attr);
#if defined(TFM_CONFIG_SL_SECURE_LIBRARY)
    // test_operations_on_invalid_key in test_suite_psa_crypto.function expects that key attributes are cleared.
    } else if (status == PSA_ERROR_INVALID_HANDLE) {
      memset(client_key_attr, 0, sizeof(struct psa_client_key_attributes_s));
#endif
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_reset_key_attributes(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(struct psa_client_key_attributes_s))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    struct psa_client_key_attributes_s *client_key_attr = out_vec[0].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    int32_t partition_id;

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

    psa_reset_key_attributes(&key_attributes);

    return tfm_crypto_key_attributes_to_client(&key_attributes,
                                               client_key_attr);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_export_key(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 1);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t key = iov->key_id;
    uint8_t *data = out_vec[0].base;
    size_t data_size = out_vec[0].len;
    mbedtls_svc_key_id_t encoded_key;
    int32_t partition_id = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key = mbedtls_svc_key_id_make(partition_id, key);

    return psa_export_key(encoded_key, data, data_size,
                          &(out_vec[0].len));
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_export_public_key(psa_invec in_vec[],
                                          size_t in_len,
                                          psa_outvec out_vec[],
                                          size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 1);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_id_t key = iov->key_id;
    uint8_t *data = out_vec[0].base;
    size_t data_size = out_vec[0].len;
    mbedtls_svc_key_id_t encoded_key;
    int32_t partition_id = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key = mbedtls_svc_key_id_make(partition_id, key);

    return psa_export_public_key(encoded_key, data, data_size,
                                 &(out_vec[0].len));
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_purge_key(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 1, 1, out_len, 0, 0);

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_id_t key = iov->key_id;
    mbedtls_svc_key_id_t encoded_key;
    int32_t partition_id = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    encoded_key = mbedtls_svc_key_id_make(partition_id, key);

    return psa_purge_key(encoded_key);
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_copy_key(psa_invec in_vec[],
                                 size_t in_len,
                                 psa_outvec out_vec[],
                                 size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 2, 2, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_id_t)) ||
        (in_vec[1].len != sizeof(struct psa_client_key_attributes_s))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_id_t source_key_id = iov->key_id;
    psa_key_id_t *target_key_id = out_vec[0].base;
    const struct psa_client_key_attributes_s *client_key_attr = in_vec[1].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    int32_t partition_id = 0;
    mbedtls_svc_key_id_t target_key;
    mbedtls_svc_key_id_t encoded_key;

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

    encoded_key = mbedtls_svc_key_id_make(partition_id, source_key_id);

    status = psa_copy_key(encoded_key, &key_attributes, &target_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    *target_key_id = target_key.MBEDTLS_PRIVATE(key_id);
#else
    *target_key_id = target_key;
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_generate_key(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    SUPPRESS_UNUSED_IOVEC_PARAM_WARNING();
    return PSA_ERROR_NOT_SUPPORTED;
#else

    CRYPTO_IN_OUT_LEN_VALIDATE(in_len, 2, 2, out_len, 1, 1);

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(struct psa_client_key_attributes_s)) ||
        (out_vec[0].len != sizeof(psa_key_id_t))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
    psa_key_id_t *key_handle = out_vec[0].base;
    const struct psa_client_key_attributes_s *client_key_attr = in_vec[1].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    int32_t partition_id = 0;
    mbedtls_svc_key_id_t encoded_key;

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

    status = psa_generate_key(&key_attributes, &encoded_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    *key_handle = encoded_key.MBEDTLS_PRIVATE(key_id);
#else
    *key_handle = encoded_key;
#endif

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}
