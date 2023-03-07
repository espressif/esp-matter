/***************************************************************************//**
 * @file
 * @brief Application logging interface
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

#ifndef APP_LOG_H
#define APP_LOG_H

#include <stdbool.h>
#include <stdint.h>
#include "sl_iostream.h"
#include "sl_iostream_handles.h"
#include "app_log_config.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APP_LOG_LEVEL_CRITICAL             0
#define APP_LOG_LEVEL_ERROR                1
#define APP_LOG_LEVEL_WARNING              2
#define APP_LOG_LEVEL_INFO                 3
#define APP_LOG_LEVEL_DEBUG                4
#define APP_LOG_LEVEL_COUNT                5

#define APP_LOG_COUNTER_FORMAT             "%u"
#define APP_LOG_TIME_FORMAT                "%u:%02u:%02u.%03u"
#define APP_LOG_TRACE_FORMAT               "%s:%d :%s: "
#define APP_LOG_STATUS_FORMAT              "Status: %s = 0x%04x "
#define APP_LOG_SEPARATOR                  " "
#define APP_LOG_UNRESOLVED_STATUS          "?"

#define APP_LOG_COLOR_RESET           "\033[0m"

#define APP_LOG_COLOR_BLACK           "\033[2;30m"
#define APP_LOG_COLOR_RED             "\033[2;31m"
#define APP_LOG_COLOR_GREEN           "\033[2;32m"
#define APP_LOG_COLOR_YELLOW          "\033[2;33m"
#define APP_LOG_COLOR_BLUE            "\033[2;34m"
#define APP_LOG_COLOR_MAGENTA         "\033[2;35m"
#define APP_LOG_COLOR_CYAN            "\033[2;36m"
#define APP_LOG_COLOR_WHITE           "\033[2;37m"
#define APP_LOG_COLOR_BRIGHT_BLACK    "\033[1;30m"
#define APP_LOG_COLOR_BRIGHT_RED      "\033[1;31m"
#define APP_LOG_COLOR_BRIGHT_GREEN    "\033[1;32m"
#define APP_LOG_COLOR_BRIGHT_YELLOW   "\033[1;33m"
#define APP_LOG_COLOR_BRIGHT_BLUE     "\033[1;34m"
#define APP_LOG_COLOR_BRIGHT_MAGENTA  "\033[1;35m"
#define APP_LOG_COLOR_BRIGHT_CYAN     "\033[1;36m"
#define APP_LOG_COLOR_BRIGHT_WHITE    "\033[1;37m"

#define APP_LOG_BACKGROUND_COLOR_NONE            "\033[0m"
#define APP_LOG_BACKGROUND_COLOR_BLACK           "\033[40m"
#define APP_LOG_BACKGROUND_COLOR_RED             "\033[41m"
#define APP_LOG_BACKGROUND_COLOR_GREEN           "\033[42m"
#define APP_LOG_BACKGROUND_COLOR_YELLOW          "\033[43m"
#define APP_LOG_BACKGROUND_COLOR_BLUE            "\033[44m"
#define APP_LOG_BACKGROUND_COLOR_MAGENTA         "\033[45m"
#define APP_LOG_BACKGROUND_COLOR_CYAN            "\033[46m"
#define APP_LOG_BACKGROUND_COLOR_WHITE           "\033[47m"
#define APP_LOG_BACKGROUND_COLOR_BRIGHT_BLACK    "\033[40,1m"
#define APP_LOG_BACKGROUND_COLOR_BRIGHT_RED      "\033[41,1m"
#define APP_LOG_BACKGROUND_COLOR_BRIGHT_GREEN    "\033[42,1m"
#define APP_LOG_BACKGROUND_COLOR_BRIGHT_YELLOW   "\033[43,1m"
#define APP_LOG_BACKGROUND_COLOR_BRIGHT_BLUE     "\033[44,1m"
#define APP_LOG_BACKGROUND_COLOR_BRIGHT_MAGENTA  "\033[45,1m"
#define APP_LOG_BACKGROUND_COLOR_BRIGHT_CYAN     "\033[46,1m"
#define APP_LOG_BACKGROUND_COLOR_BRIGHT_WHITE    "\033[47,1m"

#define APP_LOG_NL                               APP_LOG_NEW_LINE

// -----------------------------------------------------------------------------
// Global variables

extern sl_iostream_t * app_log_iostream;

// -----------------------------------------------------------------------------
// Internal functions

/***************************************************************************//**
 * Application log init
 ******************************************************************************/
void app_log_init();

/***************************************************************************//**
 * Sets IO Stream
 * @param[in] stream IO stream to set
 * @return SL_STATUS_INVALID_PARAMETER if IO stream instance is not present
 ******************************************************************************/
sl_status_t app_log_iostream_set(sl_iostream_t *stream);

/***************************************************************************//**
 * Get the current IO Stream for logging
 * @return the used IO stream instance
 ******************************************************************************/
sl_iostream_t * app_log_iostream_get(void);

/***************************************************************************//**
 * Log status string
 * @param[in] value Status code
 ******************************************************************************/
void _app_log_status_string(sl_status_t sc);

/***************************************************************************//**
 * Log time
 ******************************************************************************/
void _app_log_time();

/***************************************************************************//**
 * Log counter
 ******************************************************************************/
void _app_log_counter();

/***************************************************************************//**
 * Checking log level
 * @param[in] level log level to check
 * @return true if the given level should be printed
 ******************************************************************************/
bool _app_log_check_level(uint8_t level);

// -----------------------------------------------------------------------------
// Public API functions

/***************************************************************************//**
 * Enable or disable simple log level filter threshold
 * @param[in] enable true if simple threshold filter needs to be enabled
 ******************************************************************************/
void app_log_filter_threshold_enable(bool enable);

/***************************************************************************//**
 * Enable or disable masking for log levels
 * @param[in] enable true if masking needs to be enabled
 ******************************************************************************/
void app_log_filter_mask_enable(bool enable);

/***************************************************************************//**
 * Set simple filter threshold log level
 * @param[in] level log level threshold
 * @return SL_STATUS_OK if set correctly
 ******************************************************************************/
sl_status_t app_log_filter_threshold_set(uint8_t level);

/***************************************************************************//**
 * Set masks for log levels
 * @param[in] mask bitfield of log levels to be enabled
 * @return SL_STATUS_OK if set correctly
 ******************************************************************************/
sl_status_t app_log_filter_mask_set(uint8_t mask);

/***************************************************************************//**
 * Get simple filter threshold log level
 * @return log level threshold
 ******************************************************************************/
uint8_t app_log_filter_threshold_get(void);

/***************************************************************************//**
 * Get masks for log levels
 * @return bitfield of log levels that are enabled
 ******************************************************************************/
uint8_t app_log_filter_mask_get(void);

// -----------------------------------------------------------------------------
// Logging macro definitions

#if defined(APP_LOG_ENABLE) && APP_LOG_ENABLE

#define app_log_append(...)            \
  sl_iostream_printf(app_log_iostream, \
                     __VA_ARGS__)

#define app_log_append_level(level, ...) \
  do {                                   \
    if (_app_log_check_level(level)) {   \
      app_log_append(__VA_ARGS__);       \
    }                                    \
  } while (0)

#if defined(APP_LOG_COLOR_ENABLE) && APP_LOG_COLOR_ENABLE

#define _app_log_print_color(lev)                              \
  do {                                                         \
    switch (lev) {                                             \
      case APP_LOG_LEVEL_DEBUG:                                \
        app_log_append(APP_LOG_LEVEL_DEBUG_BACKGROUND_COLOR    \
                       APP_LOG_LEVEL_DEBUG_COLOR);             \
        break;                                                 \
      case APP_LOG_LEVEL_INFO:                                 \
        app_log_append(APP_LOG_LEVEL_INFO_BACKGROUND_COLOR     \
                       APP_LOG_LEVEL_INFO_COLOR);              \
        break;                                                 \
      case APP_LOG_LEVEL_WARNING:                              \
        app_log_append(APP_LOG_LEVEL_WARNING_BACKGROUND_COLOR  \
                       APP_LOG_LEVEL_WARNING_COLOR);           \
        break;                                                 \
      case APP_LOG_LEVEL_ERROR:                                \
        app_log_append(APP_LOG_LEVEL_ERROR_BACKGROUND_COLOR    \
                       APP_LOG_LEVEL_ERROR_COLOR);             \
        break;                                                 \
      case APP_LOG_LEVEL_CRITICAL:                             \
        app_log_append(APP_LOG_LEVEL_CRITICAL_BACKGROUND_COLOR \
                       APP_LOG_LEVEL_CRITICAL_COLOR);          \
        break;                                                 \
    }                                                          \
  } while (0)

#define _app_log_reset_color() app_log_append(APP_LOG_COLOR_RESET)

#else // APP_LOG_COLOR_ENABLE
#define _app_log_print_color(level)
#define _app_log_reset_color()
#endif // APP_LOG_COLOR_ENABLE

#if defined(APP_LOG_AUTO_NL) && APP_LOG_AUTO_NL
#define _app_log_nl_prefix()  app_log_nl()
#else
#define _app_log_nl_prefix()
#endif

#if defined(APP_LOG_PREFIX_ENABLE) && APP_LOG_PREFIX_ENABLE

#define _app_log_print_prefix(lev)                   \
  do {                                               \
    switch (lev) {                                   \
      case APP_LOG_LEVEL_DEBUG:                      \
        app_log_append(APP_LOG_LEVEL_DEBUG_PREFIX    \
                       APP_LOG_SEPARATOR);           \
        break;                                       \
      case APP_LOG_LEVEL_INFO:                       \
        app_log_append(APP_LOG_LEVEL_INFO_PREFIX     \
                       APP_LOG_SEPARATOR);           \
        break;                                       \
      case APP_LOG_LEVEL_WARNING:                    \
        app_log_append(APP_LOG_LEVEL_WARNING_PREFIX  \
                       APP_LOG_SEPARATOR);           \
        break;                                       \
      case APP_LOG_LEVEL_ERROR:                      \
        app_log_append(APP_LOG_LEVEL_ERROR_PREFIX    \
                       APP_LOG_SEPARATOR);           \
        break;                                       \
      case APP_LOG_LEVEL_CRITICAL:                   \
        app_log_append(APP_LOG_LEVEL_CRITICAL_PREFIX \
                       APP_LOG_SEPARATOR);           \
        break;                                       \
    }                                                \
  } while (0)

#else // APP_LOG_PREFIX_ENABLE
#define _app_log_print_prefix(level)
#endif // APP_LOG_PREFIX_ENABLE

#define app_log_print_trace()              \
  sl_iostream_printf(app_log_iostream,     \
                     APP_LOG_TRACE_FORMAT, \
                     __FILE__,             \
                     __LINE__,             \
                     __func__)

#if defined(APP_LOG_TRACE_ENABLE) && APP_LOG_TRACE_ENABLE
#define _app_log_print_trace()  app_log_print_trace()
#else // APP_LOG_TRACE_ENABLE
#define _app_log_print_trace()
#endif // APP_LOG_TRACE_ENABLE

#define _app_log_print_status(sc)         \
  do {                                    \
    app_log_append(APP_LOG_STATUS_FORMAT, \
                   #sc,                   \
                   (int)sc);              \
    _app_log_status_string(sc);           \
  } while (0)

#define app_log(...)                           \
  do {                                         \
    _app_log_nl_prefix();                      \
    _app_log_print_color(APP_LOG_LEVEL_DEBUG); \
    _app_log_time();                           \
    _app_log_counter();                        \
    _app_log_print_trace();                    \
    app_log_append(__VA_ARGS__);               \
  } while (0)

#define app_log_level(level, ...)      \
  do {                                 \
    if (_app_log_check_level(level)) { \
      _app_log_nl_prefix();            \
      _app_log_print_color(level);     \
      _app_log_print_prefix(level);    \
      _app_log_time();                 \
      _app_log_counter();              \
      _app_log_print_trace();          \
      app_log_append(__VA_ARGS__);     \
    }                                  \
  } while (0)

#define app_log_status_level_f(level, sc, ...)                  \
  do {                                                          \
    if (!(sc == SL_STATUS_OK) && _app_log_check_level(level)) { \
      _app_log_nl_prefix();                                     \
      _app_log_print_color(level);                              \
      _app_log_print_prefix(level);                             \
      _app_log_time();                                          \
      _app_log_counter();                                       \
      _app_log_print_trace();                                   \
      _app_log_print_status(sc);                                \
      app_log_append(__VA_ARGS__);                              \
    }                                                           \
  } while (0)

#define app_log_status_level(level, sc) \
  app_log_status_level_f(level,         \
                         sc,            \
                         "")

#define app_log_status(sc)                   \
  app_log_status_level_f(APP_LOG_LEVEL_INFO, \
                         sc,                 \
                         "")

#define app_log_status_f(sc, ...)            \
  app_log_status_level_f(APP_LOG_LEVEL_INFO, \
                         sc,                 \
                         __VA_ARGS__)

#define app_log_hexdump_level_s(level, separator, p_data, len) \
  do {                                                         \
    if (_app_log_check_level(level)) {                         \
      uint8_t *tmp = (uint8_t *)p_data;                        \
      for (uint32_t i = 0; i < len; i++) {                     \
        if (i > 0) {                                           \
          app_log_append(separator);                           \
        }                                                      \
        app_log_append(APP_LOG_HEXDUMP_PREFIX);                \
        app_log_append(APP_LOG_HEXDUMP_FORMAT,                 \
                       (int) tmp[i]);                          \
      }                                                        \
    }                                                          \
  } while (0)

#else // APP_LOG_ENABLE

#define app_log(...)
#define app_log_level(level, ...)
#define app_log_hexdump_level_s(level, separator, p_data, len)
#define app_log_status(sc) (void)sc
#define app_log_status_f(sc, ...) (void)sc
#define app_log_status_level(level, sc) (void)sc
#define app_log_status_level_f(level, sc, ...) (void)sc
#define app_log_append(...)
#define app_log_append_level(level, ...)

#endif // APP_LOG_ENABLE

#define app_log_nl()  app_log_append(APP_LOG_NEW_LINE)

#define app_log_append_debug(...)           \
  app_log_append_level(APP_LOG_LEVEL_DEBUG, \
                       __VA_ARGS__)

#define app_log_append_info(...)           \
  app_log_append_level(APP_LOG_LEVEL_INFO, \
                       __VA_ARGS__)

#define app_log_append_warning(...)           \
  app_log_append_level(APP_LOG_LEVEL_WARNING, \
                       __VA_ARGS__)

#define app_log_append_error(...)           \
  app_log_append_level(APP_LOG_LEVEL_ERROR, \
                       __VA_ARGS__)

#define app_log_append_critical(...)           \
  app_log_append_level(APP_LOG_LEVEL_CRITICAL, \
                       __VA_ARGS__)

#define app_log_debug(...)           \
  app_log_level(APP_LOG_LEVEL_DEBUG, \
                __VA_ARGS__)

#define app_log_info(...)           \
  app_log_level(APP_LOG_LEVEL_INFO, \
                __VA_ARGS__)

#define app_log_warning(...)           \
  app_log_level(APP_LOG_LEVEL_WARNING, \
                __VA_ARGS__)

#define app_log_error(...)           \
  app_log_level(APP_LOG_LEVEL_ERROR, \
                __VA_ARGS__)

#define app_log_critical(...)           \
  app_log_level(APP_LOG_LEVEL_CRITICAL, \
                __VA_ARGS__)

#define app_log_status_debug(sc)            \
  app_log_status_level(APP_LOG_LEVEL_DEBUG, \
                       sc)

#define app_log_status_info(sc)            \
  app_log_status_level(APP_LOG_LEVEL_INFO, \
                       sc)

#define app_log_status_warning(sc)            \
  app_log_status_level(APP_LOG_LEVEL_WARNING, \
                       sc)

#define app_log_status_error(sc)            \
  app_log_status_level(APP_LOG_LEVEL_ERROR, \
                       sc)

#define app_log_status_critical(sc)            \
  app_log_status_level(APP_LOG_LEVEL_CRITICAL, \
                       sc)

#define app_log_status_debug_f(sc, ...)       \
  app_log_status_level_f(APP_LOG_LEVEL_DEBUG, \
                         sc,                  \
                         __VA_ARGS__)
#define app_log_status_info_f(sc, ...)       \
  app_log_status_level_f(APP_LOG_LEVEL_INFO, \
                         sc,                 \
                         __VA_ARGS__)

#define app_log_status_warning_f(sc, ...)       \
  app_log_status_level_f(APP_LOG_LEVEL_WARNING, \
                         sc,                    \
                         __VA_ARGS__)

#define app_log_status_error_f(sc, ...)       \
  app_log_status_level_f(APP_LOG_LEVEL_ERROR, \
                         sc,                  \
                         __VA_ARGS__)

#define app_log_status_critical_f(sc, ...)       \
  app_log_status_level_f(APP_LOG_LEVEL_CRITICAL, \
                         sc,                     \
                         __VA_ARGS__)

#define app_log_hexdump_level(level, p_data, len)    \
  app_log_hexdump_level_s(level,                     \
                          APP_LOG_HEXDUMP_SEPARATOR, \
                          p_data,                    \
                          len)

#define app_log_hexdump_debug(p_data, len) \
  app_log_hexdump_level(APP_LOG_LEVEL_DEBUG, p_data, len)

#define app_log_hexdump_info(p_data, len) \
  app_log_hexdump_level(APP_LOG_LEVEL_INFO, p_data, len)

#define app_log_hexdump_warning(p_data, len) \
  app_log_hexdump_level(APP_LOG_LEVEL_WARNING, p_data, len)

#define app_log_hexdump_error(p_data, len) \
  app_log_hexdump_level(APP_LOG_LEVEL_ERROR, p_data, len)

#define app_log_hexdump_critical(p_data, len) \
  app_log_hexdump_level(APP_LOG_LEVEL_CRITICAL, p_data, len)

#define app_log_hexdump_debug_s(separator, p_data, len) \
  app_log_hexdump_level_s(APP_LOG_LEVEL_DEBUG, separator, p_data, len)

#define app_log_hexdump_info_s(separator, p_data, len) \
  app_log_hexdump_level_s(APP_LOG_LEVEL_INFO, separator, p_data, len)

#define app_log_hexdump_warning_s(separator, p_data, len) \
  app_log_hexdump_level_s(APP_LOG_LEVEL_WARNING, separator, p_data, len)

#define app_log_hexdump_error_s(separator, p_data, len) \
  app_log_hexdump_level_s(APP_LOG_LEVEL_ERROR, separator, p_data, len)

#define app_log_hexdump_critical_s(separator, p_data, len) \
  app_log_hexdump_level_s(APP_LOG_LEVEL_CRITICAL, separator, p_data, len)

#ifdef __cplusplus
}
#endif

#endif // APP_LOG_H
