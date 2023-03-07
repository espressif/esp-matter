/*******************************************************************************
* @file  rsi_crypto.c
* @brief
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

#include "rsi_driver.h"
#include "rsi_crypto.h"

#ifdef RSI_CRYPTO_ENABLE

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the SHA digest. This is a blocking API.
 * @param[in]  sha_mode     1 – For SHA1 \n 2 – For SHA256 \n 3 – For SHA384 \n 4 – For SHA512 \n
 * @param[in]  msg          - Pointer to message
 * @param[in]  msg_length   - Total message length
 * @param[in]  chunk_length - Current chunk length
 * @param[in]  pending_flag - BIT(0) - 1st chunk \n BIT(1) - Middle chunk \n BIT(2) - Last chunk \n
 * @param[out]  digest      - Output parameter to hold computed digest from SHA
 * @return	   0              - Success \n
 *             Non-Zero Value - Failure
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_sha_pen(uint8_t sha_mode,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_len,
                    uint8_t pending_flag,
                    uint8_t *digest)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_SHA_PEN_ENTRY, CRYPTO, LOG_INFO);
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t digest_len = 0;
  rsi_pkt_t *pkt;
  rsi_sha_req_t *sha;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_SHA_PEN_COMMAND_GIVEN_IN_WRONG_STATE, CRYPTO, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  // Input pointer check
  if (msg == NULL) {
    SL_PRINTF(SL_SHA_PEN_INVALID_PARAM_1, CRYPTO, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Fill digest length based on sha mode
    if (sha_mode == SHA_1) {
      digest_len = SHA_1_DIGEST_LEN;
    } else if (sha_mode == SHA_256) {
      digest_len = SHA_256_DIGEST_LEN;
    } else if (sha_mode == SHA_384) {
      digest_len = SHA_384_DIGEST_LEN;
    } else if (sha_mode == SHA_512) {
      digest_len = SHA_512_DIGEST_LEN;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      SL_PRINTF(SL_SHA_PEN_INVALID_PARAM_2, CRYPTO, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      //Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_SHA_PEN_PKT_ALLOCATION_FAILURE, CRYPTO, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (digest != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = digest;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = digest_len;
    }

    // Get Data Pointer
    sha = (rsi_sha_req_t *)pkt->data;

    // Memset before filling
    memset(sha, 0, sizeof(rsi_sha_req_t));

    // Fill Algorithm type SHA - 4
    sha->algorithm_type = SHA;

    sha->algorithm_sub_type = sha_mode;

    // Fill sha_flags BIT(0) - 1st chunk BIT(2) - Last chunk
    sha->sha_flags = pending_flag;

    // Fill total msg length
    sha->total_msg_length = msg_length;

    // Fill current chunk length
    sha->current_chunk_length = chunk_len;

    // Data
    // Memset before filling
    memset(&sha->msg[0], 0, MAX_DATA_SIZE_BYTES);

    // Copy Data
    memcpy(&sha->msg[0], msg, chunk_len);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_sha_req_t) - MAX_DATA_SIZE_BYTES + chunk_len;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send sha digest computation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_SHA_PEN_COMMAND_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  SL_PRINTF(SL_SHA_PEN_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Decide whether the SHA message can be sent once or requires multiple calls to send. This is a blocking API.
 * @param[in]  sha_mode   1 – For SHA1 \n 2 – For SHA256 \n 3 – For SHA384 \n 4 – For SHA512 \n
 * @param[in]  msg        - Pointer to message
 * @param[in]  msg_length - Total message length
 * @param[out]  digest    - Output parameter to hold computed digest from SHA
 * @return	   0              - Success \n
 *             Non-Zero Value - Failure
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_SHA_ENTRY, CRYPTO, LOG_INFO);
  uint16_t total_len = 0;
  uint16_t chunk_len = 0;
  uint16_t offset    = 0;
  uint8_t sha_flags  = 0;

  total_len = msg_length;

  while (total_len) {
    // Check total length
    if (total_len > MAX_DATA_SIZE_BYTES) {
      chunk_len = MAX_DATA_SIZE_BYTES;
      if (offset == 0) {
        // Make sha_flag as first chunk
        sha_flags |= FIRST_CHUNK;
      } else {
        // Make sha_flag as Middle chunk
        sha_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len = total_len;

      // Make sha_flag as Last chunk
      sha_flags = LAST_CHUNK;
      if (offset == 0) {
        /* If the total length is less than 1400 and offset is zero,
					 make sha_flag as both first chunk as well as last chunk*/
        sha_flags |= FIRST_CHUNK;
      }
    }

    // Send the current chunk length message
    status = rsi_sha_pen(sha_mode, msg, msg_length, chunk_len, sha_flags, digest);

    if (status != RSI_SUCCESS) {
      SL_PRINTF(SL_SHA_CHUNK_LENGTH_MSG_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Increment the offset value
    offset += chunk_len;
    msg += chunk_len;

    // Decrement the total message lenth
    total_len -= chunk_len;
  }
  SL_PRINTF(SL_SHA_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Hold computed digest from HMAC-SHA. This is a blocking API.
 * @param[in]  hmac_sha_mode - 1 – For HMAC-SHA1 \n 2 – For HMAC-SHA256 \n 3 – For HMAC-SHA384 \n 4 – For HMAC-SHA512 \n
 * @param[in]  data          - Pointer to key along with message
 * @param[in]  total_length  - Total message length
 * @param[in]  chunk_length  - Current chunk length
 * @param[in]  pending_flag  - BIT(0) - 1st chunk \n BIT(1) - Middle chunk \n BIT(2) - Last chunk \n
 * @param[out] digest        - Output parameter to hold computed digest from HMAC-SHA
 * @return	   0              - Success \n
 *             Non-Zero Value - Failure
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 */

int32_t rsi_hmac_sha_pen(uint8_t hmac_sha_mode,
                         uint8_t *data,
                         uint16_t total_length,
                         uint16_t chunk_length,
                         uint32_t key_length,
                         uint8_t pending_flag,
                         uint8_t *digest)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_HMAC_SHA_PEN_ENTRY, CRYPTO, LOG_INFO);
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t digest_len = 0;
  rsi_pkt_t *pkt;
  rsi_hmac_sha_req_t *hmac_sha;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_HMAC_SHA_PEN_COMMAND_GIVEN_IN_WRONG_STATE, CRYPTO, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  // Input pointer check
  if (data == NULL) {
    SL_PRINTF(SL_HMAC_SHA_PEN_INVALID_PARAM_1, CRYPTO, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Fill digest length based on sha mode
    if (hmac_sha_mode == SHA_1) {
      digest_len = SHA_1_DIGEST_LEN;
    } else if (hmac_sha_mode == SHA_256) {
      digest_len = SHA_256_DIGEST_LEN;
    } else if (hmac_sha_mode == SHA_384) {
      digest_len = SHA_384_DIGEST_LEN;
    } else if (hmac_sha_mode == SHA_512) {
      digest_len = SHA_512_DIGEST_LEN;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      SL_PRINTF(SL_HMAC_SHA_PEN_INVALID_PARAM_2, CRYPTO, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }
    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_HMAC_SHA_PEN_PKT_ALLOCATION_FAILURE, CRYPTO, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (digest != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = digest;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = digest_len;
    }

    // Get Data Pointer
    hmac_sha = (rsi_hmac_sha_req_t *)pkt->data;

    // Memset before filling
    memset(hmac_sha, 0, sizeof(rsi_hmac_sha_req_t));

    // Fill Algorithm type HMAC_SHA - 5
    hmac_sha->algorithm_type = HMAC_SHA;

    // Fill Algorithm sub type
    hmac_sha->algorithm_sub_type = hmac_sha_mode;

    // Fill sha_flags BIT(0) - 1st chunk BIT(2) - Last chunk
    hmac_sha->hmac_sha_flags = pending_flag;

    // Fill total msg length
    hmac_sha->total_msg_length = total_length;

    // Fill current chunk length
    hmac_sha->current_chunk_length = chunk_length;

    // Fill key length
    hmac_sha->key_length = key_length;

    // Data
    // Memset before filling
    memset(&hmac_sha->hmac_data[0], 0, MAX_DATA_SIZE_BYTES);

    // Copy Data
    memcpy(&hmac_sha->hmac_data[0], data, chunk_length);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_hmac_sha_req_t) - MAX_DATA_SIZE_BYTES + chunk_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send hmac sha digest computation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_HMAC_SHA_PEN_COMMAND_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  SL_PRINTF(SL_HMAC_SHA_PEN_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */
/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Decide whether the HMAC-SHA message can be sent once or requires multiple calls to send the message. This is a blocking API.
 * @param[in]  hmac_sha_mode - 1 – For HMAC-SHA1 \n 2 – For HMAC-SHA256 \n 3 – For HMAC-SHA384 \n 4 – For HMAC-SHA512 \n
 * @param[in]  msg           - Pointer to message
 * @param[in]  msg_length    - Total message length
 * @param[in]  key           - Pointer to HMAC key
 * @param[in]  key_length    - HMAC key length in bytes
 * @param[out]  digest       - Output parameter to hold computed digest from HMAC-SHA
 * @return	   0          - Success   \n 
 *             Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 *
 */

int32_t rsi_hmac_sha(uint8_t hmac_sha_mode,
                     uint8_t *msg,
                     uint32_t msg_length,
                     uint8_t *key,
                     uint32_t key_length,
                     uint8_t *digest,
                     uint8_t *hmac_buffer)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_HMAC_SHA_ENTRY, CRYPTO, LOG_INFO);
  uint32_t total_len     = 0;
  uint16_t chunk_len     = 0;
  uint16_t offset        = 0;
  uint8_t hmac_sha_flags = 0;
  uint8_t *data          = NULL;
  data                   = hmac_buffer;
  total_len              = msg_length + key_length;

  // Copy key in to data
  memcpy(data, key, key_length);

  // Copy message in to data
  memcpy((data + key_length), msg, msg_length);

  while (total_len) {

    // Check total length
    if (total_len > MAX_DATA_SIZE_BYTES) {
      chunk_len = MAX_DATA_SIZE_BYTES;
      if (offset == 0) {
        // Make hmac_sha_flag as first chunk
        hmac_sha_flags |= FIRST_CHUNK;
      } else {
        // Make hmac_sha_flag as Middle chunk
        hmac_sha_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len = total_len;

      // Make hmac_sha_flag as Last chunk
      hmac_sha_flags = LAST_CHUNK;
      if (offset == 0) {
        /* If the total length is less than 1400 and offset is zero,
					 make hmac_sha_flag as both first chunk as well as last chunk*/
        hmac_sha_flags |= FIRST_CHUNK;
      }
    }

    // Send the current chunk length message
    status =
      rsi_hmac_sha_pen(hmac_sha_mode, data, (msg_length + key_length), chunk_len, key_length, hmac_sha_flags, digest);

    if (status != RSI_SUCCESS) {
      SL_PRINTF(SL_HMAC_SHA_CHUNK_LENGTH_MSG_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Increment the offset value
    offset += chunk_len;
    data += chunk_len;

    // Decrement the total message lenth
    total_len -= chunk_len;
  }
  SL_PRINTF(SL_HMAC_SHA_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Decide whether the AES message can be sent once or requires multiple calls to send the message. This is a blocking API.
 * @param[in]  aes_mode   - 1 – For AES CBC mode \n 2 – For AES ECB mode \n 3 – For AES CTR mode \n
 * @param[in]  enc_dec    - 1 – For AES Encryption \n 2 – For AES Decryption \n
 * @param[in]  msg        - Pointer to message
 * @param[in]  msg_length - Total message length
 * @param[in]  key        - Pointer to AES key
 * @param[in]  key_length - AES key length in bytes
 * @param[in]  iv         - Pointer to AES iv
 * @param[out]  output    - Output parameter to hold encrypted/decrypted from AES
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_aes(uint16_t aes_mode,
                uint16_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint8_t *output)
{
  SL_PRINTF(SL_AES_ENTRY, CRYPTO, LOG_INFO);
  uint16_t total_len = 0;
  uint16_t chunk_len = 0;
  uint16_t offset    = 0;
  uint8_t aes_flags  = 0;
  int32_t status     = RSI_SUCCESS;

  total_len = msg_length;

  while (total_len) {
    // Check total length
    if (total_len > MAX_DATA_SIZE_BYTES) {
      chunk_len = MAX_DATA_SIZE_BYTES;
      if (offset == 0) {
        // Make aes_flags as first chunk
        aes_flags |= FIRST_CHUNK;
      } else {
        // Make aes_flags as Last chunk
        aes_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len = total_len;
      aes_flags = LAST_CHUNK;
      if (offset == 0) {
        /* If the total length is less than 1400 and offset is zero,
					 make aes_flags as both first chunk as well as last chunk*/
        aes_flags |= FIRST_CHUNK;
      }
    }

    // Send the current chunk length message
    status = rsi_aes_pen(aes_mode, enc_dec, msg, msg_length, chunk_len, key, key_length, iv, aes_flags, output);

    if (status != RSI_SUCCESS) {
      SL_PRINTF(SL_AES_CHUNK_LENGTH_MSG_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Increment the offset value
    offset += chunk_len;
    msg += chunk_len;

    // Decrement the total message lenth
    total_len -= chunk_len;
  }
  SL_PRINTF(SL_AES_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */

/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Encrypt/decrypt the data using AES
 * @param[in]  aes_mode     - 1 – For AES CBC mode \n 2 – For AES ECB mode \n 3 – For AES CTR mode \n
 * @param[in]  enc_dec      - 1 – For AES Encryption \n 2 – For AES Decryption \n
 * @param[in]  msg          - Pointer to message
 * @param[in]  msg_lengthm  - Total message length
 * @param[in]  chunk_length - Current chunk length
 * @param[in]  key          - Pointer to AES key
 * @param[in]  key_length   - AES key length in bytes
 * @param[in]  iv           - Pointer to AES iv
 * @param[in]  aes_flags    - BIT(0) - 1st chunk \n BIT(1) - Middle chunk \n BIT(2) - Last chunk\n 
 * @param[out]  output      - Output parameter to hold encrypted/decrypted from AES
 * @return	   0              - Success  \n
 *             Non-Zero Value - Failure
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_aes_pen(uint16_t aes_mode,
                    uint16_t enc_dec,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    uint8_t *key,
                    uint16_t key_length,
                    uint8_t *iv,
                    uint8_t aes_flags,
                    uint8_t *output)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_AES_PEN_ENTRY, CRYPTO, LOG_INFO);
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  rsi_pkt_t *pkt;
  rsi_aes_req_t *aes;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  // Input pointer check
  if (msg == NULL) {
    SL_PRINTF(SL_AES_PEN_INVALID_PARAM_1, CRYPTO, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  if (((aes_mode == CBC_MODE) || (aes_mode == CTR_MODE)) && (iv == NULL)) {
    SL_PRINTF(SL_AES_PEN_INVALID_PARAM_2, CRYPTO, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      //Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_AES_PEN_PKT_ALLOCATION_FAILURE, CRYPTO, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (output != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = output;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = msg_length;
    }

    // Get Data Pointer
    aes = (rsi_aes_req_t *)pkt->data;

    // Memset before filling
    memset(aes, 0, sizeof(rsi_aes_req_t));

    // Fill Algorithm type AES - 2
    aes->algorithm_type = AES;

    // Fill Algorithm subtype
    aes->algorithm_sub_type = aes_mode;

    // Fill enc_dec: 1 – For AES Encryption 2 – For AES Decryption
    aes->encrypt_decryption = enc_dec;

    // Fill aes_flags BIT(0) - 1st chunk BIT(2) - Last chunk
    aes->aes_flags = aes_flags;

    // Fill msg length
    aes->total_msg_length = msg_length;

    // Fill chunk length
    aes->current_chunk_length = chunk_length;

    // Fill key length
    aes->key_length = key_length;

    //KEY
    // Memset before filling
    memset(&aes->key[0], 0, key_length);

    // Copy KEY
    memcpy(&aes->key[0], key, key_length);

    if ((aes_mode == CBC_MODE) || (aes_mode == CTR_MODE)) {
      // IV
      // Memset before filling
      memset(&aes->IV[0], 0, IV_SIZE);

      // Copy IV
      memcpy(&aes->IV[0], iv, IV_SIZE);
    }

    // Data
    // Memset before filling
    memset(&aes->msg[0], 0, MAX_DATA_SIZE_BYTES);

    // Copy Data
    memcpy(&aes->msg[0], msg, chunk_length);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_aes_req_t) - MAX_DATA_SIZE_BYTES + chunk_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send aes encrypt/decrypt request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_AES_PEN_COMMAND_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  SL_PRINTF(SL_AES_PEN_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Calculate the DH key. This is a blocking API.
 * @param[in]  prime           - Pointer to the prime
 * @param[in]  prime_length    - Length of the prime
 * @param[in]  base            - Pointer to base
 * @param[in]  base_length     - Length of the base
 * @param[in]  exponent        - Pointer to exponent
 * @param[in]  exponent_length - Length of the exponent
 * @param[out] exp_result      - Output exponentiation result
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */
int32_t rsi_exponentiation(uint8_t *prime,
                           uint32_t prime_length,
                           uint8_t *base,
                           uint32_t base_length,
                           uint8_t *exponent,
                           uint32_t exponent_length,
                           uint8_t *exp_result)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_EXPONENTIATION_ENTRY, CRYPTO, LOG_INFO);
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  rsi_pkt_t *pkt;
  rsi_exp_req_t *exp;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_EXPONENTIATION_COMMAND_GIVEN_IN_WRONG_STATE, CRYPTO, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  // Input pointer check
  if ((base == NULL) || (prime == NULL) || (exponent == NULL)) {
    SL_PRINTF(SL_EXPONENTIATION_INVALID_PARAM, CRYPTO, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_EXPONENTIATION_PKT_ALLOCATION_FAILURE, CRYPTO, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (exp_result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = exp_result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = DH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    exp = (rsi_exp_req_t *)pkt->data;

    // Memset before filling
    memset(exp, 0, sizeof(rsi_exp_req_t));

    // Fill Algorithm type Exponentiation - 14
    exp->algorithm_type = DH;

    // Fill prime length
    exp->prime_length = prime_length;

    // Fill base length
    exp->base_length = base_length;

    // Fill exponent length
    exp->exponent_length = exponent_length;

    //PRIME
    // Memset before filling
    memset(&exp->prime[0], 0, prime_length);

    // Copy Prime
    memcpy(&exp->prime[0], prime, prime_length);

    //BASE
    // Memset before filling
    memset(&exp->base[0], 0, base_length);

    // Copy Base
    memcpy(&exp->base[0], base, base_length);

    // EXPONENT
    // Memset before filling
    memset(&exp->exponent[0], 0, exponent_length);

    // Copy Exponent
    memcpy(&exp->exponent[0], exponent, exponent_length);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_exp_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send DH key calculation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_EXPONENTIATION_COMMAND_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  SL_PRINTF(SL_EXPONENTIATION_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the ECDH point multiplication vector. This is a blocking API.
 * @param[in]  ecdh_mode  - 1 – For ECDH 192 \n 2 – For ECDH 224 \n 3 – For ECDH 256 \n
 * @param[in]  d          - Pointer to scalar value that needs to be multiplied
 * @param[in]  sx, sy, sz - Pointers to x, y, z coordinates of the point to be multiplied with scalar ‘d’
 * @param[out] rx, ry, rz - Pointers to x, y, z coordinates of the result point
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_ecdh_point_multiplication(uint8_t ecdh_mode,
                                      uint8_t *d,
                                      uint8_t *sx,
                                      uint8_t *sy,
                                      uint8_t *sz,
                                      uint8_t *rx,
                                      uint8_t *ry,
                                      uint8_t *rz)
{
  SL_PRINTF(SL_ECDH_POINT_MULTIPLICATION_ENTRY, CRYPTO, LOG_INFO);
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t result[96] = { 0 };
  uint8_t size       = 0;
  uint8_t offset     = 0;
  rsi_pkt_t *pkt;
  rsi_ecdh_pm_req_t *ecdh;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    if (ecdh_mode == ECDH_192) {
      size = ECDH_VECTOR_192_SIZE;
    } else if (ecdh_mode == ECDH_224) {
      size = ECDH_VECTOR_224_SIZE;
    } else if (ecdh_mode == ECDH_256) {
      size = ECDH_VECTOR_256_SIZE;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      SL_PRINTF(SL_ECDH_POINT_MULTIPLICATION_INVALID_PARAM, CRYPTO, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }
    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_ECDH_POINT_MULTIPLICATION_PKT_ALLOCATION_FAILURE, CRYPTO, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = ECDH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    ecdh = (rsi_ecdh_pm_req_t *)pkt->data;

    // Memset before filling
    memset(ecdh, 0, sizeof(rsi_ecdh_pa_ps_req_t));

    // Fill Algorithm type ECDH - 15
    ecdh->algorithm_type = ECDH;

    // Fill ecdh mode
    ecdh->ecdh_mode = ecdh_mode;

    // Fill ecdh sub mode 1 - point Multiplication
    ecdh->ecdh_sub_mode = ECDH_PM;
    // Fill Curve type
    ecdh->ecdh_curve_type = ECDH_CURVE_P;

    // Memset before filling
    memset(&ecdh->sx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sx[0], sx, size);

    // Memset before filling
    memset(&ecdh->sy[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sy[0], sy, size);

    // Memset before filling
    memset(&ecdh->sz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sz[0], sz, size);

    // Memset before filling
    memset(&ecdh->d[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->d[0], d, size);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_ecdh_pm_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send ecdh point multiplication request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);
    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Copy Data
    memcpy(rx, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(ry, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(rz, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_ECDH_POINT_MULTIPLICATION_COMMAND_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  SL_PRINTF(SL_ECDH_POINT_MULTIPLICATION_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the ECDH point addition vector. This is a blocking API.
 * @param[in]  ecdh_mode  - 1 – For ECDH 192 \n 2 – For ECDH 224 \n 3 – For ECDH 256 \n
 * @param[in]  sx, sy, sz - Pointers to x, y, z coordinates of the point1 that needs to be added
 * @param[in]  tx, ty, tz - Pointers to x, y, z coordinates of the point2 that needs to be added
 * @param[out] rx, ry, rz - Pointers to x, y, z coordinates of the result point
 * @return	    0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_ecdh_point_addition(uint8_t ecdh_mode,
                                uint8_t *sx,
                                uint8_t *sy,
                                uint8_t *sz,
                                uint8_t *tx,
                                uint8_t *ty,
                                uint8_t *tz,
                                uint8_t *rx,
                                uint8_t *ry,
                                uint8_t *rz)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_ECDH_POINT_ADDITION_ENTRY, CRYPTO, LOG_INFO);
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t result[96] = { 0 };
  uint8_t size       = 0;
  uint8_t offset     = 0;
  rsi_pkt_t *pkt;
  rsi_ecdh_pa_ps_req_t *ecdh;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if (rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE) == RSI_SUCCESS) {
    // Fill vector size based on ecdh mode
    if (ecdh_mode == ECDH_192) {
      size = ECDH_VECTOR_192_SIZE;
    } else if (ecdh_mode == ECDH_224) {
      size = ECDH_VECTOR_224_SIZE;
    } else if (ecdh_mode == ECDH_256) {
      size = ECDH_VECTOR_256_SIZE;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      SL_PRINTF(SL_ECDH_POINT_ADDITION_INVALID_PARAM, CRYPTO, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_ECDH_POINT_ADDITION_PKT_ALLOCATION_FAILURE, CRYPTO, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = ECDH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    ecdh = (rsi_ecdh_pa_ps_req_t *)pkt->data;

    // Memset before filling
    memset(ecdh, 0, sizeof(rsi_ecdh_pa_ps_req_t));

    // Fill Algorithm type ECDH - 15
    ecdh->algorithm_type = ECDH;

    // Fill ecdh mode
    ecdh->ecdh_mode = ecdh_mode;

    // Fill ecdh sub mode 2 - point Addition
    ecdh->ecdh_sub_mode = ECDH_PA;

    // Memset before filling
    memset(&ecdh->sx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sx[0], sx, size);

    // Memset before filling
    memset(&ecdh->sy[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sy[0], sy, size);

    // Memset before filling
    memset(&ecdh->sz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sz[0], sz, size);

    // Memset before filling
    memset(&ecdh->tx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->tx[0], tx, size);

    // Memset before filling
    memset(&ecdh->ty[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->ty[0], ty, size);

    // Memset before filling
    memset(&ecdh->tz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->tz[0], tz, size);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_ecdh_pa_ps_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send point addition calculation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);
    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Copy Data
    memcpy(rx, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(ry, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(rz, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_ECDH_POINT_ADDITION_COMMON_CMD_IN_PROGRESS, CRYPTO, LOG_ERROR);
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  SL_PRINTF(SL_ECDH_POINT_ADDITION_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the ECDH point subtraction vector. This is a blocking API.
 * @param[in]  ecdh_mode  - 1 – For ECDH 192 \n 2 – For ECDH 224 \n 3 – For ECDH 256 \n    
 * @param[in]  sx, sy, sz - Pointers to x, y, z coordinates of the point1 that needs to be subtracted
 * @param[in]  tx, ty, tz - Pointers to x, y, z coordinates of the point2 that needs to be subtracted
 * @param[out] rx, ry, rz - Pointers to x, y, z coordinates of the result point
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 *
 */

int32_t rsi_ecdh_point_subtraction(uint8_t ecdh_mode,
                                   uint8_t *sx,
                                   uint8_t *sy,
                                   uint8_t *sz,
                                   uint8_t *tx,
                                   uint8_t *ty,
                                   uint8_t *tz,
                                   uint8_t *rx,
                                   uint8_t *ry,
                                   uint8_t *rz)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_ECDH_POINT_SUBTRACTION_ENTRY, CRYPTO, LOG_INFO);
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t result[96] = { 0 };
  uint8_t size       = 0;
  uint8_t offset     = 0;
  rsi_pkt_t *pkt;
  rsi_ecdh_pa_ps_req_t *ecdh;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Fill vector size based on ecdh mode
    if (ecdh_mode == ECDH_192) {
      size = ECDH_VECTOR_192_SIZE;
    } else if (ecdh_mode == ECDH_224) {
      size = ECDH_VECTOR_224_SIZE;
    } else if (ecdh_mode == ECDH_256) {
      size = ECDH_VECTOR_256_SIZE;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      SL_PRINTF(SL_ECDH_POINT_SUBTRACTION_INVALID_PARAM, CRYPTO, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_ECDH_POINT_SUBTRACTION_PKT_ALLOCATION_FAILURE, CRYPTO, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = ECDH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    ecdh = (rsi_ecdh_pa_ps_req_t *)pkt->data;

    // Memset before filling
    memset(ecdh, 0, sizeof(rsi_ecdh_pa_ps_req_t));

    // Fill Algorithm type ECDH - 15
    ecdh->algorithm_type = ECDH;

    // Fill ecdh mode
    ecdh->ecdh_mode = ecdh_mode;

    // Fill ecdh sub mode 3 - point subtraction
    ecdh->ecdh_sub_mode = ECDH_PS;

    // Memset before filling
    memset(&ecdh->sx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sx[0], sx, size);

    // Memset before filling
    memset(&ecdh->sy[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sy[0], sy, size);

    // Memset before filling
    memset(&ecdh->sz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sz[0], sz, size);

    // Memset before filling
    memset(&ecdh->tx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->tx[0], tx, size);

    // Memset before filling
    memset(&ecdh->ty[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->ty[0], ty, size);

    // Memset before filling
    memset(&ecdh->tz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->tz[0], tz, size);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_ecdh_pa_ps_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send point subtraction calculation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Copy Data
    memcpy(rx, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(ry, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(rz, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_ECDH_POINT_SUBTRACTION_COMMAND_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  SL_PRINTF(SL_ECDH_POINT_SUBTRACTION_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the ECDH point double vector. This is a blocking API.
 * @param[in]  ecdh_mode  - 1 – For ECDH 192 \n 2 – For ECDH 224 \n 3 – For ECDH 256 \n
 * @param[in]  sx, sy, sz - Pointers to x, y, z coordinates of the point1 that needs to be doubled
 * @param[out] rx, ry, rz - Pointers to x, y, z coordinates of the result point
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 *
 */

int32_t rsi_ecdh_point_double(uint8_t ecdh_mode,
                              uint8_t *sx,
                              uint8_t *sy,
                              uint8_t *sz,
                              uint8_t *rx,
                              uint8_t *ry,
                              uint8_t *rz)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_ECDH_POINT_DOUBLE_ENTRY, CRYPTO, LOG_INFO);
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t result[96] = { 0 };
  uint8_t size       = 0;
  uint8_t offset     = 0;
  rsi_pkt_t *pkt;
  rsi_ecdh_pd_req_t *ecdh;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Fill vector size based on ecdh mode
    if (ecdh_mode == ECDH_192) {
      size = ECDH_VECTOR_192_SIZE;
    } else if (ecdh_mode == ECDH_224) {
      size = ECDH_VECTOR_224_SIZE;
    } else if (ecdh_mode == ECDH_256) {
      size = ECDH_VECTOR_256_SIZE;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      SL_PRINTF(SL_ECDH_POINT_DOUBLE_INVALID_PARAM, CRYPTO, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }
    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_ECDH_POINT_DOUBLE_PKT_ALLOCATION_FAILURE, CRYPTO, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = ECDH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    ecdh = (rsi_ecdh_pd_req_t *)pkt->data;

    // Memset before filling
    memset(ecdh, 0, sizeof(rsi_ecdh_pa_ps_req_t));

    // Fill Algorithm type ECDH - 15
    ecdh->algorithm_type = ECDH;

    // Fill ecdh mode
    ecdh->ecdh_mode = ecdh_mode;

    // Fill ecdh sub mode 4 - point double
    ecdh->ecdh_sub_mode = ECDH_PD;

    // Memset before filling
    memset(&ecdh->sx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sx[0], sx, size);

    // Memset before filling
    memset(&ecdh->sy[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sy[0], sy, size);

    // Memset before filling
    memset(&ecdh->sz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sz[0], sz, size);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_ecdh_pd_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send point double calculation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Copy Data
    memcpy(rx, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(ry, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(rz, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_ECDH_POINT_DOUBLE_COMMAND_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  SL_PRINTF(SL_ECDH_POINT_DOUBLE_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the ECDH point affinity vector. This is a blocking API.
 * @param[in]  ecdh_mode  - 1 – For ECDH 192 \n 2 – For ECDH 224 \n 3 – For ECDH 256 \n
 * @param[in]  sx, sy, sz - Pointers to x, y, z coordinates of the point1 that needs to be affinified
 * @param[out] rx, ry, rz - Pointers to x, y, z coordinates of the result point
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_ecdh_point_affine(uint8_t ecdh_mode,
                              uint8_t *sx,
                              uint8_t *sy,
                              uint8_t *sz,
                              uint8_t *rx,
                              uint8_t *ry,
                              uint8_t *rz)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_ECDH_POINT_AFFINE_ENTRY, CRYPTO, LOG_INFO);
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t result[96] = { 0 };
  uint8_t size       = 0;
  uint8_t offset     = 0;
  rsi_pkt_t *pkt;
  rsi_ecdh_affine_req_t *ecdh;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Fill vector size based on ecdh mode
    if (ecdh_mode == ECDH_192) {
      size = ECDH_VECTOR_192_SIZE;
    } else if (ecdh_mode == ECDH_224) {
      size = ECDH_VECTOR_224_SIZE;
    } else if (ecdh_mode == ECDH_256) {
      size = ECDH_VECTOR_256_SIZE;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      SL_PRINTF(SL_ECDH_POINT_AFFINE_INVALID_PARAM, CRYPTO, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_ECDH_POINT_AFFINE_PKT_ALLOCATION_FAILURE, CRYPTO, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = ECDH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    ecdh = (rsi_ecdh_affine_req_t *)pkt->data;

    // Memset before filling
    memset(ecdh, 0, sizeof(rsi_ecdh_affine_req_t));

    // Fill Algorithm type ECDH - 15
    ecdh->algorithm_type = ECDH;

    // Fill ecdh mode
    ecdh->ecdh_mode = ecdh_mode;

    // Fill Curve type
    ecdh->ecdh_curve_type = ECDH_CURVE_P;

    // Fill ecdh sub mode 5 - point affine
    ecdh->ecdh_sub_mode = ECDH_PAF;

    // Memset before filling
    memset(&ecdh->sx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sx[0], sx, size);

    // Memset before filling
    memset(&ecdh->sy[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sy[0], sy, size);

    // Memset before filling
    memset(&ecdh->sz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sz[0], sz, size);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_ecdh_affine_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send point affine calculation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);
    // Copy Data
    memcpy(rx, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(ry, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(rz, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    SL_PRINTF(SL_ECDH_POINT_AFFINE_COMMAND_ERROR, CRYPTO, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  SL_PRINTF(SL_SET_RTC_TIMER_EXIT, CRYPTO, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */
void reverse_8(unsigned char *xx, int no_digits)
{
  int count;
  uint8_t temp;
  for (count = 0; count < (no_digits / 2); count++) {
    temp                      = xx[count];
    xx[count]                 = xx[no_digits - count - 1];
    xx[no_digits - count - 1] = temp;
  }
}

#ifdef CHIP_9117
/*==============================================*/
/**
 * @fn         rsi_sha3_shake_pen(uint8_t pad_char,uint8_t mode, uint8_t *msg, uint16_t msg_length,
 *              uint16_t chunk_len, uint8_t pending_flag, uint8_t *digest)
 * @brief      Computes the sha3/shake digest
 * @param[in]  pad_char 0x1F SHAKE, 0x06 SHA3 
 * @param[in]  mode
 * 				21  - SHAKE_128
 *				17  - SHAKE_256, SHA3_256
 *				18  - SHA3_224
 *				13  - SHA3_384
 *				9   - SHA3_512
 * @param[in]  msg: Pointer to message 
 * @param[in]  msg_length: Total message length
 * @param[in]  chunk_length: current chunk length
 * @param[in]  pending_flag: BIT(0) - 1st chunk BIT(1) - Middle chunk BIT(2) - Last chunk 
 * @param[out]  digest:  Output parameter to hold computed digest from SHA3/SHAKE
 *  * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function computes the sha3/shake digest for the given input message
 *
 */
int32_t rsi_sha3_shake_pen(uint8_t pad_char,
                           uint8_t mode,
                           uint8_t *msg,
                           uint16_t msg_length,
                           uint16_t chunk_len,
                           uint8_t pending_flag,
                           uint8_t *digest)
{

  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t digest_len = 0;
  rsi_pkt_t *pkt;

  //!using same structure for sha3 and shake
  rsi_sha_req_t *sha3;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    //! Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  //! Input pointer check
  if (msg == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }
  if (rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE) == RSI_SUCCESS) {

    //! Fill digest length based on sha mode
    if (mode == SHA3_224) {
      digest_len = SHA_224_DIGEST_LEN;
    } else if (mode == SHA3_256 || mode == SHAKE_256) {
      digest_len = SHA_256_DIGEST_LEN;
    } else if (mode == SHA3_384) {
      digest_len = SHA_384_DIGEST_LEN;
    } else if (mode == SHA3_512) {
      digest_len = SHA_512_DIGEST_LEN;
    } else if (mode == SHAKE_128) {
      digest_len = SHAKE_128_DIGEST_LEN;
    }

    else {
      //!Changing the common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      return RSI_ERROR_INVALID_PARAM;
    }

    //! allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    //! If allocation of packet fails
    if (pkt == NULL) {
      //!Changing the common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (digest != NULL) {
      //! attach the buffer given by user
      rsi_common_cb->app_buffer = digest;

      //! length of the buffer provided by user
      rsi_common_cb->app_buffer_length = digest_len;
    }

    //! Get Data Pointer
    sha3 = (rsi_sha_req_t *)pkt->data;

    //! Memset before filling
    memset(sha3, 0, sizeof(rsi_sha_req_t));

    //! Fill Algorithm type SHA3 or SHAKE
    if (pad_char == PAD_CHAR_SHA3) {
      sha3->algorithm_type = SHA3;
    } else {
      sha3->algorithm_type = SHAKE;
    }

    sha3->algorithm_sub_type = mode;

    //! Fill sha_flags BIT(0) - 1st chunk BIT(2) - Last chunk
    sha3->sha_flags = pending_flag;

    //! Fill total msg length
    sha3->total_msg_length = msg_length;

    //! Fill current chunk length
    sha3->current_chunk_length = chunk_len;

    //!Data
    //! Memset before filling
    memset(&sha3->msg[0], 0, MAX_DATA_SIZE_BYTES);

    //! Copy Data
    memcpy(&sha3->msg[0], msg, chunk_len);

    //! Using host descriptor to set payload length
    send_size = sizeof(rsi_sha_req_t) - MAX_DATA_SIZE_BYTES + chunk_len;

    //! get the host descriptor
    host_desc = (pkt->desc);

    //! Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    //! send sha digest computation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    //! wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;
}

/*==============================================*/
/**
 * @fn         rsi_shake(uint8_t mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest) * @brief      Decides whether the sha message can be sent once or requires multiple calls to send
 * @param[in]  r 
 * 				21  - SHAKE_128
 *				17  - SHAKE_256
 * @param[in]  msg: Pointer to message 
 * @param[in]  msg_length: Total message length
 * @param[out]  digest:  Output parameter to hold computed digest from SHAKE
 *  * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function decides whether the shakeamessage can be sent once or requires multiple calls to send the message
 *
 */
int32_t rsi_shake(uint8_t mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest)
{
  int32_t status     = RSI_SUCCESS;
  uint16_t total_len = 0;
  uint16_t chunk_len = 0;
  uint16_t offset    = 0;
  uint8_t sha_flags  = 0;
  uint8_t pad_char   = PAD_CHAR_SHAKE;

  total_len = msg_length;

  while (total_len) {
    //! check total length
    if (total_len > MAX_DATA_SIZE_BYTES) {
      chunk_len = MAX_DATA_SIZE_BYTES;
      if (offset == 0) {
        //! Make sha_flag as first chunk
        sha_flags |= FIRST_CHUNK;
      } else {
        //! Make sha_flag as Middle chunk
        sha_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len = total_len;

      //! Make sha_flag as Last chunk
      sha_flags = LAST_CHUNK;
      if (offset == 0) {
        /* if the total length is less than 1400 and offset is zero 
					 then make sha_flag as both first chunk as well as last chunk*/
        sha_flags |= FIRST_CHUNK;
      }
    }

    //! send the current chunk length message
    status = rsi_sha3_shake_pen(pad_char, mode, msg, msg_length, chunk_len, sha_flags, digest);

    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Increment the offset value
    offset += chunk_len;
    msg += chunk_len;

    //! Decrement the total message lenth
    total_len -= chunk_len;
  }

  return status;
}

/*==============================================*/
/**
 * @fn         rsi_sha3(uint8_t mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest)
 * @brief      Decides whether the sha message can be sent once or requires multiple calls to send
 * @param[in]  mode 
 *				17  - SHA3_256
 *				18  - SHA3_224
 *				13  - SHA3_384
 *				9   - SHA3_512
 * @param[in]  msg: Pointer to message 
 * @param[in]  msg_length: Total message length
 * @param[out]  digest:  Output parameter to hold computed digest from SHA3
 *  * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function decides whether the sha message can be sent once or requires multiple calls to send the message
 *
 */

int32_t rsi_sha3(uint8_t mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest)
{
  int32_t status     = RSI_SUCCESS;
  uint16_t total_len = 0;
  uint16_t chunk_len = 0;
  uint16_t offset    = 0;
  uint8_t sha_flags  = 0;
  uint8_t pad_char   = PAD_CHAR_SHA3;

  total_len = msg_length;

  while (total_len) {
    //! check total length
    if (total_len > MAX_DATA_SIZE_BYTES) {
      chunk_len = MAX_DATA_SIZE_BYTES;
      if (offset == 0) {
        //! Make sha_flag as first chunk
        sha_flags |= FIRST_CHUNK;
      } else {
        //! Make sha_flag as Middle chunk
        sha_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len = total_len;

      //! Make sha_flag as Last chunk
      sha_flags = LAST_CHUNK;
      if (offset == 0) {
        /* if the total length is less than 1400 and offset is zero 
					 then make sha_flag as both first chunk as well as last chunk*/
        sha_flags |= FIRST_CHUNK;
      }
    }

    //! send the current chunk length message
    status = rsi_sha3_shake_pen(pad_char, mode, msg, msg_length, chunk_len, sha_flags, digest);

    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Increment the offset value
    offset += chunk_len;
    msg += chunk_len;

    //! Decrement the total message lenth
    total_len -= chunk_len;
  }

  return status;
}

/*==============================================*/
/**
 * @fn         rsi_chachapoly(uint16_t chachapoly_mode, uint16_t enc_dec,uint16_t dma_use, uint8_t *msg, uint16_t msg_length, uint8_t *key_chacha, uint8_t *keyr_in,uint8_t *keys_in, 
				uint8_t *nonce, uint8_t *header_input, uint16_t header_length, uint8_t *output)
 * @brief      Decides whether the CHACHAPOLY message can be sent once or requires multiple calls to send
 * @param[in]  chacha_mode : 0 – For CHACHA20POLY1305 mode 1 – For CHACHA20 mode 3 – For POLY1305 mode 
 * @param[in]  enc_dec: 0 – For CHACHAPOLY Encryption 1 – For CHACHAPOLY Decryption 
 * @param[in]  dma_use: 1 - DMA enable  0- disable DMA 
 * @param[in]  msg: Pointer to message 
 * @param[in]  msg_length: Total message length
 * @param[in]  key_chacha: Pointer to chacha key
 * @param[in]  keyr_in: pointer to keyr_in
 * @param[in]  keys_in: pointer to keys_in
 * @param[in]  nonce: pointer to nonce (1st index is IV)
 * @param[in]  header_input: Pointer to header
 * @param[in]  header_length: header length in bytes
 * @param[out]  output:  Output parameter to hold encrypted/decrypted from chachapoly
 *  * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function decides whether the chachapoly message can be sent once or requires multiple calls to send the message
 *
 */
int32_t rsi_chachapoly(uint16_t chachapoly_mode,
                       uint16_t enc_dec,
                       uint16_t dma_use,
                       uint8_t *msg,
                       uint16_t msg_length,
                       uint8_t *key_chacha,
                       uint8_t *keyr_in,
                       uint8_t *keys_in,
                       uint8_t *nonce,
                       uint8_t *header_input,
                       uint16_t header_length,
                       uint8_t *output)
{

  uint16_t total_len       = 0;
  uint16_t chunk_len       = 0;
  uint16_t offset          = 0;
  uint8_t chachapoly_flags = 0;
  int32_t status           = RSI_SUCCESS;

  total_len = msg_length;

  //! Input pointer check
  if (msg == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  while (total_len) {
    //! check total length
    if (total_len > MAX_DATA_SIZE_BYTES_FOR_CHACHAPOLY) {
      chunk_len = MAX_DATA_SIZE_BYTES_FOR_CHACHAPOLY;
      if (offset == 0) {
        //! Make chachapoly_flags as first chunk
        chachapoly_flags |= FIRST_CHUNK;
      } else {
        //! Make chachapoly_flags as Last chunk
        chachapoly_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len        = total_len;
      chachapoly_flags = LAST_CHUNK;
      if (offset == 0) {
        /* if the total length is less than 1400 and offset is zero 
					 then make chachapoly_flags as both first chunk as well as last chunk*/
        chachapoly_flags |= FIRST_CHUNK;
      }
    }

    //! send the current chunk length message
    status = rsi_chachapoly_pen(chachapoly_mode,
                                enc_dec,
                                dma_use,
                                msg,
                                msg_length,
                                chunk_len,
                                key_chacha,
                                keyr_in,
                                keys_in,
                                nonce,
                                header_input,
                                header_length,
                                chachapoly_flags,
                                output);

    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Increment the offset value
    offset += chunk_len;
    msg += chunk_len;

    //! Decrement the total message lenth
    total_len -= chunk_len;
  }

  return status;
}

/*==============================================*/
/**
 * @fn         rsi_chachapoly_pen(uint16_t chachapoly_mode, uint16_t enc_dec,uint16_t dma_use, uint8_t *msg, uint16_t msg_length,uint16_t chunk_len, uint8_t *key_chacha, uint8_t *keyr_in,uint8_t *keys_in, 
				uint8_t *nonce, uint8_t *header_input, uint16_t header_length,uint8_t chachapoly_flags,uint8_t *output)
 * @brief      Encrypt/Decrypt the data using CHACHA_POLY
 * @param[in]  chacha_mode : 0 – For CHACHA20POLY1305 mode 1 – For CHACHA20 mode 3 – For POLY1305 mode 
 * @param[in]  enc_dec: 0 – For CHACHAPOLY Encryption 1 – For CHACHAPOLY Decryption 
 * @param[in]  dma_use: 0 - DMA disable  1- DMA enable
 * @param[in]  msg: Pointer to message 
 * @param[in]  msg_length: Total message length
 * @param[in]  chunk_len: current chunk length
 * @param[in]  key_chacha: Pointer to chacha key
 * @param[in]  keyr_in: pointer to keyr_in
 * @param[in]  keys_in: pointer to keys_in
 * @param[in]  nonce: pointer to nonce (1st index is IV)
 * @param[in]  header_input: Pointer to header
 * @param[in]  header_length: header length in bytes
 * @param[in]  chachapoly_flags: BIT(0) - 1st chunk BIT(1) - Middle chunk BIT(2) - Last chunk 
 * @param[out]  output:  Output parameter to hold encrypted/decrypted from chachapoly
 *   @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function encrypt/decrypt the data using CHACHAPOLY
 *
 */
int32_t rsi_chachapoly_pen(uint16_t chachapoly_mode,
                           uint16_t enc_dec,
                           uint16_t dma_use,
                           uint8_t *msg,
                           uint16_t msg_length,
                           uint16_t chunk_len,
                           uint8_t *key_chacha,
                           uint8_t *keyr_in,
                           uint8_t *keys_in,
                           uint8_t *nonce,
                           uint8_t *header_input,
                           uint16_t header_length,
                           uint8_t chachapoly_flags,
                           uint8_t *output)
{

  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  rsi_pkt_t *pkt;
  rsi_chachapoly_req_t *chachapoly;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  //!check for valid mode
  if (chachapoly_mode > 3) {
    return RSI_ERROR_INVALID_PARAM;
  }

  //! check for proper key
  if (chachapoly_mode == CHACHAPOLY_MODE_POLY1305_KEYR_KEYS) {
    if ((keyr_in == NULL) || (keys_in == NULL))
      return RSI_ERROR_INVALID_PARAM;
  } else if (chachapoly_mode == CHACHA20POLY1305 || chachapoly_mode == CHACHA20) {
    if (key_chacha == NULL || nonce == NULL)
      return RSI_ERROR_INVALID_PARAM;
  } else {
    if ((keyr_in == NULL) || (keys_in == NULL))
      return RSI_ERROR_INVALID_PARAM;
  }

  if (rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE) == RSI_SUCCESS) {

    //! allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    //! If allocation of packet fails
    if (pkt == NULL) {
      //!Changing the common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (output != NULL) {
      //! attach the buffer given by user
      rsi_common_cb->app_buffer = output;

      //! length of the buffer provided by user
      rsi_common_cb->app_buffer_length = msg_length + TAG_SIZE;
    }

    //! Get Data Pointer
    chachapoly = (rsi_chachapoly_req_t *)pkt->data;

    //! Memset before filling
    memset(chachapoly, 0, sizeof(rsi_chachapoly_req_t));

    //! Fill Algorithm type CHACHAPOLY - 17
    chachapoly->algorithm_type = CHACHAPOLY;

    //! Fill Algorithm subtype
    chachapoly->algorithm_sub_type = chachapoly_mode;

    //! Fill enc_dec: 0 – For chachapoly Encryption 1 – For chachapoly Decryption
    chachapoly->encrypt_decryption = enc_dec;

    //! Fill the DMA
    chachapoly->dma_use = dma_use;

    //! Fill chachapoly_flags BIT(0) - 1st chunk BIT(1)-Middle chunk BIT(2) - Last chunk
    chachapoly->chachapoly_flags = chachapoly_flags;

    //! Fill msg length
    chachapoly->total_msg_length = msg_length;

    //! Fill chunk length
    chachapoly->current_chunk_length = chunk_len;

    //!Fill header length
    chachapoly->header_length = header_length;

    //!memset header before filling
    memset(&chachapoly->header_input[0], 0, header_length);

    //!copy the header input
    memcpy(&chachapoly->header_input[0], header_input, header_length);

    if (chachapoly_mode == CHACHA20POLY1305 || chachapoly_mode == CHACHA20
        || chachapoly_mode == CHACHAPOLY_MODE_POLY1305_KEYR_KEYS) {
      //!Keychacha
      //! Memset before filling
      memset(&chachapoly->key_chacha[0], 0, KEY_CHACHA_SIZE);

      //! Copy KEY chacha
      memcpy(&chachapoly->key_chacha[0], key_chacha, KEY_CHACHA_SIZE);

      //!Nonce
      //!Memset before filling
      memset(&chachapoly->nonce[0], 0, NONCE_SIZE);

      //! Copy the Nonce
      memcpy(&chachapoly->nonce[0], nonce, NONCE_SIZE);
    }

    if (chachapoly_mode == CHACHAPOLY_MODE_POLY1305_KEYR_KEYS || chachapoly_mode == POLY1305_MODE) {
      //!KEYR and KEYS
      //! Memset before filling
      memset(&chachapoly->keyr_in[0], 0, KEYR_SIZE);
      memset(&chachapoly->keys_in[0], 0, KEYS_SIZE);

      //! Copy keyr and keys
      memcpy(&chachapoly->keyr_in[0], keyr_in, KEYR_SIZE);
      memcpy(&chachapoly->keys_in[0], keys_in, KEYS_SIZE);
    }

    //!Data
    //! Memset before filling
    memset(&chachapoly->msg[0], 0, MAX_DATA_SIZE_BYTES_FOR_CHACHAPOLY);

    //! Copy Data
    memcpy(&chachapoly->msg[0], msg, chunk_len);

    //! Using host descriptor to set payload length
    send_size = sizeof(rsi_chachapoly_req_t) - MAX_DATA_SIZE_BYTES_FOR_CHACHAPOLY + chunk_len;

    //! get the host descriptor
    host_desc = (pkt->desc);

    //! Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    //! send chachapoly encrypt/decrypt request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    //! wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
  }

  else {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;
}
/*==============================================*/
/**
 * @fn         rsi_gcm(uint8_t enc_dec,uint16_t dma_use, uint8_t *msg, uint16_t msg_length, uint8_t *key, 
 * 				uint16_t key_length, uint8_t *iv,uint8_t iv_sz,uint8_t *header,uint16_t header_length, uint8_t *output,uint8_t tag_out)
 * @brief      Decides whether the GCM message can be sent once or requires multiple calls to send
 * @param[in]  enc_dec: 0 – For GCM Encryption 1 – For GCM Decryption 
 * @param[in]  dma_use: 0 - DMA disable   1: DMA Enable
 * @param[in]  msg: Pointer to message 
 * @param[in]  msg_length: Total message length
 * @param[in]  key: Pointer to GCM key
 * @param[in]  key_length: GCM key length in bits
 * @param[in]  iv: Pointer to GCM iv
 * @params[in] iv_sz:size of IV
 * @param[in]  header:pointer to header
 * @param[in]	header_length: Total length of header
 * @param[out]  output:  Output parameter to hold encrypted/decrypted from GCM
 *  * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function decides whether the GCM message can be sent once or requires multiple calls to send the message
 *
 */
int32_t rsi_gcm(uint8_t enc_dec,
                uint16_t dma_use,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint16_t iv_sz,
                uint8_t *header,
                uint16_t header_length,
                uint8_t *output)
{

  uint16_t total_len = 0;
  uint16_t chunk_len = 0;
  uint16_t offset    = 0;
  uint8_t gcm_flags  = 0;
  int32_t status     = RSI_SUCCESS;

  total_len = msg_length;

  //! Input pointer check
  if (msg == NULL || header == NULL || iv == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  while (total_len) {
    //! check total length
    if (total_len > MAX_DATA_SIZE_BYTES) {
      chunk_len = MAX_DATA_SIZE_BYTES;
      if (offset == 0) {
        //! Make gcm_flags as first chunk
        gcm_flags |= FIRST_CHUNK;
      } else {
        //! Make gcm_flags as Last chunk
        gcm_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len = total_len;
      gcm_flags = LAST_CHUNK;
      if (offset == 0) {
        /* if the total length is less than 1400 and offset is zero 
					 then make gcm_flags as both first chunk as well as last chunk*/
        gcm_flags |= FIRST_CHUNK;
      }
    }

    //! send the current chunk length message
    status = rsi_gcm_pen(enc_dec,
                         dma_use,
                         msg,
                         msg_length,
                         chunk_len,
                         key,
                         key_length,
                         iv,
                         iv_sz,
                         header,
                         header_length,
                         gcm_flags,
                         output);

    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Increment the offset value
    offset += chunk_len;
    msg += chunk_len;

    //! Decrement the total message lenth
    total_len -= chunk_len;
  }

  return status;
}
/*==============================================*/
/**
 * @fn         rsi_gcm_pen(uint8_t enc_dec,uint16_t dma_use, uint8_t *msg, uint16_t msg_length, uint16_t chunk_length, uint8_t *key, uint16_t key_length,
 * 											uint8_t *iv,uint16_t iv_sz, uint8_t *header,uint16_t header_length, uint8_t gcm_flags, uint8_t *output, uint8_t *tag_out)
 * @brief      Encrypt/Decrypt the data using GCM
 * @param[in]  enc_dec: 0 – For GCM Encryption 1 – For GCM Decryption 
 * @param[in]  dma_use: 0 - DMA disable   1 - DMA Enable
 * @param[in]  msg: Pointer to message 
 * @param[in]  msg_length: Total message length
 * @param[in]  chunk_length: current chunk length
 * @param[in]  key: Pointer to GCM key
 * @param[in]  key_length: GCM key length in bits
 * @param[in]  iv: Pointer to GCM iv
 * @params[in] iv_sz: size of IV
 * @param[in]  header:pointer to header
 * @param[in]  header_length: Total length of header
 * @param[in]  gcm_flags: BIT(0) - 1st chunk BIT(1) - Middle chunk BIT(2) - Last chunk 
 * @param[out]  output:  Output parameter to hold encrypted/decrypted from GCM
 *  * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function encrypt/decrypt the data using GCM
 *
 */
int32_t rsi_gcm_pen(uint8_t enc_dec,
                    uint16_t dma_use,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    uint8_t *key,
                    uint16_t key_length,
                    uint8_t *iv,
                    uint16_t iv_sz,
                    uint8_t *header,
                    uint16_t header_length,
                    uint8_t gcm_flags,
                    uint8_t *output)
{

  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  rsi_pkt_t *pkt;
  rsi_gcm_req_t *gcm;

  //! Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  //! Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  //! Check IV size for gcm
  //!Return error if IV size if not 12 bytes/96 bits

  if (iv_sz != GCM_IV_SIZE_IN_BYTES) {
    return RSI_ERROR_INVALID_PARAM;
  }

  if (rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE) == RSI_SUCCESS) {

    //! allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    //! If allocation of packet fails
    if (pkt == NULL) {
      //!Changing the common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      //! return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (output != NULL) {
      //! attach the buffer given by user
      rsi_common_cb->app_buffer = output;

      //! length of the buffer provided by user
      rsi_common_cb->app_buffer_length = msg_length + TAG_SIZE;
    }

    //! Get Data Pointer
    gcm = (rsi_gcm_req_t *)pkt->data;

    //! Memset before filling
    memset(gcm, 0, sizeof(rsi_gcm_req_t));

    //! Fill Algorithm type GCM - 18
    gcm->algorithm_type = GCM;

    //! Fill enc_dec: 0 – For GCM Encryption 1 – For GCM Decryption
    gcm->encrypt_decryption = enc_dec;

    //! Fill the DMA
    gcm->dma_use = dma_use;

    //! Fill aes_flags BIT(0) - 1st chunk BIT(1)- Middle chunk BIT(2) - Last chunk
    gcm->gcm_flags = gcm_flags;

    //! Fill msg length
    gcm->total_msg_length = msg_length;

    //! Fill chunk length
    gcm->current_chunk_length = chunk_length;

    //! Fill key length
    gcm->key_length = key_length;

    //!KEY
    //! Memset before filling
    memset(&gcm->key[0], 0, key_length);

    //! Copy KEY
    memcpy(&gcm->key[0], key, key_length);

    //!IV
    //! Memset before filling
    memset(&gcm->IV[0], 0, GCM_IV_SIZE_IN_BYTES);

    //! Copy IV
    memcpy(&gcm->IV[0], iv, GCM_IV_SIZE_IN_BYTES);

    //!Fill the header length
    gcm->header_length = header_length;

    //!Header
    //!Memset before filling
    memset(&gcm->header[0], 0, header_length);

    //!Copy header
    memcpy(&gcm->header[0], header, header_length);

    //!Data
    //! Memset before filling
    memset(&gcm->msg[0], 0, MAX_DATA_SIZE_BYTES);

    //! Copy Data
    memcpy(&gcm->msg[0], msg, chunk_length);

    //! Using host descriptor to set payload length
    send_size = sizeof(rsi_gcm_req_t) - MAX_DATA_SIZE_BYTES + chunk_length;

    //! get the host descriptor
    host_desc = (pkt->desc);

    //! Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

    //! send aes encrypt/decrypt request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    //! wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    //!Changing the common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    //!return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  //! get common command response stattus
  status = rsi_common_get_status();

  //! Return the status
  return status;
}

#endif

#endif
