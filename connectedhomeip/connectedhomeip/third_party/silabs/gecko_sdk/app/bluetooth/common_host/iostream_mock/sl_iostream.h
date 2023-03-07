/***************************************************************************//**
 * @file
 * @brief IO Stream mock implementation
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
#ifndef SL_IOSTREAM_H
#define SL_IOSTREAM_H

#include <stdio.h>

/// Dummy type for compatibility.
typedef int sl_iostream_t;

/// Dummy type for compatibility.
typedef enum {
  DUMMY_VALUE,
} sl_iostream_type_t;

/// sl_iostream_printf host side implementation.
#define sl_iostream_printf(stream, ...) \
  do {                                  \
    printf(__VA_ARGS__);                \
    fflush(stdout);                     \
  } while (0)

/// Dummy implementation for compatibility.
#define sl_iostream_get_default() NULL

/// Dummy implementation for compatibility.
#define sl_iostream_set_default(stream) (void)stream

#endif // SL_IOSTREAM_H
