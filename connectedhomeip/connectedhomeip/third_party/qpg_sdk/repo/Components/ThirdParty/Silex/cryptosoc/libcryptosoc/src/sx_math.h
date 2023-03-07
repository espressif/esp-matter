/**
 * @file
 * @brief Mathematic functions
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SX_MATH_H_
#define SX_MATH_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "sx_math_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include <stddef.h>

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_sx_math.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_sx_math.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */

/**
 * @brief Transform a 64-bit unsigned integer into a byte arrays.
 * @param in input value
 * @param out result destination. 8 bytes will be written.
 * @param little_endian 0: use big endian reprÃ©sentation, others: use little endian representation.
 */
void sx_math_u64_to_u8array(uint64_t in, uint8_t *out, uint32_t little_endian);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * @brief Verify if a byte array is different than 0.
 * @param in      input value
 * @param length  length of the array
 * @return 1      if the array is not all zeroes, 0 otherwise
 */
uint32_t sx_math_array_is_not_null(uint8_t *in, uint32_t length);


/**
 * @brief Get bit size of a value in an array of bytes
 * @param a array of bytes
 * @param length  length of the \p a
 * @return number of bits in \p a
 */
uint32_t sx_math_array_nbits(uint8_t *a, const size_t length);

/**
 * @brief A = A + 1. Increment an unsigned value stored as byte array.
 * @param a unsigned integer to increment, as a big endian byte array.
 * @param length size of \p a.
 * @param value the value to add to \p a
 */
void sx_math_array_increment(uint8_t *a, const size_t length, uint8_t value);

/**
 * @brief A = A + B. Add two unsigned values stored as byte arrays.
 * @param a unsigned integer to add, as a big endian byte array. Result will be placed here.
 * @param alen size of \p a.
 * @param b unsigned integer to add, as a big endian byte array.
 * @param blen size of \p b. Must be smaller or equal to \p alen.
 */
void sx_math_array_add(uint8_t *a, const size_t alen, const uint8_t *b, const size_t blen);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */
/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
