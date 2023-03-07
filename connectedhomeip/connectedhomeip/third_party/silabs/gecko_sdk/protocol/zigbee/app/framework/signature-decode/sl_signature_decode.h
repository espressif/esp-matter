/*******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

/*
 * The prototypes and macros for the signature based decoding.
 */

#ifndef SL_AF_SIGNATURE_DECODE
#define SL_AF_SIGNATURE_DECODE 1

#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// ======= Types
typedef uint32_t sl_signature_decode_status_t;

typedef uint8_t sl_signature_t[];

// ======= Macros

// Everything was ok
#define SL_SIGNATURE_DECODE_OK             0x00u

// General error
#define SL_SIGNATURE_DECODE_ERROR          0x01u

// Decoding would require more data.
#define SL_SIGNATURE_DECODE_OUT_OF_DATA    0x02u

// Invalid field type.
#define SL_SIGNATURE_DECODE_INVALID_TYPE   0x03u

// 1 byte field. Byte will be copied to a destination location uint8_t.
#define SL_SIGNATURE_FIELD_1_BYTE (0x01u)

// 2 byte word. Word will be copied to a destination location uint16_t, assuming little-endianess in the incoming buffer.
#define SL_SIGNATURE_FIELD_2_BYTES (0x02u)

// 3 byte int. It will be copied to a destination location uint32_t, assuming little-endianess in the incoming buffer.
#define SL_SIGNATURE_FIELD_3_BYTES (0x03u)

// 4 byte int. It will be copied to a destination location uint32_t, assuming little-endianess in the incoming buffer.
#define SL_SIGNATURE_FIELD_4_BYTES (0x04u)

// Field is a 1-byte length prefixed array of characters.
// Destination location should be an uint8_t *, which will point to
// the length byte.
#define SL_SIGNATURE_FIELD_STRING (0x3Du)

// Field is a 2-byte length prefixed array of characters.
// Destination location should be an uint8_t *, which will point to
// first byte of the length word.
#define SL_SIGNATURE_FIELD_LONG_STRING (0x3Eu)

// Field is a generic pointer without known size. The field
// is expected to be an uin8_t, and there is NO INCREASE IN OFFSET.
// You will typically use this for a final array.
#define SL_SIGNATURE_FIELD_POINTER (0x3Fu)

// 0x80 bit is an "optional" mask. If you set this bit,
// then the decoder will not complain if the field is
// missing.
#define SL_SIGNATURE_FIELD_MASK_OPTIONAL_FIELD (0x80u)

// 0x40 bit is a blob mask. Remaining 6 bits are the length of the blob.
// Blob will be copied into the provided buffer, so
// make sure the provided buffer has space.
// The FIELD_STRING and FIELD_LONG_STRING and GENERIC_POINTER
// are reserved
// values, so make sure you don't use the 0x3F or
// 0x3E or 0x3D for the length, since that will short-cut into
// assumption the field is a string or long string respectively.
#define SL_SIGNATURE_FIELD_MASK_BLOB (0x40u)

// Remaining 6 bits are the length bits.
#define SL_SIGNATURE_FIELD_MASK_LENGTH (0x3Fu)

// If the field spec has this value, then the next
// byte will provide additional instruction.
// This enables ability of the framework to escape beyond
// using a single bit for a spec, and do something more
// complicated.
// Note: at time of this writing, there is no special
// handling, but if they get added over time, add them
// here.
#define SL_SIGNATURE_FIELD_EXTENDED_DECODING (0xFFu)

#define sl_signature_is_field_optional(fieldSpec) ((fieldSpec) & SL_SIGNATURE_FIELD_MASK_OPTIONAL_FIELD)

// ======= Functions

/**
 * @brief Decoding function which receives an OTA message,
 * a signature of data, and the pointer to destination location,
 * typically a struct pointer.
 *
 * @param message Message buffer
 * @param messageLength Message length
 * @param payloadOffset
 * @param signature
 * @param destinationBuffer Pointer to destination buffer
 *
 * @return sl_signature_decode_status_t:
 *   SL_SIGNATURE_DECODE_OK - all good
 *   SL_SIGNATURE_DECODE_ERROR - generic error, unknown cause
 *   SL_SIGNATURE_DECODE_OUT_OF_DATA - ran out of data, but some non-optional fields haven't decoded.
 *   SL_SIGNATURE_DECODE_INVALID_TYPE - invalid field type was encountered during decoding
 */
sl_signature_decode_status_t sl_signature_decode(uint8_t *message,
                                                 uint16_t messageLength,
                                                 uint16_t payloadOffset,
                                                 const sl_signature_t signature,
                                                 uint8_t *destinationBuffer);

/**
 * @brief simple function that decodes an int from a little-endian input
 * buffer. This is intended to decode from the incoming ZCL message buffer
 * which is always little endian.
 *
 * @param message Pointer to message buffer
 * @param currentIndex
 * @param msgLen Message length
 * @param bytes Number of bytes requested
 *
 * @return uint32_t decoded intß
 */
uint32_t sl_signature_decode_int(const uint8_t* message,
                                 uint16_t currentIndex,
                                 uint16_t msgLen,
                                 uint8_t bytes);

#endif // SL_AF_SIGNATURE_DECODE
