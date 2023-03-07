/***************************************************************************//**
 * @file
 * @brief Implementation of endianness detection & operations.
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

#ifndef SL_ENDIANNESS_H
#define SL_ENDIANNESS_H

#if (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || (__little_endian__ == 1) || WIN32 || (__BYTE_ORDER == __LITTLE_ENDIAN)

#define SL_IS_LITTLE_ENDIAN   1
#define SL_IS_BIG_ENDIAN      0
#define BIGENDIAN_CPU         false     // Should not be used anymore; this is only for backward compatibility(Zigbee)

#elif (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) || (__big_endian__ == 1) || (__BYTE_ORDER == __BIG_ENDIAN)

#define SL_IS_LITTLE_ENDIAN   0
#define SL_IS_BIG_ENDIAN      1
#define BIGENDIAN_CPU         true     // Should not be used anymore; this is only for backward compatibility(Zigbee)

#else
// Endianness must be provided by the user
#if (defined(CPU_LITTLE_ENDIAN))

#define SL_IS_LITTLE_ENDIAN   1
#define SL_IS_BIG_ENDIAN      0
#define BIGENDIAN_CPU         false     // Should not be used anymore; this is only for backward compatibility(Zigbee)

#elif (defined(CPU_BIG_ENDIAN))

#define SL_IS_LITTLE_ENDIAN   0
#define SL_IS_BIG_ENDIAN      1
#define BIGENDIAN_CPU         true     // Should not be used anymore; this is only for backward compatibility(Zigbee)

#else
#error Endianess not defined
#endif
#endif

#endif
