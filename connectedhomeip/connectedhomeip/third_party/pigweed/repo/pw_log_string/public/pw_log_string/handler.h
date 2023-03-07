// Copyright 2022 The Pigweed Authors
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

#include <stdarg.h>

#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"

PW_EXTERN_C_START

// Invokes pw_log_string_HandleMessageVaList, this is implemented by the facade.
void pw_log_string_HandleMessage(int level,
                                 unsigned int flags,
                                 const char* module_name,
                                 const char* file_name,
                                 int line_number,
                                 const char* message,
                                 ...) PW_PRINTF_FORMAT(6, 7);

// Log a message with the listed attributes, this must be implemented by the
// backend.
void pw_log_string_HandleMessageVaList(int level,
                                       unsigned int flags,
                                       const char* module_name,
                                       const char* file_name,
                                       int line_number,
                                       const char* message,
                                       va_list args);

PW_EXTERN_C_END
