// Copyright 2020 The Pigweed Authors
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

#include "pw_assert/config.h"  // For PW_ASSERT_ENABLE_DEBUG
#include "pw_assert_backend/assert_backend.h"

// A header- and constexpr-safe version of PW_CHECK().
//
// If the given condition is false, crash the system. Otherwise, do nothing.
// The condition is guaranteed to be evaluated.
//
// IMPORTANT: Unlike the PW_CHECK_*() suite of macros, not all backends for
// this API capture rich information like line numbers, the file, expression
// arguments, or the stringified expression. Use these macros only when
// absolutely necessary -- in headers, constexpr contexts, or in rare cases
// where the call site overhead of a full PW_CHECK must be avoided. Use
// PW_CHECK_*() whenever possible.
#define PW_ASSERT(condition)                \
  do {                                      \
    if (!(condition)) {                     \
      PW_ASSERT_HANDLE_FAILURE(#condition); \
    }                                       \
  } while (0)

// A header- and constexpr-safe version of PW_DCHECK().
//
// Same as PW_ASSERT(), except that if PW_ASSERT_ENABLE_DEBUG == 1, the assert
// is disabled and condition is not evaluated.
//
// IMPORTANT: Unlike the PW_CHECK_*() suite of macros, not all backends for
// this API capture rich information like line numbers, the file, expression
// arguments, or the stringified expression. Use these macros only when
// absolutely necessary -- in headers, constexpr contexts, or in rare cases
// where the call site overhead of a full PW_CHECK must be avoided. Use
// PW_CHECK_*() whenever possible.
#define PW_DASSERT(condition)                            \
  do {                                                   \
    if ((PW_ASSERT_ENABLE_DEBUG == 1) && !(condition)) { \
      PW_ASSERT_HANDLE_FAILURE(#condition);              \
    }                                                    \
  } while (0)

// A header- and constexpr-safe version of PW_CHECK_OK().
//
// If the condition does not evaluate to PW_STATUS_OK, crash.
// Otherwise, do nothing. The expression is guaranteed to be evaluated.
//
// Unlike `PW_CHECK_OK`, this macro does not currently log the failed status
// kind.
//
// IMPORTANT: Unlike the PW_CHECK_*() suite of macros, not all backends for
// this API capture rich information like line numbers, the file, expression
// arguments, or the stringified expression. Use these macros only when
// absolutely necessary -- in headers, constexpr contexts, or in rare cases
// where the call site overhead of a full PW_CHECK must be avoided. Use
// PW_CHECK_*() whenever possible.
#define PW_ASSERT_OK(expression, ...)                               \
  do {                                                              \
    const _PW_ASSERT_OK_STATUS _pw_assert_ok_status = (expression); \
    if (_pw_assert_ok_status != PW_STATUS_OK) {                     \
      PW_ASSERT_HANDLE_FAILURE(#expression);                        \
    }                                                               \
  } while (0)

#ifdef __cplusplus
#define _PW_ASSERT_OK_STATUS ::pw::Status
#else
#define _PW_ASSERT_OK_STATUS pw_Status
#endif  // __cplusplus
