/***************************************************************************//**
 * @file app_se_manager_asymmetric_key_handling.c
 * @brief SE manager asymmetric key handling functions.
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
#include "app_se_manager_asymmetric_key_handling.h"

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
/// Command context
static sl_se_command_context_t cmd_ctx;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/// Constants for custom secp256k1 curve
static const uint8_t p[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                             0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                             0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                             0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xfc, 0x2f };
static const uint8_t N[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                             0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
                             0xba, 0xae, 0xdc, 0xe6, 0xaf, 0x48, 0xa0, 0x3b,
                             0xbf, 0xd2, 0x5e, 0x8c, 0xd0, 0x36, 0x41, 0x41 };
static const uint8_t Gx[] = { 0x79, 0xbe, 0x66, 0x7e, 0xf9, 0xdc, 0xbb, 0xac,
                              0x55, 0xa0, 0x62, 0x95, 0xce, 0x87, 0x0b, 0x07,
                              0x02, 0x9b, 0xfc, 0xdb, 0x2d, 0xce, 0x28, 0xd9,
                              0x59, 0xf2, 0x81, 0x5b, 0x16, 0xf8, 0x17, 0x98 };
static const uint8_t Gy[] = { 0x48, 0x3a, 0xda, 0x77, 0x26, 0xa3, 0xc4, 0x65,
                              0x5d, 0xa4, 0xfb, 0xfc, 0x0e, 0x11, 0x08, 0xa8,
                              0xfd, 0x17, 0xb4, 0x48, 0xa6, 0x85, 0x54, 0x19,
                              0x9c, 0x47, 0xd0, 0x8f, 0xfb, 0x10, 0xd4, 0xb8 };
static const uint8_t a[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const uint8_t b[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07 };

/// Structure for custom ECC curve
static const sl_se_custom_weierstrass_prime_domain_t domain = {
  .size = DOMAIN_SIZE,
  .p = p,
  .N = N,
  .Gx = Gx,
  .Gy = Gy,
  .a = a,
  .b = b,
  .a_is_zero = true,
  .a_is_minus_three = false
};

/// Buffer for asymmetric plain or wrapped key
static uint8_t asymmetric_key_buf[ECC_PRIVKEY_SIZE + ECC_PUBKEY_SIZE + WRAPPED_KEY_OVERHEAD];

/// Buffer for asymmetric custom plain or wrapped key
static uint8_t asymmetric_custom_key_buf[DOMAIN_SIZE * 6 + DOMAIN_SIZE * 2 + DOMAIN_SIZE + WRAPPED_KEY_OVERHEAD];

/// Buffer for asymmetric custom public key
static uint8_t asymmetric_custom_pubkey_buf[DOMAIN_SIZE * 6 + DOMAIN_SIZE * 2];
#else
/// Buffer for asymmetric plain key
static uint8_t asymmetric_key_buf[ECC_PRIVKEY_SIZE + ECC_PUBKEY_SIZE];
#endif

/// Buffer for asymmetric public key
static uint8_t asymmetric_pubkey_buf[ECC_PUBKEY_SIZE];

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Get asymmetric public key buffer pointer.
 ******************************************************************************/
uint8_t * get_asymmetric_pubkey_buf_ptr(void)
{
  return(asymmetric_pubkey_buf);
}

/***************************************************************************//**
 * Get asymmetric custom public key buffer pointer.
 ******************************************************************************/
uint8_t * get_asymmetric_custom_pubkey_buf_ptr(void)
{
  return(asymmetric_custom_pubkey_buf);
}
#endif

/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t init_se_manager(void)
{
  print_error_cycle(sl_se_init(), NULL);
}

/***************************************************************************//**
 * Deinitialize the SE Manager.
 ******************************************************************************/
sl_status_t deinit_se_manager(void)
{
  print_error_cycle(sl_se_deinit(), NULL);
}

/***************************************************************************//**
 * Generate a plain asymmetric key.
 ******************************************************************************/
sl_status_t create_plain_asymmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an external key buffer
  sl_se_key_descriptor_t new_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    // Set pointer to a RAM buffer to support key generation
    .storage.location.buffer.pointer = asymmetric_key_buf,
    .storage.location.buffer.size = sizeof(asymmetric_key_buf)
  };

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    new_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    new_key.storage.location.buffer.pointer = asymmetric_custom_key_buf;
    new_key.storage.location.buffer.size = sizeof(asymmetric_custom_key_buf);
    new_key.domain = &domain;
  }
#endif

  if (sl_se_validate_key(&new_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&new_key, &req_size) != SL_STATUS_OK
      || new_key.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_generate_key(&cmd_ctx, &new_key), &cmd_ctx);
}

/***************************************************************************//**
 * Export an asymmetric public key from plain asymmetric key.
 ******************************************************************************/
sl_status_t export_asymmetric_pubkey_from_plain(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an existing plain asymmetric key
  sl_se_key_descriptor_t plain_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = asymmetric_key_buf,
    .storage.location.buffer.size = sizeof(asymmetric_key_buf)
  };

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    plain_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    plain_key.storage.location.buffer.pointer = asymmetric_custom_key_buf;
    plain_key.storage.location.buffer.size = sizeof(asymmetric_custom_key_buf);
    plain_key.domain = &domain;
  }
#endif

  // Set up a key descriptor pointing to an external key buffer
  sl_se_key_descriptor_t plain_pubkey = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = asymmetric_pubkey_buf,
    .storage.location.buffer.size = sizeof(asymmetric_pubkey_buf)
  };

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    plain_pubkey.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    plain_pubkey.storage.location.buffer.pointer = asymmetric_custom_pubkey_buf;
    plain_pubkey.storage.location.buffer.size = sizeof(asymmetric_custom_pubkey_buf);
    plain_pubkey.domain = &domain;
  }
#endif

  if (sl_se_validate_key(&plain_pubkey) != SL_STATUS_OK
      || sl_se_get_storage_size(&plain_pubkey, &req_size) != SL_STATUS_OK
      || plain_pubkey.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_export_public_key(&cmd_ctx, &plain_key,
                                            &plain_pubkey), &cmd_ctx);
}

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Import a plain asymmetric key from buffer into an exportable wrapped key.
 ******************************************************************************/
sl_status_t import_plain_asymmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an existing plain asymmetric key
  sl_se_key_descriptor_t plain_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = asymmetric_key_buf,
    .storage.location.buffer.size = sizeof(asymmetric_key_buf)
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    plain_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    plain_key.storage.location.buffer.pointer = asymmetric_custom_key_buf;
    plain_key.storage.location.buffer.size = sizeof(asymmetric_custom_key_buf);
    plain_key.domain = &domain;
  }

  // Set up a key descriptor pointing to an exportable wrapped key buffer
  sl_se_key_descriptor_t wrap_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = asymmetric_key_buf,
    .storage.location.buffer.size = sizeof(asymmetric_key_buf)
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    wrap_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    wrap_key.storage.location.buffer.pointer = asymmetric_custom_key_buf;
    wrap_key.storage.location.buffer.size = sizeof(asymmetric_custom_key_buf);
    wrap_key.domain = &domain;
  }

  if (sl_se_validate_key(&wrap_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&wrap_key, &req_size) != SL_STATUS_OK
      || wrap_key.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_import_key(&cmd_ctx, &plain_key, &wrap_key),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Export a wrapped asymmetric key into a plain key buffer.
 ******************************************************************************/
sl_status_t export_wrap_asymmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an existing wrapped asymmetric key
  sl_se_key_descriptor_t wrap_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = asymmetric_key_buf,
    .storage.location.buffer.size = sizeof(asymmetric_key_buf)
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    wrap_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    wrap_key.storage.location.buffer.pointer = asymmetric_custom_key_buf;
    wrap_key.storage.location.buffer.size = sizeof(asymmetric_custom_key_buf);
    wrap_key.domain = &domain;
  }

  // Set up a key descriptor pointing to a plain key buffer
  sl_se_key_descriptor_t plain_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = asymmetric_key_buf,
    .storage.location.buffer.size = sizeof(asymmetric_key_buf)
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    plain_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    plain_key.storage.location.buffer.pointer = asymmetric_custom_key_buf;
    plain_key.storage.location.buffer.size = sizeof(asymmetric_custom_key_buf);
    plain_key.domain = &domain;
  }

  if (sl_se_validate_key(&plain_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&plain_key, &req_size) != SL_STATUS_OK
      || plain_key.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_export_key(&cmd_ctx, &wrap_key, &plain_key),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Generate a non-exportable wrapped asymmetric key.
 ******************************************************************************/
sl_status_t create_wrap_asymmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to a wrapped key buffer
  sl_se_key_descriptor_t new_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    // Set pointer to a RAM buffer to support key generation
    .storage.location.buffer.pointer = asymmetric_key_buf,
    .storage.location.buffer.size = sizeof(asymmetric_key_buf)
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    new_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    new_key.storage.location.buffer.pointer = asymmetric_custom_key_buf;
    new_key.storage.location.buffer.size = sizeof(asymmetric_custom_key_buf);
    new_key.domain = &domain;
  }

  // The size of the wrapped key buffer must have space for the overhead of the
  // key wrapping
  if (sl_se_validate_key(&new_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&new_key, &req_size) != SL_STATUS_OK
      || new_key.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_generate_key(&cmd_ctx, &new_key), &cmd_ctx);
}

/***************************************************************************//**
 * Export an asymmetric public key from wrapped asymmetric key.
 ******************************************************************************/
sl_status_t export_asymmetric_pubkey_from_wrap(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an existing wrapped key
  sl_se_key_descriptor_t wrap_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = asymmetric_key_buf,
    .storage.location.buffer.size = sizeof(asymmetric_key_buf)
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    wrap_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    wrap_key.storage.location.buffer.pointer = asymmetric_custom_key_buf;
    wrap_key.storage.location.buffer.size = sizeof(asymmetric_custom_key_buf);
    wrap_key.domain = &domain;
  }

  // Set up a key descriptor pointing to an external key buffer
  sl_se_key_descriptor_t plain_pubkey = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = asymmetric_pubkey_buf,
    .storage.location.buffer.size = sizeof(asymmetric_pubkey_buf)
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    plain_pubkey.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    plain_pubkey.storage.location.buffer.pointer = asymmetric_custom_pubkey_buf;
    plain_pubkey.storage.location.buffer.size = sizeof(asymmetric_custom_pubkey_buf);
    plain_pubkey.domain = &domain;
  }

  if (sl_se_validate_key(&plain_pubkey) != SL_STATUS_OK
      || sl_se_get_storage_size(&plain_pubkey, &req_size) != SL_STATUS_OK
      || plain_pubkey.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_export_public_key(&cmd_ctx, &wrap_key,
                                            &plain_pubkey), &cmd_ctx);
}

/***************************************************************************//**
 * Generate a non-exportable asymmetric key into a volatile SE key slot.
 ******************************************************************************/
sl_status_t create_volatile_asymmetric_key(sl_se_key_type_t key_type)
{
  // Set up a key descriptor pointing to a volatile SE key slot
  sl_se_key_descriptor_t new_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    // This key is non-exportable, but can be used from the SE slot
    .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
    .storage.location.slot = ASYMMETRIC_KEY_SLOT,
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    new_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    new_key.domain = &domain;
  }

  if (sl_se_validate_key(&new_key) != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_generate_key(&cmd_ctx, &new_key), &cmd_ctx);
}

/***************************************************************************//**
 * Export an asymmetric public key from a volatile SE key slot.
 ******************************************************************************/
sl_status_t export_asymmetric_pubkey_from_volatile(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an existing volatile SE key
  sl_se_key_descriptor_t volatile_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
    .storage.location.slot = ASYMMETRIC_KEY_SLOT,
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    volatile_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    volatile_key.domain = &domain;
  }

  // Set up a key descriptor pointing to an external key buffer
  sl_se_key_descriptor_t plain_pubkey = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = asymmetric_pubkey_buf,
    .storage.location.buffer.size = sizeof(asymmetric_pubkey_buf)
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    plain_pubkey.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    plain_pubkey.storage.location.buffer.pointer = asymmetric_custom_pubkey_buf;
    plain_pubkey.storage.location.buffer.size = sizeof(asymmetric_custom_pubkey_buf);
    plain_pubkey.domain = &domain;
  }

  if (sl_se_validate_key(&plain_pubkey) != SL_STATUS_OK
      || sl_se_get_storage_size(&plain_pubkey, &req_size) != SL_STATUS_OK
      || plain_pubkey.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_export_public_key(&cmd_ctx, &volatile_key,
                                            &plain_pubkey), &cmd_ctx);
}

/***************************************************************************//**
 * Delete a non-exportable asymmetric key in a volatile SE key slot.
 ******************************************************************************/
sl_status_t delete_volatile_asymmetric_key(sl_se_key_type_t key_type)
{
  // Set up a key descriptor pointing to an existing volatile SE key
  sl_se_key_descriptor_t volatile_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
    .storage.location.slot = ASYMMETRIC_KEY_SLOT,
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    volatile_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    volatile_key.domain = &domain;
  }

  if (sl_se_validate_key(&volatile_key) != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_delete_key(&cmd_ctx, &volatile_key), &cmd_ctx);
}

/***************************************************************************//**
 * Transfer a non-exportable wrapped asymmetric key into a volatile SE key slot.
 ******************************************************************************/
sl_status_t transfer_wrap_asymmetric_key_to_volatile(sl_se_key_type_t key_type)
{
  // Set up a key descriptor pointing to an existing wrapped key
  sl_se_key_descriptor_t wrap_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = asymmetric_key_buf,
    .storage.location.buffer.size = sizeof(asymmetric_key_buf)
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    wrap_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    wrap_key.storage.location.buffer.pointer = asymmetric_custom_key_buf;
    wrap_key.storage.location.buffer.size = sizeof(asymmetric_custom_key_buf);
    wrap_key.domain = &domain;
  }

  // Set up a key descriptor pointing to a volatile SE key slot
  sl_se_key_descriptor_t volatile_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
    .storage.location.slot = ASYMMETRIC_KEY_SLOT,
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    volatile_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    volatile_key.domain = &domain;
  }

  if (sl_se_validate_key(&volatile_key) != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_transfer_key(&cmd_ctx, &wrap_key, &volatile_key),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Transfer a non-exportable asymmetric key in a volatile SE key slot into
 * a wrapped key buffer.
 ******************************************************************************/
sl_status_t transfer_volatile_asymmetric_key_to_wrap(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an existing volatile SE key
  sl_se_key_descriptor_t volatile_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
    .storage.location.slot = ASYMMETRIC_KEY_SLOT,
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    volatile_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    volatile_key.domain = &domain;
  }

  // Set up a key descriptor pointing to a wrapped key buffer
  sl_se_key_descriptor_t wrap_key = {
    .type = key_type,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = asymmetric_key_buf,
    .storage.location.buffer.size = sizeof(asymmetric_key_buf)
  };

  if (key_type == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    wrap_key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_USES_CUSTOM_DOMAIN;
    wrap_key.storage.location.buffer.pointer = asymmetric_custom_key_buf;
    wrap_key.storage.location.buffer.size = sizeof(asymmetric_custom_key_buf);
    wrap_key.domain = &domain;
  }

  if (sl_se_validate_key(&wrap_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&wrap_key, &req_size) != SL_STATUS_OK
      || wrap_key.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_transfer_key(&cmd_ctx, &volatile_key, &wrap_key),
                    &cmd_ctx);
}
#endif

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
