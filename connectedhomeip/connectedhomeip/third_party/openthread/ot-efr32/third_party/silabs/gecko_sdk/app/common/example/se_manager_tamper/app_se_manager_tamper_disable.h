/***************************************************************************//**
 * @file app_se_manager_tamper_disable.h
 * @brief SE manager tamper disable functions.
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
#ifndef APP_SE_MANAGER_TAMPER_DISABLE_H
#define APP_SE_MANAGER_TAMPER_DISABLE_H

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

/// Command to disable tamper
#define TAMPER_DISABLE_CMD      (0xfd020001)

/// Mask to disable tamper signals
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
#define TAMPER_DISABLE_MASK     (0x00fa0000)
#else
#define TAMPER_DISABLE_MASK     (0xf2000000)
#endif

/// Tamper disable ECC curve
#define TAMPER_DISABLE_ECC_ID   (SL_SE_KEY_TYPE_ECC_P256)

/// Tamper disable private key size
#define ECC_PRIVKEY_SIZE        (32)

/// Tamper disable public key size
#define ECC_PUBKEY_SIZE         (ECC_PRIVKEY_SIZE * 2)

/// Tamper disable signature size
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

/// Structure for tamper disable token
typedef struct {
  access_cert_t certificate;    // Access certificate
  uint8_t challenge_sig[64];    // The challenge_resp_t signature signed by private certificate key
} tamper_disable_token_t;       // 220 bytes

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get challenge buffer pointer.
 *
 * @returns Pointer to challenge buffer.
 ******************************************************************************/
uint8_t * get_challenge_buf_ptr(void);

/***************************************************************************//**
 * Compare exported public command key with OTP public command key.
 *
 * @returns Returns 0 if equal.
 ******************************************************************************/
int32_t compare_public_command_key(void);

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
 * Create a tamper disable token to disable tamper signals.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_tamper_disable_token(void);

/***************************************************************************//**
 * Roll challenge to invalidate the current tamper disable token.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t roll_challenge(void);

#endif  // APP_SE_MANAGER_TAMPER_DISABLE_H
