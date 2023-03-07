/***************************************************************************//**
 * @file app_se_manager_secure_debug.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_secure_debug.h"

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

/// SE status buffer
static sl_se_status_t se_status_buf;

/// Debug status buffer
static sl_se_debug_status_t debug_lock_status_buf;

/// Serial number buffer
static uint8_t serial_number[16];

/// Challenge (nonce) buffer
static sl_se_challenge_t nonce;

/// Public command key buffer
static uint8_t pub_cmd_key[64];

/// Hard-coded unlock token
static const uint8_t unlock_token[220] = {
  0x01, 0xCE, 0xEC, 0xE5, 0x3E, 0x00, 0x00, 0x00, 0xB6, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x58, 0x8E, 0x81, 0xFF, 0xFE, 0x70, 0x34, 0xE5, 0x8F, 0x07, 0xF6, 0x78,
  0x0F, 0x1E, 0xF6, 0x25, 0x04, 0x8C, 0xDC, 0xF9, 0x63, 0xEA, 0x59, 0x30, 0x06, 0x8B, 0x8F, 0xDC,
  0x6F, 0x2E, 0x16, 0xFB, 0x08, 0xDE, 0x26, 0x0E, 0x68, 0x27, 0xCF, 0x78, 0x31, 0xA8, 0xA2, 0xA3,
  0xD6, 0x87, 0xAB, 0x88, 0x89, 0xC1, 0xB7, 0x24, 0xF6, 0x75, 0x75, 0x7E, 0x82, 0xDE, 0xCD, 0x16,
  0x56, 0x56, 0x86, 0x0F, 0xEA, 0xEC, 0x58, 0xD6, 0xCF, 0xEE, 0x49, 0x55, 0x21, 0x92, 0x3C, 0xA3,
  0xA2, 0x4E, 0x08, 0xAF, 0xEB, 0x6D, 0x14, 0xD0, 0x8F, 0xB0, 0xF1, 0xA4, 0x67, 0xC8, 0xAE, 0xBF,
  0xB4, 0x46, 0xDD, 0x93, 0x5E, 0xB2, 0x1E, 0xD8, 0xF9, 0xD7, 0xC8, 0x51, 0xD1, 0xFF, 0xC4, 0xCC,
  0xF2, 0x39, 0x60, 0x05, 0x07, 0xC3, 0x5D, 0x5D, 0x25, 0xD5, 0xA3, 0xA5, 0x7B, 0x73, 0x53, 0xC7,
  0xAD, 0x06, 0xA0, 0x1B, 0x23, 0xF6, 0xE0, 0x94, 0xD3, 0x31, 0xAE, 0xB9, 0x7E, 0xCA, 0xFA, 0x0E,
  0x1A, 0xE1, 0xCC, 0xBF, 0x4A, 0xA5, 0xCF, 0x05, 0x79, 0x3D, 0x5E, 0x92, 0xDF, 0xEA, 0x3F, 0x9D,
  0xDB, 0x8F, 0xD1, 0xC0, 0xF0, 0xAA, 0x21, 0x93, 0xA4, 0x80, 0xE9, 0x34, 0x5B, 0x92, 0xC4, 0xE3,
  0xF1, 0x43, 0xD6, 0xD1, 0xA2, 0x15, 0xFC, 0xD8, 0xDF, 0xE1, 0x02, 0x8B, 0x49, 0xF1, 0x44, 0xDF,
  0x68, 0xFA, 0xCD, 0x9B, 0x4A, 0x4F, 0x46, 0xF4, 0x14, 0xF6, 0x9A, 0xB3
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get SE status buffer pointer.
 ******************************************************************************/
sl_se_status_t * get_se_status_buf_ptr(void)
{
  return(&se_status_buf);
}

/***************************************************************************//**
 * Get debug lock status buffer pointer.
 ******************************************************************************/
sl_se_debug_status_t * get_debug_lock_status_buf_ptr(void)
{
  return(&debug_lock_status_buf);
}

/***************************************************************************//**
 * Get serial number buffer pointer.
 ******************************************************************************/
uint8_t * get_serialno_buf_ptr(void)
{
  return(serial_number);
}

/***************************************************************************//**
 * Get challenge buffer pointer.
 ******************************************************************************/
uint8_t * get_challenge_buf_ptr(void)
{
  return(nonce);
}

/***************************************************************************//**
 * Get public command key buffer pointer.
 ******************************************************************************/
uint8_t * get_pubcmdkey_buf_ptr(void)
{
  return(pub_cmd_key);
}

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
 * Get SE status.
 ******************************************************************************/
sl_status_t get_se_status(void)
{
  print_error_cycle(sl_se_get_status(&cmd_ctx, &se_status_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Get debug lock status.
 ******************************************************************************/
sl_status_t get_debug_lock_status(void)
{
  print_error_cycle(sl_se_get_debug_lock_status(&cmd_ctx,
                                                &debug_lock_status_buf),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Read public command key from SE OTP.
 ******************************************************************************/
sl_status_t read_public_command_key(void)
{
  print_error_cycle(sl_se_read_pubkey(&cmd_ctx,
                                      SL_SE_KEY_TYPE_IMMUTABLE_AUTH,
                                      pub_cmd_key,
                                      sizeof(pub_cmd_key)),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Enable the debug lock.
 ******************************************************************************/
sl_status_t enable_debug_lock(void)
{
  print_error_cycle(sl_se_apply_debug_lock(&cmd_ctx), &cmd_ctx);
}

/***************************************************************************//**
 * Perform a device mass erase and debug unlock.
 ******************************************************************************/
sl_status_t disable_debug_lock(void)
{
  print_error_cycle(sl_se_erase_device(&cmd_ctx), &cmd_ctx);
}

/***************************************************************************//**
 * Enable the secure debug.
 ******************************************************************************/
sl_status_t enable_secure_debug(void)
{
  print_error_cycle(sl_se_enable_secure_debug(&cmd_ctx), &cmd_ctx);
}

/***************************************************************************//**
 * Disable the secure debug.
 ******************************************************************************/
sl_status_t disable_secure_debug(void)
{
  print_error_cycle(sl_se_disable_secure_debug(&cmd_ctx), &cmd_ctx);
}

/***************************************************************************//**
 * Disable the device erase.
 ******************************************************************************/
sl_status_t disable_device_erase(void)
{
  // Warning: This is a ONE-TIME command which PERMANETLY disables device erase
  print_error_cycle(sl_se_disable_device_erase(&cmd_ctx), &cmd_ctx);
}

/***************************************************************************//**
 * Set debug options.
 ******************************************************************************/
sl_status_t set_debug_option(void)
{
  // Configure debug options
  debug_lock_status_buf.options_config.non_secure_invasive_debug = true;
  debug_lock_status_buf.options_config.non_secure_non_invasive_debug = true;
  debug_lock_status_buf.options_config.secure_invasive_debug = true;
  debug_lock_status_buf.options_config.secure_non_invasive_debug = true;

  print_error_cycle(sl_se_set_debug_options(&cmd_ctx,
                                            &debug_lock_status_buf.options_config),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Read the serial number of the SE and save it to access certificate.
 ******************************************************************************/
sl_status_t read_serial_number(void)
{
  print_error_cycle(sl_se_get_serialnumber(&cmd_ctx,
                                           serial_number),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Request challenge from the SE and save it to challenge response.
 ******************************************************************************/
sl_status_t request_challenge(void)
{
  print_error_cycle(sl_se_get_challenge(&cmd_ctx, nonce),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Use an unlock token to unlock the device.
 ******************************************************************************/
sl_status_t use_unlock_token(void)
{
  // Unlock the device with current debug options
  print_error_cycle(sl_se_open_debug(&cmd_ctx,
                                     (uint8_t *)unlock_token,
                                     sizeof(unlock_token),
                                     &debug_lock_status_buf.options_config),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Roll challenge to invalidate the current unlock token.
 ******************************************************************************/
sl_status_t roll_challenge(void)
{
  print_error_cycle(sl_se_roll_challenge(&cmd_ctx), &cmd_ctx);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
