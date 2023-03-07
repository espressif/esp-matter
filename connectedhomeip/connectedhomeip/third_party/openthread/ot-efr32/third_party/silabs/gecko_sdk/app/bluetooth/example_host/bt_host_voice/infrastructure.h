/***************************************************************************//**
 * @file
 * @brief Infrastructure for code development, macros for type conversion
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef INFRASTRUCTURE_H
#define INFRASTRUCTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/**************************************************************************************************
 * Macros for type conversion
 *************************************************************************************************/

#define FLT_TO_UINT32(m, e)       (((uint32_t)(m) & 0x00FFFFFFU) | (uint32_t)((int32_t)(e) << 24))

#define UINT16_TO_BITSTREAM(p, n)   { *(p)++ = (uint8_t)(n); *(p)++ = (uint8_t)((n) >> 8); }

#define UINT8_TO_BITSTREAM(p, n)    { *(p)++ = (uint8_t)(n); }

#define UINT16_TO_BYTES(n)        ((uint8_t) (n)), ((uint8_t)((n) >> 8))

#define UINT32_TO_BITSTREAM(p, n)   { *(p)++ = (uint8_t)(n); *(p)++ = (uint8_t)((n) >> 8); \
                                      *(p)++ = (uint8_t)((n) >> 16); *(p)++ = (uint8_t)((n) >> 24); }

#define UINT16_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT16_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))

#define UINT32_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT32_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))
#define UINT32_TO_BYTE2(n)        ((uint8_t) ((n) >> 16))
#define UINT32_TO_BYTE3(n)        ((uint8_t) ((n) >> 24))

#define MIN(a, b)                  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)                  (((a) > (b)) ? (a) : (b))

#define COUNTOF(x)                ((sizeof (x)) / (sizeof ((x)[0])))

#ifdef __cplusplus
};
#endif

#endif /* INFRASTRUCTURE_H */
