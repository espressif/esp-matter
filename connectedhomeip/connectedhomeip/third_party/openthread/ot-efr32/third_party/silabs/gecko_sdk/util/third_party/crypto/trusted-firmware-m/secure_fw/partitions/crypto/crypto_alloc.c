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
#include "tfm_memory_utils.h"

/**
 * \def TFM_CRYPTO_CONC_OPER_NUM
 *
 * \brief This is the default value for the maximum number of concurrent
 *        operations that can be active (allocated) at any time, supported
 *        by the implementation
 */
#ifndef TFM_CRYPTO_CONC_OPER_NUM
#define TFM_CRYPTO_CONC_OPER_NUM (8)
#endif

struct tfm_crypto_operation_s {
    uint32_t in_use;                /*!< Indicates if the operation is in use */
    int32_t owner;                  /*!< Indicates an ID of the owner of
                                     *   the context
                                     */
    enum tfm_crypto_operation_type type; /*!< Type of the operation */
    union {
#ifndef TFM_CRYPTO_AEAD_MODULE_DISABLED
        psa_aead_operation_t aead;        /*!< AEAD operation context */
#endif
#ifndef TFM_CRYPTO_CIPHER_MODULE_DISABLED
        psa_cipher_operation_t cipher;    /*!< Cipher operation context */
#endif
#ifndef TFM_CRYPTO_MAC_MODULE_DISABLED
        psa_mac_operation_t mac;          /*!< MAC operation context */
#endif
#ifndef TFM_CRYPTO_HASH_MODULE_DISABLED
        psa_hash_operation_t hash;        /*!< Hash operation context */
#endif
#ifndef TFM_CRYPTO_KEY_DERIVATION_MODULE_DISABLED
        psa_key_derivation_operation_t key_deriv; /*!< Key derivation operation context */
#endif
      uint32_t placeholder;  /* Placeholder if all modules are disabled. */
    } operation;
};

static struct tfm_crypto_operation_s operation[TFM_CRYPTO_CONC_OPER_NUM] ={{0}};

/*
 * \brief Function used to clear the memory associated to a backend context
 *
 * \param[in] index Numerical index in the database of the backend contexts
 *
 * \return None
 *
 */
static void memset_operation_context(uint32_t index)
{
    uint32_t mem_size;

    uint8_t *mem_ptr = (uint8_t *) &(operation[index].operation);

    switch(operation[index].type) {
#ifndef TFM_CRYPTO_AEAD_MODULE_DISABLED
    case TFM_CRYPTO_AEAD_OPERATION:
        mem_size = sizeof(psa_aead_operation_t);
        break;
#endif
#ifndef TFM_CRYPTO_CIPHER_MODULE_DISABLED
    case TFM_CRYPTO_CIPHER_OPERATION:
        mem_size = sizeof(psa_cipher_operation_t);
        break;
#endif
#ifndef TFM_CRYPTO_MAC_MODULE_DISABLED
    case TFM_CRYPTO_MAC_OPERATION:
        mem_size = sizeof(psa_mac_operation_t);
        break;
#endif
#ifndef TFM_CRYPTO_HASH_MODULE_DISABLED
    case TFM_CRYPTO_HASH_OPERATION:
        mem_size = sizeof(psa_hash_operation_t);
        break;
#endif
#ifndef TFM_CRYPTO_HASH_MODULE_DISABLED
    case TFM_CRYPTO_KEY_DERIVATION_OPERATION:
        mem_size = sizeof(psa_key_derivation_operation_t);
        break;
#endif
    case TFM_CRYPTO_OPERATION_NONE:
    default:
        mem_size = 0;
        break;
    }

    /* Clear the contents of the backend context */
    (void)tfm_memset(mem_ptr, 0, mem_size);
}

/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
psa_status_t tfm_crypto_init_alloc(void)
{
    /* Clear the contents of the local contexts */
    (void)tfm_memset(operation, 0, sizeof(operation));
    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_operation_alloc(enum tfm_crypto_operation_type type,
                                        uint32_t *handle,
                                        void **ctx)
{
    uint32_t i = 0;
    int32_t partition_id = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Handle must be initialised before calling a setup function */
    if (*handle != TFM_CRYPTO_INVALID_HANDLE) {
        return PSA_ERROR_BAD_STATE;
    }

    /* Init to invalid values */
    if (ctx == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    *ctx = NULL;

    for (i=0; i<TFM_CRYPTO_CONC_OPER_NUM; i++) {
        if (operation[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            operation[i].in_use = TFM_CRYPTO_IN_USE;
            operation[i].owner = partition_id;
            operation[i].type = type;
            *handle = i + 1;
            *ctx = (void *) &(operation[i].operation);
            return PSA_SUCCESS;
        }
    }

    return PSA_ERROR_NOT_PERMITTED;
}

psa_status_t tfm_crypto_operation_release(uint32_t *handle, bool clean_backend_context)
{
    uint32_t h_val = *handle;
    int32_t partition_id = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if ( (h_val != TFM_CRYPTO_INVALID_HANDLE) &&
         (h_val <= TFM_CRYPTO_CONC_OPER_NUM) &&
         (operation[h_val - 1].in_use == TFM_CRYPTO_IN_USE) &&
         (operation[h_val - 1].owner == partition_id)) {
        if (clean_backend_context) {
            memset_operation_context(h_val - 1);
        }
        operation[h_val - 1].in_use = TFM_CRYPTO_NOT_IN_USE;
        operation[h_val - 1].type = TFM_CRYPTO_OPERATION_NONE;
        operation[h_val - 1].owner = 0;
        *handle = TFM_CRYPTO_INVALID_HANDLE;
        return PSA_SUCCESS;
    }

    return PSA_ERROR_INVALID_ARGUMENT;
}

psa_status_t tfm_crypto_operation_lookup(enum tfm_crypto_operation_type type,
                                         uint32_t handle,
                                         void **ctx)
{
    int32_t partition_id = 0;
    psa_status_t status;

    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if ( (handle != TFM_CRYPTO_INVALID_HANDLE) &&
         (handle <= TFM_CRYPTO_CONC_OPER_NUM) &&
         (operation[handle - 1].in_use == TFM_CRYPTO_IN_USE) &&
         (operation[handle - 1].type == type) &&
         (operation[handle - 1].owner == partition_id)) {

        *ctx = (void *) &(operation[handle - 1].operation);
        return PSA_SUCCESS;
    }

    return PSA_ERROR_BAD_STATE;
}
/*!@}*/
