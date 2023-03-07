/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef PW_LOG_MDH_BACKEND_H_
#define PW_LOG_MDH_BACKEND_H_

#include "pw_log_mdh/config.h"
#include "pw_log_mdh/util.h"
#include "pw_preprocessor/arguments.h"
#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/concat.h"
#include "pw_preprocessor/util.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/lp_ticker_api.h"
#include "hal/serial_api.h"

/**
 * Initialize the logging backend.
 * The function accepts the serial instance used to print the logs as input.
 */
void pw_log_mdh_init(mdh_serial_t *serial);

// Private function reserved for PW_HANDLE_LOG.
void _pw_log_mdh(const char *message, ...) PW_PRINTF_FORMAT(1, 2);

// Private function used to convert the log level into the right string.
static inline const char *_pw_log_mdh_level_name(int level)
{
    switch (level) {
        case PW_LOG_LEVEL_DEBUG:
            return PW_LOG_MDH_GREEN PW_LOG_MDH_BOLD "DBG" PW_LOG_MDH_RESET;
        case PW_LOG_LEVEL_INFO:
            return PW_LOG_MDH_MAGENTA PW_LOG_MDH_BOLD "INF" PW_LOG_MDH_RESET;
        case PW_LOG_LEVEL_WARN:
            return PW_LOG_MDH_YELLOW PW_LOG_MDH_BOLD "WRN" PW_LOG_MDH_RESET;
        case PW_LOG_LEVEL_ERROR:
            return PW_LOG_MDH_RED PW_LOG_MDH_BOLD "ERR" PW_LOG_MDH_RESET;
        case PW_LOG_LEVEL_CRITICAL:
            return PW_LOG_MDH_BLACK PW_LOG_MDH_BOLD PW_LOG_MDH_RED_BG "FTL" PW_LOG_MDH_RESET;
        default:
            return PW_LOG_MDH_GREEN PW_LOG_MDH_BOLD "UNK" PW_LOG_MDH_RESET;
    }
}

// Format macros
#define PW_LOG_MDH_LEVEL_FORMAT  "%s - "
#define PW_LOG_MDH_MODULE_FORMAT "%-" PW_STRINGIFY(PW_LOG_MDH_MODULE_LENGTH) "s - "

#if PW_LOG_MDH_SHOW_FILENAME
#define PW_LOG_MDH_FILE_FORMAT "%s:%d - "
#define PW_LOG_MDH_FILENAME    PW_LOG_MDH_FILE_NAME
#define PW_LOG_MDH_LINE        __LINE__
#else
#define PW_LOG_MDH_FILE_FORMAT
#define PW_LOG_MDH_FILENAME
#define PW_LOG_MDH_LINE
#endif // PW_LOG_MDH_SHOW_FILENAME

#if PW_LOG_MDH_TIMESTAMP
#define PW_LOG_MDH_TIMESTAMP_FORMAT "%010" PRIu32 " - "
#define PW_LOG_MDH_TIMESTAMP_VALUE  lp_ticker_read()
#else
#define PW_LOG_MDH_TIMESTAMP_FORMAT
#define PW_LOG_MDH_TIMESTAMP_VALUE
#endif

#define PW_LOG_MDH_PREFIX       \
    PW_LOG_MDH_TIMESTAMP_FORMAT \
    PW_LOG_MDH_LEVEL_FORMAT     \
    PW_LOG_MDH_MODULE_FORMAT    \
    PW_LOG_MDH_FILE_FORMAT

#define PW_LOG_MDH_SUFFIX "\r\n"

// Pigweed log backend implementation
#define PW_HANDLE_LOG(level, flags, LOG_MESSAGE, ...)                                                         \
    do {                                                                                                      \
        _pw_log_mdh(PW_LOG_MDH_PREFIX LOG_MESSAGE PW_LOG_MDH_SUFFIX PW_COMMA_ARGS(PW_LOG_MDH_TIMESTAMP_VALUE) \
                        PW_COMMA_ARGS(_pw_log_mdh_level_name(level)) PW_COMMA_ARGS(PW_LOG_MODULE_NAME)        \
                            PW_COMMA_ARGS(PW_LOG_MDH_FILENAME) PW_COMMA_ARGS(PW_LOG_MDH_LINE)                 \
                                PW_COMMA_ARGS(__VA_ARGS__));                                                  \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* PW_LOG_MDH_BACKEND_H_ */
