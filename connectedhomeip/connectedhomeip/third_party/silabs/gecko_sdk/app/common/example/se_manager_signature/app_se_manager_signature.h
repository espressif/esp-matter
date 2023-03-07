/***************************************************************************//**
 * @file app_se_manager_signature.h
 * @brief SE manager signature functions.
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
#ifndef APP_SE_MANAGER_SIGNATURE_H
#define APP_SE_MANAGER_SIGNATURE_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_entropy.h"
#include "sl_se_manager_key_handling.h"
#include "sl_se_manager_signature.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Overhead of wrapped key buffer
#define WRAPPED_KEY_OVERHEAD      (12 + 16)

/// Padding bytes for ECC P521
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG) \
  && (_SILICON_LABS_32B_SERIES_2_CONFIG < 3)
  #define P521_PADDING_BYTES      2
#else
  #define P521_PADDING_BYTES      0
#endif

/// Private key size of EdDSA key
#define ECC_ED25519_PRIVKEY_SIZE  (SL_SE_KEY_TYPE_ECC_ED25519 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK)

/// Private key size of ECC Weierstrass Prime keys
#define ECC_P192_PRIVKEY_SIZE     (SL_SE_KEY_TYPE_ECC_P192 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK)
#define ECC_P256_PRIVKEY_SIZE     (SL_SE_KEY_TYPE_ECC_P256 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK)
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
#define ECC_P384_PRIVKEY_SIZE     (SL_SE_KEY_TYPE_ECC_P384 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK)
#define ECC_P521_PRIVKEY_SIZE     ((SL_SE_KEY_TYPE_ECC_P521 & SL_SE_KEY_TYPE_ATTRIBUTES_MASK) + P521_PADDING_BYTES)

/// Use the biggest ECC curve as private key size
#define ECC_PRIVKEY_SIZE          (ECC_P521_PRIVKEY_SIZE)

/// Domain size of asymmetric custom key
#define DOMAIN_SIZE               (32)
#else
/// Use the biggest ECC curve as private key size
#define ECC_PRIVKEY_SIZE          (ECC_P256_PRIVKEY_SIZE)
#endif

/// Public key size = private key size x 2
#define ECC_PUBKEY_SIZE           (ECC_PRIVKEY_SIZE * 2)

/// Signature size = private key size x 2
#define SIGNATURE_SIZE            (ECC_PRIVKEY_SIZE * 2)

/// Plain message buffer size
#define PLAIN_MSG_SIZE            (4096)

/// Internal SE key slot used for asymmetric key
#define ASYMMETRIC_KEY_SLOT       (SL_SE_KEY_SLOT_VOLATILE_0)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get plain message buffer pointer.
 *
 * @returns Returns pointer to plain message buffer.
 ******************************************************************************/
uint8_t * get_plain_msg_buf_ptr(void);

/***************************************************************************//**
 * Set plain message length.
 *
 * @param length The length of the plain message.
 ******************************************************************************/
void set_plain_msg_len(size_t length);

/***************************************************************************//**
 * Get the length of the computed signature.
 *
 * @returns Returns the computed signature length.
 ******************************************************************************/
size_t get_signature_len(sl_se_key_type_t key_type);

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
 * Generate random numbers and save them to a buffer.
 *
 * @param buf The buffer pointer for random number.
 * @param size The size of the buffer.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t generate_random_number(uint8_t *buf, uint32_t size);

/***************************************************************************//**
 * Generate a plain asymmetric key.
 *
 * @param key_type The asymmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_plain_asymmetric_key(sl_se_key_type_t key_type);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Generate a non-exportable wrapped asymmetric key.
 *
 * @param key_type The asymmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_wrap_asymmetric_key(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Generate a non-exportable asymmetric key into a volatile SE key slot.
 *
 * @param key_type The asymmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_volatile_asymmetric_key(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Delete a non-exportable asymmetric key in a volatile SE key slot.
 *
 * @param key_type The asymmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 *
 * @note For the delete operation to succeed, the properties of the key should
 *       be equal to those of the key that was previously placed in the slot.
 ******************************************************************************/
sl_status_t delete_volatile_asymmetric_key(sl_se_key_type_t key_type);
#endif

/***************************************************************************//**
 * Export the public key from private key to verify the signature.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t export_public_key(void);

/**************************************************************************//**
* Sign the message with private key.
*
* @param hash_algo The Hash algorithm to be used for signing.
* @returns Returns status code, @ref sl_status.h.
*
* @note The input parameter hash_algo does not apply for EdDSA.
******************************************************************************/
sl_status_t sign_message(sl_se_hash_type_t hash_algo);

/**************************************************************************//**
* Verify the signature with public key.
*
* @param hash_algo The Hash algorithm to be used for signature verification.
* @returns Returns status code, @ref sl_status.h.
*
* @note The input parameter hash_algo does not apply for EdDSA.
******************************************************************************/
sl_status_t verify_signature(sl_se_hash_type_t hash_algo);

#endif  // APP_SE_MANAGER_SIGNATURE_H
