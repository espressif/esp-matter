/***************************************************************************//**
 * @file
 * @brief IO Stream STDIO Component.
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

#include "sl_status.h"
#include "sl_iostream.h"
#include "sl_iostream_stdio.h"
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static sl_status_t stdio_write(void *context,
                               const void *buffer,
                               size_t buffer_length);

static sl_status_t stdio_read(void *context,
                              void *buffer,
                              size_t buffer_length,
                              size_t *bytes_read);

/*******************************************************************************
 ******************************   VARIABLES   **********************************
 ******************************************************************************/

sl_iostream_t sl_iostream_stdio = {
  .read = stdio_read,
  .write = stdio_write,
  .context = NULL
};

sl_iostream_t *sl_iostream_stdio_handle = &sl_iostream_stdio;

sl_iostream_instance_info_t sl_iostream_instance_stdio_info = {
  .handle = &sl_iostream_stdio,
  .name = "stdio",
  .type = SL_IOSTREAM_TYPE_UNDEFINED,
  .periph_id = 0,
  .init = sl_iostream_stdio_init,
};

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * STDIO Stream init.
 ******************************************************************************/
sl_status_t sl_iostream_stdio_init(void)
{
  sl_iostream_set_system_default(&sl_iostream_stdio);

  return SL_STATUS_OK;
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * Internal STDIO stream write implementation
 ******************************************************************************/
static sl_status_t stdio_write(void *context,
                               const void *buffer,
                               size_t buffer_length)
{
  sl_status_t status = SL_STATUS_OK;
  char *ch = (char *)buffer;

  (void)context;

  for (size_t i = 0; i < buffer_length; i++) {
    fputc(*ch, stdout);
    ch++;
  }
  fflush(stdout);
  return status;
}

/***************************************************************************//**
 * Internal STDIO stream read implementation
 ******************************************************************************/
static sl_status_t stdio_read(void *context,
                              void *buffer,
                              size_t buffer_length,
                              size_t *bytes_read)
{
  sl_status_t status = SL_STATUS_OK;
  char *buf = buffer;

  (void)context;

  for (size_t i = 0; i < buffer_length; i++) {
    int c = getchar();
    if (c == EOF) {
      break;
    }
    buf[i] = (char)c;
    (*bytes_read)++;
  }

  if (*bytes_read == 0) {
    status = SL_STATUS_EMPTY;
  }

  return status;
}
