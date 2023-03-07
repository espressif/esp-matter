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
#include <cstddef>
#include <iterator>

#include "pw_polyfill/standard_library/namespace.h"

#define __cpp_lib_string_view 201606L

_PW_POLYFILL_BEGIN_NAMESPACE_STD

template <typename T>
class basic_string_view {
 public:
  using traits_type = void;  // No traits object is used.
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using const_iterator = const T*;
  using iterator = const_iterator;
  using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
  using reverse_iterator = const_reverse_iterator;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  static constexpr size_type npos = size_type(-1);

  constexpr basic_string_view() noexcept : string_(nullptr), size_(0) {}
  constexpr basic_string_view(const basic_string_view&) noexcept = default;
  constexpr basic_string_view(const T* string, size_type count)
      : string_(string), size_(count) {}
  constexpr basic_string_view(const T* string)
      : string_(string), size_(CStringLength(string)) {}

  constexpr basic_string_view& operator=(const basic_string_view&) noexcept =
      default;

  constexpr const_iterator begin() const noexcept { return string_; }
  constexpr const_iterator cbegin() const noexcept { return begin(); }

  constexpr const_iterator end() const noexcept { return string_ + size_; }
  constexpr const_iterator cend() const noexcept { return end(); }

  // NOT IMPLEMENTED: Reverse iterators not supported.
  constexpr const_reverse_iterator rbegin() const noexcept;
  constexpr const_reverse_iterator crbegin() const noexcept;

  constexpr const_reverse_iterator rend() const noexcept;
  constexpr const_reverse_iterator crend() const noexcept;

  constexpr const_reference operator[](size_type pos) const {
    return data()[pos];
  }

  // NOT IMPLEMENTED: at() has no bounds checking.
  constexpr const_reference at(size_type pos) const { return data()[pos]; }

  constexpr const_reference front() const { return data()[0]; }

  constexpr const_reference back() const { return data()[size() - 1]; }

  constexpr const_pointer data() const noexcept { return string_; }

  constexpr size_type size() const noexcept { return size_; }
  constexpr size_type length() const noexcept { return size(); }

  constexpr size_type max_size() const noexcept { return ~size_t{0}; }

  [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0u; }

  constexpr void remove_prefix(size_type characters) {
    string_ += characters;
    size_ -= characters;
  }

  constexpr void remove_suffix(size_type characters) { size_ -= characters; }

  constexpr void swap(basic_string_view& other) noexcept {
    pointer temp_string = string_;
    string_ = other.string_;
    other.string_ = temp_string;

    size_type temp_size = size_;
    size_ = other.size_;
    other.size_ = temp_size;
  }

  // NOT IMPLEMENTED: copy does no bounds checking.
  constexpr size_type copy(T* dest, size_type count, size_type pos = 0) const {
    const size_type to_copy = min(count, size() - pos);
    for (size_type i = pos; i < pos + to_copy; ++i) {
      *dest++ = string_[i];
    }
    return to_copy;
  }

  constexpr basic_string_view substr(size_type pos = 0,
                                     size_type count = npos) const {
    return basic_string_view(string_ + pos, min(count, size() - pos));
  }

  // NOT IMPLEMENTED: These functions and their overloads are not defined.
  constexpr int compare(basic_string_view view) const noexcept;
  constexpr bool starts_with(basic_string_view view) const noexcept;
  constexpr bool ends_with(basic_string_view view) const noexcept;
  constexpr size_type find(basic_string_view view,
                           size_type pos = 0) const noexcept;
  constexpr size_type rfind(basic_string_view view,
                            size_type pos = npos) const noexcept;
  constexpr size_type find_first_of(basic_string_view view,
                                    size_type pos = 0) const noexcept;
  constexpr size_type find_last_of(basic_string_view view,
                                   size_type pos = npos) const noexcept;
  constexpr size_type find_first_not_of(basic_string_view view,
                                        size_type pos = 0) const noexcept;
  constexpr size_type find_last_not_of(basic_string_view view,
                                       size_type pos = npos) const noexcept;

 private:
  static constexpr size_type CStringLength(const T* string) {
    size_type length = 0;
    while (string[length] != T()) {
      length += 1;
    }
    return length;
  }

  const_pointer string_;
  size_type size_;
};

template <typename T>
constexpr bool operator==(basic_string_view<T> lhs, basic_string_view<T> rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  for (typename basic_string_view<T>::size_type i = 0; i < lhs.size(); ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

template <typename T>
constexpr bool operator!=(basic_string_view<T> lhs, basic_string_view<T> rhs) {
  return !(lhs == rhs);
}

// NOT IMPLEMENTED: Other comparison operators are not defined.

using string_view = basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

// NOT IMPLEMENTED: string_view literals cannot be implemented since they do not
//                  start with _.

_PW_POLYFILL_END_NAMESPACE_STD
