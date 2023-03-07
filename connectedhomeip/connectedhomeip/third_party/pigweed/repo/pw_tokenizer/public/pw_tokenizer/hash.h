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
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "pw_polyfill/standard.h"
#include "pw_preprocessor/util.h"

// The hash implementation uses std::string_view. Use the C implementation when
// compiling with older C++ standards.
#if PW_CXX_STANDARD_IS_SUPPORTED(17)

#include <string_view>

#include "pw_preprocessor/compiler.h"
#include "pw_tokenizer/config.h"

namespace pw::tokenizer {

// The constant to use when generating the hash. Changing this changes the value
// of all hashes, so do not change it randomly.
inline constexpr uint32_t k65599HashConstant = 65599u;

// Calculates the hash of a string. This function calculates hashes at either
// runtime or compile time in C++ code.
//
// Unlike the C hashing macro, this hash supports strings of any length. Strings
// longer than the maximum C hashing macro's length will hash different values
// in C and C++. If the same very long string is used in C and C++, the string
// will appear with both tokens in the resulting database.
//
// This hash is calculated with the following equation, where s is the string
// and k is the maximum hash length:
//
//    H(s, k) = len(s) + 65599 * s[0] + 65599^2 * s[1] + ... + 65599^k * s[k-1]
//
// The hash algorithm is a modified version of the x65599 hash used by the SDBM
// open source project. The modifications were made to support hashing in C
// macros. These are the differences from x65599:
//
//   - Characters are hashed in reverse order.
//   - The string length is hashed as the first character in the string.
//
constexpr uint32_t Hash(std::string_view string)
    PW_NO_SANITIZE("unsigned-integer-overflow") {
  // The length is hashed as if it were the first character.
  uint32_t hash = string.size();
  uint32_t coefficient = k65599HashConstant;

  // Hash all of the characters in the string as unsigned ints.
  // The coefficient calculation is done modulo 0x100000000, so the unsigned
  // integer overflows are intentional.
  for (uint8_t ch : string) {
    hash += coefficient * ch;
    coefficient *= k65599HashConstant;
  }

  return hash;
}

// Take the string as an array to support either literals or character arrays,
// but not const char*.
template <size_t kSize>
constexpr uint32_t Hash(const char (&string)[kSize]) {
  static_assert(kSize > 0);
  return Hash(std::string_view(string, kSize - 1));
}

// This hash function is equivalent to the C hashing macros. It hashses a string
// up to a maximum length.
constexpr uint32_t PwTokenizer65599FixedLengthHash(
    std::string_view string,
    size_t hash_length = PW_TOKENIZER_CFG_C_HASH_LENGTH)
    PW_NO_SANITIZE("unsigned-integer-overflow") {
  uint32_t hash = string.size();
  uint32_t coefficient = k65599HashConstant;

  for (uint8_t ch : string.substr(0, hash_length)) {
    hash += coefficient * ch;
    coefficient *= k65599HashConstant;
  }

  return hash;
}

// Character array version of PwTokenizer65599FixedLengthHash.
template <size_t kSize>
constexpr uint32_t PwTokenizer65599FixedLengthHash(
    const char (&string)[kSize],
    size_t hash_length = PW_TOKENIZER_CFG_C_HASH_LENGTH) {
  static_assert(kSize > 0);
  return PwTokenizer65599FixedLengthHash(std::string_view(string, kSize - 1),
                                         hash_length);
}

}  // namespace pw::tokenizer

#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

// C version of the fixed-length hash. Can be used to calculate hashes
// equivalent to the hashing macros at runtime in C.
PW_EXTERN_C uint32_t pw_tokenizer_65599FixedLengthHash(const char* string,
                                                       size_t string_length,
                                                       size_t hash_length);
