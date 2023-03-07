/***************************************************************************//**
 * @file
 * @brief IO Stream.
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

#include "sl_iostream.h"
#include "sl_status.h"
#include "em_core.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sli_cmsis_os2_ext_task_register.h"
#endif

#if defined(SL_CATALOG_PRINTF_PRESENT)
#include "printf.h"
#else
#include <stdio.h>
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#if defined(SL_CATALOG_KERNEL_PRESENT)
#define TASK_REGISTER_ID_INVALID   0xFF
#endif

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

#if defined(SL_CATALOG_KERNEL_PRESENT)
static sli_task_register_id_t sli_task_register_id = TASK_REGISTER_ID_INVALID;
static sl_iostream_t  *sli_iostream_system_default = NULL;
#endif
static sl_iostream_t  *sli_iostream_default = NULL;

sl_iostream_t sl_iostream_null = {
  .write   = NULL,
  .read    = NULL,
  .context = NULL
};

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

#if defined(SL_CATALOG_PRINTF_PRESENT)
static void stream_putchar(char character,
                           void *arg);
#endif

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Registers default IO stream to be used
 ******************************************************************************/
sl_status_t sl_iostream_set_default(sl_iostream_t  *stream)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  sli_task_register_id_t reg_id;
  sl_status_t status;
#endif
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (osThreadGetId() != NULL) {
    reg_id = sli_task_register_id;
    if (reg_id == TASK_REGISTER_ID_INVALID) {
      status = sli_osTaskRegisterNew(&reg_id);
      EFM_ASSERT(status == SL_STATUS_OK);
      sli_task_register_id = reg_id;
    }
  }
#endif
  sli_iostream_default = stream;
  CORE_EXIT_CRITICAL();

#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (osThreadGetId() != NULL) {
    status = sli_osTaskRegisterSetValue(NULL, reg_id, (uint32_t)stream);
    EFM_ASSERT(status == SL_STATUS_OK);
  }
#endif

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get default IO stream configured
 ******************************************************************************/
sl_iostream_t *sl_iostream_get_default(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  sl_status_t status;
  sli_task_register_id_t reg_id;
#endif
  sl_iostream_t *stream = NULL;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
#if defined(SL_CATALOG_KERNEL_PRESENT)
  reg_id = sli_task_register_id;
#endif
  stream = sli_iostream_default;
  CORE_EXIT_CRITICAL();

#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (osThreadGetId() != NULL) {
    if (reg_id != TASK_REGISTER_ID_INVALID) {
      uint32_t reg;

      status = sli_osTaskRegisterGetValue(NULL, sli_task_register_id, &reg);
      EFM_ASSERT(status == SL_STATUS_OK);

      stream = (sl_iostream_t *)reg;
    }
  }
  if (stream == NULL) {
    CORE_ENTER_CRITICAL();
    stream = sli_iostream_system_default;
    CORE_EXIT_CRITICAL();
  }
#endif

  return stream;
}

/***************************************************************************//**
 * Set systemwide default IO stream
 ******************************************************************************/
#if defined(SL_CATALOG_KERNEL_PRESENT)
sl_status_t sl_iostream_set_system_default(sl_iostream_t *stream)
{
  sl_status_t status = SL_STATUS_OK;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  sli_iostream_system_default = stream;
  CORE_EXIT_CRITICAL();
  return status;
}
#endif

/***************************************************************************//**
 * Stream write implementation
 ******************************************************************************/
sl_status_t  sl_iostream_write(sl_iostream_t *stream,
                               const void *buffer,
                               size_t buffer_length)
{
  if (stream == SL_IOSTREAM_STDOUT) {
    stream = sl_iostream_get_default();
  }

  if ((stream != NULL) && (stream->write != NULL)) {
    return stream->write(stream->context, buffer, buffer_length);
  } else {
    return SL_STATUS_INVALID_CONFIGURATION;
  }
}

/***************************************************************************//**
 * Stream read implementation
 ******************************************************************************/
sl_status_t sl_iostream_read(sl_iostream_t *stream,
                             void *buffer,
                             size_t buffer_length,
                             size_t *bytes_read)
{
  size_t   size;
  size_t  *read_size = &size;

  if (stream == SL_IOSTREAM_STDIN) {
    stream = sl_iostream_get_default();
  }

  if (bytes_read != NULL) {
    read_size = bytes_read;
  }

  if ((stream != NULL) && (stream->read != NULL)) {
    return stream->read(stream->context, buffer, buffer_length, read_size);
  } else {
    return SL_STATUS_INVALID_CONFIGURATION;
  }
}

/***************************************************************************//**
 * Stream putchar implementation
 ******************************************************************************/
sl_status_t sl_iostream_putchar(sl_iostream_t *stream,
                                char c)
{
  return sl_iostream_write(stream, &c, 1);
}

/***************************************************************************//**
 * Stream getchar implementation
 ******************************************************************************/
sl_status_t sl_iostream_getchar(sl_iostream_t *stream,
                                char *c)
{
  return sl_iostream_read(stream, c, 1, NULL);
}

/***************************************************************************//**
 * Stream printf implementation
 ******************************************************************************/
sl_status_t sl_iostream_printf(sl_iostream_t *stream,
                               const char *format,
                               ...)
{
#if !defined(SL_CATALOG_PRINTF_PRESENT)
  sl_iostream_t *default_stream;
#endif
  sl_iostream_t *output_stream = stream;
  sl_status_t status = SL_STATUS_OK;
  int ret;
  va_list va;
  va_start(va, format);

#if defined(SL_CATALOG_PRINTF_PRESENT)
  if (output_stream == SL_IOSTREAM_STDOUT) {
    output_stream = sl_iostream_get_default();
  }
  ret = vfctprintf(stream_putchar, output_stream, format, va);
#else
  if (output_stream == SL_IOSTREAM_STDOUT) {
    default_stream = sl_iostream_get_default();
    output_stream = default_stream;
  } else {
    default_stream = sl_iostream_get_default();
    if (default_stream != output_stream) {
      sl_iostream_set_default(output_stream);
    }
  }

  ret = vprintf(format, va);
  if (default_stream != output_stream) {
    sl_iostream_set_default(default_stream);
  }
#endif
  va_end(va);
  if (ret <= 0) {
    status = SL_STATUS_OBJECT_WRITE;
  }

  return status;
}

/***************************************************************************//**
 * putchar implementation for sl_iostream_printf; called by fnctprintf()
 ******************************************************************************/
#if defined(SL_CATALOG_PRINTF_PRESENT)
static void stream_putchar(char character,
                           void *arg)
{
  sl_iostream_putchar((sl_iostream_t *)arg, character);
}
#endif
