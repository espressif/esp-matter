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
#pragma once

#include <string>  // for std::char_traits
#include <type_traits>

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // std::string_view is a C++17 feature
#include <string_view>
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

#include "pw_assert/assert.h"
#include "pw_polyfill/language_feature_macros.h"

namespace pw {
namespace string_impl {

// pw::InlineString<>::size_type is unsigned short so the capacity and current
// size fit into a single word.
using size_type = unsigned short;

template <typename CharType, typename T>
using EnableIfNonArrayCharPointer = std::enable_if_t<
    std::is_pointer<T>::value && !std::is_array<T>::value &&
    std::is_same<CharType, std::remove_cv_t<std::remove_pointer_t<T>>>::value>;

template <typename T>
using EnableIfInputIterator = std::enable_if_t<
    std::is_convertible<typename std::iterator_traits<T>::iterator_category,
                        std::input_iterator_tag>::value>;

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // std::string_view is a C++17 feature
template <typename CharType, typename T>
using EnableIfStringViewLike = std::enable_if_t<
    std::is_convertible<const T&, std::basic_string_view<CharType>>() &&
    !std::is_convertible<const T&, const CharType*>()>;

template <typename CharType, typename T>
using EnableIfStringViewLikeButNotStringView = std::enable_if_t<
    !std::is_same<T, std::basic_string_view<CharType>>() &&
    std::is_convertible<const T&, std::basic_string_view<CharType>>() &&
    !std::is_convertible<const T&, const CharType*>()>;
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

// Reserved capacity that is used to represent a generic-length
// pw::InlineString.
PW_INLINE_VARIABLE constexpr size_type kGeneric = size_type(-1);

#if defined(__cpp_lib_constexpr_string) && __cpp_lib_constexpr_string >= 201907L

// Like std::string and std::string_view, pw::InlineString uses std::char_traits
// for low-level operations.
using std::char_traits;

#else

// If constexpr std::char_traits is not available, provide a custom traits class
// with constexpr versions of the necessary functions. Defer to std::char_traits
// when possible.
template <typename T>
class char_traits : private std::char_traits<T> {
 public:
  static constexpr void assign(T& dest, const T& source) noexcept {
    dest = source;
  }

  static constexpr T* assign(T* dest, size_t count, T value) {
    for (size_t i = 0; i < count; ++i) {
      dest[i] = value;
    }
    return dest;
  }

  using std::char_traits<T>::eq;

  static constexpr T* copy(T* dest, const T* source, size_t count) {
    for (size_type i = 0; i < count; ++i) {
      char_traits<T>::assign(dest[i], source[i]);
    }
    return dest;
  }

  using std::char_traits<T>::compare;
};

#endif  // __cpp_lib_constexpr_string

// Used in static_asserts to check that a C array fits in an InlineString.
constexpr bool NullTerminatedArrayFitsInString(
    size_t null_terminated_array_size, size_type capacity) {
  return null_terminated_array_size > 0u &&
         null_terminated_array_size - 1 <= capacity &&
         null_terminated_array_size - 1 < kGeneric;
}

// Constexpr utility functions for pw::InlineString. These are NOT intended for
// general use. These mostly map directly to general purpose standard library
// utilities that are not constexpr until C++20.

// Calculates the length of a C string up to the capacity. Returns capacity + 1
// if the string is longer than the capacity. This replaces
// std::char_traits<T>::length, which is unbounded. The string must contain at
// least one character.
template <typename T>
constexpr size_type BoundedStringLength(const T* string, size_type capacity) {
  size_type length = 0;
  for (; length <= capacity; ++length) {
    if (char_traits<T>::eq(string[length], T())) {
      break;
    }
  }
  return length;  // length is capacity + 1 if T() was not found.
}

// As with std::string, InlineString treats literals and character arrays as
// null-terminated strings. ArrayStringLength checks that the array size fits
// within size_type and asserts if no null terminator was found in the array.
template <typename T>
constexpr size_type ArrayStringLength(const T* array,
                                      size_type max_string_length,
                                      size_type capacity) {
  const size_type max_length = std::min(max_string_length, capacity);
  const size_type length = BoundedStringLength(array, max_length);
  PW_ASSERT(length <= max_string_length);  // The array is not null terminated
  return length;
}

template <typename T, size_t kCharArraySize>
constexpr size_type ArrayStringLength(const T (&array)[kCharArraySize],
                                      size_type capacity) {
  static_assert(kCharArraySize > 0u, "C arrays cannot have a length of 0");
  static_assert(kCharArraySize - 1 < kGeneric,
                "The size of this literal or character array is too large "
                "for pw::InlineString<>::size_type");
  return ArrayStringLength(
      array, static_cast<size_type>(kCharArraySize - 1), capacity);
}

// Constexpr version of std::copy that returns the number of copied characters.
template <typename InputIterator, typename T>
constexpr size_type IteratorCopyAndTerminate(InputIterator begin,
                                             InputIterator end,
                                             T* const string_begin,
                                             const T* const string_end) {
  T* current_position = string_begin;
  for (InputIterator it = begin; it != end; ++it) {
    PW_ASSERT(current_position != string_end);
    char_traits<T>::assign(*current_position++, *it);
  }
  char_traits<T>::assign(*current_position, T());  // Null terminate
  return static_cast<size_type>(current_position - string_begin);
}

// Constexpr lexicographical comparison.
template <typename T>
constexpr int Compare(const T* lhs,
                      size_type lhs_size,
                      const T* rhs,
                      size_type rhs_size) noexcept {
  int result = char_traits<T>::compare(lhs, rhs, std::min(lhs_size, rhs_size));
  if (result != 0 || lhs_size == rhs_size) {
    return result;
  }
  return lhs_size < rhs_size ? -1 : 1;
}

}  // namespace string_impl
}  // namespace pw
