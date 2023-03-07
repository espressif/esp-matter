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

#include "pw_preprocessor/arguments.h"
#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"

PW_EXTERN_C_START

// Empty function for compiling out log statements. Since the function is empty
// and inline, it should be completely compiled out. This function accomplishes
// following:
//
//   - Uses the arguments to PW_LOG, which avoids "unused variable" warnings.
//   - Executes expressions passed to PW_LOG, so that the behavior is consistent
//     between this null backend and a backend that actually logs.
//   - Checks the printf-style format string arguments to PW_LOG.
//
// For compatibility with C and the printf compiler attribute, the declaration
// and definition must be separate and both marked inline.
static inline void pw_log_Ignored(int level,
                                  unsigned int flags,
                                  const char* module_name,
                                  const char* message,
                                  ...) PW_PRINTF_FORMAT(4, 5);

static inline void pw_log_Ignored(int level,
                                  unsigned int flags,
                                  const char* module_name,
                                  const char* message,
                                  ...) {
  (void)level;
  (void)flags;
  (void)module_name;
  (void)message;
}

PW_EXTERN_C_END

#define PW_HANDLE_LOG(level, flags, message, ...) \
  pw_log_Ignored((level),                         \
                 (flags),                         \
                 PW_LOG_MODULE_NAME,              \
                 message PW_COMMA_ARGS(__VA_ARGS__))
