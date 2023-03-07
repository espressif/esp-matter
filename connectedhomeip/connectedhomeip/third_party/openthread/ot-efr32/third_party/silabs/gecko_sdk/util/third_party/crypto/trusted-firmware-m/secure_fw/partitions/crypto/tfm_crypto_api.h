/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CRYPTO_API_H__
#define __TFM_CRYPTO_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tfm_crypto_defs.h"
#ifdef TFM_PSA_API
#include "psa/service.h"

/**
 * \brief This define is a function pointer type to the Uniform Signature API
 *        prototype.
 */
typedef psa_status_t (*tfm_crypto_us_t)(psa_invec[],size_t,psa_outvec[],size_t);
#endif

#include "psa/crypto_client_struct.h"

#define UNIFORM_SIGNATURE_API(api_name) \
    psa_status_t api_name(psa_invec[], size_t, psa_outvec[], size_t)

/**
 * \brief List of possible operation types supported by the TFM based
 *        implementation. This type is needed by the operation allocation,
 *        lookup and release functions.
 *
 */
enum tfm_crypto_operation_type {
    TFM_CRYPTO_OPERATION_NONE = 0,
    TFM_CRYPTO_CIPHER_OPERATION = 1,
    TFM_CRYPTO_MAC_OPERATION = 2,
    TFM_CRYPTO_HASH_OPERATION = 3,
    TFM_CRYPTO_KEY_DERIVATION_OPERATION = 4,
    TFM_CRYPTO_AEAD_OPERATION = 5,

    /* Used to force the enum size */
    TFM_CRYPTO_OPERATION_TYPE_MAX = INT_MAX
};

/**
 * \brief Initialise the service
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_init(void);

/**
 * \brief Initialise the Alloc module
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_init_alloc(void);

/**
 * \brief Returns the ID of the caller
 *
 * \param[out] id Pointer to hold the ID of the caller
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_get_caller_id(int32_t *id);

/**
 * \brief Gets key attributes from client key attributes.
 *
 * \param[in]  client_key_attr  Client key attributes
 * \param[in]  client_id        Partition ID of the calling client
 * \param[out] key_attributes   Key attributes
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_key_attributes_from_client(
                    const struct psa_client_key_attributes_s *client_key_attr,
                    int32_t client_id,
                    psa_key_attributes_t *key_attributes);

/**
 * \brief Converts key attributes to client key attributes.
 *
 * \param[in]  key_attributes   Key attributes
 * \param[out] client_key_attr  Client key attributes
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_key_attributes_to_client(
                        const psa_key_attributes_t *key_attributes,
                        struct psa_client_key_attributes_s *client_key_attr);

/**
 * \brief Allocate an operation context in the backend
 *
 * \param[in]  type   Type of the operation context to allocate
 * \param[out] handle Pointer to hold the allocated handle
 * \param[out  ctx    Double pointer to the corresponding context
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_operation_alloc(enum tfm_crypto_operation_type type,
                                        uint32_t *handle,
                                        void **ctx);
/**
 * \brief Release an operation context in the backend
 *
 * \param[in] handle Pointer to the handle of the context to release
 * \param[in] clean_backend_context Clean the backend operation context
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_operation_release(uint32_t *handle, bool clean_backend_context);
/**
 * \brief Look up an operation context in the backend for the corresponding
 *        frontend operation
 *
 * \param[in]  type   Type of the operation context to look up
 * \param[in]  handle Handle of the context to lookup
 * \param[out] ctx    Double pointer to the corresponding context
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_operation_lookup(enum tfm_crypto_operation_type type,
                                         uint32_t handle,
                                         void **ctx);
/**
 * \brief Encodes the input key id and owner to output key
 *
 * \param[in]  key_id       Id of the key to encode
 * \param[out] enc_key_ptr  Pointer to encoded key with id and owner
 *
 * \return Return values as described in \ref psa_status_t
 */
psa_status_t tfm_crypto_encode_id_and_owner(psa_key_id_t key_id,
                                            mbedtls_svc_key_id_t *enc_key_ptr);

#define LIST_TFM_CRYPTO_UNIFORM_SIGNATURE_API \
    X(tfm_crypto_get_key_attributes)          \
    X(tfm_crypto_reset_key_attributes)        \
    X(tfm_crypto_open_key)                    \
    X(tfm_crypto_close_key)                   \
    X(tfm_crypto_import_key)                  \
    X(tfm_crypto_destroy_key)                 \
    X(tfm_crypto_export_key)                  \
    X(tfm_crypto_export_public_key)           \
    X(tfm_crypto_purge_key)                   \
    X(tfm_crypto_copy_key)                    \
    X(tfm_crypto_hash_compute)                \
    X(tfm_crypto_hash_compare)                \
    X(tfm_crypto_hash_setup)                  \
    X(tfm_crypto_hash_update)                 \
    X(tfm_crypto_hash_finish)                 \
    X(tfm_crypto_hash_verify)                 \
    X(tfm_crypto_hash_abort)                  \
    X(tfm_crypto_hash_clone)                  \
    X(tfm_crypto_mac_compute)                 \
    X(tfm_crypto_mac_verify)                  \
    X(tfm_crypto_mac_sign_setup)              \
    X(tfm_crypto_mac_verify_setup)            \
    X(tfm_crypto_mac_update)                  \
    X(tfm_crypto_mac_sign_finish)             \
    X(tfm_crypto_mac_verify_finish)           \
    X(tfm_crypto_mac_abort)                   \
    X(tfm_crypto_cipher_encrypt)              \
    X(tfm_crypto_cipher_decrypt)              \
    X(tfm_crypto_cipher_encrypt_setup)        \
    X(tfm_crypto_cipher_decrypt_setup)        \
    X(tfm_crypto_cipher_generate_iv)          \
    X(tfm_crypto_cipher_set_iv)               \
    X(tfm_crypto_cipher_update)               \
    X(tfm_crypto_cipher_finish)               \
    X(tfm_crypto_cipher_abort)                \
    X(tfm_crypto_aead_encrypt)                \
    X(tfm_crypto_aead_decrypt)                \
    X(tfm_crypto_aead_encrypt_setup)          \
    X(tfm_crypto_aead_decrypt_setup)          \
    X(tfm_crypto_aead_generate_nonce)         \
    X(tfm_crypto_aead_set_nonce)              \
    X(tfm_crypto_aead_set_lengths)            \
    X(tfm_crypto_aead_update_ad)              \
    X(tfm_crypto_aead_update)                 \
    X(tfm_crypto_aead_finish)                 \
    X(tfm_crypto_aead_verify)                 \
    X(tfm_crypto_aead_abort)                  \
    X(tfm_crypto_sign_message)                \
    X(tfm_crypto_verify_message)              \
    X(tfm_crypto_sign_hash)                   \
    X(tfm_crypto_verify_hash)                 \
    X(tfm_crypto_asymmetric_encrypt)          \
    X(tfm_crypto_asymmetric_decrypt)          \
    X(tfm_crypto_key_derivation_setup)        \
    X(tfm_crypto_key_derivation_get_capacity) \
    X(tfm_crypto_key_derivation_set_capacity) \
    X(tfm_crypto_key_derivation_input_bytes)  \
    X(tfm_crypto_key_derivation_input_key)    \
    X(tfm_crypto_key_derivation_key_agreement)\
    X(tfm_crypto_key_derivation_output_bytes) \
    X(tfm_crypto_key_derivation_output_key)   \
    X(tfm_crypto_key_derivation_abort)        \
    X(tfm_crypto_raw_key_agreement)           \
    X(tfm_crypto_generate_random)             \
    X(tfm_crypto_generate_key)                \

#define X(api_name) UNIFORM_SIGNATURE_API(api_name);
LIST_TFM_CRYPTO_UNIFORM_SIGNATURE_API
#undef X

#ifdef __cplusplus
}
#endif

#endif /* __TFM_CRYPTO_API_H__ */
