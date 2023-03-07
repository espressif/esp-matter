/***************************************************************************//**
 * @file app_se_manager_tamper_disable.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_tamper_disable.h"

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

/// Serial number buffer
static uint8_t serial_number[16];

/// Challenge (nonce) buffer
static sl_se_challenge_t nonce;

/// Public command key buffer
static uint8_t pub_cmd_key[64];

/// Hard-coded unlock token
static const uint8_t tamper_disable_token[220] = {
  0x01, 0xCE, 0xEC, 0xE5, 0x3E, 0x00, 0x00, 0x00, 0xB6, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x58, 0x8E, 0x81, 0xFF, 0xFE, 0x70, 0x34, 0xE5, 0x8F, 0x07, 0xF6, 0x78,
  0x0F, 0x1E, 0xF6, 0x25, 0x04, 0x8C, 0xDC, 0xF9, 0x63, 0xEA, 0x59, 0x30, 0x06, 0x8B, 0x8F, 0xDC,
  0x6F, 0x2E, 0x16, 0xFB, 0x08, 0xDE, 0x26, 0x0E, 0x68, 0x27, 0xCF, 0x78, 0x31, 0xA8, 0xA2, 0xA3,
  0xD6, 0x87, 0xAB, 0x88, 0x89, 0xC1, 0xB7, 0x24, 0xF6, 0x75, 0x75, 0x7E, 0x82, 0xDE, 0xCD, 0x16,
  0x56, 0x56, 0x86, 0x0F, 0xEA, 0xEC, 0x58, 0xD6, 0xCF, 0xEE, 0x49, 0x55, 0x21, 0x92, 0x3C, 0xA3,
  0xA2, 0x4E, 0x08, 0xAF, 0xEB, 0x6D, 0x14, 0xD0, 0x8F, 0xB0, 0xF1, 0xA4, 0x67, 0xC8, 0xAE, 0xBF,
  0xB4, 0x46, 0xDD, 0x93, 0x5E, 0xB2, 0x1E, 0xD8, 0xF9, 0xD7, 0xC8, 0x51, 0xD1, 0xFF, 0xC4, 0xCC,
  0xF2, 0x39, 0x60, 0x05, 0x07, 0xC3, 0x5D, 0x5D, 0x25, 0xD5, 0xA3, 0xA5, 0x7B, 0x73, 0x53, 0xC7,
  0xAD, 0x06, 0xA0, 0x1B, 0x23, 0xF6, 0xE0, 0x94, 0xD3, 0x31, 0xAE, 0xB9, 0x62, 0x0E, 0x22, 0xEE,
  0xF2, 0xAE, 0xFC, 0xC5, 0xA6, 0x8E, 0x30, 0xF5, 0xB6, 0xF4, 0x71, 0x6B, 0xC4, 0xBC, 0xEF, 0xF3,
  0x55, 0x5D, 0x92, 0xFA, 0x4E, 0x8B, 0xED, 0xF4, 0xA3, 0xCB, 0x76, 0x48, 0x49, 0x36, 0xEA, 0xBC,
  0x56, 0x5B, 0x5E, 0x1E, 0x7D, 0x81, 0x67, 0xC5, 0x65, 0x7E, 0xB1, 0xD9, 0xC5, 0xB8, 0xD7, 0x93,
  0xA9, 0x51, 0xF6, 0x5A, 0x54, 0x0B, 0xDE, 0xF6, 0x6E, 0xC8, 0xC7, 0xF4
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
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
 * Use a tamper disable token to disable tamper signals.
 ******************************************************************************/
sl_status_t use_tamper_disable_token(void)
{
  // Disable the tamper with tamper disable mask
  print_error_cycle(sl_se_disable_tamper(&cmd_ctx,
                                         (uint8_t *)tamper_disable_token,
                                         sizeof(tamper_disable_token),
                                         TAMPER_DISABLE_MASK),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Roll challenge to invalidate the current tamper disable token.
 ******************************************************************************/
sl_status_t roll_challenge(void)
{
  print_error_cycle(sl_se_roll_challenge(&cmd_ctx), &cmd_ctx);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
