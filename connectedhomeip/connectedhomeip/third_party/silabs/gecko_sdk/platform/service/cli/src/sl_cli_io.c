/***************************************************************************//**
 * @file
 * @brieg Functions that implement cli input and output.
 * @version x.y.z
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

#include "sl_cli_config.h"
#include "sli_cli_io.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "sl_iostream.h"

int sli_cli_io_getchar(void)
{
  char ch;
  sl_status_t status = sl_iostream_getchar(SL_IOSTREAM_STDIN, &ch);
  if (status != SL_STATUS_OK) {
    return EOF;
  }

  return ch;
}

int sli_cli_io_putchar(int ch)
{
  sl_status_t status = sl_iostream_putchar(SL_IOSTREAM_STDOUT, ch);
  if (status != SL_STATUS_OK) {
    return EOF;
  }

  return ch;
}

int sli_cli_io_printf(const char *format, ...)
{
  va_list argptr;

  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);

  return 0;
}

/** @} (end addtogroup cli) */
