/***************************************************************************//**
 * @file app_se_manager_ecdh.h
 * @brief SE manager ECDH functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef APP_SE_MANAGER_ECDH_H
#define APP_SE_MANAGER_ECDH_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_key_derivation.h"
#include "sl_se_manager_key_handling.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Overhead of wrapped key buffer
#define WRAPPED_KEY_OVERHEAD    (12 + 16)

/// Padding bytes for ECC P521
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG) \
  && (_SILICON_LABS_32B_SERIES_2_CONFIG < 3)
  #define P521_PADDING_BYTES 2
#else
  #define P521_PADDING_BYTES 0
#endif

/// Private key size of ECC Weierstrass Prime keys
#define ECC_P192_PRIVKEY_SIZE   (SL_SE_KEY_TYPE_ECC_P192 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK)
#define ECC_P256_PRIVKEY_SIZE   (SL_SE_KEY_TYPE_ECC_P256 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#define ECC_P384_PRIVKEY_SIZE   (SL_SE_KEY_TYPE_ECC_P384 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK)
#define ECC_P521_PRIVKEY_SIZE   ((SL_SE_KEY_TYPE_ECC_P521 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK) + P521_PADDING_BYTES)

/// Private key size of ECC Montgomery keys
#define ECC_X25519_PRIVKEY_SIZE (SL_SE_KEY_TYPE_ECC_X25519 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK)
#define ECC_X448_PRIVKEY_SIZE   (SL_SE_KEY_TYPE_ECC_X448 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK)

/// Use the biggest ECC curve as private key size
#define ECC_PRIVKEY_SIZE        (ECC_P521_PRIVKEY_SIZE)

/// Domain size of asymmetric custom key
#define DOMAIN_SIZE             (32)
#else
/// Private key size of ECC Montgomery keys
#define ECC_X25519_PRIVKEY_SIZE (SL_SE_KEY_TYPE_ECC_X25519 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK)

/// Use the biggest ECC curve as private key size
#define ECC_PRIVKEY_SIZE        (ECC_P256_PRIVKEY_SIZE)
#endif

/// Public key size = private key size x 2
#define ECC_PUBKEY_SIZE         (ECC_PRIVKEY_SIZE * 2)

/// Shared secret size = private key size x 2
#define SHARED_SECRET_SIZE      (ECC_PRIVKEY_SIZE * 2)

/// Internal SE key slots used for asymmetric keys
#define CLIENT_KEY_SLOT         (SL_SE_KEY_SLOT_VOLATILE_0)
#define SERVER_KEY_SLOT         (SL_SE_KEY_SLOT_VOLATILE_1)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get client shared secret buffer pointer.
 *
 * @returns Returns pointer to client shared secret buffer.
 ******************************************************************************/
uint8_t * get_client_shared_secret_buf_ptr(void);

/***************************************************************************//**
 * Get server shared secret buffer pointer.
 *
 * @returns Returns pointer to server shared secret buffer.
 ******************************************************************************/
uint8_t * get_server_shared_secret_buf_ptr(void);

/***************************************************************************//**
 * Get the length of the shared secret.
 *
 * @param key_type The asymmetric key type.
 ******************************************************************************/
size_t get_shared_secret_length(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Initialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_manager(void);

/***************************************************************************//**
 * Denitialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t deinit_se_manager(void);

/***************************************************************************//**
 * Generate a plain asymmetric key.
 *
 * @param key_type The asymmetric key type.
 * @param client True for client, false for server.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_plain_asymmetric_key(sl_se_key_type_t key_type, bool client);

/***************************************************************************//**
 * Export an asymmetric public key from plain asymmetric key.
 *
 * @param key_type The asymmetric key type.
 * @param client True for client, false for server.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t export_asymmetric_pubkey_from_plain(sl_se_key_type_t key_type,
                                                bool client);

/***************************************************************************//**
 * Compute ECDH shared secret from plain key.
 *
 * @param key_type The asymmetric key type.
 * @param client True for client, false for server.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t compute_plain_shared_secret(sl_se_key_type_t key_type, bool client);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Generate a non-exportable wrapped asymmetric key.
 *
 * @param key_type The asymmetric key type.
 * @param client True for client, false for server.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_wrap_asymmetric_key(sl_se_key_type_t key_type, bool client);

/***************************************************************************//**
 * Export an asymmetric public key from wrapped asymmetric key.
 *
 * @param key_type The asymmetric key type.
 * @param client True for client, false for server.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t export_asymmetric_pubkey_from_wrap(sl_se_key_type_t key_type,
                                               bool client);

/***************************************************************************//**
 * Generate a non-exportable asymmetric key into a volatile SE key slot.
 *
 * @param key_type The asymmetric key type.
 * @param client True for client, false for server.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_volatile_asymmetric_key(sl_se_key_type_t key_type,
                                           bool client);

/***************************************************************************//**
 * Export an asymmetric public key from a volatile SE key slot.
 *
 * @param key_type The asymmetric key type.
 * @param client True for client, false for server.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t export_asymmetric_pubkey_from_volatile(sl_se_key_type_t key_type,
                                                   bool client);

/***************************************************************************//**
 * Delete a non-exportable asymmetric key in a volatile SE key slot.
 *
 * @param key_type The asymmetric key type.
 * @param client True for client, false for server.
 * @returns Returns status code, @ref sl_status.h.
 *
 * @note For the delete operation to succeed, the properties of the key should
 *       be equal to those of the key that was previously placed in the slot.
 ******************************************************************************/
sl_status_t delete_volatile_asymmetric_key(sl_se_key_type_t key_type,
                                           bool client);

/***************************************************************************//**
 * Compute ECDH shared secret from wrapped key.
 *
 * @param key_type The asymmetric key type.
 * @param client True for client, false for server.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t compute_wrap_shared_secret(sl_se_key_type_t key_type, bool client);

/***************************************************************************//**
 * Compute ECDH shared secret from volatile key.
 *
 * @param key_type The asymmetric key type.
 * @param client True for client, false for server.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t compute_volatile_shared_secret(sl_se_key_type_t key_type,
                                           bool client);
#endif

#endif  // APP_SE_MANAGER_ECDH_H
