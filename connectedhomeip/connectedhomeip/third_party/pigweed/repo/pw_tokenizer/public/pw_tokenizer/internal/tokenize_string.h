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

// Selects the hash macro implementation to use. The implementation selected
// depends on the language (C or C++) and value of
// PW_TOKENIZER_CFG_C_HASH_LENGTH. The options are:
//
//   - C++ hash constexpr function, which works for any hash length
//   - C 80-character hash macro
//   - C 96-character hash macro
//   - C 128-character hash macro
//
// C hash macros for other lengths may be generated using generate_hash_macro.py
// and added to this file.
#pragma once

#include <stdint.h>

#define _PW_TOKENIZER_ENTRY_MAGIC 0xBAA98DEE

#ifdef __cplusplus

#include "pw_containers/to_array.h"
#include "pw_preprocessor/compiler.h"

namespace pw {
namespace tokenizer {
namespace internal {

// The C++ tokenzied string entry supports both string literals and char arrays,
// such as __func__.
template <uint32_t kDomainSize, uint32_t kStringSize>
PW_PACKED(class)
Entry {
 public:
  constexpr Entry(uint32_t token,
                  const char(&domain)[kDomainSize],
                  const char(&string)[kStringSize])
      : magic_(_PW_TOKENIZER_ENTRY_MAGIC),
        token_(token),
        domain_size_(kDomainSize),
        string_size_(kStringSize),
        domain_(containers::to_array(domain)),
        string_(containers::to_array(string)) {}

 private:
  static_assert(kStringSize > 0u && kDomainSize > 0u,
                "The string and domain must have at least a null terminator");

  uint32_t magic_;
  uint32_t token_;
  uint32_t domain_size_;
  uint32_t string_size_;
  std::array<char, kDomainSize> domain_;
  std::array<char, kStringSize> string_;
};

// Use this MakeEntry function so that the type doesn't have to be specified in
// the macro. Specifying the type causes problems when the tokenization macro is
// used as an argument to another macro because it requires template arguments,
// which the preprocessor misinterprets as macro arguments.
template <uint32_t kDomainSize, uint32_t kStringSize>
constexpr Entry<kDomainSize, kStringSize> MakeEntry(
    uint32_t token,
    const char (&domain)[kDomainSize],
    const char (&string)[kStringSize]) {
  return {token, domain, string};
}

}  // namespace internal
}  // namespace tokenizer
}  // namespace pw

#else  // In C, define a struct inline with appropriately-sized string members.

#define _PW_TOKENIZER_STRING_ENTRY(                   \
    calculated_token, domain_literal, string_literal) \
  PW_PACKED(struct) {                                 \
    uint32_t magic;                                   \
    uint32_t token;                                   \
    uint32_t domain_size;                             \
    uint32_t string_length;                           \
    char domain[sizeof(domain_literal)];              \
    char string[sizeof(string_literal)];              \
  }                                                   \
  _PW_TOKENIZER_UNIQUE(_pw_tokenizer_string_entry_)   \
  _PW_TOKENIZER_SECTION = {                           \
      _PW_TOKENIZER_ENTRY_MAGIC,                      \
      calculated_token,                               \
      sizeof(domain_literal),                         \
      sizeof(string_literal),                         \
      domain_literal,                                 \
      string_literal,                                 \
  }

#endif  // __cplusplus

// In C++17, use a constexpr function to calculate the hash.
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L && \
    defined(__cpp_inline_variables)

#include "pw_tokenizer/hash.h"

#define PW_TOKENIZER_STRING_TOKEN(format) ::pw::tokenizer::Hash(format)

#else  // In C or older C++ code, use the hashing macro.

#if PW_TOKENIZER_CFG_C_HASH_LENGTH == 80

#include "pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_80_hash_macro.h"
#define PW_TOKENIZER_STRING_TOKEN PW_TOKENIZER_65599_FIXED_LENGTH_80_HASH

#elif PW_TOKENIZER_CFG_C_HASH_LENGTH == 96

#include "pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_96_hash_macro.h"
#define PW_TOKENIZER_STRING_TOKEN PW_TOKENIZER_65599_FIXED_LENGTH_96_HASH

#elif PW_TOKENIZER_CFG_C_HASH_LENGTH == 128

#include "pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_128_hash_macro.h"
#define PW_TOKENIZER_STRING_TOKEN PW_TOKENIZER_65599_FIXED_LENGTH_128_HASH

#elif PW_TOKENIZER_CFG_C_HASH_LENGTH == 256

#include "pw_tokenizer/internal/pw_tokenizer_65599_fixed_length_256_hash_macro.h"
#define PW_TOKENIZER_STRING_TOKEN PW_TOKENIZER_65599_FIXED_LENGTH_256_HASH

#else  // unsupported hash length

// Only hash lengths for which there is a corresponding macro header
// (pw_tokenizer/internal/mash_macro_#.h) are supported. Additional macros may
// be generated with the generate_hash_macro.py function. New macro headers must
// be added to this file.
#error "Unsupported value for PW_TOKENIZER_CFG_C_HASH_LENGTH"

// Define a placeholder macro to give clearer compilation errors.
#define PW_TOKENIZER_STRING_TOKEN(unused) 0u

#endif  // PW_TOKENIZER_CFG_C_HASH_LENGTH

#endif  // __cpp_constexpr >= 201304L && defined(__cpp_inline_variables)
