/***************************************************************************//**
 * @file
 * @brief Application assert interface
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

#ifndef APP_ASSERT_H
#define APP_ASSERT_H

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#include "app_assert_config.h"
#include "sl_status.h"
#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif

#define APP_ASSERT_SEPARATOR   ": "
#define APP_ASSERT_FORMAT      "Assertion '%s' failed"
#define APP_ASSERT_TEXT        "Assertion failed"

#if defined(APP_ASSERT_ENABLE) && APP_ASSERT_ENABLE

#ifdef HOST_TOOLCHAIN
#include <stdlib.h>
#define _app_assert_abort() abort()
#else
#define _app_assert_abort() while (1)
#endif // HOST_TOOLCHAIN

#if defined(APP_ASSERT_TRACE_ENABLE) && APP_ASSERT_TRACE_ENABLE
#define _app_assert_trace() app_log_print_trace()
#else // defined(APP_ASSERT_TRACE_ENABLE) && APP_ASSERT_TRACE_ENABLE
#define _app_assert_trace()
#endif // defined(APP_ASSERT_TRACE_ENABLE) && APP_ASSERT_TRACE_ENABLE

#if defined(SL_CATALOG_APP_LOG_PRESENT) \
  && APP_ASSERT_LOG_ENABLE              \
  && defined(APP_LOG_ENABLE)            \
  && APP_LOG_ENABLE

#define _app_assert_log_status(sc, ...)            \
  do {                                             \
    app_log_nl();                                  \
    _app_log_print_color(APP_LOG_LEVEL_CRITICAL);  \
    _app_log_print_prefix(APP_LOG_LEVEL_CRITICAL); \
    _app_log_time();                               \
    _app_log_counter();                            \
    _app_assert_trace();                           \
    _app_log_print_status(sc);                     \
    app_log_append(__VA_ARGS__);                   \
  } while (0)

#define _app_assert_log(...)                       \
  do {                                             \
    app_log_nl();                                  \
    _app_log_print_color(APP_LOG_LEVEL_CRITICAL);  \
    _app_log_print_prefix(APP_LOG_LEVEL_CRITICAL); \
    _app_log_time();                               \
    _app_log_counter();                            \
    _app_assert_trace();                           \
    app_log_append(__VA_ARGS__);                   \
  } while (0)

#define app_assert_s(expr)               \
  do {                                   \
    if (!(expr)) {                       \
      _app_assert_log(APP_ASSERT_FORMAT, \
                      #expr);            \
      app_log_nl();                      \
      _app_assert_abort();               \
    }                                    \
  } while (0)

#define app_assert_status(sc)                  \
  do {                                         \
    if (!(sc == SL_STATUS_OK)) {               \
      _app_assert_log_status(sc,               \
                             APP_ASSERT_TEXT); \
      app_log_nl();                            \
      _app_assert_abort();                     \
    }                                          \
  } while (0)

#define app_assert_status_f(sc, ...)              \
  do {                                            \
    if (!(sc == SL_STATUS_OK)) {                  \
      _app_assert_log_status(sc,                  \
                             APP_ASSERT_TEXT      \
                             APP_ASSERT_SEPARATOR \
                             __VA_ARGS__);        \
      app_log_nl();                               \
      _app_assert_abort();                        \
    }                                             \
  } while (0)

#define app_assert(expr, ...)               \
  do {                                      \
    if (!(expr)) {                          \
      _app_assert_log(APP_ASSERT_FORMAT     \
                      APP_ASSERT_SEPARATOR, \
                      #expr);               \
      app_log_append(__VA_ARGS__);          \
      app_log_nl();                         \
      _app_assert_abort();                  \
    }                                       \
  } while (0)

#else // defined(SL_CATALOG_APP_LOG_PRESENT) && APP_ASSERT_LOG_ENABLE && defined(APP_LOG_ENABLE) && APP_LOG_ENABLE

#define _app_assert_printless(expr) \
  do {                              \
    if (!(expr)) {                  \
      _app_assert_abort();          \
    }                               \
  } while (0)

#define app_assert(expr, ...) \
  _app_assert_printless(expr)

#define app_assert_s(expr) \
  _app_assert_printless(expr)

#define app_assert_status(sc) \
  _app_assert_printless(sc == SL_STATUS_OK)

#define app_assert_status_f(sc, ...) \
  _app_assert_printless(sc == SL_STATUS_OK)

#endif // defined(SL_CATALOG_APP_LOG_PRESENT) && APP_ASSERT_LOG_ENABLE && defined(APP_LOG_ENABLE) && APP_LOG_ENABLE

#else // APP_ASSERT_ENABLE

#define app_assert(expr, ...)

#define app_assert_s(expr)

#define app_assert_status(sc) (void)sc

#define app_assert_status_f(sc, ...) (void)sc

#endif // APP_ASSERT_ENABLE

#endif // APP_ASSERT_H
