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

// This size report uses pw::string::Format and std::snprintf to write a single
// printf-style string to a buffer. The number of bytes written is returned.
//
// This compares the overhead of using pw::string::Format to directly calling
// std::snprintf and determining the number of bytes written. It demonstrates
// that the code for using pw::string::Format is much simpler.

#include <cstddef>
#include <cstdio>

#include "pw_bloat/bloat_this_binary.h"
#include "pw_string/format.h"

#ifndef USE_FORMAT
#error "USE_FORMAT must be defined!"
#endif  // USE_FORMAT

namespace pw::string {

char buffer_1[128];
char buffer_2[128];

char* volatile get_buffer_1 = buffer_1;
char* volatile get_buffer_2 = buffer_2;
volatile unsigned get_size;

unsigned OutputStringsToBuffer() {
  char* buffer = get_buffer_1;
  unsigned buffer_size = get_size;

#if USE_FORMAT
  // The code for using pw::string::Format is much simpler and safer.
  return Format(
             span(buffer, buffer_size), "hello %s %d", get_buffer_2, get_size)
      .size();
#else   // std::snprintf
  if (buffer_size == 0u) {
    return 0;
  }

  int result =
      std::snprintf(buffer, buffer_size, "hello %s %d", get_buffer_2, get_size);
  if (result < 0) {
    buffer[0] = '\0';
    return 0;
  }
  if (static_cast<size_t>(result) >= buffer_size) {
    return buffer_size - 1;
  }
  return result;
#endif  // USE_FORMAT
}

}  // namespace pw::string

int main() {
  pw::bloat::BloatThisBinary();
  return pw::string::OutputStringsToBuffer();
}
