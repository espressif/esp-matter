/***************************************************************************//**
 * @file app_se_manager_secure_debug.h
 * @brief SE manager secure debug functions.
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
#ifndef APP_SE_MANAGER_SECURE_DEBUG_H
#define APP_SE_MANAGER_SECURE_DEBUG_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_signature.h"
#include "sl_se_manager_util.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Overhead of wrapped key buffer
#define WRAPPED_KEY_OVERHEAD    (12 + 16)

/// Access certificate magic number
#define CERT_MAGIC_NUM          (0xe5ecce01)

/// Debug authorization bits
#define DEBUG_AUTHORIZATION     (0x003e)

/// Tamper authorization bits
#define TAMPER_AUTHORIZATION    (0xffffffb6)

/// Command to open the Cortex-M33 debug port
#define DEBUG_ACCESS_CMD        (0xfd010001)

/// Secure debug ECC curve
#define SECURE_DEBUG_ECC_ID     (SL_SE_KEY_TYPE_ECC_P256)

/// Secure debug private key size
#define ECC_PRIVKEY_SIZE        (32)

/// Secure debug public key size
#define ECC_PUBKEY_SIZE         (ECC_PRIVKEY_SIZE * 2)

/// Secure debug signature size
#define ECC_SIGNATURE_SIZE      (ECC_PRIVKEY_SIZE * 2)

/// Structure for access certificate
typedef struct {
  uint32_t magic_number;        // Certificate magic number
  uint16_t auth_debug;          // Debug authorization bits
  uint8_t  auth_misc;           // Miscellaneous authorization bits (set to 0)
  uint8_t  key_index;           // Manufacturer key index (set to 0)
  uint32_t auth_tamper;         // Tamper authorization bits
  uint8_t  serialno[16];        // Serial number
  uint8_t  pubkey[64];          // Public certificate key
  uint8_t  signature[64];       // Certificate signature
} access_cert_t;                // 156 bytes

/// Structure for challenge response
typedef struct {
  uint32_t command_value;       // The value of this command
  uint32_t parameter;           // The value of this command parameter
  uint8_t  nonce[16];           // Value of the current challenge
} challenge_resp_t;             // 24 bytes

/// Structure for unlock token
typedef struct {
  access_cert_t certificate;    // Access certificate
  uint8_t challenge_sig[64];    // The challenge_resp_t signature signed by private certificate key
} unlock_token_t;               // 220 bytes

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get SE status buffer pointer.
 *
 * @returns Returns pointer to SE status buffer.
 ******************************************************************************/
sl_se_status_t * get_se_status_buf_ptr(void);

/***************************************************************************//**
 * Get debug lock status buffer pointer.
 *
 * @returns Returns pointer to debug lock status buffer.
 ******************************************************************************/
sl_se_debug_status_t * get_debug_lock_status_buf_ptr(void);

/***************************************************************************//**
 * Get challenge buffer pointer.
 *
 * @returns Returns pointer to challenge buffer.
 ******************************************************************************/
uint8_t * get_challenge_buf_ptr(void);

/***************************************************************************//**
 * Compare exported public command key with OTP public command key.
 *
 * @returns Returns 0 if equal.
 ******************************************************************************/
int32_t compare_public_command_key(void);

/***************************************************************************//**
 * Initialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_manager(void);

/***************************************************************************//**
 * Deinitialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t deinit_se_manager(void);

/***************************************************************************//**
 * Get SE status.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_se_status(void);

/***************************************************************************//**
 * Get debug lock status.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t get_debug_lock_status(void);

/***************************************************************************//**
 * Create a private certificate key.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_private_certificate_key(void);

/***************************************************************************//**
 * Export a public certificate key from a private certificate key.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t export_public_certificate_key(void);

/***************************************************************************//**
 * Read public command key from SE OTP.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t read_public_command_key(void);

/***************************************************************************//**
 * Export a public command key from a private command key.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t export_public_command_key(void);

/***************************************************************************//**
 * Program public command key to the SE OTP.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t program_public_command_key(void);

/***************************************************************************//**
 * Enable the debug lock.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t enable_debug_lock(void);

/***************************************************************************//**
 * Perform a device mass erase and debug unlock.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t disable_debug_lock(void);

/***************************************************************************//**
 * Enable the secure debug.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t enable_secure_debug(void);

/***************************************************************************//**
 * Disable the secure debug.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t disable_secure_debug(void);

/***************************************************************************//**
 * Disable the device erase.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t disable_device_erase(void);

/***************************************************************************//**
 * Set debug options.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t set_debug_option(void);

/***************************************************************************//**
 * Read the serial number of the SE and save it to access certificate.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t read_serial_number(void);

/***************************************************************************//**
 * Sign access certificate with private command key.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sign_access_certificate(void);

/***************************************************************************//**
 * Request challenge from the SE and save it to challenge response.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t request_challenge(void);

/***************************************************************************//**
 * Sign challenge response with private certificate key.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sign_challenge_response(void);

/***************************************************************************//**
 * Create an unlock token to unlock the device.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_unlock_token(void);

/***************************************************************************//**
 * Roll challenge to invalidate the current unlock token.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t roll_challenge(void);

#endif  // APP_SE_MANAGER_SECURE_DEBUG_H
