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

/// Private certificate key buffer
static uint8_t private_certificate_key[ECC_PRIVKEY_SIZE + WRAPPED_KEY_OVERHEAD];

/// Access certificate for tamper disable token
static access_cert_t access_certificate;

/// Challenge response to generate signature for tamper disable token
static challenge_resp_t challenge_response;

/// Tamper disable token to disable tamper
static tamper_disable_token_t tamper_disable_token;

/// Hard-coded private command key
static const uint8_t private_command_key[ECC_PRIVKEY_SIZE] = {
  0xac, 0x3f, 0x26, 0xd2, 0x0f, 0x24, 0x24, 0xe4,
  0xbb, 0x25, 0x6f, 0x30, 0x25, 0xe8, 0xd9, 0x75,
  0x9a, 0xaa, 0x06, 0x45, 0x1d, 0xe2, 0xdf, 0xdb,
  0x6e, 0x02, 0x7d, 0x72, 0x98, 0xcc, 0x1d, 0xb1
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get challenge buffer pointer.
 ******************************************************************************/
uint8_t * get_challenge_buf_ptr(void)
{
  return(challenge_response.nonce);
}

/***************************************************************************//**
 * Compare exported public command key with OTP public command key.
 ******************************************************************************/
int32_t compare_public_command_key(void)
{
  return memcmp(access_certificate.pubkey, access_certificate.signature,
                sizeof(access_certificate.pubkey));
}

/***************************************************************************//**
 * Create a private certificate key.
 ******************************************************************************/
sl_status_t create_private_certificate_key(void)
{
  uint32_t reqdSize;

  // Set up a key descriptor pointing to the private certificate key buffer
  sl_se_key_descriptor_t new_key = {
    .type = TAMPER_DISABLE_ECC_ID,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = private_certificate_key,
    .storage.location.buffer.size = sizeof(private_certificate_key)
  };

  if (sl_se_validate_key(&new_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&new_key, &reqdSize) != SL_STATUS_OK
      || sizeof(private_certificate_key) < reqdSize) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_generate_key(&cmd_ctx, &new_key), &cmd_ctx);
}

/***************************************************************************//**
 * Export a public certificate key from a private certificate key.
 ******************************************************************************/
sl_status_t export_public_certificate_key(void)
{
  uint32_t reqdSize;

  // Set up a key descriptor pointing to an existing private certificate key
  sl_se_key_descriptor_t priv_key = {
    .type = TAMPER_DISABLE_ECC_ID,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = private_certificate_key,
    .storage.location.buffer.size = sizeof(private_certificate_key)
  };

  // Set up a key descriptor pointing to the public certificate key buffer
  sl_se_key_descriptor_t pub_key = {
    .type = TAMPER_DISABLE_ECC_ID,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = access_certificate.pubkey,
    .storage.location.buffer.size = sizeof(access_certificate.pubkey)
  };

  if (sl_se_validate_key(&pub_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&pub_key, &reqdSize) != SL_STATUS_OK
      || sizeof(access_certificate.pubkey) < reqdSize) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_export_public_key(&cmd_ctx, &priv_key, &pub_key),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Read public command key from SE OTP.
 ******************************************************************************/
sl_status_t read_public_command_key(void)
{
  print_error_cycle(sl_se_read_pubkey(&cmd_ctx,
                                      SL_SE_KEY_TYPE_IMMUTABLE_AUTH,
                                      access_certificate.signature,
                                      sizeof(access_certificate.signature)),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Export a public command key from a private command key.
 ******************************************************************************/
sl_status_t export_public_command_key(void)
{
  uint32_t reqdSize;

  // Set up a key descriptor pointing to an existing private command key
  sl_se_key_descriptor_t priv_key = {
    .type = TAMPER_DISABLE_ECC_ID,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = (uint8_t *)private_command_key,
    .storage.location.buffer.size = sizeof(private_command_key)
  };

  // Set up a key descriptor pointing to an external key buffer
  sl_se_key_descriptor_t pub_key = {
    .type = TAMPER_DISABLE_ECC_ID,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = access_certificate.pubkey,
    .storage.location.buffer.size = sizeof(access_certificate.pubkey)
  };

  if (sl_se_validate_key(&pub_key) != SL_STATUS_OK
      || sl_se_get_storage_size(&pub_key, &reqdSize) != SL_STATUS_OK
      || sizeof(access_certificate.pubkey) < reqdSize) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_export_public_key(&cmd_ctx,
                                            &priv_key,
                                            &pub_key), &cmd_ctx);
}

/***************************************************************************//**
 * Program public command key to the SE OTP.
 ******************************************************************************/
sl_status_t program_public_command_key(void)
{
  // Warning: This is a ONE-TIME command
  // The public key buffer must be word aligned
  print_error_cycle(sl_se_init_otp_key(&cmd_ctx,
                                       SL_SE_KEY_TYPE_IMMUTABLE_AUTH,
                                       access_certificate.pubkey,
                                       sizeof(access_certificate.pubkey)),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Read the serial number of the SE and save it to access certificate.
 ******************************************************************************/
sl_status_t read_serial_number(void)
{
  print_error_cycle(sl_se_get_serialnumber(&cmd_ctx,
                                           access_certificate.serialno),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Sign access certificate with private command key.
 ******************************************************************************/
sl_status_t sign_access_certificate(void)
{
  // Initialize the access certificate
  access_certificate.magic_number = CERT_MAGIC_NUM;
  access_certificate.auth_debug = DEBUG_AUTHORIZATION;
  access_certificate.auth_misc = 0;
  access_certificate.key_index = 0;
  access_certificate.auth_tamper = TAMPER_AUTHORIZATION;

  // Set up a key descriptor for private command key
  sl_se_key_descriptor_t priv_key = {
    .type = TAMPER_DISABLE_ECC_ID,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = (uint8_t *)private_command_key,
    .storage.location.buffer.size = sizeof(private_command_key)
  };

  print_error_cycle(sl_se_ecc_sign(&cmd_ctx,
                                   &priv_key,
                                   SL_SE_HASH_SHA256,
                                   false,
                                   (uint8_t *)&access_certificate,
                                   sizeof(access_certificate) - ECC_SIGNATURE_SIZE,
                                   access_certificate.signature,
                                   sizeof(access_certificate.signature)),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Request challenge from the SE and save it to challenge response.
 ******************************************************************************/
sl_status_t request_challenge(void)
{
  print_error_cycle(sl_se_get_challenge(&cmd_ctx, challenge_response.nonce),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Sign challenge response with private certificate key.
 ******************************************************************************/
sl_status_t sign_challenge_response(void)
{
  // Initialize the challenge response
  challenge_response.command_value = TAMPER_DISABLE_CMD;
  challenge_response.parameter = TAMPER_DISABLE_MASK;

  // Set up a key descriptor for private certificate key
  sl_se_key_descriptor_t priv_key = {
    .type = TAMPER_DISABLE_ECC_ID,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
             | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY
             | SL_SE_KEY_FLAG_NON_EXPORTABLE,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
    .storage.location.buffer.pointer = private_certificate_key,
    .storage.location.buffer.size = sizeof(private_certificate_key)
  };

  print_error_cycle(sl_se_ecc_sign(&cmd_ctx,
                                   &priv_key,
                                   SL_SE_HASH_SHA256,
                                   false,
                                   (uint8_t *)&challenge_response,
                                   sizeof(challenge_response),
                                   tamper_disable_token.challenge_sig,
                                   sizeof(tamper_disable_token.challenge_sig)),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Create a tamper disable token to disable tamper signals.
 ******************************************************************************/
sl_status_t create_tamper_disable_token(void)
{
  // Initialize the tamper disable token
  tamper_disable_token.certificate = access_certificate;

  // Disable the tamper with tamper disable mask
  print_error_cycle(sl_se_disable_tamper(&cmd_ctx,
                                         &tamper_disable_token,
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
