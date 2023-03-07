/***************************************************************************//**
 * @file app_psa_crypto_ecdh.c
 * @brief PSA Crypto ECDH functions.
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
#include "app_psa_crypto_ecdh.h"

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
/// Pointer to peer public key buffer
static uint8_t *peer_public_key_buf_ptr;

/// Peer public key length
static size_t peer_public_key_len;

/// Shared secret buffer
static uint8_t shared_secret_buf[SHARED_SECRET_SIZE];

/// Shared secret length
static size_t shared_secret_len;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get shared secret buffer pointer.
 ******************************************************************************/
uint8_t * get_shared_secret_buf_ptr(void)
{
  return(shared_secret_buf);
}

/***************************************************************************//**
 * Get shared secret length.
 ******************************************************************************/
size_t get_shared_secret_length(void)
{
  return(shared_secret_len);
}

/***************************************************************************//**
 * Set pointer to peer public key buffer.
 ******************************************************************************/
void set_peer_public_key_buf_ptr(uint8_t *ptr)
{
  peer_public_key_buf_ptr = ptr;
}

/***************************************************************************//**
 * Set peer public key length.
 ******************************************************************************/
void set_peer_public_key_len(size_t length)
{
  peer_public_key_len = length;
}

/***************************************************************************//**
 * Compute ECDH shared secret.
 ******************************************************************************/
psa_status_t compute_ecdh_shared_secret(void)
{
  print_error_cycle(psa_raw_key_agreement(PSA_ALG_ECDH,
                                          get_key_id(),
                                          peer_public_key_buf_ptr,
                                          peer_public_key_len,
                                          shared_secret_buf,
                                          sizeof(shared_secret_buf),
                                          &shared_secret_len));
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
