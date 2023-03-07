/***************************************************************************//**
 * @file
 * @brief Generic SHA-256 abstraction
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/
/*
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/platform_util.h"
#include "mbedtls/md.h"
#include <string.h>

#include "security/sha/btl_sha256.h"

static const uint8_t init_state_sha256[32] =
{
  0x6A, 0x09, 0xE6, 0x67,
  0xBB, 0x67, 0xAE, 0x85,
  0x3C, 0x6E, 0xF3, 0x72,
  0xA5, 0x4F, 0xF5, 0x3A,
  0x51, 0x0E, 0x52, 0x7F,
  0x9B, 0x05, 0x68, 0x8C,
  0x1F, 0x83, 0xD9, 0xAB,
  0x5B, 0xE0, 0xCD, 0x19
};

static const unsigned char shax_padding[64] =
{
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int sha_x_update(SHA_Type_t algo, const unsigned char *data, size_t data_len, uint8_t* state, unsigned char *buffer, uint32_t* counter)
{
  size_t blocksize, countersize, blocks, fill, left;
  uint8_t* state_in = NULL;

  switch (algo) {
    case SHA256:
      blocksize = 64;
      countersize = 64 / 32;
      break;
    default:
      return MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE;
  }

  if ( data_len == 0 ) {
    return 0;
  }

  if (counter[0] < blocksize) {
    for (size_t i = 1; i < countersize; i++) {
      if (counter[i] != 0) {
        state_in = state;
        break;
      }
    }
    if (state_in == NULL) {
      switch (algo) {
        case SHA256:
          state_in = (uint8_t*)init_state_sha256;
          break;
        default:
          return MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE;
      }
    }
  } else {
    state_in = state;
  }

  left = (counter[0] & (blocksize - 1));
  fill = blocksize - left;

  counter[0] += data_len;

  // ripple counter
  if ( counter[0] < data_len ) {
    counter[1] += 1;
    for (size_t i = 1; i < (countersize - 1); i++) {
      if ( counter[i] == 0 ) {
        counter[i + 1]++;
      }
    }
  }

  if ( (left > 0) && (data_len >= fill) ) {
    memcpy( (void *) (buffer + left), data, fill);
    sha_x_process(algo, state_in, buffer, state, 1);
    state_in = state;
    data += fill;
    data_len -= fill;
    left = 0;
  }

  if ( data_len >= blocksize ) {
    blocks = data_len / blocksize;
    sha_x_process(algo, state_in, data, state, blocks);
    state_in = state;
    data += blocksize * blocks;
    data_len -= blocksize * blocks;
  }

  if ( data_len > 0 ) {
    memcpy( (void *) (buffer + left), data, data_len);
  }

  return 0;
}

int sha_x_finish(SHA_Type_t algo, uint8_t* state, unsigned char *buffer, uint32_t* counter, unsigned char *output)
{
  size_t last_data_byte, num_pad_bytes, blocksize, countersize, outputsize;
  uint8_t msglen[16];

  switch (algo) {
    case SHA256:
      blocksize = 64;
      outputsize = 32;
      countersize = 64 / 32;
      break;
    default:
      return MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE;
  }

  // Convert counter value to bits, and put in big-endian array
  uint8_t residual = 0;
  for (size_t i = 0; i < countersize; i++) {
    size_t msglen_index = ( (countersize - i) * sizeof(uint32_t) ) - 1;

    msglen[msglen_index - 0] = ((counter[i] << 3) + residual) & 0xFF;
    msglen[msglen_index - 1] = (counter[i] >> 5) & 0xFF;
    msglen[msglen_index - 2] = (counter[i] >> 13) & 0xFF;
    msglen[msglen_index - 3] = (counter[i] >> 21) & 0xFF;

    residual = (counter[i] >> 29) & 0xFF;
  }

  last_data_byte = (counter[0] & (blocksize - 1) );
  num_pad_bytes = (last_data_byte < (blocksize - (countersize * 4)) )
                  ? ( (blocksize - (countersize * 4)) - last_data_byte)
                  : ( ((2 * blocksize) - (countersize * 4)) - last_data_byte);

  sha_x_update(algo, (uint8_t*)shax_padding, num_pad_bytes, state, buffer, counter);
  sha_x_update(algo, msglen, (countersize * 4), state, buffer, counter);

  memcpy(output, state, outputsize);

  return 0;
}

void btl_sha256_init(btl_sha256_context *ctx)
{
  memset(ctx, 0, sizeof(btl_sha256_context));
}

int btl_sha256_starts_ret(btl_sha256_context *ctx, int is224)
{
  (void)is224;
  ctx->total[0] = 0;
  ctx->total[1] = 0;

  return 0;
}

int btl_sha256_update_ret(btl_sha256_context *ctx,
                          const unsigned char *input,
                          size_t ilen)
{
  return sha_x_update(SHA256,
                      input,
                      ilen,
                      (uint8_t*)ctx->state,
                      ctx->buffer,
                      ctx->total);
}

int btl_sha256_finish_ret(btl_sha256_context *ctx, unsigned char output[32])
{
  return sha_x_finish(SHA256,
                      (uint8_t*)ctx->state,
                      ctx->buffer,
                      ctx->total,
                      output);
}


