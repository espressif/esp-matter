/***************************************************************************//**
 * @file app_se_manager_symmetric_key_handling.c
 * @brief SE manager symmetric key handling functions.
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
#include "app_se_manager_symmetric_key_handling.h"

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
/// Buffer for symmetric plain or wrapped key
static uint8_t symmetric_key_buf[KEY_BUF_SIZE + WRAPPED_KEY_OVERHEAD];
#else
static uint8_t symmetric_key_buf[KEY_BUF_SIZE];
#endif

/// Custom symmetric key size
static uint8_t custom_key_size = CUSTOM_KEY_SIZE;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Get symmetric key buffer pointer.
 ******************************************************************************/
uint8_t * get_symmetric_key_buf_ptr(void)
{
  return(symmetric_key_buf);
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
 * Generate a plain symmetric key.
 ******************************************************************************/
sl_status_t create_plain_symmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an external key buffer
  sl_se_key_descriptor_t new_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = 0,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    // Set pointer to a RAM buffer to support key generation
    .storage.location.buffer.pointer = symmetric_key_buf,
    .storage.location.buffer.size = sizeof(symmetric_key_buf)
  };

  if (sl_se_validate_key(&new_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&new_key, &req_size) != SL_STATUS_OK
      || new_key.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_generate_key(&cmd_ctx, &new_key), &cmd_ctx);
}

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Import a plain symmetric key from buffer into an exportable wrapped key.
 ******************************************************************************/
sl_status_t import_plain_symmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an existing plain symmetric key
  sl_se_key_descriptor_t plain_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = 0,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = symmetric_key_buf,
    .storage.location.buffer.size = sizeof(symmetric_key_buf)
  };

  // Set up a key descriptor pointing to an exportable wrapped key buffer
  sl_se_key_descriptor_t wrap_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = 0,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = symmetric_key_buf,
    .storage.location.buffer.size = sizeof(symmetric_key_buf)
  };

  if (sl_se_validate_key(&wrap_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&wrap_key, &req_size) != SL_STATUS_OK
      || wrap_key.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_import_key(&cmd_ctx, &plain_key, &wrap_key),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Export a wrapped symmetric key into a plain key buffer.
 ******************************************************************************/
sl_status_t export_wrap_symmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an existing wrapped symmetric key
  sl_se_key_descriptor_t wrap_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = 0,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = symmetric_key_buf,
    .storage.location.buffer.size = sizeof(symmetric_key_buf)
  };

  // Set up a key descriptor pointing to a plain key buffer
  sl_se_key_descriptor_t plain_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = 0,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = symmetric_key_buf,
    .storage.location.buffer.size = sizeof(symmetric_key_buf)
  };

  if (sl_se_validate_key(&plain_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&plain_key, &req_size) != SL_STATUS_OK
      || plain_key.storage.location.buffer.size < req_size) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_export_key(&cmd_ctx, &wrap_key, &plain_key),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Generate a non-exportable wrapped symmetric key.
 ******************************************************************************/
sl_status_t create_wrap_symmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to a wrapped key buffer
  sl_se_key_descriptor_t new_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    // Set pointer to a RAM buffer to support key generation
    .storage.location.buffer.pointer = symmetric_key_buf,
    .storage.location.buffer.size = sizeof(symmetric_key_buf)
  };

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
 * Generate a non-exportable symmetric key into a volatile SE key slot.
 ******************************************************************************/
sl_status_t create_volatile_symmetric_key(sl_se_key_type_t key_type)
{
  // Set up a key descriptor pointing to a volatile SE key slot
  sl_se_key_descriptor_t new_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
    // This key is non-exportable, but can be used from the SE slot
    .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
    .storage.location.slot = SYMMETRIC_KEY_SLOT,
  };

  if (sl_se_validate_key(&new_key) != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_generate_key(&cmd_ctx, &new_key), &cmd_ctx);
}

/***************************************************************************//**
 * Delete a non-exportable symmetric key in a volatile SE key slot.
 ******************************************************************************/
sl_status_t delete_volatile_symmetric_key(sl_se_key_type_t key_type)
{
  // Set up a key descriptor pointing to an existing volatile SE key
  sl_se_key_descriptor_t volatile_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
    .storage.location.slot = SYMMETRIC_KEY_SLOT,
  };

  if (sl_se_validate_key(&volatile_key) != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_delete_key(&cmd_ctx, &volatile_key), &cmd_ctx);
}

/***************************************************************************//**
 * Transfer a non-exportable wrapped symmetric key into a volatile SE key slot.
 ******************************************************************************/
sl_status_t transfer_wrap_symmetric_key_to_volatile(sl_se_key_type_t key_type)
{
  // Set up a key descriptor pointing to an existing wrapped key
  sl_se_key_descriptor_t wrap_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = symmetric_key_buf,
    .storage.location.buffer.size = sizeof(symmetric_key_buf)
  };

  // Set up a key descriptor pointing to a volatile SE key slot
  sl_se_key_descriptor_t volatile_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
    .storage.location.slot = SYMMETRIC_KEY_SLOT,
  };

  if (sl_se_validate_key(&volatile_key) != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_transfer_key(&cmd_ctx, &wrap_key, &volatile_key),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Transfer a non-exportable symmetric key in a volatile SE key slot into
 * a wrapped key buffer.
 ******************************************************************************/
sl_status_t transfer_volatile_symmetric_key_to_wrap(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  // Set up a key descriptor pointing to an existing volatile SE key
  sl_se_key_descriptor_t volatile_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
    .storage.location.slot = SYMMETRIC_KEY_SLOT,
  };

  // Set up a key descriptor pointing to a wrapped key buffer
  sl_se_key_descriptor_t wrap_key = {
    .type = key_type,
    .size = custom_key_size,
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = symmetric_key_buf,
    .storage.location.buffer.size = sizeof(symmetric_key_buf)
  };

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
