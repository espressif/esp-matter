/***************************************************************************/
/**
 * @file
 * @brief Data store and fetch routines.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 *
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 *
 * This software is distributed to you in Source Code format and is governed by
 * the sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __BYTE_UTIL_H__
#define __BYTE_UTIL_H__
#include <stdbool.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////

/**
 * @name  Byte Manipulation Macros
 */
//@{

/**
 * @brief Returns the low byte of the 16-bit value \c n as an \c uint8_t.
 */
#define LOW_BYTE(n) ((uint8_t)((n)&0xFF))

/**
 * @brief Returns the high byte of the 16-bit value \c n as an \c uint8_t.
 */
#define HIGH_BYTE(n) ((uint8_t)(LOW_BYTE((n) >> 8)))

/**
 * @brief Returns the value built from the two \c uint8_t
 * values \c high and \c low.
 */
#define HIGH_LOW_TO_INT(high, low) ((((uint16_t)(high)) << 8) + ((uint16_t)((low)&0xFF)))

/**
 * @brief Returns the low byte of the 32-bit value \c n as an \c uint8_t.
 */
#define BYTE_0(n) ((uint8_t)((n)&0xFF))

/**
 * @brief Returns the second byte of the 32-bit value \c n as an \c uint8_t.
 */
#define BYTE_1(n) BYTE_0((n) >> 8)

/**
 * @brief Returns the third byte of the 32-bit value \c n as an \c uint8_t.
 */
#define BYTE_2(n) BYTE_0((n) >> 16)

/**
 * @brief Returns the high byte of the 32-bit value \c n as an \c uint8_t.
 */
#define BYTE_3(n) BYTE_0((n) >> 24)

/**
 * @brief Returns the fifth byte of the 64-bit value \c n as an \c uint8_t.
 */
#define BYTE_4(n) BYTE_0((n) >> 32)

/**
 * @brief Returns the sixth byte of the 64-bit value \c n as an \c uint8_t.
 */
#define BYTE_5(n) BYTE_0((n) >> 40)

/**
 * @brief Returns the seventh byte of the 64-bit value \c n as an \c uint8_t.
 */
#define BYTE_6(n) BYTE_0((n) >> 48)

/**
 * @brief Returns the high byte of the 64-bit value \c n as an \c uint8_t.
 */
#define BYTE_7(n) BYTE_0((n) >> 56)

/**
 * @brief Returns the number of entries in an array.
 */
#define COUNTOF(a) (sizeof(a) / sizeof(a[0]))

//@} \\END Byte manipulation macros

/** @brief This function converts from a number of bits to the equivalent number of bytes.
 *
 * @param bits The number of bits.
 */
#define SL_BITS_TO_BYTES(bits) (((bits) + 7) >> 3)

/**
 * @brief This function checks if a target byte value is found within a byte array
 *
 * @param bytes A byte array to search
 * @param count Size of byte array
 * @param target Value to search for
 * @return true \c target is found in \c bytes
 * @return false No byte in \c bytes matches \c target
 */
bool sl_memory_byte_compare(const uint8_t *bytes, uint8_t count, uint8_t target);

/** @brief This function copies an array of bytes and reverses the order
 *  before writing the data to the destination.
 *
 * @param dest A pointer to the location where the data will be copied to.
 * @param src A pointer to the location where the data will be copied from.
 * @param length The length (in bytes) of the data to be copied.
 */
void sl_reverse_memcpy(uint8_t *dest, const uint8_t *src, uint8_t length);

/** @brief This function returns the value built from the two \c uint8_t values
 *  \c contents[0] and \c contents[1]. \c contents[0] is the low byte.
 */
uint16_t sl_fetch_low_high_int16u(const uint8_t *contents);

/** @brief This function returns the value built from the two \c uint8_t values
 *  \c contents[0] and \c contents[1]. \c contents[1] is the low byte.
 */
uint16_t sl_fetch_high_low_int16u(const uint8_t *contents);

/** @brief This function stores \c value in \c contents[0] and \c contents[1]. \c
 *  contents[0] is the low byte.
 */
void sl_store_low_high_int16u(uint8_t *contents, uint16_t value);

/** @brief This function stores \c value in \c contents[0] and \c contents[1]. \c
 *  contents[1] is the low byte.
 */
void sl_store_high_low_int16u(uint8_t *contents, uint16_t value);

#if !defined DOXYGEN_SHOULD_SKIP_THIS
uint32_t     sl_fetch_int32u(bool lowHigh, const uint8_t *contents);
#endif

/** @brief This function returns the value built from the four \c uint8_t values
 *  \c contents[0], \c contents[1], \c contents[2] and \c contents[3]. \c
 *  contents[0] is the low byte.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
uint32_t    sl_fetch_low_high_int32u(uint8_t *contents);
#else
#define sl_fetch_low_high_int32u(contents) (sl_fetch_int32u(true, contents))
#endif

/** @description This function returns the value built from the four \c uint8_t values
 *  \c contents[0], \c contents[1], \c contents[2] and \c contents[3]. \c
 *  contents[3] is the low byte.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
uint32_t    sl_fetch_high_low_int32u(uint8_t *contents);
#else
#define sl_fetch_high_low_int32u(contents) (sl_fetch_int32u(false, contents))
#endif

#if !defined DOXYGEN_SHOULD_SKIP_THIS
void         sl_store_int32u(bool lowHigh, uint8_t *contents, uint32_t value);
#endif

/** @brief This function stores \c value in \c contents[0], \c contents[1], \c
 *  contents[2] and \c contents[3]. \c contents[0] is the low byte.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
void        slStoreLowHighInt32u(uint8_t *contents, uint32_t value);
#else
#define slStoreLowHighInt32u(contents, value) (sl_store_int32u(true, contents, value))
#endif

#if !defined DOXYGEN_SHOULD_SKIP_THIS
void         sl_store_int48u(bool lowHigh, uint8_t *contents, uint64_t value);
#endif

/** @description This function stores \c value in \c contents[0], \c contents[1], \c
 *  contents[2] and \c contents[3]. \c contents[3] is the low byte.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
void        sl_store_high_low_int32u(uint8_t *contents, uint32_t value);
#else
#define sl_store_high_low_int32u(contents, value) (sl_store_int32u(false, contents, value))
#endif

#if !defined DOXYGEN_SHOULD_SKIP_THIS
uint64_t     sl_fetch_int48u(bool lowHigh, const uint8_t *contents);
#endif

/** @description This function stores \c value in \c contents[0] thru \c contents[5]. \c
 * contents[5] is the low byte.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
void        sl_store_high_low_int48u(uint8_t *contents, uint32_t value);
#else
#define sl_store_high_low_int48u(contents, value) (sl_store_int48u(false, contents, value))
#endif

/** @description This function returns the value built from the six \c uint8_t values
 *  \c contents[0] thru \c contents[5]. \c contents[5] is the low bytes.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
uint64_t    sl_fetch_high_low_int48u(uint8_t *contents);
#else
#define sl_fetch_high_low_int48u(contents) (sl_fetch_int48u(false, contents))
#endif

uint16_t sl_strlen(const uint8_t *const string);

int8_t sl_strcmp(const uint8_t *s1, const uint8_t *s2);

/** @brief This function returns the length in bits of the matching prefix
 * of \c x and \c y.
 */
uint16_t sl_matching_prefix_bit_length(const uint8_t *x, uint16_t xLength, const uint8_t *y, uint16_t yLength);

/** @brief This function copies \c count bits from \c from to \c to.
 */
void sl_bit_copy(uint8_t *to, const uint8_t *from, uint16_t count);

/** @brief This function returns the number of set bits in \c num.
 */
uint8_t sl_bit_count_int32u(uint32_t num);

/** @brief This function returns the value of hexadecimal digit ch (0 - 15).
 * This function returns a value > 15 if ch is not a hexadecimal digit.
 */
uint8_t sl_hex_to_int(uint8_t ch);

#endif // __BYTE_UTIL_H__
