/***************************************************************************//**
 * @file
 * @brief Silicon Labs PSA Crypto Transparent Driver Mac functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "em_device.h"

#if defined(CRYPTO_PRESENT)

#include "sli_crypto_transparent_functions.h"
#include "sli_psa_driver_common.h"
#include "crypto_management.h"
#include "psa/crypto.h"
#include "em_crypto.h"
#include "em_core.h"
#include <string.h>

#if defined(PSA_WANT_ALG_CMAC)

// Magic value for CMAC calculation.
// const_rb should be 0x87 according to the spec, but the implementation
// requires a bit- and byteswapped constant.
static const uint32_t cmac_const_rb[4] = { 0x00000000u, 0x00000000u,
                                           0x00000000u, 0xe1000000u };

static psa_status_t sli_crypto_cmac_compute(const uint8_t *input,
                                            uint32_t input_length,
                                            const uint8_t *key,
                                            const uint32_t key_length,
                                            uint8_t *iv,
                                            uint8_t *mac,
                                            uint16_t mac_length,
                                            const bool generate);

static psa_status_t sli_crypto_cmac_finalize(CRYPTO_TypeDef *crypto,
                                             bool generate,
                                             uint8_t *mac,
                                             uint16_t mac_length);

static psa_status_t sli_crypto_aes_crypt_cbc(const uint8_t *key_buffer,
                                             size_t key_buffer_size,
                                             psa_encrypt_or_decrypt_t mode,
                                             size_t length,
                                             uint8_t iv[16],
                                             const uint8_t *input,
                                             uint8_t *output);

#endif // PSA_WANT_ALG_CMAC

psa_status_t sli_crypto_transparent_mac_compute(const psa_key_attributes_t *attributes,
                                                const uint8_t *key_buffer,
                                                size_t key_buffer_size,
                                                psa_algorithm_t alg,
                                                const uint8_t *input,
                                                size_t input_length,
                                                uint8_t *mac,
                                                size_t mac_size,
                                                size_t *mac_length)
{
#if defined(PSA_WANT_ALG_CMAC)

  if (attributes == NULL
      || key_buffer == NULL
      || input == NULL
      || mac == NULL
      || mac_size == 0
      || mac_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Take in the key
  if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  size_t key_len = psa_get_key_bits(attributes) / 8;
  switch (key_len) {
    case 16:
    case 32:
      if (key_buffer_size != key_len) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      break;
    case 24:
      return PSA_ERROR_NOT_SUPPORTED;
    default:
      return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status;
  uint16_t digest_length;
  uint8_t tmp_iv[16];

  switch (PSA_ALG_FULL_LENGTH_MAC(alg)) {
    case PSA_ALG_CMAC:
      digest_length = PSA_MAC_TRUNCATED_LENGTH(alg);
      if (digest_length == 0) {
        digest_length = PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES);
      }

      if (mac_size < digest_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }

      // IV should always be zero when starting a new computation.
      memset(tmp_iv, 0, 16);
      status = sli_crypto_cmac_compute(input,
                                       input_length,
                                       key_buffer,
                                       key_len,
                                       tmp_iv,
                                       mac,
                                       digest_length,
                                       true);
      *mac_length = digest_length;
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  return status;

#else // PSA_WANT_ALG_CMAC

  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;
  (void)input;
  (void)input_length;
  (void)mac;
  (void)mac_size;
  (void)mac_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CMAC
}

psa_status_t sli_crypto_transparent_mac_sign_setup(sli_crypto_transparent_mac_operation_t *operation,
                                                   const psa_key_attributes_t *attributes,
                                                   const uint8_t *key_buffer,
                                                   size_t key_buffer_size,
                                                   psa_algorithm_t alg)
{
#if defined(PSA_WANT_ALG_CMAC)

  if (operation == NULL
      || attributes == NULL
      || (key_buffer == NULL && key_buffer_size > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // Start by resetting the context
  memset(operation, 0, sizeof(*operation));

  // Take in the key
  if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
    return PSA_ERROR_NOT_SUPPORTED;
  }

  if (!PSA_ALG_IS_MAC(alg)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (PSA_ALG_FULL_LENGTH_MAC(alg)) {
    case PSA_ALG_CMAC:
      operation->cipher_mac.alg = alg;
      break;
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

  operation->cipher_mac.key_len = psa_get_key_bits(attributes) / 8;
  if (operation->cipher_mac.key_len > key_buffer_size) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  switch (operation->cipher_mac.key_len) {
    case 16:
    case 32:
      if (key_buffer_size != operation->cipher_mac.key_len) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }
      memcpy(operation->cipher_mac.key, key_buffer, operation->cipher_mac.key_len);
      break;
    case 24:
      return PSA_ERROR_NOT_SUPPORTED;
    default:
      return PSA_ERROR_INVALID_ARGUMENT;
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_CMAC

  (void)operation;
  (void)attributes;
  (void)key_buffer;
  (void)key_buffer_size;
  (void)alg;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CMAC
}

psa_status_t sli_crypto_transparent_mac_verify_setup(sli_crypto_transparent_mac_operation_t *operation,
                                                     const psa_key_attributes_t *attributes,
                                                     const uint8_t *key_buffer,
                                                     size_t key_buffer_size,
                                                     psa_algorithm_t alg)
{
  // Since the PSA Crypto core exposes the verify functionality of the drivers without
  // actually implementing the fallback to 'sign' when the driver doesn't support verify,
  // we need to do this ourselves for the time being.
  return sli_crypto_transparent_mac_sign_setup(operation,
                                               attributes,
                                               key_buffer,
                                               key_buffer_size,
                                               alg);
}

psa_status_t sli_crypto_transparent_mac_update(sli_crypto_transparent_mac_operation_t *operation,
                                               const uint8_t *input,
                                               size_t input_length)
{
#if defined(PSA_WANT_ALG_CMAC)

  if (operation == NULL
      || (input == NULL && input_length > 0)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status;

  switch (PSA_ALG_FULL_LENGTH_MAC(operation->cipher_mac.alg)) {
    case PSA_ALG_CMAC: {
      // The streaming update operation of CMAC is like doing CBC-MAC but
      // always keeping the last block as a plaintext.
      if (input_length == 0) {
        return PSA_SUCCESS;
      }

      // Add the beginning of the input data to last unprocessed block and hash
      // it if the input is big enough to cause a new last block to be made.
      if ((operation->cipher_mac.unprocessed_len > 0)
          && (input_length > 16 - operation->cipher_mac.unprocessed_len)) {
        memcpy(&operation->cipher_mac.unprocessed_block[operation->cipher_mac.unprocessed_len],
               input,
               16 - operation->cipher_mac.unprocessed_len);

        status = sli_crypto_aes_crypt_cbc(operation->cipher_mac.key,
                                          operation->cipher_mac.key_len,
                                          PSA_CRYPTO_DRIVER_ENCRYPT,
                                          16,
                                          operation->cipher_mac.iv,
                                          operation->cipher_mac.unprocessed_block,
                                          operation->cipher_mac.iv);

        if (status != PSA_SUCCESS) {
          return PSA_ERROR_HARDWARE_FAILURE;
        }

        input += 16 - operation->cipher_mac.unprocessed_len;
        input_length -= 16 - operation->cipher_mac.unprocessed_len;
        operation->cipher_mac.unprocessed_len = 0;
      }

      // Block count including last block
      size_t block_count = (input_length + 16 - 1) / 16;

      // Hash all full blocks except for the last one
      for (uint32_t i = 1; i < block_count; ++i) {
        status = sli_crypto_aes_crypt_cbc(operation->cipher_mac.key,
                                          operation->cipher_mac.key_len,
                                          PSA_CRYPTO_DRIVER_ENCRYPT,
                                          16,
                                          operation->cipher_mac.iv,
                                          input,
                                          operation->cipher_mac.iv);

        if (status != PSA_SUCCESS) {
          return PSA_ERROR_HARDWARE_FAILURE;
        }

        input_length -= 16;
        input += 16;
      }

      // Add remaining input data that wasn't aligned to a block
      if (input_length > 0) {
        memcpy(&operation->cipher_mac.unprocessed_block[operation->cipher_mac.unprocessed_len],
               input,
               input_length);
        operation->cipher_mac.unprocessed_len += input_length;
      }
      break;
    }
    default:
      return PSA_ERROR_BAD_STATE;
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_CMAC

  (void)operation;
  (void)input;
  (void)input_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CMAC
}

psa_status_t sli_crypto_transparent_mac_sign_finish(sli_crypto_transparent_mac_operation_t *operation,
                                                    uint8_t *mac,
                                                    size_t mac_size,
                                                    size_t *mac_length)
{
#if defined(PSA_WANT_ALG_CMAC)

  if (operation == NULL
      || mac == NULL
      || mac_size == 0
      || mac_length == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  psa_status_t status;
  uint16_t digest_length;

  switch (PSA_ALG_FULL_LENGTH_MAC(operation->cipher_mac.alg)) {
    case PSA_ALG_CMAC:
      digest_length = PSA_MAC_TRUNCATED_LENGTH(operation->cipher_mac.alg);
      if (digest_length == 0) {
        digest_length = PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES);
      }

      if (mac_size < digest_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
      }

      // Calling the cmac compute function with the input set to the last
      // block and the IV set to the current hash is equal to running the
      // finalize operation.
      status = sli_crypto_cmac_compute(operation->cipher_mac.unprocessed_block,
                                       operation->cipher_mac.unprocessed_len,
                                       operation->cipher_mac.key,
                                       operation->cipher_mac.key_len,
                                       operation->cipher_mac.iv,
                                       mac,
                                       digest_length,
                                       true);

      if (status != PSA_SUCCESS) {
        return PSA_ERROR_HARDWARE_FAILURE;
      }

      *mac_length = digest_length;
      break;
    default:
      return PSA_ERROR_BAD_STATE;
  }

  return PSA_SUCCESS;

#else // PSA_WANT_ALG_CMAC

  (void)operation;
  (void)mac;
  (void)mac_size;
  (void)mac_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CMAC
}

psa_status_t sli_crypto_transparent_mac_verify_finish(sli_crypto_transparent_mac_operation_t *operation,
                                                      const uint8_t *mac,
                                                      size_t mac_length)
{
#if defined(PSA_WANT_ALG_CMAC)

  if (operation == NULL
      || mac == NULL
      || mac_length == 0) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  uint16_t digest_length;

  switch (PSA_ALG_FULL_LENGTH_MAC(operation->cipher_mac.alg)) {
    case PSA_ALG_CMAC:
      digest_length = PSA_MAC_TRUNCATED_LENGTH(operation->cipher_mac.alg);
      if (digest_length == 0) {
        digest_length = PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES);
      }

      if (mac_length != digest_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
      }

      // Calling the cmac compute function with the input set to the last
      // block and the IV set to the current hash is equal to running the
      // finalize operation.
      return sli_crypto_cmac_compute(operation->cipher_mac.unprocessed_block,
                                     operation->cipher_mac.unprocessed_len,
                                     operation->cipher_mac.key,
                                     operation->cipher_mac.key_len,
                                     operation->cipher_mac.iv,
                                     (uint8_t*)mac,
                                     mac_length,
                                     false);
    default:
      return PSA_ERROR_BAD_STATE;
  }

#else // PSA_WANT_ALG_CMAC

  (void)operation;
  (void)mac;
  (void)mac_length;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CMAC
}

psa_status_t sli_crypto_transparent_mac_abort(sli_crypto_transparent_mac_operation_t *operation)
{
#if defined(PSA_WANT_ALG_CMAC)

  if (operation == NULL) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  // There's no state in hardware that we need to preserve, so zeroing out the context suffices.
  memset(operation, 0, sizeof(*operation));
  return PSA_SUCCESS;

#else // PSA_WANT_ALG_CMAC

  (void)operation;

  return PSA_ERROR_NOT_SUPPORTED;

#endif // PSA_WANT_ALG_CMAC
}

#if defined(PSA_WANT_ALG_CMAC)

/**
 * @brief
 *  Function is an implementation of CMAC-AES (128 or 256 bit key)
 *
 * @param[in] input
 *  Pointer to data (message)
 *
 * @param[in] input_length
 *  length of input buffer in bytes
 *
 * @param[in] key
 *  Pointer to key buffer for the AES algorithm.
 *  Has to be 16 or 32 bytes long, for AES-128 and AES-256 respectively.
 *
 * @param[in] key_length
 *   The length of the key in bytes. Either 16 (AES-128) or 32 (AES-256).
 *
 * @param[in] iv
 *  128-bit initialization vector used in the CBC part of CMAC. Should be zero
 *  unless the call is used as a finalize operation of an unfinished digest, in
 *  which case it should be set to the current intermediate digest.
 *
 * @param[in/out] mac
 *  16-byte (maximum) MAC buffer. If generating, the MAC will be stored there.
 *  If verifying, the calculated MAC will be compared to the one stored in
 *  this buffer.
 *
 * @param[in] mac_length
 *  Requested length of the message digest in bytes. Can be at most a full-
 *  length output, i.e. 16 bytes.
 *
 * @param[in] generate
 *  true - Generate MAC
 *  false - Verify MAC
 *
 * @return
 *   PSA_SUCCESS if success. Error code if failure.
 *   Encryption will always succeed.
 *   Decryption may fail if the authentication fails.
 */
static psa_status_t sli_crypto_cmac_compute(const uint8_t *input,
                                            uint32_t input_length,
                                            const uint8_t *key,
                                            const uint32_t key_length,
                                            uint8_t *iv,
                                            uint8_t *mac,
                                            uint16_t mac_length,
                                            const bool generate)

{
  CORE_DECLARE_IRQ_STATE;
  uint32_t i;
  uint32_t subKey[4];
  uint32_t lastBlock[4];
  uint8_t *lastBlockBytePtr = (uint8_t *)lastBlock;
  uint8_t bytesToPad;
  psa_status_t status;
  size_t full_blocks_min_one =
    (input_length > 0 ? (input_length - 1) / 16 : 0);
  CRYPTO_TypeDef *crypto = crypto_management_acquire();

  /* Check input arguments */
  if (mac_length > 16
      || mac_length == 0
      || key == NULL
      || (key_length != 16U && key_length != 32U)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  CORE_ENTER_CRITICAL();

  /* Reset CRYPTO registers */
  crypto->CTRL = 0;
  crypto->SEQCTRL  = 0;
  crypto->SEQCTRLB = 0;

  /* Zero out DDATA0 and DDATA2 */
  CRYPTO_EXECUTE_2(crypto,
                   CRYPTO_CMD_INSTR_CLR,
                   CRYPTO_CMD_INSTR_DDATA0TODDATA2);

  /* Calculate subkeys */
  /* magic value in DATA1 */
  CRYPTO_DataWrite(&crypto->DATA1, cmac_const_rb);

  /* Key in KeyBuf */
  CRYPTO_KeyBufWriteUnaligned(crypto,
                              key,
                              (key_length == 16U) ? cryptoKey128Bits
                              : cryptoKey256Bits);

  CRYPTO_EXECUTE_17(crypto,
                    CRYPTO_CMD_INSTR_SELDATA0DATA1,
                    CRYPTO_CMD_INSTR_AESENC,
                    CRYPTO_CMD_INSTR_BBSWAP128,
                    CRYPTO_CMD_INSTR_SELDDATA0DATA1,
                    CRYPTO_CMD_INSTR_SHR,
                    CRYPTO_CMD_INSTR_EXECIFCARRY,
                    CRYPTO_CMD_INSTR_XOR,
                    CRYPTO_CMD_INSTR_EXECALWAYS,
                    CRYPTO_CMD_INSTR_BBSWAP128,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA3, // subkey1 in DATA2
                    CRYPTO_CMD_INSTR_BBSWAP128,
                    CRYPTO_CMD_INSTR_SHR,
                    CRYPTO_CMD_INSTR_EXECIFCARRY,
                    CRYPTO_CMD_INSTR_XOR,
                    CRYPTO_CMD_INSTR_EXECALWAYS,
                    CRYPTO_CMD_INSTR_BBSWAP128,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA2 // subkey2 in DATA0
                    );
  CRYPTO_InstructionSequenceWait(crypto);

  /* Prepare input message for algorithm */
  bytesToPad = 16 - (input_length % 16);

  /* Determine which subKey we're going to use */
  if (bytesToPad != 16 || input_length == 0) {
    /* Input is treated as last block being incomplete */
    /* So store SubKey 2 */
    CRYPTO_DataRead(&crypto->DATA0, subKey);
  } else {
    /* Input is treated as block aligned, so store SubKey 1 */
    CRYPTO_DataRead(&crypto->DATA2, subKey);
  }

  /* We can take a break from using CRYPTO here */
  CORE_EXIT_CRITICAL();

  /* Copy the last block of data into our local copy because we need
     to change it */
  memset(lastBlock, 0, sizeof(lastBlock));
  memcpy(lastBlock,
         &input[full_blocks_min_one * 16],
         (input_length - (full_blocks_min_one * 16)));

  if (bytesToPad != 16) {
    /* Input message needs to be padded */
    lastBlockBytePtr[16 - bytesToPad] = 0x80;
    if (bytesToPad > 1) {
      memset(&lastBlockBytePtr[16 - bytesToPad + 1], 0x00, bytesToPad - 1);
    }
  } else if (input_length == 0) {
    /* No data: set the 'last block' to immediate termination */
    memset(lastBlock, 0, sizeof(lastBlock));
    lastBlockBytePtr[0] = 0x80;
  } else {
    /* Input message was block-aligned, so no padding required */
  }
  /* Store the XOR-ed version of the last block separate from the message */
  /* to avoid contamination of the input data */
  for (i = 0; i < 4; i++) {
    lastBlock[i] ^= subKey[i];
  }

  /* Calculate hash */
  CORE_ENTER_CRITICAL();

  crypto->SEQCTRL |= 16;

  CRYPTO_KeyBufWriteUnaligned(crypto,
                              key,
                              (key_length == 16U) ? cryptoKey128Bits
                              : cryptoKey256Bits);

  CRYPTO_DataWriteUnaligned(&crypto->DATA0, iv);

  /* Push all blocks except the last one */
  for (i = 0; i < full_blocks_min_one; i++) {
    CRYPTO_DataWriteUnaligned(&crypto->DATA0XOR, &input[i * 16]);
    CRYPTO_EXECUTE_1(crypto, CRYPTO_CMD_INSTR_AESENC);
  }
  /* Don't forget to push the last block as well! */
  CRYPTO_DataWrite(&crypto->DATA0XOR, lastBlock);
  CRYPTO_EXECUTE_1(crypto, CRYPTO_CMD_INSTR_AESENC);
  CRYPTO_InstructionSequenceWait(crypto);

  status = sli_crypto_cmac_finalize(crypto, generate, mac, mac_length);

  CORE_EXIT_CRITICAL();

  crypto_management_release(crypto);

  return status;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Perform final CMAC processing.
 *
 * @param[in] generate
 *   True if MAC generation was requested.
 *   False if MAC verification was requested.
 *
 * @param[in/out] mac
 *   Pointer to location where the calculated CMAC MAC should be stored when
 *   encrypting, or where the to-be-verified MAC is located for verification.
 *
 * @param[in] mac_length
 *   Output length of the calculated MAC (in bytes).
 ******************************************************************************/
static psa_status_t sli_crypto_cmac_finalize(CRYPTO_TypeDef *crypto,
                                             bool generate,
                                             uint8_t *mac,
                                             uint16_t mac_length)
{
  uint32_t full_mac[4];
  psa_status_t status;

  if (mac_length > sizeof(full_mac)) {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  /* Read calculated MAC from CRYPTO. */
  CRYPTO_DataRead(&crypto->DATA0, full_mac);

  if (generate) {
    /* copy requested output to passed buffer */
    memcpy(mac, full_mac, mac_length);
    status = PSA_SUCCESS;
  } else {
    /* Compare the MAC with the reference value in constant time */
    status = sli_psa_safer_memcmp((const uint8_t*)mac, (const uint8_t*)full_mac, mac_length) == 0 ? PSA_SUCCESS : PSA_ERROR_INVALID_SIGNATURE;
  }

  memset(full_mac, 0, sizeof(full_mac));
  return status;
}

/*
 * AES-CBC buffer encryption/decryption
 *
 * NOTE: The input and output buffers must not overlap.
 */
static psa_status_t sli_crypto_aes_crypt_cbc(const uint8_t *key_buffer,
                                             size_t key_buffer_size,
                                             psa_encrypt_or_decrypt_t mode,
                                             size_t length,
                                             uint8_t iv[16],
                                             const uint8_t *input,
                                             uint8_t *output)
{
  CORE_DECLARE_IRQ_STATE;
  size_t processed = 0;

  CRYPTO_TypeDef *device = crypto_management_acquire();
  device->WAC = 0;
  device->CTRL = 0;

  // Set the encryption/decryption key and input data
  CORE_ENTER_CRITICAL();
  CRYPTO_KeyBufWriteUnaligned(device,
                              key_buffer,
                              (key_buffer_size == 16U) ? cryptoKey128Bits
                              : cryptoKey256Bits);
  if (mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
    CRYPTO_DataWriteUnaligned(&device->DATA0, (uint8_t *)iv);
  } else {
    CRYPTO_DataWriteUnaligned(&device->DATA2, (uint8_t *)iv);
  }
  CORE_EXIT_CRITICAL();

  while (processed < length) {
    if (mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
      CORE_ENTER_CRITICAL();
      CRYPTO_DataWriteUnaligned(&device->DATA0XOR, (uint8_t *)(&input[processed]));

      // Initiate encryption and wait for it to complete
      device->CMD = CRYPTO_CMD_INSTR_AESENC;
      while ((device->STATUS & CRYPTO_STATUS_INSTRRUNNING) != 0) ;

      CRYPTO_DataReadUnaligned(&device->DATA0, output);
      CORE_EXIT_CRITICAL();
    } else {
      // Decrypt input block, XOR IV to decrypted text, set ciphertext as next IV
      CORE_ENTER_CRITICAL();
      CRYPTO_DataWriteUnaligned(&device->DATA0, (uint8_t *)(&input[processed]));

      // Initiate decryption sequence and wait for it to complete
      CRYPTO_EXECUTE_4(device,
                       CRYPTO_CMD_INSTR_DATA0TODATA1,
                       CRYPTO_CMD_INSTR_AESDEC,
                       CRYPTO_CMD_INSTR_DATA2TODATA0XOR,
                       CRYPTO_CMD_INSTR_DATA1TODATA2);

      while ((device->STATUS & CRYPTO_STATUS_SEQRUNNING) != 0) ;

      CRYPTO_DataReadUnaligned(&device->DATA0, &output[processed]);
      CORE_EXIT_CRITICAL();
    }
    processed += 16;
  }

  if (processed >= 16) {
    // Update the supplied IV
    if (mode == PSA_CRYPTO_DRIVER_ENCRYPT) {
      memcpy(iv, &output[processed - 16], 16);
    } else {
      CORE_ENTER_CRITICAL();
      CRYPTO_DataReadUnaligned(&device->DATA2, iv);
      CORE_EXIT_CRITICAL();
    }
  }

  crypto_management_release(device);

  return PSA_SUCCESS;
}

#endif // PSA_WANT_ALG_CMAC

#endif // defined(CRYPTO_PRESENT)
