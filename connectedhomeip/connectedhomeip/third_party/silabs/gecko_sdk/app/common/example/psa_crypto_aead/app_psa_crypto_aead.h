/***************************************************************************//**
 * @file app_psa_crypto_aead.h
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
#ifndef APP_PSA_CRYPTO_AEAD_H
#define APP_PSA_CRYPTO_AEAD_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_psa_crypto_key.h"
#include "app_psa_crypto_macro.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Additional data buffer size
#define AD_SIZE                 (32)

/// Tag buffer size
#define TAG_SIZE                (16)

/// Nonce buffer size
#define NONCE_SIZE              (16)

/// Plain message buffer size
#define PLAIN_MSG_SIZE          (4096)

/// Cipher message buffer size
#define CIPHER_MSG_SIZE         (PLAIN_MSG_SIZE + TAG_SIZE)

/// Additional data length for AEAD
#define AD_LEN                  AD_SIZE

/// Tag length for AEAD
#define TAG_LEN                 (16)

/// Nonce length for AEAD
#define NONCE_LEN               (12)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get associated data buffer pointer.
 *
 * @returns Returns pointer to associated data buffer.
 ******************************************************************************/
uint8_t * get_ad_buf_ptr(void);

/***************************************************************************//**
 * Get nonce buffer pointer.
 *
 * @returns Returns pointer to nonce buffer.
 ******************************************************************************/
uint8_t * get_nonce_buf_ptr(void);

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
 * Set nonce length.
 *
 * @param length The length of the nonce.
 ******************************************************************************/
void set_nonce_len(uint8_t length);

/***************************************************************************//**
 * Set additional data length.
 *
 * @param length The length of the additional data.
 ******************************************************************************/
void set_ad_len(size_t length);

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
 * Compute the SHA256 hash of a message and compare it with an expected value.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t compare_msg_hash(void);

/***************************************************************************//**
 * Process an authenticated encryption.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t encrypt_aead(void);

/***************************************************************************//**
 * Process an authenticated decryption.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
psa_status_t decrypt_aead(void);

#endif  // APP_PSA_CRYPTO_AEAD_H
