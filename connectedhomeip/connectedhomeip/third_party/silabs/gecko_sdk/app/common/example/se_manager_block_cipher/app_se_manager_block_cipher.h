/***************************************************************************//**
 * @file app_se_manager_block_cipher.h
 * @brief SE manager block cipher functions.
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
#ifndef APP_SE_MANAGER_BLOCK_CIPHER_H
#define APP_SE_MANAGER_BLOCK_CIPHER_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_macro.h"
#include "sl_se_manager.h"
#include "sl_se_manager_cipher.h"
#include "sl_se_manager_entropy.h"
#include "sl_se_manager_key_handling.h"
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Symmetric key buffer size
#define KEY_BUF_SIZE            (32)

/// Overhead of wrapped key buffer
#define WRAPPED_KEY_OVERHEAD    (12 + 16)

/// Internal SE key slot used for symmetric key
#define BLOCK_CIPHER_KEY_SLOT   (SL_SE_KEY_SLOT_VOLATILE_0)

/// AES data block size
#define AES_BLOCK_SIZE          (16)

/// Tag length for CCM/GCM/Poly1305
#define TAG_LEN                 (16)

/// Nonce length for CTR/CCM/GCM/Poly1305
#define NONCE_LEN               (12)

/// Associated data buffer size for CCM/GCM/Poly1305
#define AD_SIZE                 (32)

/// Tag buffer size for CCM/GCM/Poly1305
#define TAG_SIZE                (16)

/// Initialization vector (IV) or nonce buffer size
#define IV_NONCE_SIZE           (16)

/// Stream block buffer size for CTR
#define STREAM_BLK_SIZE         (16)

/// Plain message buffer size
#define PLAIN_MSG_SIZE          (4096)

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
 * Get stream block buffer pointer.
 *
 * @returns Returns pointer to stream block buffer.
 ******************************************************************************/
uint8_t * get_stream_block_buf_ptr(void);

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
 * Set nonce length.
 *
 * @param length The length of the nonce.
 ******************************************************************************/
void set_nonce_len(uint8_t length);

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
 * Set stream block offset.
 *
 * @param offset The offset of the stream block.
 ******************************************************************************/
void set_stream_offset(size_t offset);

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
 * Generate random numbers and save them to a buffer.
 *
 * @param buf The buffer pointer for random number.
 * @param size The size of the buffer.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t generate_random_number(uint8_t *buf, uint32_t size);

/***************************************************************************//**
 * Generate a plain symmetric key for encryption/decryption.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_plain_symmetric_key(sl_se_key_type_t key_type);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Generate a wrapped non-exportable symmetric key for encryption/decryption.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_wrap_symmetric_key(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Delete a non-exportable symmetric key in a volatile SE key slot.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t delete_volatile_symmetric_key(sl_se_key_type_t key_type);

/***************************************************************************//**
 * Generate a volatile non-exportable symmetric key for encryption/decryption.
 *
 * @param key_type The symmetric key type.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t create_volatile_symmetric_key(sl_se_key_type_t key_type);
#endif

/***************************************************************************//**
 * Encrypt message with AES ECB.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t encrypt_aes_ecb(void);

/***************************************************************************//**
 * Decrypt message with AES ECB.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t decrypt_aes_ecb(void);

/***************************************************************************//**
 * Encrypt message with AES CTR.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t encrypt_aes_ctr(void);

/***************************************************************************//**
 * Decrypt message with AES CTR.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t decrypt_aes_ctr(void);

/***************************************************************************//**
 * Encrypt message with AES CCM.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t encrypt_aes_ccm(void);

/***************************************************************************//**
 * Decrypt message with AES CCM.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t decrypt_aes_ccm(void);

/***************************************************************************//**
 * Encrypt message with AES GCM.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t encrypt_aes_gcm(void);

/***************************************************************************//**
 * Decrypt message with AES GCM.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t decrypt_aes_gcm(void);

/***************************************************************************//**
 * Encrypt message with AES CBC.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t encrypt_aes_cbc(void);

/***************************************************************************//**
 * Decrypt message with AES CBC.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t decrypt_aes_cbc(void);

/***************************************************************************//**
 * Encrypt message with AES CFB8.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t encrypt_aes_cfb8(void);

/***************************************************************************//**
 * Decrypt message with AES CFB8.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t decrypt_aes_cfb8(void);

/***************************************************************************//**
 * Encrypt message with AES CFB128.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t encrypt_aes_cfb128(void);

/***************************************************************************//**
 * Decrypt message with AES CFB128.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t decrypt_aes_cfb128(void);

/***************************************************************************//**
 * Generate AES CMAC on the message.
 *
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t generate_aes_cmac(void);

/***************************************************************************//**
 * Compute HMAC on the message.
 *
 * @param hash_algo The Hash algorithm to compute the HMAC.
 * @returns Returns status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t generate_hmac(sl_se_hash_type_t hash_algo);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Generate a Poly1305 MAC using ChaCha20 key derivation.
 *
 * @returns Returns status code, @ref sl_status.h.
 *
 * @note Key must be 256-bit, nonce is 96-bit and counter is 32-bit.
 ******************************************************************************/
sl_status_t generate_poly1305_mac(void);

/***************************************************************************//**
 * Encrypt message with ChaCha20-Poly1305.
 *
 * @returns Returns status code, @ref sl_status.h.
 *
 * @note Key must be 256-bit, nonce is 96-bit and counter is 32-bit.
 ******************************************************************************/
sl_status_t encrypt_chacha20_poly1305(void);

/***************************************************************************//**
 * Decrypt message with ChaCha20-Poly1305.
 *
 * @returns Returns status code, @ref sl_status.h.
 *
 * @note Key must be 256-bit, nonce is 96-bit and counter is 32-bit.
 ******************************************************************************/
sl_status_t decrypt_chacha20_poly1305(void);
#endif

#endif  // APP_SE_MANAGER_BLOCK_CIPHER_H
