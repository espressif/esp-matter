/***************************************************************************//**
 * @file app_se_manager_stream_cipher.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_stream_cipher.h"

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

/// CMAC streaming context
static sl_se_cmac_multipart_context_t cmac_stream_ctx;

/// GCM streaming context
static sl_se_gcm_multipart_context_t gcm_stream_ctx;

/// Buffer for symmetric plain key
static uint8_t symmetric_key_buf[KEY_BUF_SIZE];

/// Key descriptor pointing to a symmetric plain key buffer
static sl_se_key_descriptor_t symmetric_key_desc = {
  // Set pointer to a RAM buffer to support key generation
  .storage.location.buffer.pointer = symmetric_key_buf,
  .storage.location.buffer.size = sizeof(symmetric_key_buf)
};

/// Associated data buffer for GCM
static uint8_t ad_buf[AD_SIZE];

/// IV or nonce buffer for GCM and ChaCha20
static uint8_t iv_nonce_buf[IV_NONCE_SIZE];

/// Plain message buffer
static uint8_t plain_msg_buf[PLAIN_MSG_SIZE];

/// Cipher message buffer
static uint8_t cipher_msg_buf[CIPHER_MSG_SIZE];

/// Tag buffer for gcm
static uint8_t tag_buf[TAG_SIZE];

/// Tag length for GCM
static uint8_t tag_len;

/// IV length for GCM
static uint8_t iv_len;

/// Associated data length for GCM
static size_t ad_len;

/// Plain message length
static size_t plain_msg_len;

/// Length of decrypted/encrypted output
static size_t output_len;

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
 * Get symmetric key buffer pointer.
 ******************************************************************************/
uint8_t * get_symmetric_key_buf_ptr(void)
{
  return(symmetric_key_buf);
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
 * Get tag buffer pointer.
 ******************************************************************************/
uint8_t * get_tag_buf_ptr(void)
{
  return(tag_buf);
}

/***************************************************************************//**
 * Get encrypted/decrypted output length.
 ******************************************************************************/
size_t get_output_len(void)
{
  return(output_len);
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
 * Set IV length.
 ******************************************************************************/
void set_iv_len(uint8_t length)
{
  iv_len = length;
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
 * Generate a plain symmetric key for stream cipher.
 ******************************************************************************/
sl_status_t create_plain_symmetric_key(sl_se_key_type_t key_type)
{
  uint32_t req_size;

  symmetric_key_desc.type = key_type;
  symmetric_key_desc.flags = 0;
  symmetric_key_desc.storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;

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
 * Start an AES CMAC stream.
 ******************************************************************************/
sl_status_t start_cmac_stream(void)
{
  // The command context (cmd_ctx) for streaming cannot be reused for other
  // mailbox operations until the streaming operation is finished (i.e. the
  // sl_se_cmac_multipart_finish is called).
  print_error_cycle(sl_se_cmac_multipart_starts(&cmac_stream_ctx,
                                                &cmd_ctx,
                                                &symmetric_key_desc), &cmd_ctx);
}

/***************************************************************************//**
 * Update an AES CMAC stream.
 ******************************************************************************/
sl_status_t update_cmac_stream(uint32_t offset)
{
  print_error_cycle(sl_se_cmac_multipart_update(&cmac_stream_ctx,
                                                &cmd_ctx,
                                                &symmetric_key_desc,
                                                plain_msg_buf + offset,
                                                plain_msg_len), NULL);
}

/***************************************************************************//**
 * Finish an AES CMAC stream.
 ******************************************************************************/
sl_status_t finish_cmac_stream(void)
{
  print_error_cycle(sl_se_cmac_multipart_finish(&cmac_stream_ctx,
                                                &cmd_ctx,
                                                &symmetric_key_desc,
                                                cipher_msg_buf), NULL);
}

/***************************************************************************//**
 * Start an AES GCM encryption stream.
 ******************************************************************************/
sl_status_t start_gcm_encrypt_stream(void)
{
  // The command context (cmd_ctx) for streaming cannot be reused for other
  // mailbox operations until the streaming operation is finished (i.e. the
  // sl_se_gcm_finish is called).
  print_error_cycle(sl_se_gcm_multipart_starts(&gcm_stream_ctx,
                                               &cmd_ctx,
                                               &symmetric_key_desc,
                                               SL_SE_ENCRYPT,
                                               iv_nonce_buf,
                                               iv_len,
                                               ad_buf,
                                               ad_len), &cmd_ctx);
}

/***************************************************************************//**
 * Update an AES GCM encryption stream.
 ******************************************************************************/
sl_status_t update_gcm_encrypt_stream(uint32_t offset_in, uint32_t offset_out)
{
  print_error_cycle(sl_se_gcm_multipart_update(&gcm_stream_ctx,
                                               &cmd_ctx,
                                               &symmetric_key_desc,
                                               plain_msg_len,
                                               plain_msg_buf + offset_in,
                                               cipher_msg_buf + offset_out,
                                               &output_len), NULL);
}

/***************************************************************************//**
 * Finish an AES GCM encryption stream.
 ******************************************************************************/
sl_status_t finish_gcm_encrypt_stream(uint32_t offset)
{
  print_error_cycle(sl_se_gcm_multipart_finish(&gcm_stream_ctx,
                                               &cmd_ctx,
                                               &symmetric_key_desc,
                                               tag_buf,
                                               tag_len,
                                               cipher_msg_buf + offset,
                                               plain_msg_len,
                                               (uint8_t *)&output_len), NULL);
}

/***************************************************************************//**
 * Start an AES GCM decryption stream.
 ******************************************************************************/
sl_status_t start_gcm_decrypt_stream(void)
{
  // The command context (cmd_ctx) for streaming cannot be reused for other
  // mailbox operations until the streaming operation is finished (i.e. the
  // sl_se_gcm_finish is called).
  print_error_cycle(sl_se_gcm_multipart_starts(&gcm_stream_ctx,
                                               &cmd_ctx,
                                               &symmetric_key_desc,
                                               SL_SE_DECRYPT,
                                               iv_nonce_buf,
                                               iv_len,
                                               ad_buf,
                                               ad_len), &cmd_ctx);
}

/***************************************************************************//**
 * Update an AES GCM decryption stream.
 ******************************************************************************/
sl_status_t update_gcm_decrypt_stream(uint32_t offset_in, uint32_t offset_out)
{
  print_error_cycle(sl_se_gcm_multipart_update(&gcm_stream_ctx,
                                               &cmd_ctx,
                                               &symmetric_key_desc,
                                               plain_msg_len,
                                               cipher_msg_buf + offset_in,
                                               plain_msg_buf + offset_out,
                                               &output_len), NULL);
}

/***************************************************************************//**
 * Finish an AES GCM decryption stream.
 ******************************************************************************/
sl_status_t finish_gcm_decrypt_stream(uint32_t offset)
{
  print_error_cycle(sl_se_gcm_multipart_finish(&gcm_stream_ctx,
                                               &cmd_ctx,
                                               &symmetric_key_desc,
                                               tag_buf,
                                               tag_len,
                                               plain_msg_buf + offset,
                                               plain_msg_len,
                                               (uint8_t *)&output_len), NULL);
}

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Update a ChaCha20 encryption stream.
 ******************************************************************************/
sl_status_t update_chacha20_encrypt_stream(uint32_t counter, size_t offset)
{
  uint8_t counter_array[4];

  // Change counter value to little endian
  *(uint32_t *)(counter_array) = __REV(counter);
  print_error_cycle(sl_se_chacha20_crypt(&cmd_ctx,
                                         SL_SE_ENCRYPT,
                                         &symmetric_key_desc,
                                         plain_msg_len,
                                         counter_array,
                                         iv_nonce_buf,
                                         plain_msg_buf + offset,
                                         cipher_msg_buf + offset), &cmd_ctx);
}

/***************************************************************************//**
 * Update a ChaCha20 decryption stream.
 ******************************************************************************/
sl_status_t update_chacha20_decrypt_stream(uint32_t counter, size_t offset)
{
  uint8_t counter_array[4];

  // Change counter value to little endian
  *(uint32_t *)(counter_array) = __REV(counter);
  print_error_cycle(sl_se_chacha20_crypt(&cmd_ctx,
                                         SL_SE_DECRYPT,
                                         &symmetric_key_desc,
                                         plain_msg_len,
                                         counter_array,
                                         iv_nonce_buf,
                                         cipher_msg_buf + offset,
                                         plain_msg_buf + offset), &cmd_ctx);
}
#endif

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
