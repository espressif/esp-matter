// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef CTEST_WINDOWS_H
#define CTEST_WINDOWS_H

#include "azure_macro_utils/macro_utils.h"

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif

void ULONG_ToString(char* string, size_t bufferSize, ULONG val);
int ULONG_Compare(ULONG left, ULONG right);
void ULONG64_ToString(char* string, size_t bufferSize, ULONG64 val);
int ULONG64_Compare(ULONG64 left, ULONG64 right);
void LONG_ToString(char* string, size_t bufferSize, LONG val);
int LONG_Compare(LONG left, LONG right);
void LONG64_ToString(char* string, size_t bufferSize, LONG64 val);
int LONG64_Compare(LONG64 left, LONG64 right);
void HRESULT_ToString(char* string, size_t bufferSize, HRESULT val);
int HRESULT_Compare(HRESULT left, HRESULT right);

#ifdef __cplusplus
}
#endif

#endif // CTEST_WINDOWS_H
