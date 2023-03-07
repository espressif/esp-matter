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

// This size report uses pw::StringBuilder and std::snprintf to write several
// strings to a buffer. Ten strings are written using pw::StringBuilder and
// std::snprintf, tracking the number of bytes written and whether an error
// occurred.
//
// This compares the incremental cost of using StringBuilder instead of
// std::snprintf when both are already in use in a binary.

#include <cstdio>

#include "pw_bloat/bloat_this_binary.h"
#include "pw_string/string_builder.h"

#if !defined(USE_STRING_BUILDER)
#error "USE_STRING_BUILDER must be defined"
#endif  // !defined(USE_STRING_BUILDER)

#if USE_STRING_BUILDER

#define FORMAT_STRING(string_builder_out, ...) sb << string_builder_out

#else  // std::snprintf

#define FORMAT_STRING(unused_string_builder_out, ...) \
  ProcessResult(buffer,                               \
                &bytes,                               \
                sizeof(buffer) - bytes,               \
                std::snprintf(buffer, sizeof(buffer) - bytes, __VA_ARGS__))

#endif  // USE_STRING_BUILDER

bool ProcessResult(char* buffer, unsigned* total, unsigned size, int result) {
  if (result < 0) {
    // Null-terminate since snprintf doesn't report the written count.
    buffer[size] = '\0';
    return false;
  }

  if (static_cast<unsigned>(result) >= size) {
    if (size > 0u) {
      *total += size - 1;
    }
    return false;
  }

  *total += result;
  return true;
}

volatile bool get_boolean;
volatile int get_integer;
char* volatile get_buffer;
char* volatile get_string;

int main() {
  int integer = get_integer;
  bool boolean = get_boolean;
  auto string = get_string;

  char* buffer = get_buffer;
  unsigned size = get_integer;

  unsigned bytes = 0;

  // Use std::snprintf and pw::StringBuilder so they're both accounted for in
  // the base.
  int result = std::snprintf(buffer,
                             size,
                             "Hello, %s. The answer to 3 == %d is %s.",
                             string,
                             integer,
                             boolean ? "true" : "false");

  ProcessResult(buffer, &bytes, size, result);

  pw::StringBuilder sb(pw::span(buffer, size));
  sb << "This is part of the base " << 123 << false << '\n';

  sb.clear();

  // Add five strings with either StringBuilder or std::snprintf.
  FORMAT_STRING("Three", "Three");
  FORMAT_STRING("point " << 1, "Three point %d", 1);
  FORMAT_STRING("four "
                    << "one" << ' ' << 5u,
                "four %s %u",
                "one",
                5u);
  FORMAT_STRING(string, "%s", string);
  FORMAT_STRING("-->" << string << string << string << ' ' << integer << ' '
                      << boolean << '!',
                "--> %s%s%s %d %d!",
                string,
                string,
                string,
                integer,
                boolean);

  // Add five more strings with either StringBuilder or std::snprintf.
  FORMAT_STRING("Three", "Three");
  FORMAT_STRING("point " << 1, "Three point %d", 1);
  FORMAT_STRING("four "
                    << "one" << ' ' << 5u,
                "four %s %u",
                "one",
                5u);
  FORMAT_STRING(string, "%s", string);
  FORMAT_STRING(string << string << string << ' ' << integer << ' ' << boolean,
                "%s%s%s %d %d",
                string,
                string,
                string,
                integer,
                boolean);

  bytes += sb.size();
  return bytes;
}
