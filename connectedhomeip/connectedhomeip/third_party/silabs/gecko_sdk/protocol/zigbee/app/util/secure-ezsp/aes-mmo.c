/***************************************************************************//**
 * @file
 * @brief Routines for performing AES-MMO hash on a block of data.
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

//------------------------------------------------------------------------------

// For a small hash we have a single byte 0x80 concatenated with zeroes
// and then append the length of the string in bits (up to 16 bits)
#define SMALL_BIT_STRING_LENGTH 3  // bytes

// For a big hash we have a single byte 0x80 concatenated with zeroes
// and then append the length of the string in bits (up to 32-bits)
#define BIG_BIT_STRING_LENGTH   7  // ""

// Per the spec the max size limit for this hash is 2^32 - 1 bits.

// The -1 is because the max size is limited by its representation as a bit
// string that can go from 0 -> 2^16 or 0 -> 2^32 bits.
#define MAX_SMALL_HASH_LIMIT    (8192 - 1)      // bytes
#define MAX_BIG_HASH_LIMIT      (536870912L - 1) // bytes (~512 megs)

#if defined AES_MMO_DEBUG
void debugPrintTextAndHexData(const char* text, const char* data, int length);
#else
#define debugPrintTextAndHexData(x, y, z)
#endif

#define BLOCK_SIZE EMBER_ENCRYPTION_KEY_SIZE

// This can only be turned on in simulation.
// #define HASH_DEBUG_PRINT

#if defined(EMBER_TEST) && defined(HASH_DEBUG_PRINT)
  #define simPrintText(...) fprintf(stderr, __VA_ARGS__)
#elif defined(HASH_DEBUG_PRINT)
EmberStatus emberSerialPrintf(uint8_t port, const char * formatString, ...);
EmberStatus emberSerialPrintfLine(uint8_t port, const char * formatString, ...);
EmberStatus emberSerialWaitSend(uint8_t port);
  #define HASH_DEBUG_PRINT_PORT 1
  #define simPrintText(...) emberSerialPrintf(HASH_DEBUG_PRINT_PORT, __VA_ARGS__)
#else
  #define simPrintTextAndHex(x, y)
  #define simPrintText(...)
#endif

//------------------------------------------------------------------------------

#if defined (EMBER_TEST) && defined(HASH_DEBUG_PRINT)

static void simPrintTextAndHex(const char* text, const uint8_t* data)
{
  fprintf(stderr,
          "%s: %02X%02X%02X%02X%02X%02X%02X%02X  %02X%02X%02X%02X%02X%02X%02X%02X\n",
          text,
          data[0],
          data[1],
          data[2],
          data[3],
          data[4],
          data[5],
          data[6],
          data[7],
          data[8],
          data[9],
          data[10],
          data[11],
          data[12],
          data[13],
          data[14],
          data[15]);
}

#elif defined(HASH_DEBUG_PRINT)

static void simPrintTextAndHex(const char * text, const uint8_t* data)
{
  (void) emberSerialPrintf(HASH_DEBUG_PRINT_PORT, "%p: ", text);
  (void) emberSerialPrintfLine(HASH_DEBUG_PRINT_PORT, " %X%X%X%X%X%X%X%X  %X%X%X%X%X%X%X%X",
                               data[0],
                               data[1],
                               data[2],
                               data[3],
                               data[4],
                               data[5],
                               data[6],
                               data[7],
                               data[8],
                               data[9],
                               data[10],
                               data[11],
                               data[12],
                               data[13],
                               data[14],
                               data[15]);
  (void) emberSerialWaitSend(HASH_DEBUG_PRINT_PORT);
}

#endif

static void aesHashNextBlock(const uint8_t *block, uint8_t *result)
{
  uint8_t i;
  uint8_t key[BLOCK_SIZE];

  simPrintTextAndHex("Input:     ", block);

  MEMCOPY(key, result, BLOCK_SIZE);
  MEMCOPY(result, block, BLOCK_SIZE);
  emAesEncrypt(result, key);

  for (i = 0; i < BLOCK_SIZE; i++) {
    result[i] ^= block[i];
  }

  simPrintTextAndHex("Hash Next: ", result);
}

// This is a convenience routine for hashing short blocks of data,
// less than 255 bytes.
EmberStatus emberAesHashSimple(uint8_t totalLength,
                               const uint8_t* data,
                               uint8_t* result)
{
  EmberStatus status;
  EmberAesMmoHashContext context;
  emberAesMmoHashInit(&context);
  status = emberAesMmoHashFinal(&context,
                                totalLength,
                                data);
  MEMCOPY(result, context.result, BLOCK_SIZE);
  return status;
}

void emberAesMmoHashInit(EmberAesMmoHashContext* context)
{
  MEMSET(context, 0, sizeof(EmberAesMmoHashContext));
}

// We require that the data be in multiples of BLOCK_SIZE for
// simplicity and only the final block can be be less than that.
EmberStatus emberAesMmoHashUpdate(EmberAesMmoHashContext* context,
                                  uint32_t length,
                                  const uint8_t* data)
{
  uint32_t moreDataLength = length;
  if (length % BLOCK_SIZE != 0) {
    return EMBER_INVALID_CALL;
  }

  for (;
       BLOCK_SIZE <= moreDataLength;
       data += BLOCK_SIZE,
       moreDataLength -= BLOCK_SIZE ) {
    aesHashNextBlock(data, context->result);
  }
  context->length += length;

  if (length > MAX_BIG_HASH_LIMIT) {
    return EMBER_INDEX_OUT_OF_RANGE;
  }
  return EMBER_SUCCESS;
}

EmberStatus emberAesMmoHashFinal(EmberAesMmoHashContext* context,
                                 uint32_t length,
                                 const uint8_t* finalData)
{
  bool bigHash = false;
  uint32_t bitSize;
  uint8_t remainingLength = 0;
  uint8_t temp[BLOCK_SIZE];
  MEMSET(temp, 0, BLOCK_SIZE);

  if (finalData && length > 0) {
    remainingLength = (uint8_t)(length & (BLOCK_SIZE - 1));
    if (length >= BLOCK_SIZE) {
      // Mask out the lower byte since hash update will hash
      // everything except the last piece, if the last piece
      // is less than 16 bytes.
      uint32_t hashedLength = (length & ~(BLOCK_SIZE - 1));
      EmberStatus status = emberAesMmoHashUpdate(context,
                                                 hashedLength,
                                                 finalData);
      if (EMBER_SUCCESS != status) {
        return status;
      }
      finalData += hashedLength;
    }

    if (length > 0) {
      MEMCOPY(temp, finalData, remainingLength);
    }
  }
  // Per the spec, Concatenate a 1 bit followed by all zero bits
  // (previous memset() on temp[] set the rest of the bits to zero)
  temp[remainingLength] = 0x80;

  context->length += remainingLength;
  bigHash = (context->length > MAX_SMALL_HASH_LIMIT
             ? true
             : false);

  // If appending the bit string will push us beyond the 16-byte boundary
  // we must hash that block and append another 16-byte block.
  if ((BLOCK_SIZE - remainingLength) < (bigHash
                                        ? BIG_BIT_STRING_LENGTH
                                        : SMALL_BIT_STRING_LENGTH)) {
    debugPrintTextAndHexData("Block boundary exceeded, hashing one more time:  ",
                             temp,
                             BLOCK_SIZE);

    emberAesMmoHashUpdate(context, BLOCK_SIZE, temp);

    debugPrintTextAndHexData("Hash: ",
                             context->result,
                             BLOCK_SIZE);

    // Since this extra data is due to the concatenation,
    // we remove that length.  We want the length of data only
    // and not the padding.
    context->length -= BLOCK_SIZE;
    MEMSET(temp, 0, BLOCK_SIZE);
  }

  bitSize = context->length * 8;
  if (bigHash) {
    temp[BLOCK_SIZE - 6] = (uint8_t)(bitSize >> 24);
    temp[BLOCK_SIZE - 5] = (uint8_t)(bitSize >> 16);
    temp[BLOCK_SIZE - 4] = (uint8_t)(bitSize >> 8);
    temp[BLOCK_SIZE - 3] = (uint8_t)(bitSize);
  } else {
    temp[BLOCK_SIZE - 2] = (uint8_t)(bitSize >> 8);
    temp[BLOCK_SIZE - 1] = (uint8_t)(bitSize);
  }

  debugPrintTextAndHexData("Final hash block: ", temp, BLOCK_SIZE);
  emberAesMmoHashUpdate(context, BLOCK_SIZE, temp);
  context->length -= BLOCK_SIZE;

  simPrintText("Hash complete\n");

  return EMBER_SUCCESS;
}
