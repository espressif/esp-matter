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
#include <stdbool.h>

#ifndef TFM_CRYPTO_MAX_KEY_HANDLES
#define TFM_CRYPTO_MAX_KEY_HANDLES (16)
#endif
struct tfm_crypto_handle_owner_s {
    int32_t owner;           /*!< Owner of the allocated handle */
    psa_key_handle_t handle; /*!< Allocated handle */
    uint8_t in_use;          /*!< Flag to indicate if this in use */
};

#ifndef TFM_CRYPTO_KEY_MODULE_DISABLED
static struct tfm_crypto_handle_owner_s
                                 handle_owner[TFM_CRYPTO_MAX_KEY_HANDLES] = {0};
#endif

/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
psa_status_t tfm_crypto_key_attributes_from_client(
                             const psa_client_key_attributes_t *client_key_attr,
                             int32_t client_id,
                             psa_key_attributes_t *key_attributes)
{
    if (client_key_attr == NULL || key_attributes == NULL) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* Domain parameters are not supported, ignore any passed by the client */
    key_attributes->domain_parameters = NULL;
    key_attributes->domain_parameters_size = 0;

    /* Copy core key attributes from the client core key attributes */
    key_attributes->core.type = client_key_attr->core.type;
    key_attributes->core.lifetime = client_key_attr->core.lifetime;
    key_attributes->core.policy = client_key_attr->core.policy;
    key_attributes->core.bits = client_key_attr->core.bits;
    key_attributes->core.flags = client_key_attr->core.flags;

    /* Use the client key id as the key_id and its partition id as the owner */
    key_attributes->core.id.key_id = client_key_attr->core.id;
    key_attributes->core.id.owner = client_id;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_key_attributes_to_client(
                                   const psa_key_attributes_t *key_attributes,
                                   psa_client_key_attributes_t *client_key_attr)
{
    if (client_key_attr == NULL || key_attributes == NULL) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* Domain parameters are not supported, avoid passing any to the client */
    client_key_attr->domain_parameters = NULL;
    client_key_attr->domain_parameters_size = 0;

    /* Copy core key attributes to the client core key attributes */
    client_key_attr->core.type = key_attributes->core.type;
    client_key_attr->core.lifetime = key_attributes->core.lifetime;
    client_key_attr->core.policy = key_attributes->core.policy;
    client_key_attr->core.bits = key_attributes->core.bits;
    client_key_attr->core.flags = key_attributes->core.flags;

    /* Return the key_id as the client key id, do not return the owner */
    client_key_attr->core.id = key_attributes->core.id.key_id;

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_check_handle_owner(psa_key_handle_t handle,
                                           uint32_t *index)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    int32_t partition_id = 0;
    uint32_t i = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use && handle_owner[i].handle == handle) {
            if (handle_owner[i].owner == partition_id) {
                if (index != NULL) {
                    *index = i;
                }
                return PSA_SUCCESS;
            } else {
                return PSA_ERROR_NOT_PERMITTED;
            }
        }
    }

    return PSA_ERROR_INVALID_HANDLE;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_check_key_storage(uint32_t *index)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    uint32_t i;

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            *index = i;
            return PSA_SUCCESS;
        }
    }

    return PSA_ERROR_INSUFFICIENT_MEMORY;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_set_key_storage(uint32_t index,
                                        psa_key_handle_t key_handle)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    int32_t partition_id;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    handle_owner[index].owner = partition_id;
    handle_owner[index].handle = key_handle;
    handle_owner[index].in_use = TFM_CRYPTO_IN_USE;

    return PSA_SUCCESS;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_set_key_domain_parameters(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_get_key_domain_parameters(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    /* FixMe: To be implemented */
    return PSA_ERROR_NOT_SUPPORTED;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_import_key(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else

    if ((in_len != 3) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_client_key_attributes_t)) ||
        (out_vec[0].len != sizeof(psa_key_handle_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const psa_client_key_attributes_t *client_key_attr = in_vec[1].base;
    const uint8_t *data = in_vec[2].base;
    size_t data_length = in_vec[2].len;
    psa_key_handle_t *key_handle = out_vec[0].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint32_t i = 0;
    int32_t partition_id = 0;
    bool empty_found = false;

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            empty_found = true;
            break;
        }
    }

    if (!empty_found) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
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

    status = psa_import_key(&key_attributes, data, data_length, key_handle);

    if (status == PSA_SUCCESS) {
        handle_owner[i].owner = partition_id;
        handle_owner[i].handle = *key_handle;
        handle_owner[i].in_use = TFM_CRYPTO_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_open_key(psa_invec in_vec[],
                                 size_t in_len,
                                 psa_outvec out_vec[],
                                 size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_app_key_id_t)) ||
        (out_vec[0].len != sizeof(psa_key_handle_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    psa_app_key_id_t client_key_id = *((psa_app_key_id_t *)in_vec[1].base);
    psa_key_handle_t *key_handle = out_vec[0].base;
    psa_status_t status;
    psa_key_id_t id;
    int32_t partition_id;
    uint32_t i;

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            break;
        }
    }

    if (i == TFM_CRYPTO_MAX_KEY_HANDLES) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Use the client key id as the key_id and its partition id as the owner */
    id = (psa_key_id_t){ .key_id = client_key_id, .owner = partition_id };

    status = psa_open_key(id, key_handle);

    if (status == PSA_SUCCESS) {
        handle_owner[i].owner = partition_id;
        handle_owner[i].handle = *key_handle;
        handle_owner[i].in_use = TFM_CRYPTO_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_close_key(psa_invec in_vec[],
                                  size_t in_len,
                                  psa_outvec out_vec[],
                                  size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    if ((in_len != 1) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    uint32_t index;
    psa_status_t status = tfm_crypto_check_handle_owner(key, &index);

    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_close_key(key);

    if (status == PSA_SUCCESS) {
        handle_owner[index].owner = 0;
        handle_owner[index].handle = 0;
        handle_owner[index].in_use = TFM_CRYPTO_NOT_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_destroy_key(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    (void)out_vec;

    if ((in_len != 1) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    uint32_t index;
    psa_status_t status = tfm_crypto_check_handle_owner(key, &index);

    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_destroy_key(key);

    if (status == PSA_SUCCESS) {
        handle_owner[index].owner = 0;
        handle_owner[index].handle = 0;
        handle_owner[index].in_use = TFM_CRYPTO_NOT_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_get_key_attributes(psa_invec in_vec[],
                                           size_t in_len,
                                           psa_outvec out_vec[],
                                           size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_client_key_attributes_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    psa_client_key_attributes_t *client_key_attr = out_vec[0].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    status = tfm_crypto_check_handle_owner(key, NULL);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_get_key_attributes(key, &key_attributes);

    if (status == PSA_SUCCESS) {
        status = tfm_crypto_key_attributes_to_client(&key_attributes,
                                                     client_key_attr);
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_reset_key_attributes(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
#if (TFM_CRYPTO_KEY_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_client_key_attributes_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    psa_client_key_attributes_t *client_key_attr = out_vec[0].base;
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
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    uint8_t *data = out_vec[0].base;
    size_t data_size = out_vec[0].len;

    return psa_export_key(key, data, data_size, &(out_vec[0].len));
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_export_public_key(psa_invec in_vec[],
                                          size_t in_len,
                                          psa_outvec out_vec[],
                                          size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    uint8_t *data = out_vec[0].base;
    size_t data_size = out_vec[0].len;

    return psa_export_public_key(key, data, data_size, &(out_vec[0].len));
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_copy_key(psa_invec in_vec[],
                                 size_t in_len,
                                 psa_outvec out_vec[],
                                 size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else

    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_handle_t)) ||
        (in_vec[1].len != sizeof(psa_client_key_attributes_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t source_handle = iov->key_handle;
    psa_key_handle_t *target_handle = out_vec[0].base;
    const psa_client_key_attributes_t *client_key_attr = in_vec[1].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint32_t i = 0;
    int32_t partition_id = 0;
    bool empty_found = false;

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            empty_found = true;
            break;
        }
    }

    if (!empty_found) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
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

    status = psa_copy_key(source_handle, &key_attributes, target_handle);

    if (status == PSA_SUCCESS) {
        handle_owner[i].owner = partition_id;
        handle_owner[i].handle = *target_handle;
        handle_owner[i].in_use = TFM_CRYPTO_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}

psa_status_t tfm_crypto_generate_key(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#ifdef TFM_CRYPTO_KEY_MODULE_DISABLED
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_client_key_attributes_t)) ||
        (out_vec[0].len != sizeof(psa_key_handle_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    psa_key_handle_t *key_handle = out_vec[0].base;
    const psa_client_key_attributes_t *client_key_attr = in_vec[1].base;
    psa_status_t status;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    uint32_t i = 0;
    int32_t partition_id = 0;
    bool empty_found = false;

    for (i = 0; i < TFM_CRYPTO_MAX_KEY_HANDLES; i++) {
        if (handle_owner[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            empty_found = true;
            break;
        }
    }

    if (!empty_found) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
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

    status = psa_generate_key(&key_attributes, key_handle);

    if (status == PSA_SUCCESS) {
        handle_owner[i].owner = partition_id;
        handle_owner[i].handle = *key_handle;
        handle_owner[i].in_use = TFM_CRYPTO_IN_USE;
    }

    return status;
#endif /* TFM_CRYPTO_KEY_MODULE_DISABLED */
}
/*!@}*/
