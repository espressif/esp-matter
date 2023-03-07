/***************************************************************************//**
 * @file
 * @brief STDIO compatibility layer.
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

#include "sl_stdio.h"
#include "sl_iostream.h"
#include "sl_status.h"
#include <stdbool.h>

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Put a character on default the stream.
 ******************************************************************************/
sl_status_t sl_putchar(char c)
{
  return(sl_iostream_write(SL_IOSTREAM_STDOUT, (const uint8_t *)&c, 1));
}

/***************************************************************************//**
 * Put a character on default the stream (match standard API).
 ******************************************************************************/
int sl_putchar_std_wrapper(int c)
{
  sl_status_t status = sl_putchar((char)c);
  if (status == SL_STATUS_OK) {
    return c;
  } else {
    return -1;  // EOF
  }
}

/***************************************************************************//**
 * Get a character on default the stream.
 ******************************************************************************/
sl_status_t sl_getchar(char *c)
{
  return(sl_iostream_getchar(SL_IOSTREAM_STDIN, c));
}

/***************************************************************************//**
 * Get a character on default the stream (match standard API).
 ******************************************************************************/
int sl_getchar_std_wrapper(void)
{
  char c;
  sl_status_t status = sl_getchar(&c);
  if (status == SL_STATUS_OK) {
    return (int)c;
  } else {
    return -1;  // EOF
  }
}
