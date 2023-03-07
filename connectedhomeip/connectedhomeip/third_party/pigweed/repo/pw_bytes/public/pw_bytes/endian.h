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

#include <algorithm>
#include <array>
#include <cstring>
#include <type_traits>

#include "pw_bytes/array.h"
#include "pw_bytes/bit.h"
#include "pw_bytes/span.h"
#include "pw_span/span.h"

namespace pw::bytes {
namespace internal {

// Use a struct rather than an alias to give the type a more reasonable name.
template <typename T>
struct EquivalentUintImpl
    : std::conditional<
          sizeof(T) == 1,
          uint8_t,
          std::conditional_t<
              sizeof(T) == 2,
              uint16_t,
              std::conditional_t<
                  sizeof(T) == 4,
                  uint32_t,
                  std::conditional_t<sizeof(T) == 8, uint64_t, void>>>> {
  static_assert(std::is_integral_v<T>);
};

template <typename T>
using EquivalentUint = typename EquivalentUintImpl<T>::type;

template <typename T>
constexpr std::array<std::byte, sizeof(T)> CopyLittleEndian(T value) {
  return CopyLittleEndian(static_cast<EquivalentUint<T>>(value));
}

template <>
constexpr std::array<std::byte, 1> CopyLittleEndian<uint8_t>(uint8_t value) {
  return MakeArray(value);
}
template <>
constexpr std::array<std::byte, 2> CopyLittleEndian<uint16_t>(uint16_t value) {
  return MakeArray(value & 0x00FF, (value & 0xFF00) >> 8);
}

template <>
constexpr std::array<std::byte, 4> CopyLittleEndian<uint32_t>(uint32_t value) {
  return MakeArray((value & 0x000000FF) >> 0 * 8,
                   (value & 0x0000FF00) >> 1 * 8,
                   (value & 0x00FF0000) >> 2 * 8,
                   (value & 0xFF000000) >> 3 * 8);
}

template <>
constexpr std::array<std::byte, 8> CopyLittleEndian<uint64_t>(uint64_t value) {
  return MakeArray((value & 0x00000000000000FF) >> 0 * 8,
                   (value & 0x000000000000FF00) >> 1 * 8,
                   (value & 0x0000000000FF0000) >> 2 * 8,
                   (value & 0x00000000FF000000) >> 3 * 8,
                   (value & 0x000000FF00000000) >> 4 * 8,
                   (value & 0x0000FF0000000000) >> 5 * 8,
                   (value & 0x00FF000000000000) >> 6 * 8,
                   (value & 0xFF00000000000000) >> 7 * 8);
}

template <typename T>
constexpr T ReverseBytes(T value) {
  EquivalentUint<T> uint = static_cast<EquivalentUint<T>>(value);

  if constexpr (sizeof(uint) == 1) {
    return static_cast<T>(uint);
  } else if constexpr (sizeof(uint) == 2) {
    return static_cast<T>(((uint & 0x00FF) << 8) | ((uint & 0xFF00) >> 8));
  } else if constexpr (sizeof(uint) == 4) {
    return static_cast<T>(((uint & 0x000000FF) << 3 * 8) |  //
                          ((uint & 0x0000FF00) << 1 * 8) |  //
                          ((uint & 0x00FF0000) >> 1 * 8) |  //
                          ((uint & 0xFF000000) >> 3 * 8));
  } else {
    static_assert(sizeof(uint) == 8);
    return static_cast<T>(((uint & 0x00000000000000FF) << 7 * 8) |  //
                          ((uint & 0x000000000000FF00) << 5 * 8) |  //
                          ((uint & 0x0000000000FF0000) << 3 * 8) |  //
                          ((uint & 0x00000000FF000000) << 1 * 8) |  //
                          ((uint & 0x000000FF00000000) >> 1 * 8) |  //
                          ((uint & 0x0000FF0000000000) >> 3 * 8) |  //
                          ((uint & 0x00FF000000000000) >> 5 * 8) |  //
                          ((uint & 0xFF00000000000000) >> 7 * 8));
  }
}

}  // namespace internal

// Functions for reordering bytes in the provided integral value to match the
// specified byte order. These functions are similar to the htonl() family of
// functions.
//
// If the value is converted to non-system endianness, it must NOT be used
// directly, since the value will be meaningless. Such values are only suitable
// to memcpy'd or sent to a different device.
template <typename T>
constexpr T ConvertOrder(endian from, endian to, T value) {
  return from == to ? value : internal::ReverseBytes(value);
}

// Converts a value from native byte order to the specified byte order. Since
// this function changes the value's endianness, the result should only be used
// to memcpy the bytes to a buffer or send to a different device.
template <typename T>
constexpr T ConvertOrderTo(endian to_endianness, T value) {
  return ConvertOrder(endian::native, to_endianness, value);
}

// Converts a value from the specified byte order to the native byte order.
template <typename T>
constexpr T ConvertOrderFrom(endian from_endianness, T value) {
  return ConvertOrder(from_endianness, endian::native, value);
}

// Copies the value to a std::array with the specified endianness.
template <typename T>
constexpr auto CopyInOrder(endian order, T value) {
  return internal::CopyLittleEndian(ConvertOrderTo(order, value));
}

// Reads a value from a buffer with the specified endianness.
//
// The buffer **MUST** be at least sizeof(T) bytes large! If you are not
// absolutely certain the input buffer is large enough, use the ReadInOrder
// overload that returns bool, which checks the buffer size at runtime.
template <typename T>
T ReadInOrder(endian order, const void* buffer) {
  T value;
  std::memcpy(&value, buffer, sizeof(value));
  return ConvertOrderFrom(order, value);
}

// Reads up to the smaller of max_bytes_to_read and sizeof(T) bytes from a
// buffer with the specified endianness.
//
// The value is zero-initialized. If max_bytes_to_read is smaller than
// sizeof(T), the upper bytes of the value are 0.
//
// The buffer **MUST** be at least as large as the smaller of max_bytes_to_read
// and sizeof(T)!
template <typename T>
T ReadInOrder(endian order, const void* buffer, size_t max_bytes_to_read) {
  T value = {};
  std::memcpy(&value, buffer, std::min(sizeof(value), max_bytes_to_read));
  return ConvertOrderFrom(order, value);
}

// ReadInOrder from a static-extent span, with compile-time bounds checking.
template <typename T,
          typename B,
          size_t kBufferSize,
          typename = std::enable_if_t<kBufferSize != dynamic_extent &&
                                      sizeof(B) == sizeof(std::byte)>>
T ReadInOrder(endian order, span<B, kBufferSize> buffer) {
  static_assert(kBufferSize >= sizeof(T));
  return ReadInOrder<T>(order, buffer.data());
}

// ReadInOrder from a std::array, with compile-time bounds checking.
template <typename T, typename B, size_t kBufferSize>
T ReadInOrder(endian order, const std::array<B, kBufferSize>& buffer) {
  return ReadInOrder<T>(order, span(buffer));
}

// ReadInOrder from a C array, with compile-time bounds checking.
template <typename T, typename B, size_t kBufferSize>
T ReadInOrder(endian order, const B (&buffer)[kBufferSize]) {
  return ReadInOrder<T>(order, span(buffer));
}

// Reads a value with the specified endianness from the buffer, with bounds
// checking. Returns true if successful, false if buffer is too small for a T.
template <typename T>
[[nodiscard]] bool ReadInOrder(endian order, ConstByteSpan buffer, T& value) {
  if (buffer.size() < sizeof(T)) {
    return false;
  }

  value = ReadInOrder<T>(order, buffer.data());
  return true;
}

}  // namespace pw::bytes
