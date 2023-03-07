// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "azure_c_shared_utility/xlogging.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    void consolelogger_log(LOG_CATEGORY log_category, const char* file, const char* func, int line, unsigned int options, const char* format, ...);

#if (defined(_MSC_VER))
    void consolelogger_log_with_GetLastError(const char* file, const char* func, int line, const char* format, ...);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CONSOLELOGGER_H */
