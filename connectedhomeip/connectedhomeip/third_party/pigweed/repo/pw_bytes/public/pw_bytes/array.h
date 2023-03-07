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

// Utilities for building std::byte arrays from strings or integer values at
// compile time.
#pragma once

#include <array>
#include <cstddef>
#include <iterator>

#include "pw_polyfill/language_feature_macros.h"

namespace pw::bytes {
namespace internal {

template <typename T>
constexpr bool UseBytesDirectly = std::is_integral_v<T> || std::is_enum_v<T>;

// Internal implementation functions. CopyBytes copies bytes from an array of
// byte-sized elements or the underlying bytes of an integer (as little-endian).
// std::memcpy cannot be used since it is not constexpr.
template <typename B, typename T, typename... Args>
PW_CONSTEVAL void CopyBytes(B* array, T value, Args... args) {
  static_assert(sizeof(B) == sizeof(std::byte));

  if constexpr (UseBytesDirectly<T>) {
    if constexpr (sizeof(T) == 1u) {
      *array++ = static_cast<B>(value);
    } else {
      for (size_t i = 0; i < sizeof(T); ++i) {
        *array++ = static_cast<B>(value & 0xFF);
        value >>= 8;
      }
    }
  } else {
    static_assert(sizeof(value[0]) == sizeof(B));
    for (auto b : value) {
      *array++ = static_cast<B>(b);
    }
  }

  if constexpr (sizeof...(args) > 0u) {
    CopyBytes(array, args...);
  }
}

// Evaluates to the size in bytes of an integer or byte array.
template <typename T>
PW_CONSTEVAL size_t SizeOfBytes(const T& arg) {
  if constexpr (UseBytesDirectly<T>) {
    return sizeof(arg);
  } else {
    static_assert(sizeof(arg[0]) == sizeof(std::byte));
    return std::size(arg);
  }
}

template <typename B, typename T, size_t... kIndex>
PW_CONSTEVAL auto String(const T& array, std::index_sequence<kIndex...>) {
  return std::array{static_cast<B>(array[kIndex])...};
}

template <typename T, typename U>
PW_CONSTEVAL bool CanBeRepresentedAsByteType(const U& value) {
  return static_cast<U>(static_cast<T>(value)) == value;
}

}  // namespace internal

// Concatenates arrays or integers as a byte array at compile time. Integer
// values are copied little-endian. Spans are copied byte-for-byte.
template <typename B = std::byte, typename... Args>
PW_CONSTEVAL auto Concat(Args... args) {
  std::array<B, (internal::SizeOfBytes(args) + ...)> bytes{};
  internal::CopyBytes(bytes.begin(), args...);
  return bytes;
}

// Converts a string literal to an array of bytes, without the trailing '\0'.
template <typename B = std::byte,
          size_t kSize,
          typename Indices = std::make_index_sequence<kSize - 1>>
PW_CONSTEVAL auto String(const char (&str)[kSize]) {
  return internal::String<B>(str, Indices{});
}

// String overload for the empty string "".
template <typename B = std::byte>
PW_CONSTEVAL auto String(const char (&)[1]) {
  return std::array<B, 0>{};
}

// Creates an array of bytes from values passed as template parameters. The
// values are guaranteed to be representable in the destination byte type.
template <typename B, auto... values>
PW_CONSTEVAL auto Array() {
  static_assert((internal::CanBeRepresentedAsByteType<B>(values) && ...));
  return std::array<B, sizeof...(values)>{static_cast<B>(values)...};
}

// Array() defaults to using std::byte.
template <auto... values>
PW_CONSTEVAL auto Array() {
  return Array<std::byte, values...>();
}

// Creates an initialized array of bytes. Initializes the array to a value or
// the return values from a function that accepts the index as a parameter.
template <typename B, size_t kSize, typename T>
constexpr auto Initialized(const T& value_or_function) {
  std::array<B, kSize> array{};

  for (size_t i = 0; i < kSize; ++i) {
    if constexpr (std::is_integral_v<T>) {
      array[i] = static_cast<B>(value_or_function);
    } else {
      array[i] = static_cast<B>(value_or_function(i));
    }
  }
  return array;
}

// Initialized(value_or_function) defaults to using std::byte.
template <size_t kSize, typename T>
constexpr auto Initialized(const T& value_or_function) {
  return Initialized<std::byte, kSize>(value_or_function);
}

// Creates an array of bytes from a series of function arguments. Unlike
// Array(), MakeArray() cannot check if the values fit in the destination type.
// MakeArray() should only be used when Array() is not suitable.
template <typename B = std::byte, typename... Args>
constexpr auto MakeArray(const Args&... args) {
  return std::array<B, sizeof...(args)>{static_cast<B>(args)...};
}

}  // namespace pw::bytes
