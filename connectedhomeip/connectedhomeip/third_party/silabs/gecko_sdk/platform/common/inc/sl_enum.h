/*******************************************************************************
 * @file
 * @brief SL_ENUM Implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_ENUM_H
#define SL_ENUM_H

/*******************************************************************************
 * @addtogroup enum Enumerations
 * @brief Enumerations with stable binary representation
 * @details
 * Silicon Labs libraries do not use enumerations because the ARM EABI leaves
 * their size ambiguous, which causes problems if the application is built
 * with different flags than the library. Instead, uint8_t typedefs
 * are used in compiled code for all enumerations. For documentation purposes,
 * this is converted to an actual enumeration in documentation.
 * @{
 ******************************************************************************/

#ifdef DOXYGEN
/// Enumeration mapped to uint8_t
#define SL_ENUM(name) enum name
/// Enumeration mapped to arbitrary type
#define SL_ENUM_GENERIC(name, type) enum name
#else
// NOTE: The following macros might cause MISRA warnings because
//       Macro parameters need to be enclosed in parentheses.
//       However, it is not possible in C to enclose declaration
//       identifiers in parentheses. For example:
//         typedef uint8_t (some_identifier);
//       is not syntactically correct in the C language (C99).
#define SL_ENUM(name) typedef uint8_t name; enum name##_enum
#define SL_ENUM_GENERIC(name, type) typedef type name; enum name##_enum

// For debugging, use the following define to turn this back into a proper enumeration
// #define SL_ENUM(name) typedef enum name##_enum name; enum name##_enum
#endif

/** @} end enum */

#endif // SL_ENUM_H
