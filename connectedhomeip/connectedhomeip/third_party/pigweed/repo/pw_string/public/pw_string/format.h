// Copyright 2019 The Pigweed Authors
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

// This provides the pw::string::Format functions, which are safer alternatives
// to std::snprintf and std::vsnprintf. The snprintf return value is awkward to
// interpret, and misinterpreting it can lead to serious bugs.
//
// These functions return a StatusWithSize. The Status is set to reflect any
// errors and the return value is always the number of characters written before
// the null terminator.

#include <cstdarg>

#include "pw_preprocessor/compiler.h"
#include "pw_span/span.h"
#include "pw_status/status_with_size.h"

namespace pw::string {

// Writes a printf-style formatted string to the provided buffer, similarly to
// std::snprintf. Returns the number of characters written, excluding the null
// terminator. The buffer is always null-terminated unless it is empty.
//
// The status is
//
//   OkStatus() if the operation succeeded,
//   Status::ResourceExhausted() if the buffer was too small to fit the output,
//   Status::InvalidArgument() if there was a formatting error.
//
PW_PRINTF_FORMAT(2, 3)
StatusWithSize Format(span<char> buffer, const char* format, ...);

// Writes a printf-style formatted string with va_list-packed arguments to the
// provided buffer, similarly to std::vsnprintf. The return value is the same as
// above.
PW_PRINTF_FORMAT(2, 0)
StatusWithSize FormatVaList(span<char> buffer,
                            const char* format,
                            va_list args);

}  // namespace pw::string
