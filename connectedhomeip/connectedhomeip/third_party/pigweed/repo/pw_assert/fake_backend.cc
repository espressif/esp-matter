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

#include "pw_assert_test/fake_backend.h"

#include <cstring>

#include "pw_span/span.h"
#include "pw_string/string_builder.h"

// Global that's publicly accessible to read captured assert contents.
struct pw_CapturedAssert pw_captured_assert;

bool IsDirSeparator(char c) { return c == '/' || c == '\\'; }

const char* GetFileBasename(const char* filename) {
  int length = std::strlen(filename);
  if (length == 0) {
    return filename;
  }

  // Start on the last character, find the parent directory.
  const char* basename = filename + std::strlen(filename) - 1;
  while (basename != filename && !IsDirSeparator(*basename)) {
    basename--;
  }
  if (IsDirSeparator(*basename)) {
    basename++;
  }
  return basename;
}

void pw_CaptureAssert(const char* file_name,
                      int line_number,
                      const char* function_name,
                      const char* message,
                      ...) {
  // Triggered
  pw_captured_assert.triggered = 1;

  // Filename
  pw_captured_assert.file_name = file_name;

  // Line number
  pw_captured_assert.line_number = line_number;

  // Function name
  pw_captured_assert.function_name = function_name;

  // Message
  pw::StringBuilder builder(pw_captured_assert.message);
  va_list args;
  va_start(args, message);
  builder.FormatVaList(message, args);
  va_end(args);
}
