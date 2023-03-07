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

// This header provides internal macros used by the tokenizer module.
#pragma once

#include <stdint.h>

#include "pw_preprocessor/arguments.h"
#include "pw_tokenizer/config.h"

// The size of the argument types variable determines the number of arguments
// supported in tokenized strings.
#if PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES == 4

#include "pw_tokenizer/internal/argument_types_macro_4_byte.h"

// Encoding types in a uint32_t supports 14 arguments with 2 bits per argument.
#define PW_TOKENIZER_MAX_SUPPORTED_ARGS 14
#define PW_TOKENIZER_TYPE_COUNT_SIZE_BITS 4u
#define PW_TOKENIZER_TYPE_COUNT_MASK 0x0Fu

typedef uint32_t pw_tokenizer_ArgTypes;

#elif PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES == 8

#include "pw_tokenizer/internal/argument_types_macro_8_byte.h"

// Encoding types in a uint64_t supports 29 arguments with 2 bits per argument.
#define PW_TOKENIZER_MAX_SUPPORTED_ARGS 29
#define PW_TOKENIZER_TYPE_COUNT_SIZE_BITS 6u
#define PW_TOKENIZER_TYPE_COUNT_MASK 0x1Fu  // only 5 bits will be needed

typedef uint64_t pw_tokenizer_ArgTypes;

#else

#error "Unsupported value for PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES"

#endif  // PW_TOKENIZER_CFG_ARG_TYPES_SIZE_BYTES

// The tokenized string encoding function is a variadic function that works
// similarly to printf. Instead of a format string, however, the argument types
// are packed into a pw_tokenizer_ArgTypes.
//
// The four supported argument types are represented by two-bit argument codes.
// Just four types are required because only printf-compatible arguments are
// supported, and variadic arguments are further converted to a more limited set
// of types.
//
// char* values cannot be printed as pointers with %p. These arguments are
// always encoded as strings. To format a char* as an address, cast it to void*
// or an integer.
#define PW_TOKENIZER_ARG_TYPE_INT ((pw_tokenizer_ArgTypes)0)
#define PW_TOKENIZER_ARG_TYPE_INT64 ((pw_tokenizer_ArgTypes)1)
#define PW_TOKENIZER_ARG_TYPE_DOUBLE ((pw_tokenizer_ArgTypes)2)
#define PW_TOKENIZER_ARG_TYPE_STRING ((pw_tokenizer_ArgTypes)3)

// Select the int argument type based on the size of the type. Values smaller
// than int are promoted to int.
#define _PW_TOKENIZER_SELECT_INT_TYPE(type)                \
  (sizeof(type) <= sizeof(int) ? PW_TOKENIZER_ARG_TYPE_INT \
                               : PW_TOKENIZER_ARG_TYPE_INT64)

// The _PW_VARARGS_TYPE macro selects the varargs-promoted type at compile time.
// The macro has to be different for C and C++ because C doesn't support
// templates and C++ doesn't support _Generic.
#ifdef __cplusplus

#include <type_traits>

#define _PW_VARARGS_TYPE(arg) ::pw::tokenizer::VarargsType<decltype(arg)>()

namespace pw {
namespace tokenizer {

#ifdef __cpp_if_constexpr  // C++17 version

// This function selects the matching type enum for supported argument types.
template <typename T>
constexpr pw_tokenizer_ArgTypes VarargsType() {
  using ArgType = std::decay_t<T>;

  if constexpr (std::is_floating_point<ArgType>()) {
    return PW_TOKENIZER_ARG_TYPE_DOUBLE;
  } else if constexpr (!std::is_null_pointer<ArgType>() &&
                       std::is_convertible<ArgType, const char*>()) {
    return PW_TOKENIZER_ARG_TYPE_STRING;
  } else if constexpr (sizeof(ArgType) == sizeof(int64_t)) {
    return PW_TOKENIZER_ARG_TYPE_INT64;
  } else {
    static_assert(sizeof(ArgType) <= sizeof(int));
    return PW_TOKENIZER_ARG_TYPE_INT;
  }
}

#else  // C++14 version

template <typename T,
          bool kIsDouble = std::is_floating_point<T>(),
          bool kIsString = !std::is_null_pointer<T>() &&
                           std::is_convertible<T, const char*>(),
          bool kIsInt64 = sizeof(T) == sizeof(int64_t)>
struct SelectVarargsType;

template <typename T, bool kDontCare1, bool kDontCare2>
struct SelectVarargsType<T, true, kDontCare1, kDontCare2> {
  static constexpr pw_tokenizer_ArgTypes kValue = PW_TOKENIZER_ARG_TYPE_DOUBLE;
};

template <typename T, bool kDontCare>
struct SelectVarargsType<T, false, true, kDontCare> {
  static constexpr pw_tokenizer_ArgTypes kValue = PW_TOKENIZER_ARG_TYPE_STRING;
};

template <typename T>
struct SelectVarargsType<T, false, false, true> {
  static constexpr pw_tokenizer_ArgTypes kValue = PW_TOKENIZER_ARG_TYPE_INT64;
};

template <typename T>
struct SelectVarargsType<T, false, false, false> {
  static constexpr pw_tokenizer_ArgTypes kValue = PW_TOKENIZER_ARG_TYPE_INT;
};

template <typename T>
constexpr pw_tokenizer_ArgTypes VarargsType() {
  return SelectVarargsType<typename std::decay<T>::type>::kValue;
}

#endif  // __cpp_if_constexpr

}  // namespace tokenizer
}  // namespace pw

#else  // C version

// This uses a C11 _Generic to select the matching enum value for each supported
// argument type. _Generic evaluates to the expression matching the type of the
// provided expression at compile time.
// clang-format off
#define _PW_VARARGS_TYPE(arg)                                            \
  _Generic((arg),                                                        \
               _Bool:  PW_TOKENIZER_ARG_TYPE_INT,                        \
                char:  PW_TOKENIZER_ARG_TYPE_INT,                        \
         signed char:  PW_TOKENIZER_ARG_TYPE_INT,                        \
       unsigned char:  PW_TOKENIZER_ARG_TYPE_INT,                        \
        signed short:  PW_TOKENIZER_ARG_TYPE_INT,                        \
      unsigned short:  PW_TOKENIZER_ARG_TYPE_INT,                        \
          signed int:  PW_TOKENIZER_ARG_TYPE_INT,                        \
        unsigned int:  PW_TOKENIZER_ARG_TYPE_INT,                        \
         signed long: _PW_TOKENIZER_SELECT_INT_TYPE(signed long),        \
       unsigned long: _PW_TOKENIZER_SELECT_INT_TYPE(unsigned long),      \
    signed long long: _PW_TOKENIZER_SELECT_INT_TYPE(signed long long),   \
  unsigned long long: _PW_TOKENIZER_SELECT_INT_TYPE(unsigned long long), \
               float:  PW_TOKENIZER_ARG_TYPE_DOUBLE,                     \
              double:  PW_TOKENIZER_ARG_TYPE_DOUBLE,                     \
         long double:  PW_TOKENIZER_ARG_TYPE_DOUBLE,                     \
               char*:  PW_TOKENIZER_ARG_TYPE_STRING,                     \
         const char*:  PW_TOKENIZER_ARG_TYPE_STRING,                     \
             default: _PW_TOKENIZER_SELECT_INT_TYPE(void*))
// clang-format on

#endif  // __cplusplus

// Encodes the types of the provided arguments as a pw_tokenizer_ArgTypes
// value. Depending on the size of pw_tokenizer_ArgTypes, the bottom 4 or 6
// bits store the number of arguments and the remaining bits store the types,
// two bits per type.
//
// The arguments are not evaluated; only their types are used to
// select the set their corresponding PW_TOKENIZER_ARG_TYPEs.
#define PW_TOKENIZER_ARG_TYPES(...) \
  PW_DELEGATE_BY_ARG_COUNT(_PW_TOKENIZER_TYPES_, __VA_ARGS__)

#define _PW_TOKENIZER_TYPES_0() ((pw_tokenizer_ArgTypes)0)
