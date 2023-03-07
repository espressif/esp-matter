/***************************************************************************//**
 * @file app_se_manager_symmetric_key_handling.h
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
#ifndef APP_SE_MANAGER_SYMMETRIC_KEY_HANDLING_H
#define APP_SE_MANAGER_SYMMETRIC_KEY_HANDLING_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_key_handling.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Symmetric key buffer size
#define KEY_BUF_SIZE            (32)

/// Overhead of wrapped key buffer
#define WRAPPED_KEY_OVERHEAD    (12 + 16)

/// Custom key size
#define CUSTOM_KEY_SIZE         (28)

/// Internal SE key slot used for symmetric key
#define SYMMETRIC_KEY_SLOT      (SL_SE_KEY_SLOT_VOLATILE_0)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Get symmetric key buffer pointer.
 *
 * @returns Returns pointer to symmetric key buffer.
 ******************************************************************************/
uint8_t * get_symmetric_key_buf_ptr(void);
#endif

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
 * Generate a plain symmetric key.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_plain_symmetric_key(sl_se_key_type_t key_type);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Import a plain symmetric key from buffer into an exportable wrapped key.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 *
 * @note In order to run the export key example, the
 *       SL_SE_KEY_FLAG_NON_EXPORTABLE flag is not set in the wrapped key.
 ******************************************************************************/
sl_status_t import_plain_symmetric_key(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Export a wrapped symmetric key into a plain key buffer.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 *
 * @note For export of key to work, no restriction flags must be set and the
 *       properties of the wrapped key struct must be identical to the ones used
 *       when the key was created (imported or generated).
 ******************************************************************************/
sl_status_t export_wrap_symmetric_key(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Generate a non-exportable wrapped symmetric key.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_wrap_symmetric_key(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Generate a non-exportable symmetric key into a volatile SE key slot.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_volatile_symmetric_key(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Delete a non-exportable symmetric key in a volatile SE key slot.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 *
 * @note For the delete operation to succeed, the properties of the key should
 *       be equal to those of the key that was previously placed in the slot.
 ******************************************************************************/
sl_status_t delete_volatile_symmetric_key(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Transfer a non-exportable wrapped symmetric key into a volatile SE key slot.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t transfer_wrap_symmetric_key_to_volatile(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Transfer a non-exportable symmetric key in a volatile SE key slot into
 * a wrapped key buffer.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t transfer_volatile_symmetric_key_to_wrap(sl_se_key_type_t key_type);
#endif

#endif  // APP_SE_MANAGER_SYMMETRIC_KEY_HANDLING_H
