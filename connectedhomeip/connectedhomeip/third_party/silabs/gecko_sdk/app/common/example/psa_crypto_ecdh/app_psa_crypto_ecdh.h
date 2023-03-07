/***************************************************************************//**
 * @file app_psa_crypto_ecdh.h
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
#ifndef APP_PSA_CRYPTO_ECDH_H
#define APP_PSA_CRYPTO_ECDH_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_psa_crypto_key.h"
#include "app_psa_crypto_macro.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// ECDH shared secret size
#define SHARED_SECRET_SIZE      (66)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get shared secret buffer pointer.
 *
 * @returns Returns pointer to shared secret buffer.
 ******************************************************************************/
uint8_t * get_shared_secret_buf_ptr(void);

/***************************************************************************//**
 * Get shared secret length.
 *
 * @returns Returns shared secret length.
 ******************************************************************************/
size_t get_shared_secret_length(void);

/***************************************************************************//**
 * Set pointer to peer public key buffer.
 *
 * @param ptr The pointer to peer public key buffer.
 ******************************************************************************/
void set_peer_public_key_buf_ptr(uint8_t *ptr);

/***************************************************************************//**
 * Set peer public key length.
 *
 * @param length The length of peer public key.
 ******************************************************************************/
void set_peer_public_key_len(size_t length);

/***************************************************************************//**
 * Compute ECDH shared secret.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t compute_ecdh_shared_secret(void);

#endif  // APP_PSA_CRYPTO_ECDH_H
