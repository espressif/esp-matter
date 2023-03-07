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

// This size report uses pw::string::Format and std::snprintf to write to the
// same buffer numerous times. No error handling or size checking is done.
//
// This compares the overhead of calling pw::string::Format with a span to
// calling std::snprintf with a separate pointer and buffer size.

#ifndef USE_FORMAT
#error "USE_FORMAT must be defined"
#endif  // USE_FORMAT

#if USE_FORMAT

#include "pw_string/format.h"

#define FORMAT_CASE(...)                  \
  pw::string::Format(buffer, __VA_ARGS__) \
      .IgnoreError()  // TODO(b/242598609): Handle Status properly

#else  // std::snprintf

#include <cstdio>

#define FORMAT_CASE(...) std::snprintf(buffer, buffer_size, __VA_ARGS__)

#endif  // USE_FORMAT

namespace pw::string {

char buffer_1[128];
char buffer_2[128];

char* volatile get_buffer_1 = buffer_1;
char* volatile get_buffer_2 = buffer_2;
volatile unsigned get_size;

void OutputStringsToBuffer() {
#if USE_FORMAT
  auto buffer = span(get_buffer_1, get_size);
#else
  char* buffer = get_buffer_1;
  unsigned buffer_size = get_size;
#endif  // USE_FORMAT

  const char* string = get_buffer_2;
  unsigned value = get_size;

  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, value);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);
  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, value);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);

  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, value);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);
  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, value);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);

  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, value);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);
  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, value);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);

  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, value);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);
  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, value);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);

  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, value);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);
  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, value);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);
}

}  // namespace pw::string

int main() {
  pw::string::OutputStringsToBuffer();
  return 0;
}
