// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCK_LOG_H
#define UMOCK_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

    void UMOCK_LOG(const char* format, ...)
        #if defined(__GNUC__) || defined(__clang__)
            __attribute__ ((format (printf, 1, 2)))
        #endif
        ;

#ifdef __cplusplus
}
#endif

#endif /* UMOCK_LOG_H */
