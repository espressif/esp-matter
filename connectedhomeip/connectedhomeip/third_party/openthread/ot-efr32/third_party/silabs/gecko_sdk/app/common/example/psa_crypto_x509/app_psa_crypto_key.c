/***************************************************************************//**
 * @file app_psa_crypto_key.c
 * @brief PSA Crypto key functions.
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_psa_crypto_key.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Key buffer
static uint8_t key_buf[KEY_BUF_SIZE];

/// Key length in bytes
static size_t key_len;

/// Key identifier
static psa_key_id_t key_id;

/// Key attributes
static psa_key_attributes_t key_attr;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get key buffer pointer.
 ******************************************************************************/
uint8_t * get_key_buf_ptr(void)
{
  return(key_buf);
}

/***************************************************************************//**
 * Get key length.
 ******************************************************************************/
size_t get_key_len(void)
{
  return(key_len);
}

/***************************************************************************//**
 * Get key identifier.
 ******************************************************************************/
psa_key_id_t get_key_id(void)
{
  return(key_id);
}

/***************************************************************************//**
 * Get key algorithm.
 ******************************************************************************/
psa_algorithm_t get_key_algo(void)
{
  psa_algorithm_t algo;

  if (psa_get_key_attributes(key_id, &key_attr) != PSA_SUCCESS) {
    return(0);
  }
  algo = psa_get_key_algorithm(&key_attr);

  // Free resources
  psa_reset_key_attributes(&key_attr);
  return(algo);
}

/***************************************************************************//**
 * Set key length.
 ******************************************************************************/
void set_key_len(size_t length)
{
  key_len = length;
}

/***************************************************************************//**
 * Set key identifier.
 ******************************************************************************/
void set_key_id(psa_key_id_t id)
{
  key_id = id;
}

/***************************************************************************//**
 * Initialize the PSA Crypto.
 ******************************************************************************/
psa_status_t init_psa_crypto(void)
{
  print_error_cycle(psa_crypto_init());
}

/***************************************************************************//**
 * Create or import a key.
 ******************************************************************************/
psa_status_t create_import_key(uint8_t storage_type,
                               psa_key_type_t type,
                               size_t size,
                               psa_key_id_t id,
                               psa_key_usage_t usage,
                               psa_algorithm_t algo)
{
  // Use algorithm to set key usage and type
  if (PSA_ALG_IS_RANDOMIZED_ECDSA(algo) || algo == PSA_ALG_PURE_EDDSA) {
    // DSA
    if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(type)) {
      // Private key
      if (algo == PSA_ALG_ECDSA_ANY) {
        usage |= PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
      } else {
        usage |= PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE;
      }
    } else {
      // Public key
      if (algo == PSA_ALG_ECDSA_ANY) {
        usage |= PSA_KEY_USAGE_VERIFY_HASH;
      } else {
        usage |= PSA_KEY_USAGE_VERIFY_MESSAGE;
      }
    }
  } else if (PSA_ALG_IS_ECDH(algo)) {
    // ECDH
    usage |= PSA_KEY_USAGE_DERIVE;
  } else if (algo == PSA_ALG_ECB_NO_PADDING
             || algo == PSA_ALG_CBC_NO_PADDING
             || algo == PSA_ALG_CFB
             || algo == PSA_ALG_CTR
             || algo == PSA_ALG_CCM
             || algo == PSA_ALG_GCM) {
    // AES cipher or AEAD
    type = PSA_KEY_TYPE_AES;
    usage |= PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
  } else if (algo == PSA_ALG_STREAM_CIPHER || algo == PSA_ALG_CHACHA20_POLY1305) {
    // CHACHA20 cipher or AEAD, key size must be 256-bit
    if (size != 256) {
      return(PSA_ERROR_INVALID_ARGUMENT);
    }
    type = PSA_KEY_TYPE_CHACHA20;
    usage |= PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
  } else if (algo == PSA_ALG_CMAC) {
    // CMAC
    type = PSA_KEY_TYPE_AES;
    usage |= PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
  } else if (PSA_ALG_IS_HMAC(algo)) {
    // HMAC
    type = PSA_KEY_TYPE_HMAC;
    usage |= PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
  } else if (PSA_ALG_IS_HKDF(algo)) {
    // HKDF
    type = PSA_KEY_TYPE_DERIVE;
    usage |= PSA_KEY_USAGE_DERIVE;
  } else {
    // Algorithm is not supported yet.
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  // Setup key attributes
  key_attr = psa_key_attributes_init();
  psa_set_key_type(&key_attr, type);
  psa_set_key_bits(&key_attr, size);
  psa_set_key_usage_flags(&key_attr, usage);
  psa_set_key_algorithm(&key_attr, algo);

  switch (storage_type) {
    // Plain key
    case (CREATE_KEY_OFFSET + VOLATILE_PLAIN_KEY):
    case (IMPORT_KEY_OFFSET + VOLATILE_PLAIN_KEY):
      break;

    case (CREATE_KEY_OFFSET + PERSISTENT_PLAIN_KEY):
    case (IMPORT_KEY_OFFSET + PERSISTENT_PLAIN_KEY):
      psa_set_key_id(&key_attr, id);
      break;

#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    // Wrapped key
    case (CREATE_KEY_OFFSET + VOLATILE_WRAP_KEY):
    case (IMPORT_KEY_OFFSET + VOLATILE_WRAP_KEY):
      psa_set_key_lifetime(&key_attr,
                           PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_VOLATILE,
                                                                          WRAP_KEY_LOCATION));
      break;

    case (CREATE_KEY_OFFSET + PERSISTENT_WRAP_KEY):
    case (IMPORT_KEY_OFFSET + PERSISTENT_WRAP_KEY):
      psa_set_key_id(&key_attr, id);
      psa_set_key_lifetime(&key_attr,
                           PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT,
                                                                          WRAP_KEY_LOCATION));
      break;
#endif

    // Invalid key storage
    default:
      return(PSA_ERROR_NOT_SUPPORTED);
  }

  // Create or import a key
  if (storage_type < IMPORT_KEY_OFFSET) {
    print_error_cycle(psa_generate_key(&key_attr, &key_id));
  } else {
    print_error_cycle(psa_import_key(&key_attr,
                                     key_buf,
                                     key_len,
                                     &key_id));
  }
}

/***************************************************************************//**
 * Copy a key.
 ******************************************************************************/
psa_status_t copy_key(psa_key_id_t id,
                      psa_key_usage_t usage,
                      psa_algorithm_t algo)
{
  // Use algorithm to set copied key usage
  // Key usage will be masked off by source key usage (bitwise-and)
  if (PSA_ALG_IS_RANDOMIZED_ECDSA(algo) || algo == PSA_ALG_PURE_EDDSA) {
    // DSA
    if (algo == PSA_ALG_ECDSA_ANY) {
      usage |= PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
    } else {
      usage |= PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE;
    }
  } else if (PSA_ALG_IS_ECDH(algo)) {
    // ECDH
    usage |= PSA_KEY_USAGE_DERIVE;
  } else if (algo == PSA_ALG_ECB_NO_PADDING
             || algo == PSA_ALG_CBC_NO_PADDING
             || algo == PSA_ALG_CFB
             || algo == PSA_ALG_CTR
             || algo == PSA_ALG_CCM
             || algo == PSA_ALG_GCM
             || algo == PSA_ALG_STREAM_CIPHER
             || algo == PSA_ALG_CHACHA20_POLY1305) {
    // AES or CHACHA20 (cipher or AEAD)
    usage |= PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
  } else if (algo == PSA_ALG_CMAC || PSA_ALG_IS_HMAC(algo)) {
    // CMAC or HMAC
    usage |= PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
  } else if (PSA_ALG_IS_HKDF(algo)) {
    // HKDF
    usage |= PSA_KEY_USAGE_DERIVE;
  } else {
    // Algorithm is not supported yet.
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  // Setup copied key attributes, key type and size are from source key
  key_attr = psa_key_attributes_init();
  psa_set_key_usage_flags(&key_attr, usage);
  psa_set_key_algorithm(&key_attr, algo);

  // Persistent plain key if id is non-zero
  if (id >= PSA_KEY_ID_USER_MIN) {
    psa_set_key_id(&key_attr, id);
  }

  // Copy a key, key_id will be overwritten by copied key id
  print_error_cycle(psa_copy_key(key_id,
                                 &key_attr,
                                 &key_id));
}

/***************************************************************************//**
 * Set up key attributes of the derived symmetric key.
 ******************************************************************************/
psa_status_t derive_key_attr(uint8_t storage_type,
                             size_t size,
                             psa_key_id_t id,
                             psa_key_usage_t usage,
                             psa_algorithm_t algo)
{
  psa_key_type_t type;

  if (algo == PSA_ALG_ECB_NO_PADDING
      || algo == PSA_ALG_CBC_NO_PADDING
      || algo == PSA_ALG_CFB
      || algo == PSA_ALG_CTR
      || algo == PSA_ALG_CCM
      || algo == PSA_ALG_GCM) {
    // AES cipher or AEAD
    type = PSA_KEY_TYPE_AES;
    usage |= PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
  } else if (algo == PSA_ALG_STREAM_CIPHER || algo == PSA_ALG_CHACHA20_POLY1305) {
    // CHACHA20 cipher or AEAD, key size must be 256-bit
    if (size != 256) {
      return(PSA_ERROR_INVALID_ARGUMENT);
    }
    type = PSA_KEY_TYPE_CHACHA20;
    usage |= PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
  } else if (algo == PSA_ALG_CMAC) {
    // CMAC
    type = PSA_KEY_TYPE_AES;
    usage |= PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
  } else if (PSA_ALG_IS_HMAC(algo)) {
    // HMAC
    type = PSA_KEY_TYPE_HMAC;
    usage |= PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
  } else {
    // Algorithm is not supported yet.
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  // Setup key attributes
  key_attr = psa_key_attributes_init();
  psa_set_key_type(&key_attr, type);
  psa_set_key_bits(&key_attr, size);
  psa_set_key_usage_flags(&key_attr, usage);
  psa_set_key_algorithm(&key_attr, algo);

  switch (storage_type) {
    // Plain key
    case (CREATE_KEY_OFFSET + VOLATILE_PLAIN_KEY):
    case (IMPORT_KEY_OFFSET + VOLATILE_PLAIN_KEY):
      break;

    case (CREATE_KEY_OFFSET + PERSISTENT_PLAIN_KEY):
    case (IMPORT_KEY_OFFSET + PERSISTENT_PLAIN_KEY):
      psa_set_key_id(&key_attr, id);
      break;

#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    // Wrapped key
    case (CREATE_KEY_OFFSET + VOLATILE_WRAP_KEY):
    case (IMPORT_KEY_OFFSET + VOLATILE_WRAP_KEY):
      psa_set_key_lifetime(&key_attr,
                           PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_VOLATILE,
                                                                          WRAP_KEY_LOCATION));
      break;

    case (CREATE_KEY_OFFSET + PERSISTENT_WRAP_KEY):
    case (IMPORT_KEY_OFFSET + PERSISTENT_WRAP_KEY):
      psa_set_key_id(&key_attr, id);
      psa_set_key_lifetime(&key_attr,
                           PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT,
                                                                          WRAP_KEY_LOCATION));
      break;
#endif

    // Invalid key storage
    default:
      return(PSA_ERROR_NOT_SUPPORTED);
  }

  return(PSA_SUCCESS);
}

/***************************************************************************//**
 * Derive a symmetric key.
 ******************************************************************************/
psa_status_t derive_key(psa_key_derivation_operation_t *operation)
{
  // Derive a key
  print_error_cycle(psa_key_derivation_output_key(&key_attr, operation,
                                                  &key_id));
}

/***************************************************************************//**
 * Derive a symmetric key using Silicon Labs custom API.
 ******************************************************************************/
psa_status_t sl_derive_key(psa_algorithm_t algo,
                           psa_key_id_t master_id,
                           const uint8_t *kdf_info,
                           size_t info_len,
                           const uint8_t *kdf_salt,
                           size_t salt_len,
                           size_t iterations)
{
  // Derive a key
  print_error_cycle(sl_psa_key_derivation_single_shot(algo, master_id,
                                                      kdf_info, info_len,
                                                      kdf_salt, salt_len,
                                                      iterations,
                                                      &key_attr,
                                                      &key_id));
}

/***************************************************************************//**
 * Free any auxiliary resources that the key attribute object might contain.
 ******************************************************************************/
void reset_key_attr(void)
{
  psa_reset_key_attributes(&key_attr);
}

/***************************************************************************//**
 * Export a key.
 ******************************************************************************/
psa_status_t export_key(void)
{
  print_error_cycle(psa_export_key(key_id,
                                   key_buf,
                                   sizeof(key_buf),
                                   &key_len));
}

/***************************************************************************//**
 * Export a public key.
 ******************************************************************************/
psa_status_t export_public_key(void)
{
  print_error_cycle(psa_export_public_key(key_id,
                                          key_buf,
                                          sizeof(key_buf),
                                          &key_len));
}

/***************************************************************************//**
 * Destroy a key.
 ******************************************************************************/
psa_status_t destroy_key(void)
{
  psa_key_id_t temp_id = key_id;

  // Reset key identifier
  key_id = 0;
  print_error_cycle(psa_destroy_key(temp_id));
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
