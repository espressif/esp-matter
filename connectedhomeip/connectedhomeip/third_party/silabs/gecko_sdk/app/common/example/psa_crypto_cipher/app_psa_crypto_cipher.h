/***************************************************************************//**
 * @file app_psa_crypto_cipher.h
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
#ifndef APP_PSA_CRYPTO_CIPHER_H
#define APP_PSA_CRYPTO_CIPHER_H

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
/// Plain message buffer size
#define PLAIN_MSG_SIZE          (4096)

/// Cipher message buffer size (IV + Ciphertext)
#define CIPHER_MSG_SIZE         (PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES) + PLAIN_MSG_SIZE)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get IV buffer pointer.
 *
 * @returns Returns pointer to IV buffer.
 ******************************************************************************/
uint8_t * get_iv_buf_ptr(void);

/***************************************************************************//**
 * Get plain message buffer pointer.
 *
 * @returns Returns pointer to plain message buffer.
 ******************************************************************************/
uint8_t * get_plain_msg_buf_ptr(void);

/***************************************************************************//**
 * Get cipher message buffer pointer.
 *
 * @returns Returns pointer to cipher message buffer.
 ******************************************************************************/
uint8_t * get_cipher_msg_buf_ptr(void);

/***************************************************************************//**
 * Set IV length.
 *
 * @param length The length of the IV.
 ******************************************************************************/
void set_iv_len(uint8_t length);

/***************************************************************************//**
 * Set plain message length.
 *
 * @param length The length of the plain message.
 ******************************************************************************/
void set_plain_msg_len(size_t length);

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
 * Process an unauthenticated encryption.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t encrypt_cipher(void);

/***************************************************************************//**
 * Process an unauthenticated decryption.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t decrypt_cipher(void);

/***************************************************************************//**
 * Start an encryption.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t start_cipher_encryption(void);

/***************************************************************************//**
 * Start a decryption.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t start_cipher_decryption(void);

/***************************************************************************//**
 * Generate an initialization vector (IV) for encryption.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t generate_cipher_iv(void);

/***************************************************************************//**
 * Set the initialization vector (IV) for encryption or decryption.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t set_cipher_iv(void);

/***************************************************************************//**
 * Update an encryption.
 *
 * @param offset The update pointer offset.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t update_cipher_encryption(uint32_t offset);

/***************************************************************************//**
 * Update a decryption.
 *
 * @param offset The update pointer offset.
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t update_cipher_decryption(uint32_t offset);

/***************************************************************************//**
 * Finish an encryption.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t finish_cipher_encryption(void);

/***************************************************************************//**
 * Finish a decryption.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t finish_cipher_decryption(void);

#endif  // APP_PSA_CRYPTO_CIPHER_H
