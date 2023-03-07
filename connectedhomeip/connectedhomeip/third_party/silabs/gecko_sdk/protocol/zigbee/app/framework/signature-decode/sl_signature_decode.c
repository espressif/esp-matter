/***************************************************************************//**
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

/**
 * Functions for the signature based decoding.
 */
#include "sl_signature_decode.h"

// If you enable this macro, then you will get printouts
// of the message attempted to be decoding and the signatures
// of it in simulation.
//#define SL_SIGNATURE_DECODE_DEBUG 1

#ifdef SL_SIGNATURE_DECODE_DEBUG
#include <stdio.h>
#include <stdlib.h>
static void printDebugInfo(uint8_t *message,
                           uint16_t messageLength,
                           uint16_t payloadOffset,
                           const sl_signature_t signature
                           )
{
  int i;
  fprintf(stderr, "==> Signature decode:\n  msg: ");
  for ( i = payloadOffset; i < messageLength; i++ ) {
    fprintf(stderr, " %02x", message[i]);
  }
  fprintf(stderr, "\n  sig:");
  for ( i = 0; i < signature[0]; i++ ) {
    uint8_t fieldSpec = signature[1 + 2 * i];
    uint8_t fieldOffset = signature[2 + 2 * i];
    fprintf(stderr, " (%02x / %02x) ", fieldSpec, fieldOffset);
  }
  fprintf(stderr, "<==\n");
}
#endif // SL_SIGNATURE_DECODE_DEBUG

// retrieves an uint32_t which contains between 1 and 4 bytes of relevent data
// depending on number of bytes requested.
// If there is not enough data in the message, this function returns 0.
uint32_t sl_signature_decode_int(const uint8_t* message,
                                 uint16_t currentIndex,
                                 uint16_t msgLen,
                                 uint8_t bytes)
{
  uint32_t result = 0;
  uint8_t i = bytes;
  if ((currentIndex + bytes) > msgLen) {
    return 0;
  }
  // Little endian. This function does nothing useful for a big-endian buffer.
  while (i > 0) {
    result = (result << 8) + message[(currentIndex + i) - 1];
    i--;
  }
  return result;
}

// Main decoding function. See si_signature_decode.h for nicer user-readable comment.
sl_signature_decode_status_t sl_signature_decode(uint8_t *message,
                                                 uint16_t messageLength,
                                                 uint16_t payloadOffset,
                                                 const sl_signature_t signature,
                                                 uint8_t *destinationBuffer)
{
  // Running offset counter
  uint16_t offset = payloadOffset;

  // Source locations. Keep ints separately to let compiler deal with endian-sensitive memcpys.
  uint16_t int16buf;
  uint32_t int32buf;
  uint8_t *src;

  uint16_t increaseBy = 0;
  uint8_t destSize = 0; // If this is 0, then we assume a pointer.

  // Mask for the state of data. Bits mean following:
  //   0x01 - 1 = it's a pointer, 0 = it's a memcpy
#define DATA_IS_POINTER 0x01u
  //   0x02 - 1 = data missing, 0 = had enough data
#define DATA_IS_MISSING 0x02u
  uint8_t dataMask;

#ifdef SL_SIGNATURE_DECODE_DEBUG
  // In debug mode, we print out all the messages we're decoding.
  printDebugInfo(message, messageLength, payloadOffset, signature);
#endif
  for ( uint8_t signatureIndex = 0; signatureIndex < signature[0]; signatureIndex++ ) {
    uint8_t fieldSpec = signature[1 + 2 * signatureIndex];
    uint8_t fieldOffset = signature[2 + 2 * signatureIndex];
    dataMask = 0; // reset the state
    src = message + offset;

    if ( fieldSpec & SL_SIGNATURE_FIELD_MASK_BLOB ) {
      // Blobs are simple. We simply copy the number of
      // bytes that are specified in the low 6 bits.
      destSize = fieldSpec & SL_SIGNATURE_FIELD_MASK_LENGTH;
      increaseBy = fieldSpec & SL_SIGNATURE_FIELD_MASK_LENGTH;
      if ( offset + increaseBy > messageLength ) {
        dataMask |= DATA_IS_MISSING;
      }
    } else {
      switch (fieldSpec & SL_SIGNATURE_FIELD_MASK_LENGTH) {
        case SL_SIGNATURE_FIELD_1_BYTE:
          destSize = sizeof(uint8_t);
          increaseBy = 1;
          if ( offset >= messageLength ) {
            dataMask |= DATA_IS_MISSING;
          }
          break;
        case SL_SIGNATURE_FIELD_2_BYTES:
          increaseBy = 2;
          destSize = sizeof(uint16_t);
          if ( offset + increaseBy <= messageLength ) {
            int16buf = sl_signature_decode_int(message, offset, messageLength, 2);
          } else {
            dataMask |= DATA_IS_MISSING;
          }
          src = (uint8_t*)&int16buf;
          break;
        case SL_SIGNATURE_FIELD_3_BYTES:
          increaseBy = 3;
          destSize = sizeof(uint32_t);
          if ( offset + increaseBy <= messageLength ) {
            int32buf = sl_signature_decode_int(message, offset, messageLength, 3);
          } else {
            dataMask |= DATA_IS_MISSING;
          }
          src = (uint8_t*)&int32buf;
          break;
        case SL_SIGNATURE_FIELD_4_BYTES:
          increaseBy = 4;
          destSize = sizeof(uint32_t);
          if ( offset + increaseBy <= messageLength ) {
            int32buf = sl_signature_decode_int(message, offset, messageLength, 4);
          } else {
            dataMask |= DATA_IS_MISSING;
          }
          src = (uint8_t*)&int32buf;
          break;
        case SL_SIGNATURE_FIELD_LONG_STRING:
          dataMask |= DATA_IS_POINTER;
          destSize = sizeof(uint8_t*);
          if ( offset >= messageLength - 1 ) {
            dataMask |= DATA_IS_MISSING;
          } else {
            increaseBy = sl_signature_decode_int(message, offset, messageLength, 2);
            if ( increaseBy == 0xFFFFu ) {
              increaseBy = 2;
            } else {
              increaseBy += 2;
            }
            if ( offset + increaseBy > messageLength ) {
              dataMask |= DATA_IS_MISSING;
            }
          }
          break;
        case SL_SIGNATURE_FIELD_STRING:
          dataMask |= DATA_IS_POINTER;
          destSize = sizeof(uint8_t*);
          if ( offset >= messageLength ) {
            dataMask |= DATA_IS_MISSING;
          } else {
            increaseBy = message[offset];
            if ( increaseBy == 0xFFu ) {
              increaseBy = 1;
            } else {
              increaseBy++;
            }
            if ( offset + increaseBy > messageLength ) {
              dataMask |= DATA_IS_MISSING;
            }
          }
          break;
        case SL_SIGNATURE_FIELD_POINTER:
          dataMask |= DATA_IS_POINTER;
          destSize = sizeof(uint8_t*);
          increaseBy = 0;
          break;
        default:
          return SL_SIGNATURE_DECODE_INVALID_TYPE;
          break;
      }
    }

    if ( dataMask & DATA_IS_MISSING ) {
      // We don't have enough data...
      if ( sl_signature_is_field_optional(fieldSpec) ) {
        // ... but field is optional. Populate data with 0 and be done.
        // ASSUMPTION: NULL is all zeroes. Is it ever not?
        memset(destinationBuffer + fieldOffset, 0, destSize);
        // Consider the message to be fully consumed (if subsequent optional fields exist)
        offset = messageLength;
        increaseBy = 0;
      } else {
        // ... and field was not optional, so get out of here.
        return SL_SIGNATURE_DECODE_OUT_OF_DATA;
      }
    } else {
      // We do have enough data...
      if ( dataMask & DATA_IS_POINTER ) {
        // ... and we are simply pointing a pointer into the buffer.
        *((uint8_t**)(destinationBuffer + fieldOffset)) = (uint8_t*)src;
      } else {
        // ... and we are copying it to the destination location.
        memcpy(destinationBuffer + fieldOffset, src, destSize);
      }
    }

    // Now increase the offset
    offset += increaseBy;
  }
  return SL_SIGNATURE_DECODE_OK;
}
