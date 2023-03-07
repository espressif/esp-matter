/***************************************************************************//**
 * @file
 * @brief Hardware-accelerated AES implementation for Silicon Labs devices
 *        containing an AES peripheral.
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
 * functions in aes.c using the AES hardware accelerator incorporated
 * in Series-0 MCU devices from Silicon Laboratories.
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"

#if defined(AES_PRESENT) && (AES_COUNT == 1)
#if defined(MBEDTLS_AES_C)
#if defined(MBEDTLS_AES_ALT)

#include "em_aes.h"
#include "em_cmu.h"
#include "em_bus.h"
#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include <string.h>

#if defined(MBEDTLS_THREADING_C)
#include "mbedtls/threading.h"
#include "em_core.h"
/* Mutex for protecting access to the AES instance */
static mbedtls_threading_mutex_t aes_mutex;
static volatile bool aes_mutex_inited = false;
#endif

/* Parameter validation macros based on platform_util.h */
#define AES_VALIDATE_RET(cond) \
  MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_AES_BAD_INPUT_DATA)
#define AES_VALIDATE(cond) \
  MBEDTLS_INTERNAL_VALIDATE(cond)

static void aes_lock(void)
{
#if defined(MBEDTLS_THREADING_C)
  if ( !aes_mutex_inited ) {
    /* Turn off interrupts that can cause preemption */
    CORE_irqState_t critical_irq_state = CORE_EnterCritical();
    if ( !aes_mutex_inited ) {
      mbedtls_mutex_init(&aes_mutex);
      aes_mutex_inited = true;
    }
    CORE_ExitCritical(critical_irq_state);
  }
  mbedtls_mutex_lock(&aes_mutex);
#endif
  BUS_RegBitWrite(&(CMU->HFCORECLKEN0), _CMU_HFCORECLKEN0_AES_SHIFT, 1);
  return;
}

static void aes_unlock(void)
{
#if defined(MBEDTLS_THREADING_C)
  if ( aes_mutex_inited ) {
    mbedtls_mutex_unlock(&aes_mutex);
  }
#endif
  BUS_RegBitWrite(&(CMU->HFCORECLKEN0), _CMU_HFCORECLKEN0_AES_SHIFT, 0);
  return;
}

/*
 * Initialize AES context
 */
void mbedtls_aes_init(mbedtls_aes_context *ctx)
{
  AES_VALIDATE(ctx != NULL);

  memset(ctx, 0, sizeof(mbedtls_aes_context) );
}

/*
 * Clear AES context
 */
void mbedtls_aes_free(mbedtls_aes_context *ctx)
{
  if ( ctx == NULL ) {
    return;
  }

  memset(ctx, 0, sizeof(mbedtls_aes_context) );
}

#if defined(MBEDTLS_CIPHER_MODE_XTS)
void mbedtls_aes_xts_init(mbedtls_aes_xts_context *ctx)
{
  AES_VALIDATE(ctx != NULL);

  mbedtls_aes_init(&ctx->crypt);
  mbedtls_aes_init(&ctx->tweak);
}

void mbedtls_aes_xts_free(mbedtls_aes_xts_context *ctx)
{
  if ( ctx == NULL ) {
    return;
  }

  mbedtls_aes_free(&ctx->crypt);
  mbedtls_aes_free(&ctx->tweak);
}

static int mbedtls_aes_xts_decode_keys(const unsigned char *key,
                                       unsigned int keybits,
                                       const unsigned char **key1,
                                       unsigned int *key1bits,
                                       const unsigned char **key2,
                                       unsigned int *key2bits)
{
  const unsigned int half_keybits = keybits / 2;
  const unsigned int half_keybytes = half_keybits / 8;

  switch ( keybits ) {
    case 256: break;
    case 512: break;
    default: return(MBEDTLS_ERR_AES_INVALID_KEY_LENGTH);
  }

  *key1bits = half_keybits;
  *key2bits = half_keybits;
  *key1 = &key[0];
  *key2 = &key[half_keybytes];

  return 0;
}

int mbedtls_aes_xts_setkey_enc(mbedtls_aes_xts_context *ctx,
                               const unsigned char *key,
                               unsigned int keybits)
{
  int ret;
  const unsigned char *key1, *key2;
  unsigned int key1bits, key2bits;

  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(key != NULL);

  ret = mbedtls_aes_xts_decode_keys(key, keybits, &key1, &key1bits,
                                    &key2, &key2bits);
  if ( ret != 0 ) {
    return(ret);
  }

  /* Set the tweak key. Always set tweak key for the encryption mode. */
  ret = mbedtls_aes_setkey_enc(&ctx->tweak, key2, key2bits);
  if ( ret != 0 ) {
    return(ret);
  }

  /* Set crypt key for encryption. */
  return mbedtls_aes_setkey_enc(&ctx->crypt, key1, key1bits);
}

int mbedtls_aes_xts_setkey_dec(mbedtls_aes_xts_context *ctx,
                               const unsigned char *key,
                               unsigned int keybits)
{
  int ret;
  const unsigned char *key1, *key2;
  unsigned int key1bits, key2bits;

  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(key != NULL);

  ret = mbedtls_aes_xts_decode_keys(key, keybits, &key1, &key1bits,
                                    &key2, &key2bits);
  if ( ret != 0 ) {
    return(ret);
  }

  /* Set the tweak key. Always set tweak key for encryption. */
  ret = mbedtls_aes_setkey_enc(&ctx->tweak, key2, key2bits);
  if ( ret != 0 ) {
    return(ret);
  }

  /* Set crypt key for decryption. */
  return mbedtls_aes_setkey_dec(&ctx->crypt, key1, key1bits);
}

/* Endianess with 64 bits values */
#ifndef GET_UINT64_LE
#define GET_UINT64_LE(n, b, i)               \
  {                                          \
    (n) = ( (uint64_t) (b)[(i) + 7] << 56)   \
          | ( (uint64_t) (b)[(i) + 6] << 48) \
          | ( (uint64_t) (b)[(i) + 5] << 40) \
          | ( (uint64_t) (b)[(i) + 4] << 32) \
          | ( (uint64_t) (b)[(i) + 3] << 24) \
          | ( (uint64_t) (b)[(i) + 2] << 16) \
          | ( (uint64_t) (b)[(i) + 1] <<  8) \
          | ( (uint64_t) (b)[(i)]);          \
  }
#endif

#ifndef PUT_UINT64_LE
#define PUT_UINT64_LE(n, b, i)                   \
  {                                              \
    (b)[(i) + 7] = (unsigned char) ( (n) >> 56); \
    (b)[(i) + 6] = (unsigned char) ( (n) >> 48); \
    (b)[(i) + 5] = (unsigned char) ( (n) >> 40); \
    (b)[(i) + 4] = (unsigned char) ( (n) >> 32); \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24); \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16); \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8); \
    (b)[(i)] = (unsigned char) ( (n)       );    \
  }
#endif

typedef unsigned char mbedtls_be128[16];

/*
 * GF(2^128) multiplication function
 *
 * This function multiplies a field element by x in the polynomial field
 * representation. It uses 64-bit word operations to gain speed but compensates
 * for machine endianess and hence works correctly on both big and little
 * endian machines.
 */
static void mbedtls_gf128mul_x_ble(unsigned char r[16],
                                   const unsigned char x[16])
{
  uint64_t a, b, ra, rb;

  GET_UINT64_LE(a, x, 0);
  GET_UINT64_LE(b, x, 8);

  ra = (a << 1)  ^ 0x0087 >> (8 - ( (b >> 63) << 3) );
  rb = (a >> 63) | (b << 1);

  PUT_UINT64_LE(ra, r, 0);
  PUT_UINT64_LE(rb, r, 8);
}

/*
 * AES-XTS buffer encryption/decryption
 */
int mbedtls_aes_crypt_xts(mbedtls_aes_xts_context *ctx,
                          int mode,
                          size_t length,
                          const unsigned char data_unit[16],
                          const unsigned char *input,
                          unsigned char *output)
{
  int ret;
  size_t blocks = length / 16;
  size_t leftover = length % 16;
  unsigned char tweak[16];
  unsigned char prev_tweak[16];
  unsigned char tmp[16];

  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(mode == MBEDTLS_AES_ENCRYPT
                   || mode == MBEDTLS_AES_DECRYPT);
  AES_VALIDATE_RET(data_unit != NULL);
  AES_VALIDATE_RET(input != NULL);
  AES_VALIDATE_RET(output != NULL);

  /* Data units must be at least 16 bytes long. */
  if ( length < 16 ) {
    return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
  }

  /* NIST SP 800-38E disallows data units larger than 2**20 blocks. */
  if ( length > (1 << 20) * 16 ) {
    return MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH;
  }

  /* Compute the tweak. */
  ret = mbedtls_aes_crypt_ecb(&ctx->tweak, MBEDTLS_AES_ENCRYPT,
                              data_unit, tweak);
  if ( ret != 0 ) {
    return(ret);
  }

  while ( blocks-- ) {
    size_t i;

    if ( leftover && (mode == MBEDTLS_AES_DECRYPT) && blocks == 0 ) {
      /* We are on the last block in a decrypt operation that has
       * leftover bytes, so we need to use the next tweak for this block,
       * and this tweak for the lefover bytes. Save the current tweak for
       * the leftovers and then update the current tweak for use on this,
       * the last full block. */
      memcpy(prev_tweak, tweak, sizeof(tweak) );
      mbedtls_gf128mul_x_ble(tweak, tweak);
    }

    for ( i = 0; i < 16; i++ ) {
      tmp[i] = input[i] ^ tweak[i];
    }

    ret = mbedtls_aes_crypt_ecb(&ctx->crypt, mode, tmp, tmp);
    if ( ret != 0 ) {
      return(ret);
    }

    for ( i = 0; i < 16; i++ ) {
      output[i] = tmp[i] ^ tweak[i];
    }

    /* Update the tweak for the next block. */
    mbedtls_gf128mul_x_ble(tweak, tweak);

    output += 16;
    input += 16;
  }

  if ( leftover ) {
    /* If we are on the leftover bytes in a decrypt operation, we need to
    * use the previous tweak for these bytes (as saved in prev_tweak). */
    unsigned char *t = mode == MBEDTLS_AES_DECRYPT ? prev_tweak : tweak;

    /* We are now on the final part of the data unit, which doesn't divide
     * evenly by 16. It's time for ciphertext stealing. */
    size_t i;
    unsigned char *prev_output = output - 16;

    /* Copy ciphertext bytes from the previous block to our output for each
     * byte of cyphertext we won't steal. At the same time, copy the
     * remainder of the input for this final round (since the loop bounds
     * are the same). */
    for ( i = 0; i < leftover; i++ ) {
      output[i] = prev_output[i];
      tmp[i] = input[i] ^ t[i];
    }

    /* Copy ciphertext bytes from the previous block for input in this
     * round. */
    for (; i < 16; i++ ) {
      tmp[i] = prev_output[i] ^ t[i];
    }

    ret = mbedtls_aes_crypt_ecb(&ctx->crypt, mode, tmp, tmp);
    if ( ret != 0 ) {
      return ret;
    }

    /* Write the result back to the previous block, overriding the previous
     * output we copied. */
    for ( i = 0; i < 16; i++ ) {
      prev_output[i] = tmp[i] ^ t[i];
    }
  }

  return(0);
}
#endif /* MBEDTLS_CIPHER_MODE_XTS */

/*
 * AES key schedule (encryption)
 */
int mbedtls_aes_setkey_enc(mbedtls_aes_context *ctx,
                           const unsigned char *key,
                           unsigned int keybits)
{
  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(key != NULL);

  switch (keybits) {
    case 128:
      break;
    case 192:
      // AES-192 is not supported on series-0 devices.
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
    case 256:
#if defined(AES_CTRL_AES256)
      break;
#else
      // AES-256 is not supported on some series-0 devices.
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
#endif
    default:
      return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
  }

  ctx->keybits = keybits;
  memcpy(ctx->key, key, keybits / 8);

  return 0;
}

/*
 * AES key schedule (decryption)
 */
int mbedtls_aes_setkey_dec(mbedtls_aes_context *ctx,
                           const unsigned char *key,
                           unsigned int keybits)
{
  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(key != NULL);

  switch (keybits) {
    case 128:
      ctx->keybits = keybits;
      aes_lock();
      AES_DecryptKey128(ctx->key, key);
      aes_unlock();
      break;
    case 192:
      // AES-192 is not supported on series-0 devices.
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
    case 256:
#if defined(AES_CTRL_AES256)
      ctx->keybits = keybits;
      aes_lock();
      AES_DecryptKey256(ctx->key, key);
      aes_unlock();
      break;
#else
      // AES-256 is not supported on some series-0 devices.
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
#endif
    default:
      return MBEDTLS_ERR_AES_INVALID_KEY_LENGTH;
  }

  return 0;
}

int mbedtls_internal_aes_encrypt(mbedtls_aes_context *ctx,
                                 const unsigned char input[16],
                                 unsigned char output[16])
{
  return mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, input, output);
}

int mbedtls_internal_aes_decrypt(mbedtls_aes_context *ctx,
                                 const unsigned char input[16],
                                 unsigned char output[16])
{
  return mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_DECRYPT, input, output);
}

/*
 * AES-ECB block encryption/decryption
 */
int mbedtls_aes_crypt_ecb(mbedtls_aes_context *ctx,
                          int mode,
                          const unsigned char input[16],
                          unsigned char output[16])
{
  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(input != NULL);
  AES_VALIDATE_RET(output != NULL);
  AES_VALIDATE_RET(mode == MBEDTLS_AES_ENCRYPT
                   || mode == MBEDTLS_AES_DECRYPT);

  switch ( ctx->keybits ) {
    case 128:
      aes_lock();
      AES_ECB128(output,
                 input,
                 16,
                 ctx->key,
                 mode == MBEDTLS_AES_ENCRYPT ? true : false);
      aes_unlock();
      break;
#if defined(AES_CTRL_AES256)
    case 256:
      aes_lock();
      AES_ECB256(output,
                 input,
                 16,
                 ctx->key,
                 mode == MBEDTLS_AES_ENCRYPT ? true : false);
      aes_unlock();
      break;
#endif
    default:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

  return(0);
}

#if defined(MBEDTLS_CIPHER_MODE_CBC)
/*
 * AES-CBC buffer encryption/decryption
 */
int mbedtls_aes_crypt_cbc(mbedtls_aes_context *ctx,
                          int mode,
                          size_t length,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
  uint8_t tmpIv[16];

  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(mode == MBEDTLS_AES_ENCRYPT
                   || mode == MBEDTLS_AES_DECRYPT);
  AES_VALIDATE_RET(iv != NULL);
  AES_VALIDATE_RET(input != NULL);
  AES_VALIDATE_RET(output != NULL);

  if ( length % 16 ) {
    return(MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH);
  }

  if ( mode == MBEDTLS_AES_DECRYPT ) {
    if ( length >= 16 ) {
      memcpy(tmpIv, &input[length - 16], 16);
    }
  }

  switch ( ctx->keybits ) {
    case 128:
      aes_lock();
      AES_CBC128(output,
                 input,
                 length,
                 ctx->key,
                 iv,
                 mode == MBEDTLS_AES_ENCRYPT ? true : false);
      aes_unlock();
      break;
#if defined(AES_CTRL_AES256)
    case 256:
      aes_lock();
      AES_CBC256(output,
                 input,
                 length,
                 ctx->key,
                 iv,
                 mode == MBEDTLS_AES_ENCRYPT ? true : false);
      aes_unlock();
      break;
#endif
    default:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

  if ( length >= 16 ) {
    if ( mode == MBEDTLS_AES_ENCRYPT ) {
      memcpy(iv, &output[length - 16], 16);
    } else {
      memcpy(iv, tmpIv, 16);
    }
  }

  return(0);
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CIPHER_MODE_CFB)
/*
 * AES-CFB128 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb128(mbedtls_aes_context *ctx,
                             int mode,
                             size_t length,
                             size_t *iv_off,
                             unsigned char iv[16],
                             const unsigned char *input,
                             unsigned char *output)
{
  size_t n = (iv_off != NULL) ? *iv_off : 0;

  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(mode == MBEDTLS_AES_ENCRYPT
                   || mode == MBEDTLS_AES_DECRYPT);
  AES_VALIDATE_RET(iv_off != NULL);
  AES_VALIDATE_RET(iv != NULL);
  AES_VALIDATE_RET(input != NULL);
  AES_VALIDATE_RET(output != NULL);

  if ( n > 15 ) {
    return(MBEDTLS_ERR_AES_BAD_INPUT_DATA);
  }

  if ( (n > 0) || (length & 0xf) ) {
    // IV offset or length not aligned to block size
    int c;

    if ( mode == MBEDTLS_AES_DECRYPT ) {
      while ( length-- ) {
        if ( n == 0 ) {
          mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
        }

        c = *input++;
        *output++ = (unsigned char)(c ^ iv[n]);
        iv[n] = (unsigned char) c;

        n = (n + 1) & 0x0F;
      }
    } else {
      while ( length-- ) {
        if ( n == 0 ) {
          mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
        }

        iv[n] = *output++ = (unsigned char)(iv[n] ^ *input++);

        n = (n + 1) & 0x0F;
      }
    }

    if (iv_off) {
      *iv_off = n;
    }
    return(0);
  } else {
    switch ( ctx->keybits ) {
      case 128:
        aes_lock();
        AES_CFB128(output,
                   input,
                   length,
                   ctx->key,
                   iv,
                   mode == MBEDTLS_AES_ENCRYPT ? true : false);
        aes_unlock();
        break;

#if defined(AES_CTRL_AES256)
      case 256:
        aes_lock();
        AES_CFB256(output,
                   input,
                   length,
                   ctx->key,
                   iv,
                   mode == MBEDTLS_AES_ENCRYPT ? true : false);
        aes_unlock();
        break;
#endif
      default:
        return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
    }

    if ( length >= 16 ) {
      if ( mode == MBEDTLS_AES_ENCRYPT ) {
        memcpy(iv, &output[length - 16], 16);
      } else {
        memcpy(iv, &input[length - 16], 16);
      }
    }

    return(0);
  }
}

/*
 * AES-CFB8 buffer encryption/decryption
 */
int mbedtls_aes_crypt_cfb8(mbedtls_aes_context *ctx,
                           int mode,
                           size_t length,
                           unsigned char iv[16],
                           const unsigned char *input,
                           unsigned char *output)
{
  unsigned char c;
  unsigned char ov[17];

  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(mode == MBEDTLS_AES_ENCRYPT
                   || mode == MBEDTLS_AES_DECRYPT);
  AES_VALIDATE_RET(iv != NULL);
  AES_VALIDATE_RET(input != NULL);
  AES_VALIDATE_RET(output != NULL);

  while ( length-- ) {
    memcpy(ov, iv, 16);
    mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);

    if ( mode == MBEDTLS_AES_DECRYPT ) {
      ov[16] = *input;
    }

    c = *output++ = (unsigned char)(iv[0] ^ *input++);

    if ( mode == MBEDTLS_AES_ENCRYPT ) {
      ov[16] = c;
    }

    memcpy(iv, ov + 1, 16);
  }

  return(0);
}
#endif /*MBEDTLS_CIPHER_MODE_CFB */

#if defined(MBEDTLS_CIPHER_MODE_CTR)
/*
 * AES-CTR Nonce update function
 */
static void aes_ctr_update_nonce(uint8_t *nonce_counter)
{
  for ( size_t i = 16; i > 0; i-- ) {
    if ( ++nonce_counter[i - 1] != 0 ) {
      break;
    }
  }
}

/*
 * AES-CTR buffer encryption/decryption
 */
int mbedtls_aes_crypt_ctr(mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *nc_off,
                          unsigned char nonce_counter[16],
                          unsigned char stream_block[16],
                          const unsigned char *input,
                          unsigned char *output)
{
  size_t n = (nc_off != NULL) ? *nc_off : 0;

  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(nc_off != NULL);
  AES_VALIDATE_RET(nonce_counter != NULL);
  AES_VALIDATE_RET(stream_block != NULL);
  AES_VALIDATE_RET(input != NULL);
  AES_VALIDATE_RET(output != NULL);

  if ( (n > 0) || (length & 0xf) || ctx->keybits == 192 ) {
    // IV offset or length not aligned to block size
    int c, i;

    while ( length-- ) {
      if ( n == 0 ) {
        mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, nonce_counter, stream_block);

        for ( i = 16; i > 0; i-- ) {
          if ( ++nonce_counter[i - 1] != 0 ) {
            break;
          }
        }
      }
      c = *input++;
      *output++ = (unsigned char)(c ^ stream_block[n]);

      n = (n + 1) & 0x0F;
    }

    if (nc_off) {
      *nc_off = n;
    }
    return(0);
  } else {
    switch ( ctx->keybits ) {
      case 128:
        aes_lock();
        AES_CTR128(output,
                   input,
                   length,
                   ctx->key,
                   nonce_counter,
                   &aes_ctr_update_nonce);
        aes_unlock();
        break;

#if defined(AES_CTRL_AES256)
      case 256:
        aes_lock();
        AES_CTR256(output,
                   input,
                   length,
                   ctx->key,
                   nonce_counter,
                   &aes_ctr_update_nonce);
        aes_unlock();
        break;
#endif
      default:
        return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
        break;
    }

    return(0);
  }
}
#endif /* MBEDTLS_CIPHER_MODE_CTR */

#if defined(MBEDTLS_CIPHER_MODE_OFB)
/*
 * AES-OFB (Output Feedback Mode) buffer encryption/decryption
 */
int mbedtls_aes_crypt_ofb(mbedtls_aes_context *ctx,
                          size_t length,
                          size_t *iv_off,
                          unsigned char iv[16],
                          const unsigned char *input,
                          unsigned char *output)
{
  int ret = 0;
  size_t n;

  AES_VALIDATE_RET(ctx != NULL);
  AES_VALIDATE_RET(iv_off != NULL);
  AES_VALIDATE_RET(iv != NULL);
  AES_VALIDATE_RET(input != NULL);
  AES_VALIDATE_RET(output != NULL);

  n = *iv_off;

  if ( n > 15 ) {
    return MBEDTLS_ERR_AES_BAD_INPUT_DATA;
  }

  while ( length-- ) {
    if ( n == 0 ) {
      ret = mbedtls_aes_crypt_ecb(ctx, MBEDTLS_AES_ENCRYPT, iv, iv);
      if ( ret != 0 ) {
        goto exit;
      }
    }
    *output++ =  *input++ ^ iv[n];

    n = (n + 1) & 0x0F;
  }

  *iv_off = n;

  exit:
  return(ret);
}
#endif /* MBEDTLS_CIPHER_MODE_OFB */

#endif /* MBEDTLS_AES_ALT */
#endif /* MBEDTLS_AES_C */
#endif /* AES_PRESENT && (AES_COUNT == 1) */
