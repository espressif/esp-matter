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

// Log a message with the listed attributes.
void pw_Log(int level,
            unsigned int flags,
            const char* module_name,
            const char* file_name,
            int line_number,
            const char* function_name,
            const char* message,
            ...) PW_PRINTF_FORMAT(7, 8);

PW_EXTERN_C_END

// Log a message with many attributes included.
//
// This is the log macro frontend that funnels everything into the C handler
// above, pw_Log(). It's not efficient at the callsite, since it passes many
// arguments. Additionally, the use of the __FUNC__ macro adds a static const
// char[] variable inside functions with a log.
//
// TODO(b/235289435): Reconsider the naming of this module when more is in
// place.
#define PW_HANDLE_LOG(level, flags, message, ...) \
  do {                                            \
    pw_Log((level),                               \
           (flags),                               \
           PW_LOG_MODULE_NAME,                    \
           __FILE__,                              \
           __LINE__,                              \
           __func__,                              \
           message PW_COMMA_ARGS(__VA_ARGS__));   \
  } while (0)

#ifdef __cplusplus

#include <string_view>

namespace pw::log_basic {

// Sets the function to use to send log messages. Defaults to
// pw::sys_io::WriteLine.
void SetOutput(void (*log_output)(std::string_view log));

}  // namespace pw::log_basic

#endif  // __cplusplus
