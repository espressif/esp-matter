/***************************************************************************//**
 * @file app_se_manager_block_cipher.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_block_cipher.h"

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
/// Command context
static sl_se_command_context_t cmd_ctx;

/// Buffer for symmetric plain or wrapped key
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
static uint8_t symmetric_key_buf[KEY_BUF_SIZE + WRAPPED_KEY_OVERHEAD];
#else
static uint8_t symmetric_key_buf[KEY_BUF_SIZE];
#endif

/// Key descriptor for encryption/decryption
static sl_se_key_descriptor_t symmetric_key_desc;

/// Associated data buffer for CCM/GCM/Poly1305
static uint8_t ad_buf[AD_SIZE];

/// Initialization vector (IV) or nonce buffer
static uint8_t iv_nonce_buf[IV_NONCE_SIZE];

/// Stream block buffer for CTR
static uint8_t stream_block_buf[STREAM_BLK_SIZE];

/// Plain message buffer
static uint8_t plain_msg_buf[PLAIN_MSG_SIZE];

/// Cipher message buffer
static uint8_t cipher_msg_buf[CIPHER_MSG_SIZE];

/// Tag length for CCM/GCM/Poly1305
static uint8_t tag_len;

/// Nonce length for CTR/CCM/GCM/Poly1305
static uint8_t nonce_len;

/// Associated data length for CCM/GCM/Poly1305
static size_t ad_len;

/// Plain message length
static size_t plain_msg_len;

/// Stream offset for CTR
static uint32_t stream_offset;

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
 * Get IV or nonce buffer pointer.
 ******************************************************************************/
uint8_t * get_iv_nonce_buf_ptr(void)
{
  return(iv_nonce_buf);
}

/***************************************************************************//**
 * Get stream block pointer.
 ******************************************************************************/
uint8_t * get_stream_block_buf_ptr(void)
{
  return(stream_block_buf);
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
 * Get symmetric key length.
 ******************************************************************************/
uint16_t get_symmetric_key_len(void)
{
  switch (symmetric_key_desc.type) {
    case SL_SE_KEY_TYPE_SYMMETRIC:
      return symmetric_key_desc.size;

    case SL_SE_KEY_TYPE_AES_128:
      return 128;

    case SL_SE_KEY_TYPE_AES_192:
      return 192;

    // SL_SE_KEY_TYPE_CHACHA20 = SL_SE_KEY_TYPE_AES_256
    case SL_SE_KEY_TYPE_AES_256:
      return 256;

    default:
      return 0;
  }
}

/***************************************************************************//**
 * Set message tag length.
 ******************************************************************************/
void set_tag_len(uint8_t length)
{
  tag_len = length;
}

/***************************************************************************//**
 * Set nonce length.
 ******************************************************************************/
void set_nonce_len(uint8_t length)
{
  nonce_len = length;
}

/***************************************************************************//**
 * Set associated data length.
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
 * Set stream block offset.
 ******************************************************************************/
void set_stream_offset(size_t offset)
{
  stream_offset = offset;
}

/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t init_se_manager(void)
{
  print_error_cycle(sl_se_init(), NULL);
}

/***************************************************************************//**
 * Deinitialize the SE Manager.
 ******************************************************************************/
sl_status_t deinit_se_manager(void)
{
  print_error_cycle(sl_se_deinit(), NULL);
}

/***************************************************************************//**
 * Generate random numbers and save them to a buffer.
 ******************************************************************************/
sl_status_t generate_random_number(uint8_t *buf, uint32_t size)
{
  print_error_cycle(sl_se_get_random(&cmd_ctx, buf, size), &cmd_ctx);
}

/***************************************************************************//**
 * Generate a plain symmetric key for encryption/decryption.
 ******************************************************************************/
sl_status_t create_plain_symmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  symmetric_key_desc.type = key_type;
  symmetric_key_desc.flags = 0;
  symmetric_key_desc.storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;
  symmetric_key_desc.storage.location.buffer.pointer = symmetric_key_buf;
  symmetric_key_desc.storage.location.buffer.size = sizeof(symmetric_key_buf);

  if ((sl_se_validate_key(&symmetric_key_desc) != SL_STATUS_OK)
      || (sl_se_get_storage_size(&symmetric_key_desc,
                                 &req_size) != SL_STATUS_OK)
      || (sizeof(symmetric_key_buf) < req_size)) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_generate_key(&cmd_ctx, &symmetric_key_desc),
                    &cmd_ctx);
}

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

/***************************************************************************//**
 * Generate a wrapped non-exportable symmetric key for encryption/decryption.
 ******************************************************************************/
sl_status_t create_wrap_symmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  symmetric_key_desc.type = key_type;
  symmetric_key_desc.flags = SL_SE_KEY_FLAG_NON_EXPORTABLE;
  symmetric_key_desc.storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED;
  symmetric_key_desc.storage.location.buffer.pointer = symmetric_key_buf;
  symmetric_key_desc.storage.location.buffer.size = sizeof(symmetric_key_buf);

  if ((sl_se_validate_key(&symmetric_key_desc) != SL_STATUS_OK)
      || (sl_se_get_storage_size(&symmetric_key_desc,
                                 &req_size) != SL_STATUS_OK)
      || (sizeof(symmetric_key_buf) < req_size)) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_generate_key(&cmd_ctx, &symmetric_key_desc),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Delete a non-exportable symmetric key in a volatile SE key slot.
 ******************************************************************************/
sl_status_t delete_volatile_symmetric_key(sl_se_key_type_t key_type)
{
  symmetric_key_desc.type = key_type;
  symmetric_key_desc.flags = SL_SE_KEY_FLAG_NON_EXPORTABLE;
  symmetric_key_desc.storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE;
  symmetric_key_desc.storage.location.slot = BLOCK_CIPHER_KEY_SLOT;

  if (sl_se_validate_key(&symmetric_key_desc) != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_delete_key(&cmd_ctx, &symmetric_key_desc), &cmd_ctx);
}

/***************************************************************************//**
 * Generate a volatile non-exportable symmetric key for encryption/decryption.
 ******************************************************************************/
sl_status_t create_volatile_symmetric_key(sl_se_key_type_t key_type)
{
  symmetric_key_desc.type = key_type;
  symmetric_key_desc.flags = SL_SE_KEY_FLAG_NON_EXPORTABLE;
  symmetric_key_desc.storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE;
  symmetric_key_desc.storage.location.slot = BLOCK_CIPHER_KEY_SLOT;

  if (sl_se_validate_key(&symmetric_key_desc) != SL_STATUS_OK) {
    return SL_STATUS_FAIL;
  }

  print_error_cycle(sl_se_generate_key(&cmd_ctx, &symmetric_key_desc),
                    &cmd_ctx);
}

#endif

/***************************************************************************//**
 * Encrypt message with AES ECB.
 ******************************************************************************/
sl_status_t encrypt_aes_ecb(void)
{
  print_error_cycle(sl_se_aes_crypt_ecb(&cmd_ctx,
                                        &symmetric_key_desc,
                                        SL_SE_ENCRYPT,
                                        plain_msg_len,
                                        plain_msg_buf,
                                        cipher_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Decrypt message with AES ECB.
 ******************************************************************************/
sl_status_t decrypt_aes_ecb(void)
{
  print_error_cycle(sl_se_aes_crypt_ecb(&cmd_ctx,
                                        &symmetric_key_desc,
                                        SL_SE_DECRYPT,
                                        plain_msg_len,
                                        cipher_msg_buf,
                                        plain_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Encrypt message with AES CTR.
 ******************************************************************************/
sl_status_t encrypt_aes_ctr(void)
{
  print_error_cycle(sl_se_aes_crypt_ctr(&cmd_ctx,
                                        &symmetric_key_desc,
                                        plain_msg_len,
                                        &stream_offset,
                                        iv_nonce_buf,
                                        stream_block_buf,
                                        plain_msg_buf,
                                        cipher_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Decrypt message with AES CTR.
 ******************************************************************************/
sl_status_t decrypt_aes_ctr(void)
{
  print_error_cycle(sl_se_aes_crypt_ctr(&cmd_ctx,
                                        &symmetric_key_desc,
                                        plain_msg_len,
                                        &stream_offset,
                                        iv_nonce_buf,
                                        stream_block_buf,
                                        cipher_msg_buf,
                                        plain_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Encrypt message with AES CCM.
 ******************************************************************************/
sl_status_t encrypt_aes_ccm(void)
{
  print_error_cycle(sl_se_ccm_encrypt_and_tag(&cmd_ctx,
                                              &symmetric_key_desc,
                                              plain_msg_len,
                                              iv_nonce_buf,
                                              nonce_len,
                                              ad_buf,
                                              ad_len,
                                              plain_msg_buf,
                                              cipher_msg_buf,
                                              cipher_msg_buf + plain_msg_len,
                                              tag_len), &cmd_ctx);
}

/***************************************************************************//**
 * Decrypt message with AES CCM.
 ******************************************************************************/
sl_status_t decrypt_aes_ccm(void)
{
  print_error_cycle(sl_se_ccm_auth_decrypt(&cmd_ctx,
                                           &symmetric_key_desc,
                                           plain_msg_len,
                                           iv_nonce_buf,
                                           nonce_len,
                                           ad_buf,
                                           ad_len,
                                           cipher_msg_buf,
                                           plain_msg_buf,
                                           cipher_msg_buf + plain_msg_len,
                                           tag_len), &cmd_ctx);
}

/***************************************************************************//**
 * Encrypt message with AES GCM.
 ******************************************************************************/
sl_status_t encrypt_aes_gcm(void)
{
  print_error_cycle(sl_se_gcm_crypt_and_tag(&cmd_ctx,
                                            &symmetric_key_desc,
                                            SL_SE_ENCRYPT,
                                            plain_msg_len,
                                            iv_nonce_buf,
                                            nonce_len,
                                            ad_buf,
                                            ad_len,
                                            plain_msg_buf,
                                            cipher_msg_buf,
                                            tag_len,
                                            cipher_msg_buf + plain_msg_len),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Decrypt message with AES GCM.
 ******************************************************************************/
sl_status_t decrypt_aes_gcm(void)
{
  print_error_cycle(sl_se_gcm_auth_decrypt(&cmd_ctx,
                                           &symmetric_key_desc,
                                           plain_msg_len,
                                           iv_nonce_buf,
                                           nonce_len,
                                           ad_buf,
                                           ad_len,
                                           cipher_msg_buf,
                                           plain_msg_buf,
                                           tag_len,
                                           cipher_msg_buf + plain_msg_len),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Encrypt message with AES CBC.
 ******************************************************************************/
sl_status_t encrypt_aes_cbc(void)
{
  print_error_cycle(sl_se_aes_crypt_cbc(&cmd_ctx,
                                        &symmetric_key_desc,
                                        SL_SE_ENCRYPT,
                                        plain_msg_len,
                                        iv_nonce_buf,
                                        plain_msg_buf,
                                        cipher_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Decrypt message with AES CBC.
 ******************************************************************************/
sl_status_t decrypt_aes_cbc(void)
{
  print_error_cycle(sl_se_aes_crypt_cbc(&cmd_ctx,
                                        &symmetric_key_desc,
                                        SL_SE_DECRYPT,
                                        plain_msg_len,
                                        iv_nonce_buf,
                                        cipher_msg_buf,
                                        plain_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Encrypt message with AES CFB8.
 ******************************************************************************/
sl_status_t encrypt_aes_cfb8(void)
{
  print_error_cycle(sl_se_aes_crypt_cfb8(&cmd_ctx,
                                         &symmetric_key_desc,
                                         SL_SE_ENCRYPT,
                                         plain_msg_len,
                                         iv_nonce_buf,
                                         plain_msg_buf,
                                         cipher_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Decrypt message with AES CFB8.
 ******************************************************************************/
sl_status_t decrypt_aes_cfb8(void)
{
  print_error_cycle(sl_se_aes_crypt_cfb8(&cmd_ctx,
                                         &symmetric_key_desc,
                                         SL_SE_DECRYPT,
                                         plain_msg_len,
                                         iv_nonce_buf,
                                         cipher_msg_buf,
                                         plain_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Encrypt message with AES CFB128.
 ******************************************************************************/
sl_status_t encrypt_aes_cfb128(void)
{
  print_error_cycle(sl_se_aes_crypt_cfb128(&cmd_ctx,
                                           &symmetric_key_desc,
                                           SL_SE_ENCRYPT,
                                           plain_msg_len,
                                           &stream_offset,
                                           iv_nonce_buf,
                                           plain_msg_buf,
                                           cipher_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Decrypt message with AES CFB128.
 ******************************************************************************/
sl_status_t decrypt_aes_cfb128(void)
{
  print_error_cycle(sl_se_aes_crypt_cfb128(&cmd_ctx,
                                           &symmetric_key_desc,
                                           SL_SE_DECRYPT,
                                           plain_msg_len,
                                           &stream_offset,
                                           iv_nonce_buf,
                                           cipher_msg_buf,
                                           plain_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Generate AES CMAC on the message.
 ******************************************************************************/
sl_status_t generate_aes_cmac(void)
{
  // The length of the CMAC output memory buffer
  // (cipher_msg_buf in this example) must be at least 16 bytes wide
  print_error_cycle(sl_se_cmac(&cmd_ctx,
                               &symmetric_key_desc,
                               plain_msg_buf,
                               plain_msg_len,
                               cipher_msg_buf), &cmd_ctx);
}

/***************************************************************************//**
 * Generate HMAC on the message.
 ******************************************************************************/
sl_status_t generate_hmac(sl_se_hash_type_t hash_algo)
{
  // The length of the HMAC output memory buffer
  // (cipher_msg_buf in this example) must be at least the size of the
  // corresponding hash type (e.g. 32 bytes for SHA256)
  print_error_cycle(sl_se_hmac(&cmd_ctx,
                               &symmetric_key_desc,
                               hash_algo,
                               plain_msg_buf,
                               plain_msg_len,
                               cipher_msg_buf,
                               sizeof(cipher_msg_buf)), &cmd_ctx);
}

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)

/***************************************************************************//**
 * Generate a Poly1305 MAC using ChaCha20 key derivation.
 ******************************************************************************/
sl_status_t generate_poly1305_mac(void)
{
  print_error_cycle(sl_se_poly1305_genkey_tag(&cmd_ctx,
                                              &symmetric_key_desc,
                                              plain_msg_len,
                                              iv_nonce_buf,
                                              plain_msg_buf,
                                              cipher_msg_buf
                                              + plain_msg_len), &cmd_ctx);
}

/***************************************************************************//**
 * Encrypt message with ChaCha20-Poly1305.
 ******************************************************************************/
sl_status_t encrypt_chacha20_poly1305(void)
{
  print_error_cycle(sl_se_chacha20_poly1305_encrypt_and_tag(&cmd_ctx,
                                                            &symmetric_key_desc,
                                                            plain_msg_len,
                                                            iv_nonce_buf,
                                                            ad_buf,
                                                            ad_len,
                                                            plain_msg_buf,
                                                            cipher_msg_buf,
                                                            cipher_msg_buf
                                                            + plain_msg_len),
                    &cmd_ctx);
}

/***************************************************************************//**
 * Decrypt message with ChaCha20-Poly1305.
 ******************************************************************************/
sl_status_t decrypt_chacha20_poly1305(void)
{
  print_error_cycle(sl_se_chacha20_poly1305_auth_decrypt(&cmd_ctx,
                                                         &symmetric_key_desc,
                                                         plain_msg_len,
                                                         iv_nonce_buf,
                                                         ad_buf,
                                                         ad_len,
                                                         cipher_msg_buf,
                                                         plain_msg_buf,
                                                         cipher_msg_buf
                                                         + plain_msg_len),
                    &cmd_ctx);
}

#endif

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
