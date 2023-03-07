/***************************************************************************//**
 * @file app_psa_crypto_cipher.c
 * @brief PSA Crypto cipher functions.
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
#include "app_psa_crypto_cipher.h"

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
/// The state object for multi-part cipher operations
static psa_cipher_operation_t cipher_op;

/// Initialization vector (IV) buffer
static uint8_t iv_buf[PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES)];

/// Initialization vector (IV) length
static size_t iv_len;

/// Plain message buffer
static uint8_t plain_msg_buf[PLAIN_MSG_SIZE];

/// Plain message length
static size_t plain_msg_len;

/// Cipher message buffer
static uint8_t cipher_msg_buf[CIPHER_MSG_SIZE];

/// Encryption or decryption output length
static size_t out_len;

/// Encryption or decryption total output length
static uint32_t out_total;

/// Buffer for hash value
static uint8_t hash_buf[PSA_HASH_LENGTH(PSA_ALG_SHA_256)];

/// Hash length
static size_t hash_len;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get IV buffer pointer.
 ******************************************************************************/
uint8_t * get_iv_buf_ptr(void)
{
  return(iv_buf);
}

/***************************************************************************//**
 * Get plain message buffer pointer.
 ******************************************************************************/
uint8_t * get_plain_msg_buf_ptr(void)
{
  return(plain_msg_buf);
}

/***************************************************************************//**
 * Get cipher message buffer pointer.
 ******************************************************************************/
uint8_t * get_cipher_msg_buf_ptr(void)
{
  return(cipher_msg_buf);
}

/***************************************************************************//**
 * Set IV length.
 ******************************************************************************/
void set_iv_len(uint8_t length)
{
  iv_len = length;
}

/***************************************************************************//**
 * Set plain message length.
 ******************************************************************************/
void set_plain_msg_len(size_t length)
{
  plain_msg_len = length;
}

/***************************************************************************//**
 * Generate random numbers and save them to a buffer.
 ******************************************************************************/
psa_status_t generate_random_number(uint8_t *buf, uint32_t size)
{
  print_error_cycle(psa_generate_random(buf, size));
}

/***************************************************************************//**
 * Compute the hash of the message.
 ******************************************************************************/
psa_status_t compute_msg_hash(void)
{
  print_error_cycle(psa_hash_compute(PSA_ALG_SHA_256,
                                     plain_msg_buf,
                                     plain_msg_len,
                                     hash_buf,
                                     sizeof(hash_buf),
                                     &hash_len));
}

/***************************************************************************//**
 * Compute the hash of a message and compare it with an expected value.
 ******************************************************************************/
psa_status_t compare_msg_hash(void)
{
  print_error_cycle(psa_hash_compare(PSA_ALG_SHA_256,
                                     plain_msg_buf,
                                     plain_msg_len,
                                     hash_buf,
                                     hash_len));
}

/***************************************************************************//**
 * Process an unauthenticated encryption.
 ******************************************************************************/
psa_status_t encrypt_cipher(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  print_error_cycle(psa_cipher_encrypt(get_key_id(),
                                       algo,
                                       plain_msg_buf,
                                       plain_msg_len,
                                       cipher_msg_buf,
                                       sizeof(cipher_msg_buf),
                                       &out_len));
}

/***************************************************************************//**
 * Process an unauthenticated decryption.
 ******************************************************************************/
psa_status_t decrypt_cipher(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  print_error_cycle(psa_cipher_decrypt(get_key_id(),
                                       algo,
                                       cipher_msg_buf,
                                       out_len,
                                       plain_msg_buf,
                                       sizeof(plain_msg_buf),
                                       &out_len));
}

/***************************************************************************//**
 * Start an encryption.
 ******************************************************************************/
psa_status_t start_cipher_encryption(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  out_len = 0;
  out_total = 0;
  cipher_op = psa_cipher_operation_init();
  print_error_cycle(psa_cipher_encrypt_setup(&cipher_op, get_key_id(), algo));
}

/***************************************************************************//**
 * Start a decryption.
 ******************************************************************************/
psa_status_t start_cipher_decryption(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  out_total = 0;
  cipher_op = psa_cipher_operation_init();
  print_error_cycle(psa_cipher_decrypt_setup(&cipher_op, get_key_id(), algo));
}

/***************************************************************************//**
 * Generate an initialization vector (IV) for encryption.
 ******************************************************************************/
psa_status_t generate_cipher_iv(void)
{
  print_error_cycle(psa_cipher_generate_iv(&cipher_op,
                                           iv_buf,
                                           sizeof(iv_buf),
                                           &iv_len));
}

/***************************************************************************//**
 * Set the initialization vector (IV) for encryption or decryption.
 ******************************************************************************/
psa_status_t set_cipher_iv(void)
{
  print_error_cycle(psa_cipher_set_iv(&cipher_op,
                                      iv_buf,
                                      iv_len));
}

/***************************************************************************//**
 * Update an encryption.
 ******************************************************************************/
psa_status_t update_cipher_encryption(uint32_t offset)
{
  out_total += out_len;
  print_error_cycle(psa_cipher_update(&cipher_op,
                                      plain_msg_buf + offset,
                                      plain_msg_len,
                                      cipher_msg_buf + out_total,
                                      sizeof(cipher_msg_buf) - out_total,
                                      &out_len));
}

/***************************************************************************//**
 * Update a decryption.
 ******************************************************************************/
psa_status_t update_cipher_decryption(uint32_t offset)
{
  out_total += out_len;
  print_error_cycle(psa_cipher_update(&cipher_op,
                                      cipher_msg_buf + offset,
                                      plain_msg_len,
                                      plain_msg_buf + out_total,
                                      sizeof(plain_msg_buf) - out_total,
                                      &out_len));
}

/***************************************************************************//**
 * Finish an encryption.
 ******************************************************************************/
psa_status_t finish_cipher_encryption(void)
{
  out_total += out_len;
  print_error_cycle(psa_cipher_finish(&cipher_op,
                                      cipher_msg_buf + out_total,
                                      sizeof(cipher_msg_buf) - out_total,
                                      &out_len));
}

/***************************************************************************//**
 * Finish a decryption.
 ******************************************************************************/
psa_status_t finish_cipher_decryption(void)
{
  out_total += out_len;
  print_error_cycle(psa_cipher_finish(&cipher_op,
                                      plain_msg_buf + out_total,
                                      sizeof(plain_msg_buf) - out_total,
                                      &out_len));
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
