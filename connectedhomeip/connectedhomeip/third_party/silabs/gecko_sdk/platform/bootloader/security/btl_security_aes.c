/***************************************************************************//**
 * @file
 * @brief AES decryption functionality for Silicon Labs bootloader
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "btl_security_aes.h"
#include "btl_security_types.h"

#include <string.h> // For memory copy functions

// Debug
#include "debug/btl_debug.h"
#include <stdio.h>

// Initialize AES context variable
void btl_initAesContext(void *ctx)
{
#if defined(_CMU_CLKEN1_MASK) && defined(CRYPTOACC_PRESENT)
  CMU->CLKEN1_SET = CMU_CLKEN1_CRYPTOACC;
  CMU->CRYPTOACCCLKCTRL_SET = CMU_CRYPTOACCCLKCTRL_AESEN;
#elif defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_SEMAILBOXHOST;
#endif

  AesContext_t *context = (AesContext_t *)ctx;
  mbedtls_aes_init(&(context->aesContext));
}

// Set initial key
void btl_setAesKey(void          *ctx,
                   const uint8_t *key,
                   unsigned int  keySize,
                   bool          encryptNotDecrypt)
{
  AesContext_t *context = (AesContext_t *)ctx;
  if (encryptNotDecrypt) {
    mbedtls_aes_setkey_enc(&(context->aesContext), key, keySize);
  } else {
    mbedtls_aes_setkey_dec(&(context->aesContext), key, keySize);
  }
}

// Process AES block. Block size is 16 bytes == 128 bit.
void btl_processAesBlock(void    *ctx,
                         uint8_t *inputBlock,
                         uint8_t *outputBlock,
                         bool    encryptNotDecrypt)
{
  AesContext_t *context = (AesContext_t *)ctx;
  if (encryptNotDecrypt) {
    mbedtls_aes_crypt_ecb(&(context->aesContext),
                          MBEDTLS_AES_ENCRYPT,
                          inputBlock,
                          outputBlock);
  } else {
    mbedtls_aes_crypt_ecb(&(context->aesContext),
                          MBEDTLS_AES_DECRYPT,
                          inputBlock,
                          outputBlock);
  }
}

// Initialize AES-CTR algorithm in CCM mode as used by GBL
void btl_initAesCcm(void          *ctx,
                    uint8_t       flags,
                    uint8_t       *nonce,
                    uint32_t      counter,
                    const uint8_t *key,
                    unsigned int  keySize)
{
#if defined(_CMU_CLKEN1_MASK) && defined(CRYPTOACC_PRESENT)
  CMU->CLKEN1_SET = CMU_CLKEN1_CRYPTOACC;
  CMU->CRYPTOACCCLKCTRL_SET = CMU_CRYPTOACCCLKCTRL_AESEN;
#elif defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_SEMAILBOXHOST;
#endif

  AesCtrContext_t *context = (AesCtrContext_t *)ctx;

#if defined(SEMAILBOX_PRESENT) && defined(BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE) \
  && (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE == 1)
  (void)key;
  context->aesKeyDesc.type = SL_SE_KEY_TYPE_SYMMETRIC;
  context->aesKeyDesc.size = keySize / 8UL; // keySize in bytes
  context->aesKeyDesc.flags = SL_SE_KEY_FLAG_NON_EXPORTABLE; // Non exportable key
  context->aesKeyDesc.storage.method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE;
  context->aesKeyDesc.storage.location.slot = SL_SE_KEY_SLOT_APPLICATION_AES_128_KEY;
#else
  // Store the key
  mbedtls_aes_init(&(context->aesContext));
  mbedtls_aes_setkey_enc(&(context->aesContext), key, keySize);
#endif
  // Indicate start of stream by setting offset to 0
  context->offsetInBlock = 0;

  // CCM uses counter mode with the following concatenated:
  //  * flags   (1  byte)
  //  * nonce   (12 bytes)
  //  * counter (3  bytes)
  context->counter[0] = flags;
  memcpy(&(context->counter[1]), nonce, 12);
  context->counter[13] = (counter & 0x00FF0000) >> 16;
  context->counter[14] = (counter & 0x0000FF00) >> 8;
  context->counter[15] = (counter & 0x000000FF);
}

// Process AES-CTR data.
void btl_processAesCtrData(void          *ctx,
                           const uint8_t *input,
                           uint8_t       *output,
                           size_t        length)
{
  AesCtrContext_t *context = (AesCtrContext_t *)ctx;
#if defined(SEMAILBOX_PRESENT) && defined(BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE) \
  && (BOOTLOADER_USE_SYMMETRIC_KEY_FROM_SE_STORAGE == 1)
  sl_se_command_context_t cmd_ctx;
  sl_se_init_command_context(&cmd_ctx);
  sl_se_aes_crypt_ctr(&cmd_ctx,
                      &(context->aesKeyDesc),
                      length,
                      (uint32_t *)&(context->offsetInBlock),
                      context->counter,
                      context->streamBlock,
                      input,
                      output);
#else
  mbedtls_aes_crypt_ctr(&(context->aesContext),
                        length,
                        &(context->offsetInBlock),
                        context->counter,
                        context->streamBlock,
                        input,
                        output);
#endif
}
