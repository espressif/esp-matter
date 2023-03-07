/***************************************************************************//**
 * @file
 * @brief AES-GCM abstraction via Silicon Labs SE
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

/**
 * This file includes alternative plugin implementations of various
 * functions in gcm.c using the SE accelerator incorporated
 * in Series-2 devices with Secure Engine from Silicon Laboratories.
 */

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#include "mbedtls/build_info.h"

#if defined(MBEDTLS_GCM_ALT) && defined(MBEDTLS_GCM_C)
#include "mbedtls/gcm.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "se_management.h"
#include <string.h>

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize(void *v, size_t n)
{
  if (n == 0) {
    return;
  }
  volatile unsigned char *p = v;
  while ( n-- ) *p++ = 0;
}

static void sx_math_u64_to_u8array(uint64_t in, uint8_t *out)
{
  uint32_t i = 0;
  for (i = 0; i < 8; i++) {
    out[7 - i] = (in >>  8 * i) & 0xFF;
  }
}

static int sli_validate_gcm_params(size_t tag_len,
                                   size_t iv_len,
                                   size_t add_len)
{
  // NOTE: tag lengths != 16 byte are only supported as of SE FW v1.2.0.
  //   Earlier firmware versions will return an error trying to verify non-16-byte
  //   tags using this function.
  if ( tag_len < 4 || tag_len > 16 || iv_len == 0 ) {
    return (MBEDTLS_ERR_GCM_BAD_INPUT);
  }

  /* AD are limited to 2^64 bits, so 2^61 bytes. Since the length of AAD is
   * limited by the mbedtls API to a size_t, length checking only needs to be
   * done on 64-bit platforms. */
#if SIZE_MAX > 0xFFFFFFFFUL
  if (add_len >> 61 != 0) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }
#else
  (void) add_len;
#endif /* 64-bit size_t */

  /* Library does not support non-12-byte IVs */
  if (iv_len != 12) {
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

  return 0;
}

/*
 * Initialize a context
 */
void mbedtls_gcm_init(mbedtls_gcm_context *ctx)
{
  if (ctx == NULL) {
    return;
  }

  memset(ctx, 0, sizeof(mbedtls_gcm_context) );
}

// Set key
int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx,
                       mbedtls_cipher_id_t cipher,
                       const unsigned char *key,
                       unsigned int keybits)
{
  if (ctx == NULL
      || key == NULL
      || cipher != MBEDTLS_CIPHER_ID_AES
      || (keybits != 128 && keybits != 192 && keybits != 256)) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }

  /* Store key in gcm context */
  ctx->keybits = keybits;
  memcpy(ctx->key, key, keybits / 8);

  return 0;
}

int mbedtls_gcm_starts(mbedtls_gcm_context *ctx,
                       int mode,
                       const unsigned char *iv,
                       size_t iv_len)
{
  int status;

  /* Check input parameters. */
  if (ctx == NULL
      || iv == NULL ) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }

  status = sli_validate_gcm_params(16, iv_len, 0);
  if (status) {
    return status;
  }

  /* Store input in context data structure. */
  ctx->mode = mode;
  ctx->len = 0;
  ctx->add_len = 0;
  ctx->last_op = false;
  ctx->iv_len = iv_len;
  memcpy(ctx->se_ctx_dec, iv, iv_len);
  memcpy(ctx->se_ctx_enc, iv, iv_len);

  return 0;
}

int mbedtls_gcm_update_ad(mbedtls_gcm_context *ctx,
                          const unsigned char *add,
                          size_t add_len)
{
  int status;
  SE_Response_t se_response;

  if (add_len > 0 && add == NULL) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }

  if (add_len == 0) {
    return 0;
  }

  // This implementation only supports adding the full AD in one shot
  if ( ctx->add_len > 0 ) {
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

  status = sli_validate_gcm_params(16, 12, add_len);
  if (status) {
    return status;
  }

  // Start with encryption
  // Need to do encryption twice: once to create the context, the other to pre-compute the tag in case there's no more data coming
  // (SE doesn't support a type of 'finalize' command. All operations with 'END' set need to contain some data.)
  SE_Command_t gcm_cmd_enc = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_ENCRYPT | SE_COMMAND_OPTION_CONTEXT_START);
  SE_Command_t gcm_cmd_enc_full = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_ENCRYPT | SE_COMMAND_OPTION_CONTEXT_WHOLE);

  SE_DataTransfer_t key_in = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
  SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_dec, ctx->iv_len);
  SE_DataTransfer_t aad_in = SE_DATATRANSFER_DEFAULT((void*)add, add_len);

  SE_DataTransfer_t key_in_full = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
  SE_DataTransfer_t iv_in_full = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_dec, ctx->iv_len);
  SE_DataTransfer_t aad_in_full = SE_DATATRANSFER_DEFAULT((void*)add, add_len);

  SE_DataTransfer_t ctx_out = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_enc, sizeof(ctx->se_ctx_enc));
  SE_DataTransfer_t tag_out = SE_DATATRANSFER_DEFAULT(ctx->tagbuf, sizeof(ctx->tagbuf));

  SE_addDataInput(&gcm_cmd_enc, &key_in);
  SE_addDataInput(&gcm_cmd_enc, &iv_in);
  SE_addDataInput(&gcm_cmd_enc, &aad_in);

  SE_addDataInput(&gcm_cmd_enc_full, &key_in_full);
  SE_addDataInput(&gcm_cmd_enc_full, &iv_in_full);
  SE_addDataInput(&gcm_cmd_enc_full, &aad_in_full);

  SE_addDataOutput(&gcm_cmd_enc, &ctx_out);
  SE_addDataOutput(&gcm_cmd_enc_full, &tag_out);

  SE_addParameter(&gcm_cmd_enc, ctx->keybits / 8);
  SE_addParameter(&gcm_cmd_enc, add_len);
  SE_addParameter(&gcm_cmd_enc, 0);

  SE_addParameter(&gcm_cmd_enc_full, ctx->keybits / 8);
  SE_addParameter(&gcm_cmd_enc_full, add_len);
  SE_addParameter(&gcm_cmd_enc_full, 0);

  status = se_management_acquire();
  if (status != 0) {
    return status;
  }
  /* Execute GCM operation */
  SE_executeCommand(&gcm_cmd_enc_full);
  se_response = SE_readCommandResponse();
  SE_executeCommand(&gcm_cmd_enc);
  se_response |= SE_readCommandResponse();

  se_management_release();

  // Continue with decryption if needed
  if (ctx->mode == MBEDTLS_GCM_DECRYPT) {
    SE_Command_t gcm_cmd_dec = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_DECRYPT | SE_COMMAND_OPTION_CONTEXT_START);

    SE_DataTransfer_t key_in_dec = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
    SE_DataTransfer_t iv_in_dec = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_dec, ctx->iv_len);
    SE_DataTransfer_t aad_in_dec = SE_DATATRANSFER_DEFAULT((void*)add, add_len);

    SE_DataTransfer_t ctx_out_dec = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_dec, sizeof(ctx->se_ctx_dec));

    SE_addDataInput(&gcm_cmd_dec, &key_in_dec);
    SE_addDataInput(&gcm_cmd_dec, &iv_in_dec);
    SE_addDataInput(&gcm_cmd_dec, &aad_in_dec);

    SE_addDataOutput(&gcm_cmd_dec, &ctx_out_dec);

    SE_addParameter(&gcm_cmd_dec, ctx->keybits / 8);
    SE_addParameter(&gcm_cmd_dec, add_len);
    SE_addParameter(&gcm_cmd_dec, 0);

    status = se_management_acquire();
    if (status != 0) {
      return status;
    }
    /* Execute GCM operation */
    SE_executeCommand(&gcm_cmd_dec);
    se_response = SE_readCommandResponse();
    se_management_release();
  }

  if (se_response == SE_RESPONSE_OK) {
    ctx->add_len = add_len;
    return 0;
  } else {
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }
}

int mbedtls_gcm_update(mbedtls_gcm_context *ctx,
                       const unsigned char *input, size_t input_length,
                       unsigned char *output, size_t output_size,
                       size_t *output_length)
{
  int status;
  SE_Response_t se_response;
  uint8_t lena_lenc[16];
  *output_length = 0;

  if (ctx == NULL
      || (input_length > 0 && input == NULL)
      || (input_length > 0 && output == NULL)) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }

  if (input_length > output_size) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }

  if (input_length == 0) {
    return 0;
  }

  /* Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
   * Also check for possible overflow */
  if ( ctx->len + input_length < ctx->len
       || (uint64_t) ctx->len + input_length > 0xFFFFFFFE0ull ) {
    return(MBEDTLS_ERR_GCM_BAD_INPUT);
  }

  if ( ctx->last_op == true ) {
    // We've already closed the input stream, no way back.
    return (MBEDTLS_ERR_GCM_BAD_INPUT);
  }

  // Approach:
  // * Encryption: encrypt regularly with context store. If input length is not a block multiple, close the operation and store the resulting tag in a tag buffer.
  // * Decryption: decrypt regularly with context store. For each decryption, re-encrypt the decrypted data with context store. If input length is not a block multiple, close both operations and store the tag from the re-encrypt in a tag buffer.
  // * In both cases, the (re-)encryption is done twice: once assuming there is more data to follow, and once assuming this is the final block.
  //   Explanation: SE doesn't support a type of 'finalize' command. All operations with 'END' set need to contain some data.

  // Figure out whether we'll be closing out
  bool first_op = (ctx->add_len == 0 && ctx->len == 0) ? true : false;

  if ( input_length % 16 != 0) {
    // Indicate that this is our last op
    ctx->last_op = true;
  }

  if (first_op && ctx->last_op) {
    // Need to store length in context for later.
    ctx->len = input_length;
    // optimisation: delegate to all-in-one handler
    status = mbedtls_gcm_crypt_and_tag(ctx, ctx->mode, input_length,
                                       ctx->se_ctx_dec, ctx->iv_len,
                                       NULL, 0,
                                       input, output,
                                       // Compute max tag size (16 bytes)
                                       16, ctx->tagbuf);
    if (status == 0) {
      *output_length = input_length;
    }
    return status;
  }

  sx_math_u64_to_u8array(ctx->add_len << 3, &lena_lenc[0]);
  sx_math_u64_to_u8array((ctx->len + input_length) << 3, &lena_lenc[8]);

  // Need to be sure we can get the SE before starting to change any context variables
  status = se_management_acquire();
  if (status != 0) {
    return status;
  }

  ctx->len += input_length;

  if (ctx->mode == MBEDTLS_GCM_DECRYPT) {
    // Run decryption first
    SE_Command_t gcm_cmd_dec = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_DECRYPT | (first_op ? SE_COMMAND_OPTION_CONTEXT_START : (ctx->last_op ? SE_COMMAND_OPTION_CONTEXT_END : SE_COMMAND_OPTION_CONTEXT_ADD)));

    SE_DataTransfer_t key_in_dec = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
    SE_DataTransfer_t iv_ctx_in_dec = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_dec, (first_op ? ctx->iv_len : sizeof(ctx->se_ctx_dec)));
    SE_DataTransfer_t data_in_dec = SE_DATATRANSFER_DEFAULT((void*)input, input_length);
    SE_DataTransfer_t lenalenc_in_dec = SE_DATATRANSFER_DEFAULT(lena_lenc, sizeof(lena_lenc));
    SE_DataTransfer_t data_out_dec = SE_DATATRANSFER_DEFAULT(output, input_length);
    SE_DataTransfer_t ctx_out_dec = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_dec, sizeof(ctx->se_ctx_dec));
    SE_DataTransfer_t mac_in_dec = SE_DATATRANSFER_DEFAULT(ctx->tagbuf, sizeof(ctx->tagbuf));

    SE_addDataInput(&gcm_cmd_dec, &key_in_dec);
    SE_addDataInput(&gcm_cmd_dec, &iv_ctx_in_dec);
    SE_addDataInput(&gcm_cmd_dec, &data_in_dec);

    SE_addDataOutput(&gcm_cmd_dec, &data_out_dec);
    if (!ctx->last_op) {
      SE_addDataOutput(&gcm_cmd_dec, &ctx_out_dec);
    } else {
      SE_addDataInput(&gcm_cmd_dec, &lenalenc_in_dec);
      SE_addDataInput(&gcm_cmd_dec, &mac_in_dec);
    }

    SE_addParameter(&gcm_cmd_dec, ctx->keybits / 8);
    SE_addParameter(&gcm_cmd_dec, 0);
    SE_addParameter(&gcm_cmd_dec, input_length);

    SE_executeCommand(&gcm_cmd_dec);
    se_response = SE_readCommandResponse();
    // Getting an 'invalid signature' error here is acceptable, since we're not trying to verify the tag
    if (se_response == SE_RESPONSE_INVALID_SIGNATURE) {
      se_response = SE_RESPONSE_OK;
    }
    if (se_response != SE_RESPONSE_OK) {
      goto exit;
    }
  }

  if (!ctx->last_op) {
    // we need to do the final calculation first, such that we keep the input context intact
    SE_Command_t gcm_cmd_enc_final = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_ENCRYPT | (first_op ? SE_COMMAND_OPTION_CONTEXT_WHOLE : SE_COMMAND_OPTION_CONTEXT_END));

    SE_DataTransfer_t key_in_enc_final = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
    SE_DataTransfer_t iv_ctx_in_enc_final = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_enc, (first_op ? ctx->iv_len : sizeof(ctx->se_ctx_enc)));
    SE_DataTransfer_t lenalenc_in_enc_final = SE_DATATRANSFER_DEFAULT(lena_lenc, sizeof(lena_lenc));
    SE_DataTransfer_t data_in_enc_final = SE_DATATRANSFER_DEFAULT(ctx->mode == MBEDTLS_GCM_ENCRYPT ? (void*)input : (void*)output, input_length);
    SE_DataTransfer_t data_out_enc_final = SE_DATATRANSFER_DEFAULT(NULL, input_length);
    data_out_enc_final.length |= SE_DATATRANSFER_DISCARD;
    SE_DataTransfer_t tag_out_final = SE_DATATRANSFER_DEFAULT(ctx->tagbuf, sizeof(ctx->tagbuf));

    SE_addDataInput(&gcm_cmd_enc_final, &key_in_enc_final);
    SE_addDataInput(&gcm_cmd_enc_final, &iv_ctx_in_enc_final);
    SE_addDataInput(&gcm_cmd_enc_final, &data_in_enc_final);

    if (!first_op) {
      SE_addDataInput(&gcm_cmd_enc_final, &lenalenc_in_enc_final);
    }

    SE_addDataOutput(&gcm_cmd_enc_final, &data_out_enc_final);
    SE_addDataOutput(&gcm_cmd_enc_final, &tag_out_final);

    SE_addParameter(&gcm_cmd_enc_final, ctx->keybits / 8);
    SE_addParameter(&gcm_cmd_enc_final, 0);
    SE_addParameter(&gcm_cmd_enc_final, input_length);

    SE_executeCommand(&gcm_cmd_enc_final);
    se_response = SE_readCommandResponse();
    if (se_response != SE_RESPONSE_OK) {
      goto exit;
    }
  }

  // Explicit scope block to help with stack usage optimisation
  // Re-encrypt the decrypted data to keep the ongoing calculation alive in case we can
  // continue calculation with another call to mbedtls_gcm_update.
  {
    SE_Command_t gcm_cmd_enc = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_ENCRYPT | (first_op ? SE_COMMAND_OPTION_CONTEXT_START : (ctx->last_op ? SE_COMMAND_OPTION_CONTEXT_END : SE_COMMAND_OPTION_CONTEXT_ADD)));
    SE_DataTransfer_t key_in_enc = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
    SE_DataTransfer_t iv_ctx_in_enc = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_enc, (first_op ? ctx->iv_len : sizeof(ctx->se_ctx_enc)));
    SE_DataTransfer_t lenalenc_in_enc = SE_DATATRANSFER_DEFAULT(lena_lenc, sizeof(lena_lenc));
    SE_DataTransfer_t data_in_enc = SE_DATATRANSFER_DEFAULT(ctx->mode == MBEDTLS_GCM_ENCRYPT ? (void*)input : (void*)output, input_length);

    SE_DataTransfer_t data_out_enc = SE_DATATRANSFER_DEFAULT(output, input_length);
    if (ctx->mode == MBEDTLS_GCM_DECRYPT) {
      data_out_enc.data = NULL;
      data_out_enc.length |= SE_DATATRANSFER_DISCARD;
    }

    SE_DataTransfer_t tag_out_enc = SE_DATATRANSFER_DEFAULT(ctx->tagbuf, sizeof(ctx->tagbuf));
    SE_DataTransfer_t ctx_out_enc = SE_DATATRANSFER_DEFAULT(ctx->se_ctx_enc, sizeof(ctx->se_ctx_enc));

    SE_addDataInput(&gcm_cmd_enc, &key_in_enc);
    SE_addDataInput(&gcm_cmd_enc, &iv_ctx_in_enc);
    SE_addDataInput(&gcm_cmd_enc, &data_in_enc);

    if (ctx->last_op) {
      SE_addDataInput(&gcm_cmd_enc, &lenalenc_in_enc);
    }

    SE_addDataOutput(&gcm_cmd_enc, &data_out_enc);

    if (ctx->last_op) {
      SE_addDataOutput(&gcm_cmd_enc, &tag_out_enc);
    } else {
      SE_addDataOutput(&gcm_cmd_enc, &ctx_out_enc);
    }

    SE_addParameter(&gcm_cmd_enc, ctx->keybits / 8);
    SE_addParameter(&gcm_cmd_enc, 0);
    SE_addParameter(&gcm_cmd_enc, input_length);

    SE_executeCommand(&gcm_cmd_enc);
    se_response = SE_readCommandResponse();
  }

  exit:
  se_management_release();

  if (se_response == SE_RESPONSE_OK) {
    *output_length = input_length;
    return(0);
  } else {
    mbedtls_zeroize(output, output_size);
    return(MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
  }
}

int mbedtls_gcm_finish(mbedtls_gcm_context *ctx,
                       unsigned char *output, size_t output_size,
                       size_t *output_length,
                       unsigned char *tag,
                       size_t tag_len)
{
  // Voiding these because our implementation does not support
  // partial-block input (i.e. passing a partial block to
  // update() will have caused the operation to finish already)
  (void) output;
  (void) output_size;
  *output_length = 0;

  if (ctx == NULL || tag == NULL) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }

  int status = sli_validate_gcm_params(tag_len, 12, 16);
  if (status) {
    return status;
  }

  if (ctx->add_len == 0 && ctx->len == 0) {
    return mbedtls_gcm_crypt_and_tag(ctx, MBEDTLS_GCM_ENCRYPT,
                                     0, ctx->se_ctx_enc, 12,
                                     NULL, 0,
                                     NULL, NULL,
                                     tag_len, tag);
  }

  memcpy(tag, ctx->tagbuf, tag_len);
  return(0);
}

int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *ctx,
                              int mode,
                              size_t length,
                              const unsigned char *iv,
                              size_t iv_len,
                              const unsigned char *add,
                              size_t add_len,
                              const unsigned char *input,
                              unsigned char *output,
                              size_t tag_len,
                              unsigned char *tag)
{
  SE_Response_t se_response;
  uint8_t tagbuf[16];
  int status;

  /* Check input parameters. */
  if (ctx == NULL
      || iv == NULL
      || (add_len > 0 && add == NULL)
      || (length > 0 && input == NULL)
      || (length > 0 && output == NULL)
      || tag == NULL) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }

  status = sli_validate_gcm_params(tag_len, iv_len, add_len);
  if (status) {
    return status;
  }

  if ( mode == MBEDTLS_GCM_DECRYPT ) {
    // Extract plaintext first
    SE_Command_t gcm_cmd = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_DECRYPT | ((tag_len & 0xFF) << 8));

    SE_DataTransfer_t key_in = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
    SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
    SE_DataTransfer_t aad_in = SE_DATATRANSFER_DEFAULT((void*)add, add_len);
    SE_DataTransfer_t data_in = SE_DATATRANSFER_DEFAULT((void*)input, length);
    SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(output, length);
    if (output == NULL) {
      data_out.length |= SE_DATATRANSFER_DISCARD;
    }
    SE_DataTransfer_t tag_in = SE_DATATRANSFER_DEFAULT(tag, tag_len);

    SE_addDataInput(&gcm_cmd, &key_in);
    SE_addDataInput(&gcm_cmd, &iv_in);
    SE_addDataInput(&gcm_cmd, &aad_in);
    SE_addDataInput(&gcm_cmd, &data_in);
    SE_addDataInput(&gcm_cmd, &tag_in);

    SE_addDataOutput(&gcm_cmd, &data_out);

    SE_addParameter(&gcm_cmd, ctx->keybits / 8);
    SE_addParameter(&gcm_cmd, add_len);
    SE_addParameter(&gcm_cmd, length);

    status = se_management_acquire();
    if (status != 0) {
      return status;
    }
    SE_executeCommand(&gcm_cmd);
    se_response = SE_readCommandResponse();
    se_management_release();
    // Getting an 'invalid signature' error here is acceptable, since we're not trying to verify the tag
    if (se_response == SE_RESPONSE_INVALID_SIGNATURE) {
      se_response = SE_RESPONSE_OK;
    }
    if (se_response != SE_RESPONSE_OK) {
      goto exit;
    }
    // Re-encrypt the extracted plaintext to generate the tag to match
    input = output;
    output = NULL;
  }

  // Explicit scope block to help with stack usage optimisation
  {
    SE_Command_t gcm_cmd = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_ENCRYPT);

    SE_DataTransfer_t key_in = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
    SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
    SE_DataTransfer_t aad_in = SE_DATATRANSFER_DEFAULT((void*)add, add_len);
    SE_DataTransfer_t data_in = SE_DATATRANSFER_DEFAULT((void*)input, length);
    SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(output, length);
    if (output == NULL) {
      data_out.length |= SE_DATATRANSFER_DISCARD;
    }
    SE_DataTransfer_t mac_out = SE_DATATRANSFER_DEFAULT(tagbuf, sizeof(tagbuf));

    SE_addDataInput(&gcm_cmd, &key_in);
    SE_addDataInput(&gcm_cmd, &iv_in);
    SE_addDataInput(&gcm_cmd, &aad_in);
    SE_addDataInput(&gcm_cmd, &data_in);

    SE_addDataOutput(&gcm_cmd, &data_out);
    SE_addDataOutput(&gcm_cmd, &mac_out);

    SE_addParameter(&gcm_cmd, ctx->keybits / 8);
    SE_addParameter(&gcm_cmd, add_len);
    SE_addParameter(&gcm_cmd, length);

    status = se_management_acquire();
    if (status != 0) {
      return status;
    }
    /* Execute GCM operation */
    SE_executeCommand(&gcm_cmd);
    se_response = SE_readCommandResponse();
    se_management_release();
  }

  exit:
  if (se_response == SE_RESPONSE_OK) {
    // For encryption, copy requested tag size to output tag buffer.
    memcpy(tag, tagbuf, tag_len);
    return(0);
  } else {
    mbedtls_zeroize(output, length);
    mbedtls_zeroize(tagbuf, sizeof(tagbuf));
    return(MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
  }
}

int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *ctx,
                             size_t length,
                             const unsigned char *iv,
                             size_t iv_len,
                             const unsigned char *add,
                             size_t add_len,
                             const unsigned char *tag,
                             size_t tag_len,
                             const unsigned char *input,
                             unsigned char *output)
{
  SE_Response_t se_response;
  int status;

  /* Check input parameters. */
  if (ctx == NULL
      || iv == NULL
      || (add_len > 0 && add == NULL)
      || (length > 0 && input == NULL)
      || (length > 0 && output == NULL)
      || tag == NULL) {
    return MBEDTLS_ERR_GCM_BAD_INPUT;
  }

  status = sli_validate_gcm_params(tag_len, iv_len, add_len);
  if (status) {
    return status;
  }

  // AES-GCM encryption and decryption are symmetrical. The SE only
  // supports checking tag length of 16 bytes. In order to support
  // smaller tag lengths, the decrypt-and-check routine is implemented
  // as a call to encrypt-and-MAC, and a manual check of the MAC vs the
  // expected MAC on the right tag length.

  SE_Command_t gcm_cmd = SE_COMMAND_DEFAULT(SE_COMMAND_AES_GCM_DECRYPT | ((tag_len & 0xFF) << 8));

  SE_DataTransfer_t key_in = SE_DATATRANSFER_DEFAULT(ctx->key, ctx->keybits / 8);
  SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT((void*)iv, iv_len);
  SE_DataTransfer_t aad_in = SE_DATATRANSFER_DEFAULT((void*)add, add_len);
  SE_DataTransfer_t data_in = SE_DATATRANSFER_DEFAULT((void*)input, length);
  SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(output, length);
  if (output == NULL) {
    data_out.length |= SE_DATATRANSFER_DISCARD;
  }
  SE_DataTransfer_t tag_in = SE_DATATRANSFER_DEFAULT((void*)tag, tag_len);

  SE_addDataInput(&gcm_cmd, &key_in);
  SE_addDataInput(&gcm_cmd, &iv_in);
  SE_addDataInput(&gcm_cmd, &aad_in);
  SE_addDataInput(&gcm_cmd, &data_in);
  SE_addDataInput(&gcm_cmd, &tag_in);

  SE_addDataOutput(&gcm_cmd, &data_out);

  SE_addParameter(&gcm_cmd, ctx->keybits / 8);
  SE_addParameter(&gcm_cmd, add_len);
  SE_addParameter(&gcm_cmd, length);

  status = se_management_acquire();
  if (status != 0) {
    return status;
  }
  SE_executeCommand(&gcm_cmd);
  se_response = SE_readCommandResponse();
  se_management_release();

  if (se_response == SE_RESPONSE_OK) {
    return(0);
  } else {
    mbedtls_zeroize(output, length);
    if (se_response == SE_RESPONSE_INVALID_SIGNATURE) {
      return(MBEDTLS_ERR_GCM_AUTH_FAILED);
    } else {
      return(MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED);
    }
  }
}

void mbedtls_gcm_free(mbedtls_gcm_context *ctx)
{
  if ( ctx == NULL ) {
    return;
  }
  mbedtls_zeroize(ctx, sizeof(mbedtls_gcm_context) );
}

#endif /* MBEDTLS_GCM_ALT && MBEDTLS_GCM_C */

#endif /* CRYPTOACC_PRESENT */
