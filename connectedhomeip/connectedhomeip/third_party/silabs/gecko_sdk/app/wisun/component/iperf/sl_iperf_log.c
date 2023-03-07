/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "sl_iperf_log.h"
#include "sl_iperf_config.h"

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Set log buffer
 * @details helper function
 * @param[in,out] log Log pointer
 * @param[in] buff Buffer pointer
 * @param[in] size Size of buffer
 *****************************************************************************/
static inline void _set_log_buff(sl_iperf_log_t * const log,
                                 char * const buff,
                                 const size_t size);

/**************************************************************************//**
 * @brief Default log printer
 * @details Default log printer with varadic args. Both of buffer and stdout printer implemented.
 * @param[in,out] log Log instance
 * @param[in] format Format string
 * @param[in] ... args
 * @return int32_t Result by functions like snprintf and printf
 *****************************************************************************/
static int32_t _def_log_print(sl_iperf_log_t * const log,
                              const char * format, ...);

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_iperf_log_init(sl_iperf_log_t * const log)
{
  if (log == NULL) {
    return;
  }

  // Set log buffer
  _set_log_buff(log, NULL, 0U);

  // Log buffered default false
  log->buffered = false;

  // Log not colored
  log->colored = false;

  // Default print is printf
  log->print = _def_log_print;

  // Reset res of calls
  log->last_res = 0;
}

void sl_iperf_log_set_printer(sl_iperf_log_t * const log, sl_iperf_log_print_t printer)
{
  if (printer != NULL && log != NULL) {
    log->print = printer;
  }
}

void sl_iperf_log_set_buff_printer(sl_iperf_log_t * const log,
                                   char * const buff,
                                   const size_t buff_size)
{
  if (log == NULL  || buff == NULL || buff_size == 0U) {
    return;
  }

  log->buff.buff = buff;
  log->buff.pos = buff;
  log->buff.size = buff_size;
}

void sl_iperf_log_set_buff(sl_iperf_log_t * const log, char * const buff, const size_t size)
{
  if (log == NULL || buff == NULL || size == 0U) {
    return;
  }
  _set_log_buff(log, buff, size);
}

void sl_iperf_log_reset_buff(sl_iperf_log_t * const log)
{
  if (log != NULL) {
    _set_log_buff(log, log->buff.buff, log->buff.size);
  }
}

void sl_iperf_log_destroy_buff(sl_iperf_log_t * const log)
{
  if (log != NULL) {
    _set_log_buff(log, NULL, 0U);
  }
}

bool sl_iperf_log_buff_is_full(sl_iperf_log_t * const log)
{
  return log == NULL ? false : (bool)(log->buff.pos >= (log->buff.buff + log->buff.size));
}

static int32_t _def_log_print(sl_iperf_log_t * const log, const char * format, ...)
{
  va_list args;
  int32_t res = -1L;
  size_t free_bytes = 0;

  if (log == NULL) {
    return res;
  }

  va_start(args, format);
  res = vprintf(format, args);

  if (log->buffered
      && log->buff.buff != NULL
      && log->buff.size) {
    free_bytes = log->buff.size - (log->buff.pos - log->buff.buff);

    if (free_bytes) {
      res = vsnprintf(log->buff.buff, free_bytes, format, args);
      if (res > 0L) {
        log->buff.pos += res;
      }
    }
  }

  va_end(args);
  return res;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

static inline void _set_log_buff(sl_iperf_log_t * const log, char * const buff, const size_t size)
{
  // Enable buffered mode
  log->buffered = true;

  // Set content
  log->buff.buff = buff;
  log->buff.pos = buff;
  log->buff.size = size;
}
