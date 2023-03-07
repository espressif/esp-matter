/***************************************************************************//**
 * @file app_psa_crypto_aead.c
 * @brief PSA Crypto AEAD functions.
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
#include "app_psa_crypto_aead.h"

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
/// Nonce buffer for AEAD
static uint8_t nonce_buf[NONCE_SIZE];

/// Nonce length for AEAD
static size_t nonce_len;

/// Additional data buffer for AEAD
static uint8_t ad_buf[AD_SIZE];

/// Additional data length for AEAD
static size_t ad_len;

/// Plain message buffer
static uint8_t plain_msg_buf[PLAIN_MSG_SIZE];

/// Plain message length
static size_t plain_msg_len;

/// Cipher message buffer
static uint8_t cipher_msg_buf[CIPHER_MSG_SIZE];

/// AEAD output length
static size_t out_len;

/// Buffer for hash value
static uint8_t hash_buf[PSA_HASH_LENGTH(PSA_ALG_SHA_256)];

/// Hash length
static size_t hash_len;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get associated data buffer pointer.
 ******************************************************************************/
uint8_t * get_ad_buf_ptr(void)
{
  return(ad_buf);
}

/***************************************************************************//**
 * Get nonce buffer pointer.
 ******************************************************************************/
uint8_t * get_nonce_buf_ptr(void)
{
  return(nonce_buf);
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
 * Set nonce length.
 ******************************************************************************/
void set_nonce_len(uint8_t length)
{
  nonce_len = length;
}

/***************************************************************************//**
 * Set additional data length.
 ******************************************************************************/
void set_ad_len(size_t length)
{
  ad_len = length;
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
 * Compute the SHA256 hash of a message and compare it with an expected value.
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
 * Process an authenticated encryption.
 ******************************************************************************/
psa_status_t encrypt_aead(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  print_error_cycle(psa_aead_encrypt(get_key_id(),
                                     algo,
                                     nonce_buf,
                                     nonce_len,
                                     ad_buf,
                                     ad_len,
                                     plain_msg_buf,
                                     plain_msg_len,
                                     cipher_msg_buf,
                                     sizeof(cipher_msg_buf),
                                     &out_len));
}

/***************************************************************************//**
 * Process an authenticated decryption.
 ******************************************************************************/
psa_status_t decrypt_aead(void)
{
  psa_algorithm_t algo = get_key_algo();
  if (algo == 0) {
    return(PSA_ERROR_NOT_SUPPORTED);
  }

  print_error_cycle(psa_aead_decrypt(get_key_id(),
                                     algo,
                                     nonce_buf,
                                     nonce_len,
                                     ad_buf,
                                     ad_len,
                                     cipher_msg_buf,
                                     out_len,
                                     plain_msg_buf,
                                     sizeof(plain_msg_buf),
                                     &out_len));
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
