// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef UMOCKAUTOIGNOREARGS_H
#define UMOCKAUTOIGNOREARGS_H

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stddef.h>
#endif

int umockautoignoreargs_is_call_argument_ignored(const char* call, size_t argument_index, int* is_argument_ignored);

#ifdef __cplusplus
}
#endif

#endif /* UMOCKAUTOIGNOREARGS_H */
