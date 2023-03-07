// Copyright 2021 The Pigweed Authors
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

#include <cstddef>

namespace pw {
namespace string {
namespace internal {

// Calculates the length of a null-terminated string up to the specified maximum
// length. If str is nullptr, returns 0.
//
// This function is a constexpr version of C11's strnlen_s.
//
// WARNING: this will return a length even if the string is NOT null-terminated!
// ClampedCString and NullTerminatedLength are recommended which are built on
// top of this.
constexpr size_t ClampedLength(const char* str, size_t max_len) {
  size_t length = 0;

  if (str != nullptr) {
    for (; length < max_len; ++length) {
      if (str[length] == '\0') {
        break;
      }
    }
  }

  return length;
}

}  // namespace internal
}  // namespace string
}  // namespace pw
