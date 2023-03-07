/***************************************************************************//**
 * @file app_psa_crypto_dsa.h
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
#ifndef APP_PSA_CRYPTO_DSA_H
#define APP_PSA_CRYPTO_DSA_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_psa_crypto_key.h"
#include "app_psa_crypto_macro.h"
#if defined(SEMAILBOX_PRESENT)
#include "sl_psa_values.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// DSA signature size
#define SIGNATURE_SIZE          (132)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Set pointer to message or hash buffer.
 *
 * @param ptr The pointer to message or hash buffer.
 ******************************************************************************/
void set_msg_hash_buf_ptr(uint8_t *ptr);

/***************************************************************************//**
 * Set message or hash length.
 *
 * @param length The length of the message or hash.
 ******************************************************************************/
void set_msg_hash_len(size_t length);

/***************************************************************************//**
 * Set DSA algorithm.
 *
 * @param alg The DSA algorithm to be used.
 ******************************************************************************/
void set_dsa_algo(psa_algorithm_t alg);

/***************************************************************************//**
 * Generate random numbers and save them to a buffer.
 *
 * @param buf The buffer pointer for random number.
 * @param size The size of the buffer.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t generate_random_number(uint8_t *buf, uint32_t size);

/***************************************************************************//**
 * Sign an already-calculated hash with a private key.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t sign_hash(void);

/***************************************************************************//**
 * Verify the signature of a hash using a public key.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t verify_hash(void);

/***************************************************************************//**
 * Hash and sign a message with a private key.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t sign_message(void);

/***************************************************************************//**
 * Hash a message and verify the signature with a public key.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t verify_message(void);

#endif  // APP_PSA_CRYPTO_DSA_H
