/***************************************************************************//**
 * @file app_psa_crypto_dsa.c
 * @brief PSA Crypto DSA functions.
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
#include "app_psa_crypto_dsa.h"

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
/// Pointer to message or hash buffer to sign or verify
static uint8_t *msg_hash_buf_ptr;

/// Message or hash length to sign or verify
static size_t msg_hash_len;

/// Signature buffer
static uint8_t signature_buf[SIGNATURE_SIZE];

/// Signature length in bytes
static size_t signature_len;

/// Algorithm for DSA
psa_algorithm_t dsa_algo;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Set pointer to message or hash buffer.
 ******************************************************************************/
void set_msg_hash_buf_ptr(uint8_t *ptr)
{
  msg_hash_buf_ptr = ptr;
}

/***************************************************************************//**
 * Set message or hash length.
 ******************************************************************************/
void set_msg_hash_len(size_t length)
{
  msg_hash_len = length;
}

/***************************************************************************//**
 * Set DSA algorithm.
 ******************************************************************************/
void set_dsa_algo(psa_algorithm_t alg)
{
  dsa_algo = alg;
}

/***************************************************************************//**
 * Generate random numbers and save them to a buffer.
 ******************************************************************************/
psa_status_t generate_random_number(uint8_t *buf, uint32_t size)
{
  print_error_cycle(psa_generate_random(buf, size));
}

/***************************************************************************//**
 * Sign an already-calculated hash with a private key.
 ******************************************************************************/
psa_status_t sign_hash(void)
{
  print_error_cycle(psa_sign_hash(get_key_id(),
                                  dsa_algo,
                                  msg_hash_buf_ptr,
                                  msg_hash_len,
                                  signature_buf,
                                  sizeof(signature_buf),
                                  &signature_len));
}

/***************************************************************************//**
 * Verify the signature of a hash using a public key.
 ******************************************************************************/
psa_status_t verify_hash(void)
{
  print_error_cycle(psa_verify_hash(get_key_id(),
                                    dsa_algo,
                                    msg_hash_buf_ptr,
                                    msg_hash_len,
                                    signature_buf,
                                    signature_len));
}

/***************************************************************************//**
 * Hash and sign a message with a private key.
 ******************************************************************************/
psa_status_t sign_message(void)
{
  print_error_cycle(psa_sign_message(get_key_id(),
                                     dsa_algo,
                                     msg_hash_buf_ptr,
                                     msg_hash_len,
                                     signature_buf,
                                     sizeof(signature_buf),
                                     &signature_len));
}

/***************************************************************************//**
 * Hash a message and verify the signature with a public key.
 ******************************************************************************/
psa_status_t verify_message(void)
{
  print_error_cycle(psa_verify_message(get_key_id(),
                                       dsa_algo,
                                       msg_hash_buf_ptr,
                                       msg_hash_len,
                                       signature_buf,
                                       signature_len));
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
