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

// Features from the <bit> header introduced in C++20.
#pragma once

#if __has_include(<bit>)
#include <bit>
#endif  // __has_include(<bit>)

namespace pw {

#ifdef __cpp_lib_endian

using std::endian;

#elif defined(__GNUC__)

enum class endian {
  little = __ORDER_LITTLE_ENDIAN__,
  big = __ORDER_BIG_ENDIAN__,
  native = __BYTE_ORDER__,
};

#else

static_assert(false,
              "The pw::endian enum is not defined for this compiler. Add a "
              "definition to pw_bytes/bit.h.");

#endif  // __cpp_lib_endian

}  // namespace pw
