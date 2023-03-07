/***************************************************************************//**
 * @file
 * @brief This file implements HMAC (Hashed message authentication code)
 * calculation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "aes.h"

// Use a shorter, more accurate name here.
#define BLOCK_SIZE EMBER_ENCRYPTION_KEY_SIZE

//------------------------------------------------------------------------------

static void xorKeyWithByte(const uint8_t key[BLOCK_SIZE],
                           uint8_t byte,
                           uint8_t result[BLOCK_SIZE])
{
  uint8_t i;
  for (i = 0; i < BLOCK_SIZE; i++) {
    result[i] = key[i] ^ byte;
  }
}

// B.1.4 Keyed hash function for message authentication
// This is HMAC (see FIPS PUB 198) using the AES hash.  HMAC is a
// method for computing a hash from a key and a data message using
// a message-only hash function.
//
// This is all mystic commands from the ancients.  We are only
// following orders.
//
// Expects:
//   All Keys are EMBER_ENCRYPTION_SIZE (16 bytes) in length.
//
// Note:  '||' in this pseudo-code means concatenation
// 1. Hash ( ( Key ^ 0x36 ) || Data )
// 2. Hash ( ( Key ^ 0x5C ) || <Result of Step 1> )

void emberHmacAesHash(const uint8_t *key,
                      const uint8_t *data,
                      uint8_t dataLength,
                      uint8_t *result)
{
  uint8_t buffer[BLOCK_SIZE];
  uint8_t run;
  EmberAesMmoHashContext context;

  for (run = 0; run < 2; run++) {
    xorKeyWithByte(key,
                   (run == 0
                    ? 0x36
                    : 0x5C),
                   buffer);

    emberAesMmoHashInit(&context);
    emberAesMmoHashUpdate(&context, BLOCK_SIZE, buffer);
    emberAesMmoHashFinal(&context,
                         (run == 0
                          ? dataLength
                          : BLOCK_SIZE),
                         (run == 0
                          ? data
                          : result));
    MEMMOVE(result, context.result, BLOCK_SIZE);
  }
}
