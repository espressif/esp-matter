// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CTRS_SPRINTF_H
#define CTRS_SPRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

    char* ctrs_sprintf_char(const char* format, ...);
    void ctrs_sprintf_free(char* string);

#ifdef __cplusplus
}
#endif

#endif /* CTRS_SPRINTF_H */

