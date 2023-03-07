/***************************************************************************//**
 * @file
 * @brief Advanced Encryption Standard (AES) accelerator peripheral API.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_aes.h"
#if defined(AES_COUNT) && (AES_COUNT > 0)

#include "sl_assert.h"
#include <string.h>

/***************************************************************************//**
 * @addtogroup aes
 * @details
 *  This module contains functions to control the AES peripheral of Silicon
 *  Labs 32-bit MCUs and SoCs.
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define AES_BLOCKSIZE    16

/* Convenience macro to fetch a 32-bit unsigned integer from a potentially
 * unaligned byte array with native endianness.
 * Optimising compilers should be able to recognize that this operation is a
 * direct access on architectures which support unaligned access. */
#define __UINT_FROM_BYTEARRAY(ptr_to_array) \
  ( ((uint32_t)((ptr_to_array)[3])) << 24   \
    | ((uint32_t)((ptr_to_array)[2])) << 16 \
    | ((uint32_t)((ptr_to_array)[1])) <<  8 \
    | ((uint32_t)((ptr_to_array)[0])) <<  0)

/* Convenience macro to put a 32-bit unsigned integer into a potentially
 * unaligned byte array with native endianness.
 * Optimising compilers should be able to recognize that this operation is a
 * direct write on architectures which support unaligned access. */
#define __UINT_TO_BYTEARRAY(ptr_to_array, value) \
  do {                                           \
    uint32_t valbuf = value;                     \
    (ptr_to_array)[3] = (valbuf >> 24) & 0xFFu;  \
    (ptr_to_array)[2] = (valbuf >> 16) & 0xFFu;  \
    (ptr_to_array)[1] = (valbuf >>  8) & 0xFFu;  \
    (ptr_to_array)[0] = (valbuf >>  0) & 0xFFu;  \
  } while (0)

/* Convenience macro to fetch a byte-reversed 32-bit unsigned integer from a
 * potentially unaligned byte array. */
#define __REV_FROM_BYTEARRAY(ptr_to_array) \
  __REV(__UINT_FROM_BYTEARRAY(ptr_to_array))

/* Convenience macro to put a byte-reversed 32-bit unsigned integer into a
 * potentially unaligned byte array. */
#define __REV_TO_BYTEARRAY(ptr_to_array, value) \
  __UINT_TO_BYTEARRAY(ptr_to_array, __REV(value))

/** @endcond */

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Cipher-block chaining (CBC) cipher mode encryption/decryption, 128 bit key.
 *
 * @details
 *   Encryption:
 * @verbatim
 *           Plaintext                  Plaintext
 *               |                          |
 *               V                          V
 * InitVector ->XOR        +-------------->XOR
 *               |         |                |
 *               V         |                V
 *       +--------------+  |        +--------------+
 * Key ->| Block cipher |  |  Key ->| Block cipher |
 *       |  encryption  |  |        |  encryption  |
 *       +--------------+  |        +--------------+
 *               |---------+                |
 *               V                          V
 *           Ciphertext                 Ciphertext
 * @endverbatim
 *   Decryption:
 * @verbatim
 *         Ciphertext                 Ciphertext
 *              |----------+                |
 *              V          |                V
 *       +--------------+  |        +--------------+
 * Key ->| Block cipher |  |  Key ->| Block cipher |
 *       |  decryption  |  |        |  decryption  |
 *       +--------------+  |        +--------------+
 *               |         |                |
 *               V         |                V
 * InitVector ->XOR        +-------------->XOR
 *               |                          |
 *               V                          V
 *           Plaintext                  Plaintext
 * @endverbatim
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place encrypted/decrypted data. Must be at least @p len long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding data to encrypt/decrypt. Must be at least @p len long.
 *
 * @param[in] len
 *   A number of bytes to encrypt/decrypt. Must be a multiple of 16.
 *
 * @param[in] key
 *   When encrypting, this is the 128 bit encryption key. When
 *   decrypting, this is the 128 bit decryption key. The decryption key may
 *   be generated from the encryption key with AES_DecryptKey128().
 *   On devices supporting key buffering, this argument can be null. If so, the
 *   key will not be loaded as it is assumed the key has been loaded
 *   into KEYHA previously.
 *
 * @param[in] iv
 *   128 bit initialization vector.
 *
 * @param[in] encrypt
 *   Set to true to encrypt, false to decrypt.
 ******************************************************************************/
void AES_CBC128(uint8_t *out,
                const uint8_t *in,
                unsigned int len,
                const uint8_t *key,
                const uint8_t *iv,
                bool encrypt)
{
  int            i;
  /* Need to buffer one block when decrypting in case 'out' replaces 'in'. */
  uint32_t       prev[AES_BLOCKSIZE / sizeof(uint32_t)];

  EFM_ASSERT(!(len % AES_BLOCKSIZE));

  /* A number of blocks to process. */
  len /= AES_BLOCKSIZE;

  #if defined(AES_CTRL_KEYBUFEN)
  /* Load the key into a high key for a key buffer usage. */
  for (i = 3; i >= 0; i--) {
    AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
  }
  #endif

  if (encrypt) {
    /* Enable encryption with auto start using XOR. */
    #if defined(AES_CTRL_KEYBUFEN)
    AES->CTRL = AES_CTRL_KEYBUFEN | AES_CTRL_XORSTART;
    #else
    AES->CTRL = AES_CTRL_XORSTART;
    #endif

    /* Load an initialization vector. Since writing to DATA, it will */
    /* not trigger encryption. */
    for (i = 3; i >= 0; i--) {
      AES->DATA = __REV_FROM_BYTEARRAY(&iv[i * 4]);
    }

    /* Encrypt data. */
    while (len--) {
      #if !defined(AES_CTRL_KEYBUFEN)
      /* Load key. */
      for (i = 3; i >= 0; i--) {
        AES->KEYLA = __REV_FROM_BYTEARRAY(&key[i * 4]);
      }
      #endif

      /* Load data and trigger encryption. */
      for (i = 3; i >= 0; i--) {
        AES->XORDATA = __REV_FROM_BYTEARRAY(&in[i * 4]);
      }
      in += AES_BLOCKSIZE;

      /* Wait for completion. */
      while (AES->STATUS & AES_STATUS_RUNNING)
        ;

      /* Save encrypted data. */
      for (i = 3; i >= 0; i--) {
        __REV_TO_BYTEARRAY(&out[i * 4], AES->DATA);
      }
      out += AES_BLOCKSIZE;
    }
  } else {
    /* Select decryption mode. */
    #if defined(AES_CTRL_KEYBUFEN)
    AES->CTRL = AES_CTRL_DECRYPT | AES_CTRL_KEYBUFEN | AES_CTRL_DATASTART;
    #else
    AES->CTRL = AES_CTRL_DECRYPT | AES_CTRL_DATASTART;
    #endif

    /* Copy initialization vector to the previous buffer to avoid special handling. */
    memcpy(prev, iv, AES_BLOCKSIZE);

    /* Decrypt data. */
    while (len--) {
      #if !defined(AES_CTRL_KEYBUFEN)
      /* Load key. */
      for (i = 3; i >= 0; i--) {
        AES->KEYLA = __REV_FROM_BYTEARRAY(&key[i * 4]);
      }
      #endif

      /* Load data and trigger decryption. */
      for (i = 3; i >= 0; i--) {
        AES->DATA = __REV_FROM_BYTEARRAY(&in[i * 4]);
      }

      /* Wait for completion. */
      while (AES->STATUS & AES_STATUS_RUNNING)
        ;

      /* To avoid an additional buffer, hardware is used directly for XOR and buffer. */
      /* (Writing to XORDATA will not trigger encoding, triggering enabled on DATA.) */
      for (i = 3; i >= 0; i--) {
        AES->XORDATA = __REV(prev[i]);
      }
      memcpy(prev, in, AES_BLOCKSIZE);
      in += AES_BLOCKSIZE;

      /* Fetch decrypted data in a separate loop */
      /* due to internal auto-shifting of words. */
      for (i = 3; i >= 0; i--) {
        __REV_TO_BYTEARRAY(&out[i * 4], AES->DATA);
      }
      out += AES_BLOCKSIZE;
    }
  }
}

#if defined(AES_CTRL_AES256)
/***************************************************************************//**
 * @brief
 *   Cipher-block chaining (CBC) cipher mode encryption/decryption, 256 bit key.
 *
 * @details
 *   See AES_CBC128() for the CBC figure.
 *
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place encrypted/decrypted data. Must be at least @p len long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding data to encrypt/decrypt. Must be at least @p len long.
 *
 * @param[in] len
 *   A number of bytes to encrypt/decrypt. Must be a multiple of 16.
 *
 * @param[in] key
 *   When encrypting, this is the 256 bit encryption key. When
 *   decrypting, this is the 256 bit decryption key. The decryption key may
 *   be generated from the encryption key with AES_DecryptKey256().
 *
 * @param[in] iv
 *   128 bit initialization vector to use.
 *
 * @param[in] encrypt
 *   Set to true to encrypt, false to decrypt.
 ******************************************************************************/
void AES_CBC256(uint8_t *out,
                const uint8_t *in,
                unsigned int len,
                const uint8_t *key,
                const uint8_t *iv,
                bool encrypt)
{
  int            i;
  int            j;
  /* Buffer one block when decrypting in case output replaces input. */
  uint32_t       prev[AES_BLOCKSIZE / sizeof(uint32_t)];

  EFM_ASSERT(!(len % AES_BLOCKSIZE));

  /* A number of blocks to process. */
  len /= AES_BLOCKSIZE;

  if (encrypt) {
    /* Enable encryption with auto start using XOR. */
    AES->CTRL = AES_CTRL_AES256 | AES_CTRL_XORSTART;

    /* Load an initialization vector. Since writing to DATA, it will */
    /* not trigger encryption. */
    for (i = 3; i >= 0; i--) {
      AES->DATA = __REV_FROM_BYTEARRAY(&iv[i * 4]);
    }

    /* Encrypt data. */
    while (len--) {
      /* Load the key and data and trigger encryption. */
      for (i = 3, j = 7; i >= 0; i--, j--) {
        AES->KEYLA = __REV_FROM_BYTEARRAY(&key[j * 4]);
        AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
        /* Write data last, since will trigger encryption on the last iteration. */
        AES->XORDATA = __REV_FROM_BYTEARRAY(&in[i * 4]);
      }
      in += AES_BLOCKSIZE;

      /* Wait for completion. */
      while (AES->STATUS & AES_STATUS_RUNNING)
        ;

      /* Save encrypted data. */
      for (i = 3; i >= 0; i--) {
        __REV_TO_BYTEARRAY(&out[i * 4], AES->DATA);
      }
      out += AES_BLOCKSIZE;
    }
  } else {
    /* Select decryption mode. */
    AES->CTRL = AES_CTRL_AES256 | AES_CTRL_DECRYPT | AES_CTRL_DATASTART;

    /* Copy the initialization vector to the previous buffer to avoid special handling. */
    memcpy(prev, iv, AES_BLOCKSIZE);

    /* Decrypt data. */
    while (len--) {
      /* Load the key and data and trigger decryption. */
      for (i = 3, j = 7; i >= 0; i--, j--) {
        AES->KEYLA = __REV_FROM_BYTEARRAY(&key[j * 4]);
        AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
        /* Write data last, since will trigger encryption on the last iteration. */
        AES->DATA = __REV_FROM_BYTEARRAY(&in[i * 4]);
      }

      /* Wait for completion. */
      while (AES->STATUS & AES_STATUS_RUNNING)
        ;

      /* To avoid an additional buffer, hardware is used directly for XOR and buffer. */
      for (i = 3; i >= 0; i--) {
        AES->XORDATA = __REV(prev[i]);
      }
      memcpy(prev, in, AES_BLOCKSIZE);
      in += AES_BLOCKSIZE;

      /* Fetch decrypted data in a separate loop */
      /* due to internal auto-shifting of words. */
      for (i = 3; i >= 0; i--) {
        __REV_TO_BYTEARRAY(&out[i * 4], AES->DATA);
      }
      out += AES_BLOCKSIZE;
    }
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Cipher feedback (CFB) cipher mode encryption/decryption, 128 bit key.
 *
 * @details
 *   Encryption:
 * @verbatim
 *           InitVector    +----------------+
 *               |         |                |
 *               V         |                V
 *       +--------------+  |        +--------------+
 * Key ->| Block cipher |  |  Key ->| Block cipher |
 *       |  encryption  |  |        |  encryption  |
 *       +--------------+  |        +--------------+
 *               |         |                |
 *               V         |                V
 *  Plaintext ->XOR        |   Plaintext ->XOR
 *               |---------+                |
 *               V                          V
 *           Ciphertext                 Ciphertext
 * @endverbatim
 *   Decryption:
 * @verbatim
 *          InitVector     +----------------+
 *               |         |                |
 *               V         |                V
 *       +--------------+  |        +--------------+
 * Key ->| Block cipher |  |  Key ->| Block cipher |
 *       |  encryption  |  |        |  encryption  |
 *       +--------------+  |        +--------------+
 *               |         |                |
 *               V         |                V
 *              XOR<- Ciphertext           XOR<- Ciphertext
 *               |                          |
 *               V                          V
 *           Plaintext                  Plaintext
 * @endverbatim
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place encrypted/decrypted data. Must be at least @p len long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding data to encrypt/decrypt. Must be at least @p len long.
 *
 * @param[in] len
 *   A number of bytes to encrypt/decrypt. Must be a multiple of 16.
 *
 * @param[in] key
 *   128 bit encryption key is used for both encryption and decryption modes.
 *
 * @param[in] iv
 *   128 bit initialization vector to use.
 *
 * @param[in] encrypt
 *   Set to true to encrypt, false to decrypt.
 ******************************************************************************/
void AES_CFB128(uint8_t *out,
                const uint8_t *in,
                unsigned int len,
                const uint8_t *key,
                const uint8_t *iv,
                bool encrypt)
{
  int            i;
  const uint8_t  *data;
  uint8_t        tmp[AES_BLOCKSIZE];

  EFM_ASSERT(!(len % AES_BLOCKSIZE));

  #if defined(AES_CTRL_KEYBUFEN)
  AES->CTRL = AES_CTRL_KEYBUFEN | AES_CTRL_DATASTART;
  #else
  AES->CTRL = AES_CTRL_DATASTART;
  #endif

  #if defined(AES_CTRL_KEYBUFEN)
  /* Load the key into high key for key buffer usage. */
  for (i = 3; i >= 0; i--) {
    AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
  }
  #endif

  /* Encrypt/decrypt data. */
  data = iv;
  len /= AES_BLOCKSIZE;
  while (len--) {
    #if !defined(AES_CTRL_KEYBUFEN)
    /* Load the key. */
    for (i = 3; i >= 0; i--) {
      AES->KEYLA = __REV_FROM_BYTEARRAY(&key[i * 4]);
    }
    #endif

    /* Load data and trigger encryption. */
    for (i = 3; i >= 0; i--) {
      AES->DATA = __REV_FROM_BYTEARRAY(&data[i * 4]);
    }

    /* Do some required processing before waiting for completion. */
    if (encrypt) {
      data = out;
    } else {
      /* Copy the current ciphertext block since it may be overwritten. */
      memcpy(tmp, in, AES_BLOCKSIZE);
      data = tmp;
    }

    /* Wait for completion. */
    while (AES->STATUS & AES_STATUS_RUNNING)
      ;

    /* Save encrypted/decrypted data. */
    for (i = 3; i >= 0; i--) {
      __UINT_TO_BYTEARRAY(&out[i * 4], __REV(AES->DATA) ^ __UINT_FROM_BYTEARRAY(&in[i * 4]));
    }
    out += AES_BLOCKSIZE;
    in  += AES_BLOCKSIZE;
  }
}

#if defined(AES_CTRL_AES256)
/***************************************************************************//**
 * @brief
 *   Cipher feedback (CFB) cipher mode encryption/decryption, 256 bit key.
 *
 * @details
 *   See AES_CFB128() for the CFB figure.
 *
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place encrypted/decrypted data. Must be at least @p len long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding data to encrypt/decrypt. Must be at least @p len long.
 *
 * @param[in] len
 *   A number of bytes to encrypt/decrypt. Must be a multiple of 16.
 *
 * @param[in] key
 *   256 bit encryption key is used for both encryption and decryption modes.
 *
 * @param[in] iv
 *   128 bit initialization vector to use.
 *
 * @param[in] encrypt
 *   Set to true to encrypt, false to decrypt.
 ******************************************************************************/
void AES_CFB256(uint8_t *out,
                const uint8_t *in,
                unsigned int len,
                const uint8_t *key,
                const uint8_t *iv,
                bool encrypt)
{
  int            i;
  int            j;
  const uint8_t  *data;
  uint8_t        tmp[AES_BLOCKSIZE];

  EFM_ASSERT(!(len % AES_BLOCKSIZE));

  /* Select encryption mode. */
  AES->CTRL = AES_CTRL_AES256 | AES_CTRL_DATASTART;

  /* Encrypt/decrypt data. */
  data = iv;
  len /= AES_BLOCKSIZE;
  while (len--) {
    /* Load the key and block to be encrypted/decrypted. */
    for (i = 3, j = 7; i >= 0; i--, j--) {
      AES->KEYLA = __REV_FROM_BYTEARRAY(&key[j * 4]);
      AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
      /* Write data last, since will trigger encryption on last iteration. */
      AES->DATA = __REV_FROM_BYTEARRAY(&data[i * 4]);
    }

    /* Do some required processing before waiting for completion. */
    if (encrypt) {
      data = out;
    } else {
      /* Copy the current ciphertext block since it may be overwritten. */
      memcpy(tmp, in, AES_BLOCKSIZE);
      data = tmp;
    }

    while (AES->STATUS & AES_STATUS_RUNNING)
      ;

    /* Save encrypted/decrypted data. */
    for (i = 3; i >= 0; i--) {
      __UINT_TO_BYTEARRAY(&out[i * 4], __REV(AES->DATA) ^ __UINT_FROM_BYTEARRAY(&in[i * 4]));
    }
    out += AES_BLOCKSIZE;
    in  += AES_BLOCKSIZE;
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Counter (CTR) cipher mode encryption/decryption, 128 bit key.
 *
 * @details
 *   Encryption:
 * @verbatim
 *           Counter                    Counter
 *              |                          |
 *              V                          V
 *       +--------------+           +--------------+
 * Key ->| Block cipher |     Key ->| Block cipher |
 *       |  encryption  |           |  encryption  |
 *       +--------------+           +--------------+
 *              |                          |
 * Plaintext ->XOR            Plaintext ->XOR
 *              |                          |
 *              V                          V
 *         Ciphertext                 Ciphertext
 * @endverbatim
 *   Decryption:
 * @verbatim
 *           Counter                    Counter
 *              |                          |
 *              V                          V
 *       +--------------+           +--------------+
 * Key ->| Block cipher |     Key ->| Block cipher |
 *       |  encryption  |           |  encryption  |
 *       +--------------+           +--------------+
 *               |                          |
 * Ciphertext ->XOR           Ciphertext ->XOR
 *               |                          |
 *               V                          V
 *           Plaintext                  Plaintext
 * @endverbatim
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place encrypted/decrypted data. Must be at least @p len long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding data to encrypt/decrypt. Must be at least @p len long.
 *
 * @param[in] len
 *   A number of bytes to encrypt/decrypt. Must be a multiple of 16.
 *
 * @param[in] key
 *   128 bit encryption key.
 *   On devices supporting key buffering this argument can be null. If so, the
 *   key will not be loaded, as it is assumed the key has been loaded
 *   into KEYHA previously.
 *
 * @param[in,out] ctr
 *   128 bit initial counter value. The counter is updated after each AES
 *   block encoding through use of @p ctrFunc.
 *
 * @param[in] ctrFunc
 *   A function used to update the counter value.
 ******************************************************************************/
void AES_CTR128(uint8_t *out,
                const uint8_t *in,
                unsigned int len,
                const uint8_t *key,
                uint8_t *ctr,
                AES_CtrFuncPtr_TypeDef ctrFunc)
{
  int            i;

  EFM_ASSERT(!(len % AES_BLOCKSIZE));
  EFM_ASSERT(ctrFunc);

  #if defined(AES_CTRL_KEYBUFEN)
  AES->CTRL = AES_CTRL_KEYBUFEN | AES_CTRL_DATASTART;
  #else
  AES->CTRL = AES_CTRL_DATASTART;
  #endif

  #if defined(AES_CTRL_KEYBUFEN)
  if (key) {
    /* Load the key into high key for key buffer usage. */
    for (i = 3; i >= 0; i--) {
      AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
    }
  }
  #endif

  /* Encrypt/decrypt data. */
  len /= AES_BLOCKSIZE;
  while (len--) {
    #if !defined(AES_CTRL_KEYBUFEN)
    /* Load the key. */
    for (i = 3; i >= 0; i--) {
      AES->KEYLA = __REV_FROM_BYTEARRAY(&key[i * 4]);
    }
    #endif

    /* Load ctr to be encrypted/decrypted. */
    for (i = 3; i >= 0; i--) {
      AES->DATA = __REV_FROM_BYTEARRAY(&ctr[i * 4]);
    }
    /* Increment ctr for the next use. */
    ctrFunc(ctr);

    /* Wait for completion. */
    while (AES->STATUS & AES_STATUS_RUNNING)
      ;

    /* Save encrypted/decrypted data. */
    for (i = 3; i >= 0; i--) {
      __UINT_TO_BYTEARRAY(&out[i * 4], __REV(AES->DATA) ^ __UINT_FROM_BYTEARRAY(&in[i * 4]));
    }
    out += AES_BLOCKSIZE;
    in  += AES_BLOCKSIZE;
  }
}

#if defined(AES_CTRL_AES256)
/***************************************************************************//**
 * @brief
 *   Counter (CTR) cipher mode encryption/decryption, 256 bit key.
 *
 * @details
 *   See AES_CTR128() for CTR figure.
 *
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place encrypted/decrypted data. Must be at least @p len long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding data to encrypt/decrypt. Must be at least @p len long.
 *
 * @param[in] len
 *   A number of bytes to encrypt/decrypt. Must be a multiple of 16.
 *
 * @param[in] key
 *   256 bit encryption key.
 *
 * @param[in,out] ctr
 *   128 bit initial counter value. The counter is updated after each AES
 *   block encoding through use of @p ctrFunc.
 *
 * @param[in] ctrFunc
 *   Function used to update counter value.
 ******************************************************************************/
void AES_CTR256(uint8_t *out,
                const uint8_t *in,
                unsigned int len,
                const uint8_t *key,
                uint8_t *ctr,
                AES_CtrFuncPtr_TypeDef ctrFunc)
{
  int            i;
  int            j;

  EFM_ASSERT(!(len % AES_BLOCKSIZE));
  EFM_ASSERT(ctrFunc);

  /* Select encryption mode with auto trigger. */
  AES->CTRL = AES_CTRL_AES256 | AES_CTRL_DATASTART;

  /* Encrypt/decrypt data. */
  len /= AES_BLOCKSIZE;
  while (len--) {
    /* Load the key and block to be encrypted/decrypted. */
    for (i = 3, j = 7; i >= 0; i--, j--) {
      AES->KEYLA = __REV_FROM_BYTEARRAY(&key[j * 4]);
      AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
      /* Write data last, since will trigger encryption on last iteration. */
      AES->DATA = __REV_FROM_BYTEARRAY(&ctr[i * 4]);
    }
    /* Increment ctr for the next use. */
    ctrFunc(ctr);

    /* Wait for completion. */
    while (AES->STATUS & AES_STATUS_RUNNING)
      ;

    /* Save encrypted/decrypted data. */
    for (i = 3; i >= 0; i--) {
      __UINT_TO_BYTEARRAY(&out[i * 4], __REV(AES->DATA) ^ __UINT_FROM_BYTEARRAY(&in[i * 4]));
    }
    out += AES_BLOCKSIZE;
    in  += AES_BLOCKSIZE;
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Update last 32 bits of 128 bit counter by incrementing with 1.
 *
 * @details
 *   Notice that no special consideration is given to possible wrap around. If
 *   32 least significant bits are 0xFFFFFFFF, they will be updated to 0x00000000,
 *   ignoring overflow.
 *
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[in,out] ctr
 *   A buffer holding 128 bit counter to be updated.
 ******************************************************************************/
void AES_CTRUpdate32Bit(uint8_t *ctr)
{
  __REV_TO_BYTEARRAY(&ctr[12], __REV_FROM_BYTEARRAY(&ctr[12]) + 1);
}

/***************************************************************************//**
 * @brief
 *   Generate a 128 bit decryption key from the 128 bit encryption key. The decryption
 *   key is used for some cipher modes when decrypting.
 *
 * @details
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place 128 bit decryption key. Must be at least 16 bytes long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding 128 bit encryption key. Must be at least 16 bytes long.
 ******************************************************************************/
void AES_DecryptKey128(uint8_t *out, const uint8_t *in)
{
  int            i;

  /* Load key */
  for (i = 3; i >= 0; i--) {
    AES->KEYLA = __REV_FROM_BYTEARRAY(&in[i * 4]);
  }

  /* Do dummy encryption to generate decrypt key */
  AES->CTRL = 0;
  AES_IntClear(AES_IF_DONE);
  AES->CMD = AES_CMD_START;

  /* Wait for completion */
  while (AES->STATUS & AES_STATUS_RUNNING)
    ;

  /* Save decryption key */
  for (i = 3; i >= 0; i--) {
    __REV_TO_BYTEARRAY(&out[i * 4], AES->KEYLA);
  }
}

#if defined(AES_CTRL_AES256)
/***************************************************************************//**
 * @brief
 *   Generate a 256 bit decryption key from the 256 bit encryption key. The decryption
 *   key is used for some cipher modes when decrypting.
 *
 * @details
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place 256 bit decryption key. Must be at least 32 bytes long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding 256 bit encryption key. Must be at least 32 bytes long.
 ******************************************************************************/
void AES_DecryptKey256(uint8_t *out, const uint8_t *in)
{
  int            i;
  int            j;

  /* Load key */
  for (i = 3, j = 7; i >= 0; i--, j--) {
    AES->KEYLA = __REV_FROM_BYTEARRAY(&in[j * 4]);
    AES->KEYHA = __REV_FROM_BYTEARRAY(&in[i * 4]);
  }

  /* Do dummy encryption to generate decrypt key */
  AES->CTRL = AES_CTRL_AES256;
  AES->CMD  = AES_CMD_START;

  /* Wait for completion */
  while (AES->STATUS & AES_STATUS_RUNNING)
    ;

  /* Save decryption key */
  for (i = 3, j = 7; i >= 0; i--, j--) {
    __REV_TO_BYTEARRAY(&out[j * 4], AES->KEYLA);
    __REV_TO_BYTEARRAY(&out[i * 4], AES->KEYHA);
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Electronic Codebook (ECB) cipher mode encryption/decryption, 128 bit key.
 *
 * @details
 *   Encryption:
 * @verbatim
 *          Plaintext                  Plaintext
 *              |                          |
 *              V                          V
 *       +--------------+           +--------------+
 * Key ->| Block cipher |     Key ->| Block cipher |
 *       |  encryption  |           |  encryption  |
 *       +--------------+           +--------------+
 *              |                          |
 *              V                          V
 *         Ciphertext                 Ciphertext
 * @endverbatim
 *   Decryption:
 * @verbatim
 *         Ciphertext                 Ciphertext
 *              |                          |
 *              V                          V
 *       +--------------+           +--------------+
 * Key ->| Block cipher |     Key ->| Block cipher |
 *       |  decryption  |           |  decryption  |
 *       +--------------+           +--------------+
 *              |                          |
 *              V                          V
 *          Plaintext                  Plaintext
 * @endverbatim
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place encrypted/decrypted data. Must be at least @p len long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding data to encrypt/decrypt. Must be at least @p len long.
 *
 * @param[in] len
 *   A number of bytes to encrypt/decrypt. Must be a multiple of 16.
 *
 * @param[in] key
 *   When encrypting, this is the 128 bit encryption key. When
 *   decrypting, this is the 128 bit decryption key. The decryption key may
 *   be generated from the encryption key with AES_DecryptKey128().
 *
 * @param[in] encrypt
 *   Set to true to encrypt, false to decrypt.
 ******************************************************************************/
void AES_ECB128(uint8_t *out,
                const uint8_t *in,
                unsigned int len,
                const uint8_t *key,
                bool encrypt)
{
  int            i;

  EFM_ASSERT(!(len % AES_BLOCKSIZE));

  #if defined(AES_CTRL_KEYBUFEN)
  /* Load the key into high key for key buffer usage. */
  for (i = 3; i >= 0; i--) {
    AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
  }
  #endif

  if (encrypt) {
    /* Select encryption mode. */
    #if defined(AES_CTRL_KEYBUFEN)
    AES->CTRL = AES_CTRL_KEYBUFEN | AES_CTRL_DATASTART;
    #else
    AES->CTRL = AES_CTRL_DATASTART;
    #endif
  } else {
    /* Select decryption mode. */
    #if defined(AES_CTRL_KEYBUFEN)
    AES->CTRL = AES_CTRL_DECRYPT | AES_CTRL_KEYBUFEN | AES_CTRL_DATASTART;
    #else
    AES->CTRL = AES_CTRL_DECRYPT | AES_CTRL_DATASTART;
    #endif
  }

  /* Encrypt/decrypt data. */
  len /= AES_BLOCKSIZE;
  while (len--) {
    #if !defined(AES_CTRL_KEYBUFEN)
    /* Load the key. */
    for (i = 3; i >= 0; i--) {
      AES->KEYLA = __REV_FROM_BYTEARRAY(&key[i * 4]);
    }
    #endif

    /* Load a block to be encrypted/decrypted. */
    for (i = 3; i >= 0; i--) {
      AES->DATA = __REV_FROM_BYTEARRAY(&in[i * 4]);
    }
    in += AES_BLOCKSIZE;

    /* Wait for completion. */
    while (AES->STATUS & AES_STATUS_RUNNING)
      ;

    /* Save encrypted/decrypted data. */
    for (i = 3; i >= 0; i--) {
      __REV_TO_BYTEARRAY(&out[i * 4], AES->DATA);
    }
    out += AES_BLOCKSIZE;
  }
}

#if defined(AES_CTRL_AES256)
/***************************************************************************//**
 * @brief
 *   Electronic Codebook (ECB) cipher mode encryption/decryption, 256 bit key.
 *
 * @details
 *   See AES_ECB128() for the ECB figure.
 *
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place encrypted/decrypted data. Must be at least @p len long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding data to encrypt/decrypt. Must be at least @p len long.
 *
 * @param[in] len
 *   A number of bytes to encrypt/decrypt. Must be a multiple of 16.
 *
 * @param[in] key
 *   When encrypting, this is the 256 bit encryption key. When
 *   decrypting, this is the 256 bit decryption key. The decryption key may
 *   be generated from the encryption key with AES_DecryptKey256().
 *
 * @param[in] encrypt
 *   Set to true to encrypt, false to decrypt.
 ******************************************************************************/
void AES_ECB256(uint8_t *out,
                const uint8_t *in,
                unsigned int len,
                const uint8_t *key,
                bool encrypt)
{
  int            i;
  int            j;

  EFM_ASSERT(!(len % AES_BLOCKSIZE));

  if (encrypt) {
    /* Select encryption mode. */
    AES->CTRL = AES_CTRL_AES256 | AES_CTRL_DATASTART;
  } else {
    /* Select decryption mode. */
    AES->CTRL = AES_CTRL_DECRYPT | AES_CTRL_AES256 | AES_CTRL_DATASTART;
  }

  /* Encrypt/decrypt data. */
  len /= AES_BLOCKSIZE;
  while (len--) {
    /* Load the key and block to be encrypted/decrypted. */
    for (i = 3, j = 7; i >= 0; i--, j--) {
      AES->KEYLA = __REV_FROM_BYTEARRAY(&key[j * 4]);
      AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
      /* Write data last, since will trigger encryption on last iteration. */
      AES->DATA = __REV_FROM_BYTEARRAY(&in[i * 4]);
    }
    in += AES_BLOCKSIZE;

    /* Wait for completion. */
    while (AES->STATUS & AES_STATUS_RUNNING)
      ;

    /* Save encrypted/decrypted data. */
    for (i = 3; i >= 0; i--) {
      __REV_TO_BYTEARRAY(&out[i * 4], AES->DATA);
    }
    out += AES_BLOCKSIZE;
  }
}
#endif

/***************************************************************************//**
 * @brief
 *   Output feedback (OFB) cipher mode encryption/decryption, 128 bit key.
 *
 * @details
 *   Encryption:
 * @verbatim
 *          InitVector    +----------------+
 *              |         |                |
 *              V         |                V
 *       +--------------+ |        +--------------+
 * Key ->| Block cipher | |  Key ->| Block cipher |
 *       |  encryption  | |        |  encryption  |
 *       +--------------+ |        +--------------+
 *              |         |                |
 *              |---------+                |
 *              V                          V
 * Plaintext ->XOR            Plaintext ->XOR
 *              |                          |
 *              V                          V
 *         Ciphertext                 Ciphertext
 * @endverbatim
 *   Decryption:
 * @verbatim
 *          InitVector    +----------------+
 *              |         |                |
 *              V         |                V
 *       +--------------+ |        +--------------+
 * Key ->| Block cipher | |  Key ->| Block cipher |
 *       |  encryption  | |        |  encryption  |
 *       +--------------+ |        +--------------+
 *              |         |                |
 *              |---------+                |
 *              V                          V
 * Ciphertext ->XOR           Ciphertext ->XOR
 *              |                          |
 *              V                          V
 *          Plaintext                  Plaintext
 * @endverbatim
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place encrypted/decrypted data. Must be at least @p len long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding data to encrypt/decrypt. Must be at least @p len long.
 *
 * @param[in] len
 *   A number of bytes to encrypt/decrypt. Must be a multiple of 16.
 *
 * @param[in] key
 *   128 bit encryption key.
 *
 * @param[in] iv
 *   128 bit initialization vector to use.
 ******************************************************************************/
void AES_OFB128(uint8_t *out,
                const uint8_t *in,
                unsigned int len,
                const uint8_t *key,
                const uint8_t *iv)
{
  int            i;

  EFM_ASSERT(!(len % AES_BLOCKSIZE));

  /* Select encryption mode, trigger explicitly by command. */
  #if defined(AES_CTRL_KEYBUFEN)
  AES->CTRL = AES_CTRL_KEYBUFEN;
  #else
  AES->CTRL = 0;
  #endif

  /* Load the key into high key for key buffer usage. */
  /* Load the initialization vector. */
  for (i = 3; i >= 0; i--) {
    #if defined(AES_CTRL_KEYBUFEN)
    AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
    #endif
    AES->DATA  = __REV_FROM_BYTEARRAY(&iv[i * 4]);
  }

  /* Encrypt/decrypt data. */
  len /= AES_BLOCKSIZE;
  while (len--) {
    #if !defined(AES_CTRL_KEYBUFEN)
    /* Load the key. */
    for (i = 3; i >= 0; i--) {
      AES->KEYLA = __REV_FROM_BYTEARRAY(&key[i * 4]);
    }
    #endif

    AES->CMD = AES_CMD_START;

    /* Wait for completion. */
    while (AES->STATUS & AES_STATUS_RUNNING)
      ;

    /* Save encrypted/decrypted data. */
    for (i = 3; i >= 0; i--) {
      __UINT_TO_BYTEARRAY(&out[i * 4], __REV(AES->DATA) ^ __UINT_FROM_BYTEARRAY(&in[i * 4]));
    }
    out += AES_BLOCKSIZE;
    in  += AES_BLOCKSIZE;
  }
}

#if defined(AES_CTRL_AES256)
/***************************************************************************//**
 * @brief
 *   Output feedback (OFB) cipher mode encryption/decryption, 256 bit key.
 *
 * @details
 *   See AES_OFB128() for OFB figure.
 *
 *   See general comments on layout and byte ordering of parameters.
 *
 * @param[out] out
 *   A buffer to place encrypted/decrypted data. Must be at least @p len long. It
 *   may be set equal to @p in, in which case the input buffer is overwritten.
 *
 * @param[in] in
 *   A buffer holding data to encrypt/decrypt. Must be at least @p len long.
 *
 * @param[in] len
 *   A number of bytes to encrypt/decrypt. Must be a multiple of 16.
 *
 * @param[in] key
 *   256 bit encryption key.
 *
 * @param[in] iv
 *   128 bit initialization vector to use.
 ******************************************************************************/
void AES_OFB256(uint8_t *out,
                const uint8_t *in,
                unsigned int len,
                const uint8_t *key,
                const uint8_t *iv)
{
  int            i;
  int            j;

  EFM_ASSERT(!(len % AES_BLOCKSIZE));

  /* Select encryption mode, trigger explicitly by command. */
  AES->CTRL = AES_CTRL_AES256;

  /* Load the initialization vector. */
  for (i = 3; i >= 0; i--) {
    AES->DATA = __REV_FROM_BYTEARRAY(&iv[i * 4]);
  }

  /* Encrypt/decrypt data. */
  len /= AES_BLOCKSIZE;
  while (len--) {
    /* Load the key. */
    for (i = 3, j = 7; i >= 0; i--, j--) {
      AES->KEYLA = __REV_FROM_BYTEARRAY(&key[j * 4]);
      AES->KEYHA = __REV_FROM_BYTEARRAY(&key[i * 4]);
    }

    AES->CMD = AES_CMD_START;

    /* Wait for completion. */
    while (AES->STATUS & AES_STATUS_RUNNING)
      ;

    /* Save encrypted/decrypted data. */
    for (i = 3; i >= 0; i--) {
      __UINT_TO_BYTEARRAY(&out[i * 4], __REV(AES->DATA) ^ __UINT_FROM_BYTEARRAY(&in[i * 4]));
    }
    out += AES_BLOCKSIZE;
    in  += AES_BLOCKSIZE;
  }
}
#endif

/** @} (end addtogroup aes) */
#endif /* defined(AES_COUNT) && (AES_COUNT > 0) */
