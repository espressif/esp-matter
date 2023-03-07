/***************************************************************************//**
 * @file app_se_manager_stream_cipher.h
 * @brief SE manager stream cipher functions.
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
#ifndef APP_SE_MANAGER_STREAM_CIPHER_H
#define APP_SE_MANAGER_STREAM_CIPHER_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_cipher.h"
#include "sl_se_manager_key_handling.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Symmetric key buffer size
#define KEY_BUF_SIZE            (32)

/// AES data block size
#define AES_BLOCK_SIZE          (16)

/// Associated data buffer size for GCM
#define AD_SIZE                 (16)

/// Tag buffer size for GCM
#define TAG_SIZE                (16)

/// Initialization vector (IV) and nonce buffer size
#define IV_NONCE_SIZE           (12)

/// Plain message buffer size
#define PLAIN_MSG_SIZE          (256)

/// Cipher message buffer size
#define CIPHER_MSG_SIZE         (PLAIN_MSG_SIZE + TAG_SIZE)

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
 * Get IV or nonce buffer pointer.
 *
 * @returns Returns pointer to IV or nonce buffer.
 ******************************************************************************/
uint8_t * get_iv_nonce_buf_ptr(void);

/***************************************************************************//**
 * Get symmetric key buffer pointer.
 *
 * @returns Returns pointer to symmetric key buffer.
 ******************************************************************************/
uint8_t * get_symmetric_key_buf_ptr(void);

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
 * Get tag buffer pointer.
 *
 * @returns Returns pointer to tag buffer.
 ******************************************************************************/
uint8_t * get_tag_buf_ptr(void);

/***************************************************************************//**
 * Get cipher decrypted/encrypted output length.
 *
 * @returns Returns decrypted/encrypted output length.
 ******************************************************************************/
size_t get_output_len(void);

/***************************************************************************//**
 * Get symmetric key length.
 *
 * @returns Returns symmetric key length.
 ******************************************************************************/
uint16_t get_symmetric_key_len(void);

/***************************************************************************//**
 * Set message tag length.
 *
 * @param length The length of the tag.
 ******************************************************************************/
void set_tag_len(uint8_t length);

/***************************************************************************//**
 * Set IV length.
 *
 * @param length The length of the IV.
 ******************************************************************************/
void set_iv_len(uint8_t length);

/***************************************************************************//**
 * Set associated data length.
 *
 * @param length The length of the associated data.
 ******************************************************************************/
void set_ad_len(size_t length);

/***************************************************************************//**
 * Set plain message length.
 *
 * @param length The length of the plain message.
 ******************************************************************************/
void set_plain_msg_len(size_t length);

/***************************************************************************//**
 * Initialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t init_se_manager(void);

/***************************************************************************//**
 * Denitialize the SE Manager.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t deinit_se_manager(void);

/***************************************************************************//**
 * Generate a plain symmetric key for stream cipher.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_plain_symmetric_key(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Start an AES CMAC stream.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t start_cmac_stream(void);

/***************************************************************************//**
 * Update an AES CMAC stream.
 *
 * @param offset The update pointer offset.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t update_cmac_stream(uint32_t offset);

/***************************************************************************//**
 * Finish an AES CMAC stream.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t finish_cmac_stream(void);

/***************************************************************************//**
 * Start an AES GCM encryption stream.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t start_gcm_encrypt_stream(void);

/***************************************************************************//**
 * Update an AES GCM encryption stream.
 *
 * @param offset The update pointer offset.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t update_gcm_encrypt_stream(uint32_t offset_in, uint32_t offset_out);

/***************************************************************************//**
 * Finish an AES GCM encryption stream.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t finish_gcm_encrypt_stream(uint32_t offset);

/***************************************************************************//**
 * Start an AES GCM decryption stream.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t start_gcm_decrypt_stream(void);

/***************************************************************************//**
 * Update an AES GCM decryption stream.
 *
 * @param offset The update pointer offset.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t update_gcm_decrypt_stream(uint32_t offset_in, uint32_t offset_out);

/***************************************************************************//**
 * Finish an AES GCM decryption stream.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t finish_gcm_decrypt_stream(uint32_t offset);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Update a ChaCha20 encryption stream.
 *
 * @param counter The initial counter.
 * @param offset The update pointer offset.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t update_chacha20_encrypt_stream(uint32_t counter, size_t offset);

/***************************************************************************//**
 * Update a ChaCha20 decryption stream.
 *
 * @param counter The initial counter.
 * @param offset The update pointer offset.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t update_chacha20_decrypt_stream(uint32_t counter, size_t offset);
#endif

#endif  // APP_SE_MANAGER_STREAM_CIPHER_H
