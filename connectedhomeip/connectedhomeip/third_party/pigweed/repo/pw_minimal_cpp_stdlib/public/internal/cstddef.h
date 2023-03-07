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

#include "pw_polyfill/standard_library/namespace.h"

_PW_POLYFILL_BEGIN_NAMESPACE_STD

using ::ptrdiff_t;
using ::size_t;
using nullptr_t = decltype(nullptr);
using ::max_align_t;

#define __cpp_lib_byte 201603L

enum class byte : unsigned char {};

template <typename I>
constexpr I to_integer(byte b) noexcept {
  return I(b);
}

constexpr byte operator|(byte l, byte r) noexcept {
  return byte(static_cast<unsigned int>(l) | static_cast<unsigned int>(r));
}

constexpr byte operator&(byte l, byte r) noexcept {
  return byte(static_cast<unsigned int>(l) & static_cast<unsigned int>(r));
}

constexpr byte operator^(byte l, byte r) noexcept {
  return byte(static_cast<unsigned int>(l) ^ static_cast<unsigned int>(r));
}

constexpr byte operator~(byte b) noexcept {
  return byte(~static_cast<unsigned int>(b));
}

template <typename I>
constexpr byte operator<<(byte b, I shift) noexcept {
  return byte(static_cast<unsigned int>(b) << shift);
}

template <typename I>
constexpr byte operator>>(byte b, I shift) noexcept {
  return byte(static_cast<unsigned int>(b) >> shift);
}

constexpr byte& operator|=(byte& l, byte r) noexcept { return l = l | r; }
constexpr byte& operator&=(byte& l, byte r) noexcept { return l = l & r; }
constexpr byte& operator^=(byte& l, byte r) noexcept { return l = l ^ r; }

template <typename I>
inline byte& operator<<=(byte& b, I shift) noexcept {
  return b = b << shift;
}

template <typename I>
inline byte& operator>>=(byte& b, I shift) noexcept {
  return b = b >> shift;
}

_PW_POLYFILL_END_NAMESPACE_STD
