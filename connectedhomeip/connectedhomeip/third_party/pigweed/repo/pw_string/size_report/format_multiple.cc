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

// This size report uses pw::string::Format and std::snprintf to build a string
// in a buffer with multiple calls. The process aborts if an error occurs and
// the total string size is returned.
//
// This compares the overhead of using pw::string::Format to directly calling
// std::snprintf and doing error handling. It demonstrates that the code for
// using pw::string::Format is much simpler.

#include "pw_bloat/bloat_this_binary.h"

#ifndef USE_FORMAT
#error "USE_FORMAT must be defined"
#endif  // USE_FORMAT

#if USE_FORMAT

#include "pw_string/format.h"

#define FORMAT_FUNCTION(...) \
  pw::string::Format(span(buffer, buffer_size - string_size), __VA_ARGS__)
#define CHECK_RESULT(result) ProcessResult(&string_size, result)

namespace {

bool ProcessResult(unsigned* total, pw::StatusWithSize result) {
  *total += result.size();
  return result.ok();
}

}  // namespace

#else  // std::snprintf

#include <cstdio>

#define FORMAT_FUNCTION(...) std::snprintf(buffer, buffer_size, __VA_ARGS__)
#define CHECK_RESULT(result) \
  ProcessResult(buffer, &string_size, buffer_size - string_size, result)

namespace {

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

}  // namespace

#endif  // USE_FORMAT

#define FORMAT_CASE(...)                             \
  if (!CHECK_RESULT(FORMAT_FUNCTION(__VA_ARGS__))) { \
    return string_size;                              \
  }

namespace pw::string {

char buffer_1[128];
char buffer_2[128];

char* volatile get_buffer_1 = buffer_1;
char* volatile get_buffer_2 = buffer_2;
volatile unsigned get_size;

unsigned OutputStringsToBuffer() {
  char* buffer = get_buffer_1;
  const char* string = get_buffer_2;

  unsigned buffer_size = get_size;
  unsigned string_size = 0;

  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, buffer_size);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);

  FORMAT_CASE("The quick brown");
  FORMAT_CASE("%s", string);
  FORMAT_CASE("jumped over the %s d%ug.", string, buffer_size);
  FORMAT_CASE("One two %s %d %s", "three", 4, "five");
  FORMAT_CASE("a %c %x d %s %f g", 'b', 0xc, "e", 0.0f);

  return string_size;
}

}  // namespace pw::string

int main() {
  pw::bloat::BloatThisBinary();
  return pw::string::OutputStringsToBuffer();
}
