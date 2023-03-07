// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.
#pragma once

#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>

#include "pw_log_zephyr/config.h"

#ifndef PW_LOG_MODULE_NAME
#define PW_LOG_MODULE_NAME ""
#endif

// If the consumer defined PW_LOG_LEVEL use it, otherwise fallback to the global
// CONFIG_PIGWEED_LOG_LEVEL set by Kconfig.
#ifdef PW_LOG_LEVEL
#if PW_LOG_LEVEL == PW_LOG_LEVEL_DEBUG
// Map PW_LOG_LEVEL_DEBUG to LOG_LEVEL_DBG
#define LOG_LEVEL LOG_LEVEL_DBG
#elif PW_LOG_LEVEL == PW_LOG_LEVEL_INFO
// Map PW_LOG_LEVEL_INFO to LOG_LEVEL_INF
#define LOG_LEVEL LOG_LEVEL_INF
#elif PW_LOG_LEVEL == PW_LOG_LEVEL_WARN
// Map PW_LOG_LEVEL_WARN to LOG_LEVEL_WRN
#define LOG_LEVEL LOG_LEVEL_WRN
#elif (PW_LOG_LEVEL == PW_LOG_LEVEL_ERROR) ||  \
    (PW_LOG_LEVEL == PW_LOG_LEVEL_CRITICAL) || \
    (PW_LOG_LEVEL == PW_LOG_LEVEL_FATAL)
// Map PW_LOG_LEVEL_(ERROR|CRITICAL|FATAL) to LOG_LEVEL_ERR
#define LOG_LEVEL LOG_LEVEL_ERR
#endif
#else
// Default to the Kconfig value
#define LOG_LEVEL CONFIG_PIGWEED_LOG_LEVEL
#endif

LOG_MODULE_DECLARE(PW_LOG_ZEPHYR_MODULE_NAME, LOG_LEVEL);

#define PW_HANDLE_LOG(level, flags, ...)             \
  do {                                               \
    switch (level) {                                 \
      case PW_LOG_LEVEL_INFO:                        \
        LOG_INF(PW_LOG_MODULE_NAME " " __VA_ARGS__); \
        break;                                       \
      case PW_LOG_LEVEL_WARN:                        \
        LOG_WRN(PW_LOG_MODULE_NAME " " __VA_ARGS__); \
        break;                                       \
      case PW_LOG_LEVEL_ERROR:                       \
      case PW_LOG_LEVEL_CRITICAL:                    \
        LOG_ERR(PW_LOG_MODULE_NAME " " __VA_ARGS__); \
        break;                                       \
      case PW_LOG_LEVEL_FATAL:                       \
        LOG_ERR(PW_LOG_MODULE_NAME " " __VA_ARGS__); \
        LOG_PANIC();                                 \
        break;                                       \
      case PW_LOG_LEVEL_DEBUG:                       \
      default:                                       \
        LOG_DBG(PW_LOG_MODULE_NAME " " __VA_ARGS__); \
        break;                                       \
    }                                                \
  } while (0)
