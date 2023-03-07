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

// Provides the ToString function, which outputs string representations of
// arbitrary types to a buffer.
//
// ToString returns the number of characters written, excluding the null
// terminator, and a status. A null terminator is always written if the output
// buffer has room.
//
// ToString functions may be defined for any type. This is done by providing a
// ToString template specialization in the pw namespace. The specialization must
// follow ToString's semantics:
//
//   1. Always null terminate if the output buffer has room.
//   2. Return the number of characters written, excluding the null terminator,
//      as a StatusWithSize.
//   3. If the buffer is too small to fit the output, return a StatusWithSize
//      with the number of characters written and a status of
//      RESOURCE_EXHAUSTED. Other status codes may be used for different errors.
//
// For example, providing the following specialization would allow ToString, and
// any classes that use it, to print instances of a custom type:
//
//   namespace pw {
//
//   template <>
//   StatusWithSize ToString<SomeCustomType>(const SomeCustomType& value,
//                           span<char> buffer) {
//     return /* ... implementation ... */;
//   }
//
//   }  // namespace pw
//
// Note that none of the functions in this module use std::snprintf. ToString
// overloads may use snprintf if needed, but the ToString semantics must be
// maintained.
//
// ToString is a low-level function. To write complex objects to string, a
// StringBuilder may be easier to work with. StringBuilder's operator<< may be
// overloaded for custom types.

#include <string_view>
#include <type_traits>

#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"
#include "pw_string/format.h"
#include "pw_string/internal/config.h"
#include "pw_string/type_to_string.h"

namespace pw {

// This function provides string printing numeric types, enums, and anything
// that convertible to a std::string_view, such as std::string.
template <typename T>
StatusWithSize ToString(const T& value, span<char> buffer) {
  if constexpr (std::is_same_v<std::remove_cv_t<T>, bool>) {
    return string::BoolToString(value, buffer);
  } else if constexpr (std::is_same_v<std::remove_cv_t<T>, char>) {
    return string::Copy(std::string_view(&value, 1), buffer);
  } else if constexpr (std::is_integral_v<T>) {
    return string::IntToString(value, buffer);
  } else if constexpr (std::is_enum_v<T>) {
    return string::IntToString(std::underlying_type_t<T>(value), buffer);
  } else if constexpr (std::is_floating_point_v<T>) {
    if constexpr (string::internal::config::kEnableDecimalFloatExpansion) {
      // TODO(hepler): Look into using the float overload of std::to_chars when
      // it is available.
      return string::Format(buffer, "%.3f", value);
    } else {
      return string::FloatAsIntToString(value, buffer);
    }
  } else if constexpr (std::is_convertible_v<T, std::string_view>) {
    return string::CopyStringOrNull(value, buffer);
  } else if constexpr (std::is_pointer_v<std::remove_cv_t<T>> ||
                       std::is_null_pointer_v<T>) {
    return string::PointerToString(value, buffer);
  } else {
    // By default, no definition of UnknownTypeToString is provided.
    return string::UnknownTypeToString(value, buffer);
  }
}

// ToString overloads for Pigweed types. To override ToString for a custom type,
// specialize the ToString template function.
inline StatusWithSize ToString(Status status, span<char> buffer) {
  return string::Copy(status.str(), buffer);
}

inline StatusWithSize ToString(pw_Status status, span<char> buffer) {
  return ToString(Status(status), buffer);
}

inline StatusWithSize ToString(std::byte byte, span<char> buffer) {
  return string::IntToHexString(static_cast<unsigned>(byte), buffer, 2);
}

}  // namespace pw
