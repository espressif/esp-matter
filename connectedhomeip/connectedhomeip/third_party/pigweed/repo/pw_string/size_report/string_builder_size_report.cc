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

// This size report uses pw::StringBuilder (via pw::MakeString) and
// std::snprintf to build a string.
//
// This compares the binary size cost of StringBuilder when it is used instead
// of std::snprintf and when it is used alongside std::snprintf. StringBuilder
// does not use std::snprintf (unless StringBuilder::Format is called), so using
// StringBuilder may allow binaries to avoid std::snprintf altogether.

#include <cstdio>

#include "pw_bloat/bloat_this_binary.h"
#include "pw_string/string_builder.h"

#if !defined(USE_STRING_BUILDER) || !defined(PROVIDE_BASE_SNPRINTF)
#error "USE_STRING_BUILDER must be defined"
#endif  // !defined(USE_STRING_BUILDER) || !defined(PROVIDE_BASE_SNPRINTF)

#if USE_STRING_BUILDER

#include "pw_string/string_builder.h"

#else

unsigned BytesWritten(char* buffer, unsigned size, int result) {
  if (result < 0) {
    // Null-terminate since snprintf doesn't report the written count.
    buffer[size] = '\0';
    return 0;
  }

  if (static_cast<unsigned>(result) >= size) {
    return size > 0u ? size - 1 : 0;
  }

  return result;
}

#endif

char string[64] = "hello";

volatile bool get_boolean;
volatile int get_integer;
const char* volatile get_string;

int main() {
  int integer = get_integer;
  bool boolean = get_boolean;

#if PROVIDE_BASE_SNPRINTF
  std::snprintf(string, sizeof(string), "%d", 1234);
#endif  // PROVIDE_BASE_SNPRINTF

#if USE_STRING_BUILDER
  return pw::MakeString<64>("Hello, ",
                            string,
                            ". The answer to 3 == ",
                            integer,
                            " is ",
                            boolean)
      .size();

#else
  char buffer[64];

  int result = std::snprintf(buffer,
                             sizeof(buffer),
                             "Hello, %s. The answer to 3 == %d is %s.",
                             string,
                             integer,
                             boolean ? "true" : "false");

  return BytesWritten(buffer, sizeof(buffer), result);

#endif  // USE_STRING_BUILDER
}
