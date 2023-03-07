// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_BIT_H_
#define LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_BIT_H_

#include <cstring>
#include <limits>

#include "internal/bit.h"
#include "memory.h"
#include "version.h"

#if __has_include(<bit>) && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#include <bit>

#endif  //__has_include(<bit>) && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

namespace cpp20 {

#if defined(__cpp_lib_bit_cast) && __cpp_lib_bit_cast >= 201806L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::bit_cast;

#else  // Use builtin to provide constexpr bit_cast.

#if defined(__has_builtin) && !defined(LIB_STDCOMPAT_NO_BUILTIN_BITCAST)
#if __has_builtin(__builtin_bit_cast)

template <typename To, typename From>
constexpr std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable<To>::value &&
                               std::is_trivially_copyable<From>::value,
                           To>
bit_cast(const From& from) {
  return __builtin_bit_cast(To, from);
}

// Since there are two #if checks, using #else would require code duplication.
// Define a temporary internal macro to indicate that bit_cast was defined.
#define LIB_STDCOMPAT_BIT_CAST_DEFINED_

#endif  // __has_builtin(__builtin_bit_cast)
#endif  // defined(__has_builtin) && !defined(LIB_STDCOMPAT_NO_BUILTIN_BITCAST)

// Use memcpy instead, not constexpr though.
#ifndef LIB_STDCOMPAT_BIT_CAST_DEFINED_

#define LIB_STDCOMPAT_NONCONSTEXPR_BITCAST 1

template <typename To, typename From>
std::enable_if_t<sizeof(To) == sizeof(From) && std::is_trivially_copyable<To>::value &&
                     std::is_trivially_copyable<From>::value,
                 To>
bit_cast(const From& from) {
  std::aligned_storage_t<sizeof(To)> uninitialized_to;
  std::memcpy(static_cast<void*>(&uninitialized_to),
              static_cast<const void*>(cpp17::addressof(from)), sizeof(To));
  return *reinterpret_cast<const To*>(&uninitialized_to);
}

#endif  // LIB_STDCOMPAT_BIT_CAST_DEFINED_

#undef LIB_STDCOMPAT_BIT_CAST_DEFINED_

#endif  //  __cpp_lib_bit_cast >= 201806L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::countl_one;
using std::countl_zero;
using std::countr_one;
using std::countr_zero;
using std::popcount;
using std::rotl;
using std::rotr;

#else

template <class T>
constexpr std::enable_if_t<std::is_unsigned<T>::value, int> countr_zero(T x) noexcept {
  if (x == 0) {
    return std::numeric_limits<T>::digits;
  }

  return internal::count_zeros_from_right(x);
}

template <class T>
constexpr std::enable_if_t<std::is_unsigned<T>::value, int> countl_zero(T x) noexcept {
  if (x == 0) {
    return std::numeric_limits<T>::digits;
  }

  return internal::count_zeros_from_left(x);
}

template <class T>
constexpr std::enable_if_t<std::is_unsigned<T>::value, int> countl_one(T x) noexcept {
  return countl_zero(static_cast<T>(~x));
}

template <class T>
constexpr std::enable_if_t<std::is_unsigned<T>::value, int> countr_one(T x) noexcept {
  return countr_zero(static_cast<T>(~x));
}

template <class T>
[[gnu::warn_unused_result]] constexpr std::enable_if_t<std::is_unsigned<T>::value, T> rotl(
    T x, int s) noexcept {
  return internal::rotl(x, s);
}

template <class T>
[[gnu::warn_unused_result]] constexpr std::enable_if_t<std::is_unsigned<T>::value, T> rotr(
    T x, int s) noexcept {
  return internal::rotr(x, s);
}

template <class T>
constexpr int popcount(T x) noexcept {
  return internal::popcount(x);
}

#endif

#if defined(__cpp_lib_int_pow2) && __cpp_lib_int_pow2 >= 202002L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::bit_ceil;
using std::bit_floor;
using std::bit_width;
using std::has_single_bit;

#else  // Provide polyfills for power of two bit functions.

template <typename T>
constexpr std::enable_if_t<std::is_unsigned<T>::value, bool> has_single_bit(T value) {
  return popcount(value) == static_cast<T>(1);
}

template <typename T>
constexpr std::enable_if_t<std::is_unsigned<T>::value, T> bit_width(T value) {
  return internal::bit_width(value);
}

template <typename T>
constexpr std::enable_if_t<std::is_unsigned<T>::value, T> bit_ceil(T value) {
  if (value <= 1) {
    return T(1);
  }
  return internal::bit_ceil<T>(value);
}

template <typename T>
constexpr std::enable_if_t<std::is_unsigned<T>::value, T> bit_floor(T value) {
  if (value == 0) {
    return 0;
  }
  return internal::bit_floor(value);
}

#endif  // __cpp_lib_int_pow2 >= 202002L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#if defined(__cpp_lib_endian) && __cpp_lib_endian >= 201907L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::endian;

#else  // Provide polyfill for endian enum.

enum class endian {
  little = 0x11771E,
  big = 0xB16,
  native = internal::native_endianess<little, big>(),
};

#endif

}  // namespace cpp20

#endif  // LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_BIT_H_
