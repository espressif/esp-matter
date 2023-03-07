/***************************************************************************//**
 * @file app_psa_crypto_hash.h
 * @brief PSA Crypto hash functions.
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
#ifndef APP_PSA_CRYPTO_HASH_H
#define APP_PSA_CRYPTO_HASH_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_psa_crypto_key.h"
#include "app_psa_crypto_macro.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Hash value buffer size (SHA1 to SHA512)
#define HASH_BUF_SIZE   (64)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get hash value buffer pointer.
 *
 * @returns Returns pointer to the hash value buffer.
 ******************************************************************************/
uint8_t * get_hash_buf_ptr(void);

/***************************************************************************//**
 * Get size of hash value.
 *
 * @returns Returns size of hash value.
 ******************************************************************************/
uint8_t get_hash_size(void);

/***************************************************************************//**
 * Set pointer to message buffer.
 *
 * @param ptr The pointer to message buffer.
 ******************************************************************************/
void set_msg_buf_ptr(uint8_t *ptr);

/***************************************************************************//**
 * Set message length.
 *
 * @param length The length of the message.
 ******************************************************************************/
void set_msg_len(size_t length);

/***************************************************************************//**
 * Set hash algorithm.
 *
 * @param type The hash algorithm to be used.
 ******************************************************************************/
void set_hash_algo(psa_algorithm_t type);

/***************************************************************************//**
 * Generate random numbers and save them to a buffer.
 *
 * @param buf The buffer pointer for random number.
 * @param size The size of the buffer.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t generate_random_number(uint8_t *buf, uint32_t size);

/***************************************************************************//**
 * Compute the hash of the message.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t compute_msg_hash(void);

/***************************************************************************//**
 * Compute the hash of a message and compare it with an expected value.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t compare_msg_hash(void);

/***************************************************************************//**
 * Start a hash stream.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t start_hash_stream(void);

/***************************************************************************//**
 * Update a hash stream.
 *
 * @param offset The update pointer offset.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t update_hash_stream(uint32_t offset);

/***************************************************************************//**
 * Finish a hash stream.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t finish_hash_stream(void);

/***************************************************************************//**
 * Clone a hash operation.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t clone_hash_operation(void);

/***************************************************************************//**
 * Compare the hash with an expected value.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t verify_hash_stream(void);

#endif  // APP_PSA_CRYPTO_HASH_H
