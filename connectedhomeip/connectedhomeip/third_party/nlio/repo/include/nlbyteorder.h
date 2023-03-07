/**
 *    Copyright (c) 2020 nlio Authors. All Rights Reserved.
 *    Copyright 2012-2017 Nest Labs Inc. All Rights Reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      This file defines macros for performing in place byte-
 *      swapping of compile-time constants via the C preprocessor as
 *      well as functions for performing byte-swapping by value and in
 *      place by pointer for 16-, 32-, and 64-bit types.
 */

#ifndef NLBYTEORDER_H
#define NLBYTEORDER_H

#include <nlio-private.h>

#include <stdint.h>

/*
 * If we are compiling under clang, GCC, or any such compatible
 * compiler, in which -fno-builtins or -ffreestanding might be
 * asserted, thereby eliminating built-in function optimization, we
 * STILL want to leverage built-in bswap{16,32,64}, if available. We
 * want this because it allows the compiler to use
 * architecture-specific machine instructions or inline code
 * generation to optimize an otherwise-generic and non-optimized code
 * for byte reordering, which is exactly the kind of efficiency that
 * would be expected of nlByteOrder.
 */

#if __nlIOHasBuiltin(__builtin_bswap16)
#define __nlBYTEORDER_BSWAP16 __builtin_bswap16
#else
#define __nlBYTEORDER_BSWAP16 nlByteOrderConstantSwap16
#endif

#if __nlIOHasBuiltin(__builtin_bswap32)
#define __nlBYTEORDER_BSWAP32 __builtin_bswap32
#else
#define __nlBYTEORDER_BSWAP32 nlByteOrderConstantSwap32
#endif

#if __nlIOHasBuiltin(__builtin_bswap64)
#define __nlBYTEORDER_BSWAP64 __builtin_bswap64
#else
#define __nlBYTEORDER_BSWAP64 nlByteOrderConstantSwap64
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @def nlByteOrderConstantSwap16
 *
 *  @brief
 *     Performs a preprocessor-compatible in place byte swap of the
 *     provided 16-bit value.
 *
 */
#define nlByteOrderConstantSwap16(c)                                 \
    ((uint16_t)                                                      \
        ((((uint16_t)(c) & (uint16_t)0x00ffU) << 8) |                \
         (((uint16_t)(c) & (uint16_t)0xff00U) >> 8)))

/**
 *  @def nlByteOrderConstantSwap32
 *
 *  @brief
 *     Performs a preprocessor-compatible in place byte swap of the
 *     provided 32-bit value.
 *
 */
#define nlByteOrderConstantSwap32(c)                                 \
    ((uint32_t)                                                      \
        ((((uint32_t)(c) & (uint32_t)0x000000ffUL) << 24) |          \
         (((uint32_t)(c) & (uint32_t)0x0000ff00UL) <<  8) |          \
         (((uint32_t)(c) & (uint32_t)0x00ff0000UL) >>  8) |          \
         (((uint32_t)(c) & (uint32_t)0xff000000UL) >> 24)))

/**
 *  @def nlByteOrderConstantSwap64
 *
 *  @brief
 *     Performs a preprocessor-compatible in place byte swap of the
 *     provided 64-bit value.
 *
 */
#define nlByteOrderConstantSwap64(c)                                 \
    ((uint64_t)                                                      \
        ((((uint64_t)(c) & (uint64_t)0x00000000000000ffULL) << 56) | \
         (((uint64_t)(c) & (uint64_t)0x000000000000ff00ULL) << 40) | \
         (((uint64_t)(c) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
         (((uint64_t)(c) & (uint64_t)0x00000000ff000000ULL) <<  8) | \
         (((uint64_t)(c) & (uint64_t)0x000000ff00000000ULL) >>  8) | \
         (((uint64_t)(c) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
         (((uint64_t)(c) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
         (((uint64_t)(c) & (uint64_t)0xff00000000000000ULL) >> 56)))

/**
 *  @def NLBYTEORDER_LITTLE_ENDIAN
 *
 *  @brief
 *     Constant preprocessor definition used to test #NLBYTEORDER
 *     against to determine whether the target system uses little
 *     endian byte ordering.
 *
 *  @code
 *  #if NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN
 *  
 *      Do something that is little endian byte ordering-specific.
 *
 *  #endif
 *  @endcode
 *
 */
#define NLBYTEORDER_LITTLE_ENDIAN       0x1234

/**
 *  @def NLBYTEORDER_BIG_ENDIAN
 *
 *  @brief
 *     Constant preprocessor definition used to test #NLBYTEORDER
 *     against to determine whether the target system uses big
 *     endian byte ordering.
 *
 *  @code
 *  #if NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN
 *  
 *      Do something that is little endian byte ordering-specific.
 *
 *  #endif
 *  @endcode
 *
 */
#define NLBYTEORDER_BIG_ENDIAN          0x4321

/**
 *  @def NLBYTEORDER_UNKNOWN_ENDIAN
 *
 *  @brief
 *     Constant preprocessor definition used to test #NLBYTEORDER
 *     against to determine whether the target system uses unknown
 *     byte ordering.
 *
 *  @code
 *  #elif NLBYTEORDER == NLBYTEORDER_UNKNOWN_ENDIAN
 *  #error "Unknown byte ordering!"
 *  #endif
 *  @endcode
 *
 */
#define NLBYTEORDER_UNKNOWN_ENDIAN      0xFFFF

/**
 *  @def NLBYTEORDER
 *
 *  @brief
 *     Constant preprocessor definition containing the target system
 *     byte ordering. May be one of:
 *
 *       - NLBYTEORDER_BIG_ENDIAN
 *       - NLBYTEORDER_LITTLE_ENDIAN
 *       - NLBYTEORDER_UNKNOWN_ENDIAN
 *
 */
#if defined(__BYTE_ORDER__)
# if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define NLBYTEORDER NLBYTEORDER_LITTLE_ENDIAN
# elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define NLBYTEORDER NLBYTEORDER_BIG_ENDIAN
# endif /* __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ */
#elif defined(__LITTLE_ENDIAN__) && (__LITTLE_ENDIAN__ == 1)
#define NLBYTEORDER NLBYTEORDER_LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN__) && (__BIG_ENDIAN__ == 1)
#define NLBYTEORDER NLBYTEORDER_BIG_ENDIAN
#else
#error "Endianness undefined!"
#define NLBYTEORDER NLBYTEORDER_UNKNOWN_ENDIAN
#endif /* defined(__BYTE_ORDER__) */

enum {
    nlByteOrderUnknown	    = NLBYTEORDER_UNKNOWN_ENDIAN,
    nlByteOrderLittleEndian = NLBYTEORDER_LITTLE_ENDIAN,
    nlByteOrderBigEndian    = NLBYTEORDER_BIG_ENDIAN
};

/**
 * This represents a type for a byte ordering.
 */
typedef uint16_t nlByteOrder;

/**
 * This returns the byte order of the current system.
 *
 * @return The byte order of the current system.
 */
static inline nlByteOrder nlByteOrderGetCurrent(void)
{
#if (NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN)
    return nlByteOrderLittleEndian;
#elif (NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN)
    return nlByteOrderBigEndian;
#else
    return nlByteOrderUnknown;
#endif
}

/**
 * This unconditionally performs a byte order swap by value of the
 * specified 16-bit value.
 *
 * @param[in]  inValue  The 16-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint16_t nlByteOrderValueSwap16(uint16_t inValue)
{
    return __nlBYTEORDER_BSWAP16(inValue);
}

/**
 * This unconditionally performs a byte order swap by value of the
 * specified 32-bit value.
 *
 * @param[in]  inValue  The 32-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint32_t nlByteOrderValueSwap32(uint32_t inValue)
{
    return __nlBYTEORDER_BSWAP32(inValue);
}

/**
 * This unconditionally performs a byte order swap by value of the
 * specified 64-bit value.
 *
 * @param[in]  inValue  The 64-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint64_t nlByteOrderValueSwap64(uint64_t inValue)
{
    return __nlBYTEORDER_BSWAP64(inValue);
}

/**
 * This unconditionally performs a byte order swap by pointer in place
 * of the specified 16-bit value.
 *
 * @warning  The input value is assumed to be on a natural alignment
 * boundary for the target system. It is the responsibility of the
 * caller to perform any necessary alignment to avoid system faults
 * for systems that do not support unaligned accesses.
 *
 * @param[inout]  inValue  A pointer to the 16-bit value to be byte
 *                         order swapped.
 */
static inline void nlByteOrderPointerSwap16(uint16_t *inValue)
{
    *inValue = nlByteOrderValueSwap16(*inValue);
}

/**
 * This unconditionally performs a byte order swap by pointer in place
 * of the specified 32-bit value.
 *
 * @warning  The input value is assumed to be on a natural alignment
 * boundary for the target system. It is the responsibility of the
 * caller to perform any necessary alignment to avoid system faults
 * for systems that do not support unaligned accesses.
 *
 * @param[inout]  inValue  A pointer to the 32-bit value to be byte
 *                         order swapped.
 */
static inline void nlByteOrderPointerSwap32(uint32_t *inValue)
{
    *inValue = nlByteOrderValueSwap32(*inValue);
}

/**
 * This unconditionally performs a byte order swap by pointer in place
 * of the specified 64-bit value.
 *
 * @warning  The input value is assumed to be on a natural alignment
 * boundary for the target system. It is the responsibility of the
 * caller to perform any necessary alignment to avoid system faults
 * for systems that do not support unaligned accesses.
 *
 * @param[inout]  inValue  A pointer to the 64-bit value to be byte
 *                         order swapped.
 */
static inline void nlByteOrderPointerSwap64(uint64_t *inValue)
{
    *inValue = nlByteOrderValueSwap64(*inValue);
}

#if (NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN)
#include <nlbyteorder-little.h>
#elif (NLBYTEORDER == NLBYTEORDER_BIG_ENDIAN)
#include <nlbyteorder-big.h>
#endif /* (NLBYTEORDER == NLBYTEORDER_LITTLE_ENDIAN) */

#ifdef __cplusplus
}
#endif

#undef __nlBYTEORDER_BSWAP16
#undef __nlBYTEORDER_BSWAP32
#undef __nlBYTEORDER_BSWAP64

#endif /* NLBYTEORDER_H */


