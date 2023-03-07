/***************************************************************************//**
 * @file
 * @brief General purpose utilities.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_COMMON_H
#define SL_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "sl_assert.h"

#if !defined(__STATIC_INLINE)
#if !defined(__unix__) && defined(__arm__)
/* Compiler agnostic definitions */
#include "cmsis_compiler.h"
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#define __STATIC_INLINE static inline
#else
#warning Please provide a macro for your compiler and architecture
#define __STATIC_INLINE static
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup common COMMON - Common Utilities
 * @brief General purpose utilities and cross-compiler support
 * @details
 *  This SDK supports the following compilers/IDEs:
 *  @li Simplicity Studio
 *  @li IAR Embedded Workbench
 *  @li Keil µVision IDE
 *  @li Plain armgcc
 *
 * Certain compiler features such as alignment is implemented differently in the tools.
 * Therefore, macros such as @ref SL_ALIGN are provided to enable compiler independent
 * code.
 *
 * @note RAM code macros are implemented in [RAMFUNC](../../emlib/api).
 * Cross-compiler RAM code support needs extended documentation and it is therefore
 * implemented as a separate module.
 *
 * @{
 ******************************************************************************/

/** @brief Round n up to closest interval of i. */
#define SL_CEILING(n, i)   ((((n) + (i) - 1U) / (i)) * (i))

/** @brief Round n down to closest interval of i. */
#define SL_FLOOR(n, i) ((n / i) * i)

/** @brief Stringify X */
#define STRINGIZE(X) #X

#if !defined(__GNUC__)
/* Not GCC compilers */

/** @brief Macro for getting minimum value. */
#define SL_MIN(a, b) ((a) < (b) ? (a) : (b))

/** @brief Macro for getting maximum value. */
#define SL_MAX(a, b) ((a) > (b) ? (a) : (b))

/** @brief Macros for handling packed structures. */
#define SL_PACK_START(X) _Pragma(STRINGIZE(pack(X)))
#define SL_PACK_END()    _Pragma("pack()")
#define SL_ATTRIBUTE_PACKED

#if defined(__CC_ARM)
/** @brief MDK-ARM compiler: Macros for handling aligned structures. */
#define SL_ALIGN(X) __align(X)

/** MDK-ARM compiler: Macro for handling weak symbols. */
#define SL_WEAK __attribute__ ((weak))

/** MDK-ARM compiler: Macro for handling non-returning functions. */
#define SL_NORETURN __attribute__ ((noreturn))

/** MDK-ARM compiler: Macro for handling section placement */
#define SL_ATTRIBUTE_SECTION(X) __attribute__ ((section(X)))
#endif

#if defined(__ICCARM__)

#if (__VER__ >= 8000000)
/** @brief Obsoleted macro from version 8.00 and on . */
#define _STD_BEGIN
/** @brief Obsoleted macro from version 8.00 and on . */
#define _STD_END
#endif

/** @brief IAR Embedded Workbench: Macros for handling aligned structures. */
#define SL_ALIGN(X) _Pragma(STRINGIZE(data_alignment = X))

/** @brief IAR Embedded Workbench: Macros for handling weak symbols. */
#define SL_WEAK __weak

/** @brief IAR Embedded Workbench: Macro for handling non-returning functions. */
#define SL_NORETURN __noreturn

/* *INDENT-OFF* */
/** IAR Embedded Workbench: Macro for handling section placement */
#define SL_ATTRIBUTE_SECTION(X) @ X
#endif
/* *INDENT-ON* */

#define SL_ATTRIBUTE_ALIGN(X)

/** @brief Macro for notifying the compiler of an intended
 *  switch case fallthrough. */
#define SL_FALLTHROUGH

/** @brief A macro for notifying the compiler to ignore type limit check. */
#define SL_IGNORE_TYPE_LIMIT_BEGIN
#define SL_IGNORE_TYPE_LIMIT_END

#else // !defined(__GNUC__)
/* GCC compilers */

/** @brief A macro for getting the minimum value. No side-effects, a and b are evaluated one time only. */
#define SL_MIN(a, b) __extension__({ __typeof__(a)_a = (a); __typeof__(b)_b = (b); _a < _b ? _a : _b; })

/** @brief A macro for getting the maximum value. No side-effects, a and b are evaluated one time only. */
#define SL_MAX(a, b) __extension__({ __typeof__(a)_a = (a); __typeof__(b)_b = (b); _a > _b ? _a : _b; })

/** @brief A GCC style macro for handling packed structures. */
#define SL_ATTRIBUTE_PACKED __attribute__ ((packed))

/** @brief A macro for handling packed structures.
 *  @n Use this macro before the structure definition.
 *  @n X denotes the maximum alignment of structure members. X is not supported with
 *  GCC. GCC always uses 1 byte maximum alignment.
 */
#define SL_PACK_START(x)

/** @brief A macro for handling packed structures.
 *  @n Use this macro after the structure definition.
 *  @n With GCC, add SL_ATTRIBUTE_PACKED after the closing curly braces of the structure
 *  definition.
 */
#define SL_PACK_END()

/** @brief GCC style macro for aligning a variable. */
#define SL_ATTRIBUTE_ALIGN(X) __attribute__ ((aligned(X)))

/** @brief A macro for aligning a variable.
 *  @n Use this macro before the variable definition.
 *  @n X denotes the storage alignment value in bytes.
 *  @n To be GCC-compatible, use SL_ATTRIBUTE_ALIGN(X) before the semicolon on normal
 *  variables. Use SL_ATTRIBUTE_ALIGN(X) before the opening curly brace on structure variables.
 */
#define SL_ALIGN(X)

/** @brief A macro for defining a weak symbol. */
#define SL_WEAK __attribute__ ((weak))

/** @brief A macro for handling non-returning functions. */
#define SL_NORETURN __attribute__ ((noreturn))

/** A macro for placing a variable in a section.
 *  @n Use this macro after the variable definition, before the equal sign or a semicolon.
 *  @n X denotes the section to place the variable in.
 */
#define SL_ATTRIBUTE_SECTION(X) __attribute__ ((section(X)))

/** @brief A macro for notifying the compiler of an intended
 *  switch case fallthrough. */
#if __GNUC__ >= 7
  #define SL_FALLTHROUGH __attribute__ ((fallthrough));
#else
  #define SL_FALLTHROUGH
#endif

/** @brief A macro for notifying the compiler to ignore type limit check. */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
  #define SL_IGNORE_TYPE_LIMIT_BEGIN \
  _Pragma("GCC diagnostic push")     \
  _Pragma("GCC diagnostic ignored \"-Wtype-limits\"")
  #define SL_IGNORE_TYPE_LIMIT_END \
  _Pragma("GCC diagnostic pop")
#else
  #define SL_IGNORE_TYPE_LIMIT_BEGIN
  #define SL_IGNORE_TYPE_LIMIT_END
#endif

#endif // !defined(__GNUC__)

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** @brief
 *    Macro for marking deprecated functions
 *
 *  @details
 *    SL_DEPRECATED_API_SDK_<RELEASE> is used to mark functions that are
 *    deprecated and should not be used from a given version of the SDK.
 *    The accompanying SL_SUPPRESS_DEPRECATION_WARNINGS_SDK_<RELEASE>
 *    define can be set to suppress warnings generated when using
 *    deprecated APIs.
 */
#ifdef SL_SUPPRESS_DEPRECATION_WARNINGS_SDK_3_0
#define SL_DEPRECATED_API_SDK_3_0
#else
#define SL_DEPRECATED_API_SDK_3_0 __attribute__ ((deprecated))
#endif

#ifdef SL_SUPPRESS_DEPRECATION_WARNINGS_SDK_3_2
#define SL_DEPRECATED_API_SDK_3_2
#else
#define SL_DEPRECATED_API_SDK_3_2 __attribute__ ((deprecated))
#endif

#ifdef SL_SUPPRESS_DEPRECATION_WARNINGS_SDK_3_3
#define SL_DEPRECATED_API_SDK_3_3
#else
#define SL_DEPRECATED_API_SDK_3_3 __attribute__ ((deprecated))
#endif

#ifdef SL_SUPPRESS_DEPRECATION_WARNINGS_SDK_4_1
#define SL_DEPRECATED_API_SDK_4_1
#else
#define SL_DEPRECATED_API_SDK_4_1 __attribute__ ((deprecated))
#endif
/** @endcond */

/***************************************************************************//**
 * @brief
 *   Count trailing number of zeros. Use CLZ instruction if available.
 *
 * @param[in] value
 *   Data value to check for number of trailing zero bits.
 *
 * @return
 *   A number of trailing zeros in value.
 ******************************************************************************/
__STATIC_INLINE uint32_t SL_CTZ(uint32_t value)
{
#if defined(__CORTEX_M) && (__CORTEX_M >= 3U)
  return __CLZ(__RBIT(value));

#else
  uint32_t zeros;
  for (zeros = 0; (zeros < 32) && ((value & 0x1) == 0); zeros++, value >>= 1) {
    ;
  }
  return zeros;
#endif
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/* Deprecated function. New code should use @ref SL_CTZ. */
__STATIC_INLINE uint32_t EFM32_CTZ(uint32_t value)
{
  return SL_CTZ(value);
}
/** @endcond */

/***************************************************************************//**
 * @brief
 *   Reverse the bits. Use the RBIT instruction if available, else process.
 *
 * @param[in] value
 *   Data value to reverse.
 *
 * @return
 *   A reversed value.
 ******************************************************************************/
__STATIC_INLINE uint32_t SL_RBIT(uint32_t value)
{
  uint32_t result;

#if defined(__CORTEX_M) && (__CORTEX_M >= 0x03U)
  result = __RBIT(value);
#else
  int32_t s = 4 * 8 - 1;

  result = value;
  for (value >>= 1U; value != 0U; value >>= 1U) {
    result <<= 1U;
    result |= value & 1U;
    s--;
  }
  result <<= s;
#endif
  return result;
}

/***************************************************************************//**
 * @brief
 *   Reverse the bits. Use the RBIT instruction if available, else process.
 *
 * @param[in] value
 *   16-bit data value to reverse.
 *
 * @return
 *   A 16-bit reversed value.
 ******************************************************************************/
__STATIC_INLINE uint16_t SL_RBIT16(uint16_t value)
{
  return (uint16_t)(SL_RBIT(value) >> 16);
}

/***************************************************************************//**
 * @brief
 *   Reverse the bits. Use the RBIT instruction if available, else process.
 *
 * @param[in] value
 *   8-bit data value to reverse.
 *
 * @return
 *   A 8-bit reversed value.
 ******************************************************************************/
__STATIC_INLINE uint8_t SL_RBIT8(uint8_t value)
{
  return (uint8_t)(SL_RBIT(value) >> 24);
}

/***************************************************************************//**
 * @brief
 *   Convert logarithm of 2 to division factor.
 *
 * @param[in] log2
 *   Logarithm of 2.
 *
 * @return
 *   Dividend.
 ******************************************************************************/
__STATIC_INLINE uint32_t SL_Log2ToDiv(uint32_t log2)
{
  EFM_ASSERT(log2 < 32U);
  return 1UL << log2;
}

/** @} (end addtogroup common) */

#ifdef __cplusplus
}
#endif

#endif /* SL_COMMON_H */
