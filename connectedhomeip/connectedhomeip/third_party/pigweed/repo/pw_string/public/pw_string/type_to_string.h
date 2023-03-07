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
#pragma once

// This file provides functions for writing string representations of a few
// types to character buffers. Generally, the generic ToString function defined
// in "pw_string/to_string.h" should be used instead of these functions.

#include <cstdint>
#include <string_view>
#include <type_traits>

#include "pw_span/span.h"
#include "pw_status/status_with_size.h"
#include "pw_string/util.h"

namespace pw::string {

// Returns the number of digits in the decimal representation of the provided
// non-negative integer. Returns 1 for 0 or 1 + log base 10 for other numbers.
uint_fast8_t DecimalDigitCount(uint64_t integer);

// Returns the number of digits in the hexadecimal representation of the
// provided non-negative integer.
constexpr uint_fast8_t HexDigitCount(uint64_t integer) {
  return static_cast<uint_fast8_t>((64 - __builtin_clzll(integer | 1u) + 3) /
                                   4);
}

// Writes an integer as a null-terminated string in base 10. Returns the number
// of characters written, excluding the null terminator, and the status.
//
// Numbers are never truncated; if the entire number does not fit, only a null
// terminator is written and the status is RESOURCE_EXHAUSTED.
//
// IntToString is templated, but a single 64-bit integer implementation is used
// for all integer types. The template is used for two reasons:
//
//   1. IntToString(int64_t) and IntToString(uint64_t) overloads are ambiguous
//      when called with types other than int64_t or uint64_t. Using the
//      template allows IntToString to be called with any integral type.
//
//   2. Templating IntToString allows the compiler to emit small functions like
//      IntToString<int> or IntToString<short> that perform casting / sign
//      extension on the various integer types. This saves code size, since call
//      sites pass their arguments directly and casting instructions are shared.
//
template <typename T>
StatusWithSize IntToString(T value, span<char> buffer) {
  if constexpr (std::is_signed_v<T>) {
    return IntToString<int64_t>(value, buffer);
  } else {
    return IntToString<uint64_t>(value, buffer);
  }
}

template <>
StatusWithSize IntToString(uint64_t value, span<char> buffer);

template <>
StatusWithSize IntToString(int64_t value, span<char> buffer);

// Writes an integer as a hexadecimal string. Semantics match IntToString. The
// output is lowercase without a leading 0x. min_width adds leading zeroes such
// that the final string is at least the specified number of characters wide.
StatusWithSize IntToHexString(uint64_t value,
                              span<char> buffer,
                              uint_fast8_t min_width = 0);

// Rounds a floating point number to an integer and writes it as a
// null-terminated string. Returns the number of characters written, excluding
// the null terminator, and the status.
//
// Numbers are never truncated; if the entire number does not fit, only a null
// terminator is written and the status is RESOURCE_EXHAUSTED.
//
// WARNING: This is NOT a fully-functioning float-printing implementation! It
// simply outputs the closest integer, "inf", or "NaN". Floating point numbers
// too large to represent as a 64-bit int are treated as infinite.
//
// Examples:
//
//   FloatAsIntToString(1.25, buffer)     -> writes "1" to the buffer
//   FloatAsIntToString(-4.9, buffer)     -> writes "-5" to the buffer
//   FloatAsIntToString(3.5e20, buffer)   -> writes "inf" to the buffer
//   FloatAsIntToString(INFINITY, buffer) -> writes "-inf" to the buffer
//   FloatAsIntToString(-NAN, buffer)     -> writes "-NaN" to the buffer
//
StatusWithSize FloatAsIntToString(float value, span<char> buffer);

// Writes a bool as "true" or "false". Semantics match CopyEntireString.
StatusWithSize BoolToString(bool value, span<char> buffer);

// String used to represent null pointers.
inline constexpr std::string_view kNullPointerString("(null)");

// Writes the pointer's address or kNullPointerString. Semantics match
// CopyEntireString.
StatusWithSize PointerToString(const void* pointer, span<char> buffer);

// Specialized form of pw::string::Copy which supports nullptr values.
//
// Copies the string to the buffer, truncating if the full string does not fit.
// Always null terminates if buffer.size() > 0.
//
// If value is a nullptr, then "(null)" is used as a fallback.
//
// Returns the number of characters written, excluding the null terminator. If
// the string is truncated, the status is RESOURCE_EXHAUSTED.
inline StatusWithSize CopyStringOrNull(const std::string_view& value,
                                       span<char> buffer) {
  return Copy(value, buffer);
}
inline StatusWithSize CopyStringOrNull(const char* value, span<char> buffer) {
  if (value == nullptr) {
    return PointerToString(value, buffer);
  }
  return Copy(value, buffer);
}

// Copies the string to the buffer, if the entire string fits. Always null
// terminates if buffer.size() > 0.
//
// If value is a nullptr, then "(null)" is used as a fallback.
//
// Returns the number of characters written, excluding the null terminator. If
// the full string does not fit, only a null terminator is written and the
// status is RESOURCE_EXHAUSTED.
StatusWithSize CopyEntireStringOrNull(const std::string_view& value,
                                      span<char> buffer);

// Same as the string_view form of CopyEntireString, except that if value is a
// nullptr, then "(null)" is used as a fallback.
inline StatusWithSize CopyEntireStringOrNull(const char* value,
                                             span<char> buffer) {
  if (value == nullptr) {
    return PointerToString(value, buffer);
  }
  return CopyEntireStringOrNull(std::string_view(value), buffer);
}

// This function is a fallback that is called if by ToString if no overload
// matches. No definition is provided, so attempting to print an unsupported
// type causes a linker error.
//
// Applications may define pw::string::UnknownTypeToString to support generic
// printing for unknown types, if desired. Implementations must follow the
// ToString semantics.
template <typename T>
StatusWithSize UnknownTypeToString(const T& value, span<char> buffer);

}  // namespace pw::string
