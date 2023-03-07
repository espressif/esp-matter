/***************************************************************************//**
 * @file app_se_manager_secure_identity.c
 * @brief SE manager secure identity functions.
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
#include "app_se_manager_secure_identity.h"

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

/// Certificate size buffer
static sl_se_cert_size_type_t cert_size_buf;

/// Challenge buffer
static uint8_t challenge_buf[SL_SE_CHALLENGE_SIZE];

/// Certificate buffer
static uint8_t cert_buf[CERT_SIZE];

/// Public device key buffer
static uint8_t pub_device_key_buf[SL_SE_CERT_KEY_SIZE];

/// Signature buffer
static uint8_t signature_buf[SL_SE_CERT_SIGN_SIZE];

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get certificate size.
 ******************************************************************************/
uint32_t get_cert_size(uint8_t cert_type)
{
  if (cert_type == SL_SE_CERT_BATCH) {
    return cert_size_buf.batch_id_size;
  } else if (cert_type == SL_SE_CERT_DEVICE_SE) {
    return cert_size_buf.se_id_size;
  } else if (cert_type == SL_SE_CERT_DEVICE_HOST) {
    return cert_size_buf.host_id_size;
  } else {
    return 0;
  }
}

/***************************************************************************//**
 * Get challenge buffer pointer.
 ******************************************************************************/
uint8_t * get_challenge_buf_ptr(void)
{
  return(challenge_buf);
}

/***************************************************************************//**
 * Get certificate buffer pointer.
 ******************************************************************************/
uint8_t * get_cert_buf_ptr(void)
{
  return(cert_buf);
}

/***************************************************************************//**
 * Get public key buffer pointer.
 ******************************************************************************/
uint8_t * get_pub_device_key_buf_ptr(void)
{
  return(pub_device_key_buf);
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
 * Generate random numbers and save them to a buffer.
 ******************************************************************************/
sl_status_t generate_random_number(uint8_t *buf, uint32_t size)
{
  print_error_cycle(sl_se_get_random(&cmd_ctx, buf, size), &cmd_ctx);
}

/***************************************************************************//**
 * Read size of stored certificates in the SE.
 ******************************************************************************/
sl_status_t read_cert_size(void)
{
  print_error_cycle(sl_se_read_cert_size(&cmd_ctx, &cert_size_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Read stored certificates in SE.
 ******************************************************************************/
sl_status_t read_cert_data(uint8_t cert_type)
{
  print_error_cycle(sl_se_read_cert(&cmd_ctx,
                                    cert_type,
                                    cert_buf,
                                    get_cert_size(cert_type)), &cmd_ctx);
}

/***************************************************************************//**
 * Sign challenge with private device key.
 ******************************************************************************/
sl_status_t sign_challenge(void)
{
  // Set up a key descriptor for private device key
  sl_se_key_descriptor_t private_device_key = SL_SE_APPLICATION_ATTESTATION_KEY;

  // Sign challenge
  print_error_cycle(sl_se_ecc_sign(&cmd_ctx,
                                   &private_device_key,
                                   SL_SE_HASH_SHA256,
                                   false,
                                   challenge_buf,
                                   sizeof(challenge_buf),
                                   signature_buf,
                                   sizeof(signature_buf)), &cmd_ctx);
}

/***************************************************************************//**
 * Get on-chip public device key.
 ******************************************************************************/
sl_status_t get_public_device_key(void)
{
  print_error_cycle(sl_se_read_pubkey(&cmd_ctx,
                                      SL_SE_KEY_TYPE_IMMUTABLE_ATTESTATION,
                                      cert_buf,
                                      SL_SE_CERT_KEY_SIZE), &cmd_ctx);
}

/***************************************************************************//**
 * Verify signature with on-chip public device key.
 ******************************************************************************/
sl_status_t verify_signature_local(void)
{
  // Set up a key descriptor for on-chip public device key
  sl_se_key_descriptor_t pub_device_key = {
    .type = SL_SE_KEY_TYPE_ECC_P256,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = cert_buf,
    .storage.location.buffer.size = SL_SE_CERT_KEY_SIZE
  };

  // Verify signature
  print_error_cycle(sl_se_ecc_verify(&cmd_ctx,
                                     &pub_device_key,
                                     SL_SE_HASH_SHA256,
                                     false,
                                     challenge_buf,
                                     sizeof(challenge_buf),
                                     signature_buf,
                                     sizeof(signature_buf)), &cmd_ctx);
}

/***************************************************************************//**
 * Verify signature with public device key in device certificate.
 ******************************************************************************/
sl_status_t verify_signature_remote(void)
{
  // Set up a key descriptor for public device key in device certificate
  sl_se_key_descriptor_t pub_device_key = {
    .type = SL_SE_KEY_TYPE_ECC_P256,
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
             | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY,
    .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
    .storage.location.buffer.pointer = pub_device_key_buf,
    .storage.location.buffer.size = sizeof(pub_device_key_buf)
  };

  // Verify signature
  print_error_cycle(sl_se_ecc_verify(&cmd_ctx,
                                     &pub_device_key,
                                     SL_SE_HASH_SHA256,
                                     false,
                                     challenge_buf,
                                     sizeof(challenge_buf),
                                     signature_buf,
                                     sizeof(signature_buf)), &cmd_ctx);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
