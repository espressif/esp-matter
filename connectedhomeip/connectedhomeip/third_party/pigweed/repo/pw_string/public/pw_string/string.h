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

#include <cstddef>
#include <initializer_list>
#include <iterator>

#include "pw_assert/assert.h"
#include "pw_polyfill/standard.h"
#include "pw_preprocessor/compiler.h"
#include "pw_string/internal/string_impl.h"

// Messages to use in static_assert statements.
#define _PW_STRING_CAPACITY_TOO_SMALL_FOR_ARRAY                               \
  "The pw::InlineString's capacity is too small to hold the assigned string " \
  "literal or character array. When assigning a literal or array to a "       \
  "pw::InlineString, the pw::InlineString's capacity must be large enough "   \
  "for the entire string, not counting the null terminator."

#define _PW_STRING_CAPACITY_TOO_SMALL_FOR_STRING                               \
  "When assigning one pw::InlineString with known capacity to another, the "   \
  "capacity of the destination pw::InlineString must be at least as large as " \
  "the source string."

namespace pw {

// pw::InlineBasicString<T, kCapacity> is a fixed-capacity version of
// std::basic_string. It implements mostly the same API as std::basic_string,
// but the capacity of the string is fixed at construction and cannot grow.
// Attempting to increase the size beyond the capacity triggers an assert.
//
// A pw::InlineString alias of pw::InlineBasicString<char>, equivalent to
// std::string, is defined below.
//
// pw::InlineBasicString has a template parameter for the capacity, but the
// capacity does not have to be known to use the string. The
// pw::InlineBasicString template inherits from a pw::InlineBasicString
// specialization with capacity of the reserved value pw::InlineString<>::npos.
// The actual capacity is stored in a single word alongside the size. This
// allows code to work with strings of any capacity through a pw::InlineString<>
// or pw::InlineBasicString<T> reference.
template <typename T, string_impl::size_type kCapacity = string_impl::kGeneric>
class InlineBasicString final
    : public InlineBasicString<T, string_impl::kGeneric> {
 public:
  using typename InlineBasicString<T, string_impl::kGeneric>::value_type;
  using typename InlineBasicString<T, string_impl::kGeneric>::size_type;
  using typename InlineBasicString<T, string_impl::kGeneric>::difference_type;
  using typename InlineBasicString<T, string_impl::kGeneric>::reference;
  using typename InlineBasicString<T, string_impl::kGeneric>::const_reference;
  using typename InlineBasicString<T, string_impl::kGeneric>::pointer;
  using typename InlineBasicString<T, string_impl::kGeneric>::const_pointer;
  using typename InlineBasicString<T, string_impl::kGeneric>::iterator;
  using typename InlineBasicString<T, string_impl::kGeneric>::const_iterator;
  using typename InlineBasicString<T, string_impl::kGeneric>::reverse_iterator;
  using
      typename InlineBasicString<T,
                                 string_impl::kGeneric>::const_reverse_iterator;

  using InlineBasicString<T, string_impl::kGeneric>::npos;

  // Constructors

  constexpr InlineBasicString() noexcept
      : InlineBasicString<T, string_impl::kGeneric>(kCapacity), buffer_() {}

  constexpr InlineBasicString(size_type count, T ch) : InlineBasicString() {
    Fill(data(), ch, count);
  }

  template <size_type kOtherCapacity>
  constexpr InlineBasicString(const InlineBasicString<T, kOtherCapacity>& other,
                              size_type index,
                              size_type count = npos)
      : InlineBasicString() {
    CopySubstr(data(), other.data(), other.size(), index, count);
  }

  constexpr InlineBasicString(const T* string, size_type count)
      : InlineBasicString() {
    Copy(data(), string, count);
  }

  template <typename U,
            typename = string_impl::EnableIfNonArrayCharPointer<T, U>>
  constexpr InlineBasicString(U c_string)
      : InlineBasicString(
            c_string, string_impl::BoundedStringLength(c_string, kCapacity)) {}

  template <size_t kCharArraySize>
  constexpr InlineBasicString(const T (&array)[kCharArraySize])
      : InlineBasicString() {
    static_assert(
        string_impl::NullTerminatedArrayFitsInString(kCharArraySize, kCapacity),
        _PW_STRING_CAPACITY_TOO_SMALL_FOR_ARRAY);
    Copy(data(), array, string_impl::ArrayStringLength(array, max_size()));
  }

  template <typename InputIterator,
            typename = string_impl::EnableIfInputIterator<InputIterator>>
  constexpr InlineBasicString(InputIterator start, InputIterator finish)
      : InlineBasicString() {
    IteratorCopy(start, finish, data(), data() + max_size());
  }

  // Use the default copy for InlineBasicString with the same capacity.
  constexpr InlineBasicString(const InlineBasicString&) = default;

  // When copying from an InlineBasicString with a different capacity, check
  // that the destination capacity is at least as large as the source capacity.
  template <size_type kOtherCapacity>
  constexpr InlineBasicString(const InlineBasicString<T, kOtherCapacity>& other)
      : InlineBasicString(other.data(), other.size()) {
    static_assert(
        kOtherCapacity == string_impl::kGeneric || kOtherCapacity <= kCapacity,
        _PW_STRING_CAPACITY_TOO_SMALL_FOR_STRING);
  }

  constexpr InlineBasicString(std::initializer_list<T> list)
      : InlineBasicString(list.begin(), list.size()) {}

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // std::string_view is a C++17 feature
  // Unlike std::string, pw::InlineString<> supports implicit conversions from
  // std::string_view. However, explicit conversions are still required from
  // types that convert to std::string_view, as with std::string.
  //
  // pw::InlineString<> allows implicit conversions from std::string_view
  // because it can be cumbersome to specify the capacity parameter. In
  // particular, this can make using aggregate initialization more difficult.
  //
  // This explicit constructor is enabled for an argument that converts to
  // std::string_view, but is not a std::string_view.
  template <
      typename StringViewLike,
      string_impl::EnableIfStringViewLikeButNotStringView<T, StringViewLike>* =
          nullptr>
  explicit constexpr InlineBasicString(const StringViewLike& string)
      : InlineBasicString(std::basic_string_view<T>(string)) {}

  // This converting constructor is enabled for std::string_view, but not types
  // that convert to it.
  template <typename StringView,
            std::enable_if_t<
                std::is_same<StringView, std::basic_string_view<T>>::value>* =
                nullptr>
  constexpr InlineBasicString(const StringView& view)
      : InlineBasicString(view.data(), view.size()) {}

  template <typename StringView,
            typename = string_impl::EnableIfStringViewLike<T, StringView>>
  constexpr InlineBasicString(const StringView& string,
                              size_type index,
                              size_type count)
      : InlineBasicString() {
    const std::basic_string_view<T> view = string;
    CopySubstr(data(), view.data(), view.size(), index, count);
  }
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

  InlineBasicString(std::nullptr_t) = delete;  // Cannot construct from nullptr

  // Assignment operators

  constexpr InlineBasicString& operator=(const InlineBasicString& other) =
      default;

  // Checks capacity rather than current size.
  template <size_type kOtherCapacity>
  constexpr InlineBasicString& operator=(
      const InlineBasicString<T, kOtherCapacity>& other) {
    return assign<kOtherCapacity>(other);  // NOLINT
  }

  template <size_t kCharArraySize>
  constexpr InlineBasicString& operator=(const T (&array)[kCharArraySize]) {
    return assign<kCharArraySize>(array);  // NOLINT
  }

  // Use SFINAE to avoid ambiguity with the array overload.
  template <typename U,
            typename = string_impl::EnableIfNonArrayCharPointer<T, U>>
  constexpr InlineBasicString& operator=(U c_string) {
    return assign(c_string);  // NOLINT
  }

  constexpr InlineBasicString& operator=(T ch) {
    static_assert(kCapacity != 0,
                  "Cannot assign a character to pw::InlineString<0>");
    return assign(1, ch);  // NOLINT
  }

  constexpr InlineBasicString& operator=(std::initializer_list<T> list) {
    return assign(list);  // NOLINT
  }

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // std::string_view is a C++17 feature
  template <typename StringView,
            typename = string_impl::EnableIfStringViewLike<T, StringView>>
  constexpr InlineBasicString& operator=(const StringView& string) {
    return assign(string);  // NOLINT
  }
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

  constexpr InlineBasicString& operator=(std::nullptr_t) = delete;

  template <size_type kOtherCapacity>
  constexpr InlineBasicString& operator+=(
      const InlineBasicString<T, kOtherCapacity>& string) {
    return append(string);
  }

  constexpr InlineBasicString& operator+=(T character) {
    push_back(character);
    return *this;
  }

  template <size_t kCharArraySize>
  constexpr InlineBasicString& operator+=(const T (&array)[kCharArraySize]) {
    return append(array);
  }

  template <typename U,
            typename = string_impl::EnableIfNonArrayCharPointer<T, U>>
  constexpr InlineBasicString& operator+=(U c_string) {
    return append(c_string);
  }

  constexpr InlineBasicString& operator+=(std::initializer_list<T> list) {
    return append(list.begin(), list.size());
  }

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // std::string_view is a C++17 feature
  template <typename StringView,
            typename = string_impl::EnableIfStringViewLike<T, StringView>>
  constexpr InlineBasicString& operator+=(const StringView& string) {
    return append(string);
  }
#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

  // The data() and size() functions are defined differently for the generic and
  // known-size specializations. This is to support using pw::InlineBasicString
  // in constexpr statements. This data() implementation simply returns the
  // underlying buffer. The generic-capacity data() function casts *this to
  // InlineBasicString<T, 0>, so is only constexpr when the capacity is actually
  // 0.
  constexpr pointer data() { return buffer_; }
  constexpr const_pointer data() const { return buffer_; }

  // Use the size() function from the base, but define max_size() to return the
  // kCapacity template parameter instead of reading the stored capacity value.
  using InlineBasicString<T, string_impl::kGeneric>::size;
  constexpr size_type max_size() const noexcept { return kCapacity; }

  // Most string functions are defined in separate file so they can be shared
  // between the known capacity and generic capacity versions of
  // InlineBasicString.
#include "pw_string/internal/string_common_functions.inc"

 private:
  using InlineBasicString<T, string_impl::kGeneric>::PushBack;
  using InlineBasicString<T, string_impl::kGeneric>::PopBack;
  using InlineBasicString<T, string_impl::kGeneric>::Copy;
  using InlineBasicString<T, string_impl::kGeneric>::CopySubstr;
  using InlineBasicString<T, string_impl::kGeneric>::Fill;
  using InlineBasicString<T, string_impl::kGeneric>::IteratorCopy;
  using InlineBasicString<T, string_impl::kGeneric>::CopyExtend;
  using InlineBasicString<T, string_impl::kGeneric>::CopyExtendSubstr;
  using InlineBasicString<T, string_impl::kGeneric>::FillExtend;
  using InlineBasicString<T, string_impl::kGeneric>::IteratorExtend;
  using InlineBasicString<T, string_impl::kGeneric>::Resize;
  using InlineBasicString<T, string_impl::kGeneric>::SetSizeAndTerminate;

  // Store kCapacity + 1 bytes to reserve space for a null terminator.
  // InlineBasicString<T, 0> only stores a null terminator.
  T buffer_[kCapacity + 1];
};

// Generic-capacity version of pw::InlineBasicString. Generic-capacity strings
// cannot be constructed; they can only be used as references to fixed-capacity
// pw::InlineBasicString objects.
template <typename T>
class InlineBasicString<T, string_impl::kGeneric> {
 public:
  using value_type = T;
  using size_type = string_impl::size_type;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = value_type*;
  using const_iterator = const value_type*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  static constexpr size_type npos = string_impl::kGeneric;

  InlineBasicString() = delete;  // Must specify capacity to construct a string.

  // For the generic-capacity pw::InlineBasicString, cast this object to a
  // fixed-capacity class so the address of the data can be found. Even though
  // the capacity isn't known at compile time, the location of the data never
  // changes.
  constexpr pointer data() noexcept {
    return static_cast<InlineBasicString<T, 0>*>(this)->data();
  }
  constexpr const_pointer data() const noexcept {
    return static_cast<const InlineBasicString<T, 0>*>(this)->data();
  }

  constexpr size_type size() const noexcept { return length_; }
  constexpr size_type max_size() const noexcept { return capacity_; }

  // Most string functions are defined in separate file so they can be shared
  // between the known capacity and generic capacity versions of
  // InlineBasicString.
#include "pw_string/internal/string_common_functions.inc"

 protected:
  explicit constexpr InlineBasicString(size_type capacity)
      : capacity_(capacity), length_(0) {}

  // The generic-capacity InlineBasicString<T> is not copyable or movable, but
  // BasicStrings can copied or assigned through a fixed capacity derived class.
  InlineBasicString(const InlineBasicString&) = default;

  InlineBasicString& operator=(const InlineBasicString&) = default;

  constexpr void PushBack(T* data, T ch);

  constexpr void PopBack(T* data) {
    PW_ASSERT(!empty());
    SetSizeAndTerminate(data, size() - 1);
  }

  constexpr InlineBasicString& Copy(T* data,
                                    const T* source,
                                    size_type new_size);

  constexpr InlineBasicString& CopySubstr(T* data,
                                          const T* source,
                                          size_type source_size,
                                          size_type index,
                                          size_type count);

  constexpr InlineBasicString& Fill(T* data, T fill_char, size_type new_size);

  template <typename InputIterator>
  constexpr InlineBasicString& IteratorCopy(InputIterator start,
                                            InputIterator finish,
                                            T* data_start,
                                            const T* data_finish) {
    set_size(string_impl::IteratorCopyAndTerminate(
        start, finish, data_start, data_finish));
    return *this;
  }

  constexpr InlineBasicString& CopyExtend(T* data,
                                          const T* source,
                                          size_type count);

  constexpr InlineBasicString& CopyExtendSubstr(T* data,
                                                const T* source,
                                                size_type source_size,
                                                size_type index,
                                                size_type count);

  constexpr InlineBasicString& FillExtend(T* data,
                                          T fill_char,
                                          size_type count);

  template <typename InputIterator>
  constexpr InlineBasicString& IteratorExtend(InputIterator start,
                                              InputIterator finish,
                                              T* data_start,
                                              const T* data_finish) {
    length_ += string_impl::IteratorCopyAndTerminate(
        start, finish, data_start, data_finish);
    return *this;
  }

  constexpr void Resize(T* data, size_type new_size, T ch);

  constexpr void set_size(size_type length) { length_ = length; }
  constexpr void SetSizeAndTerminate(T* data, size_type length) {
    string_impl::char_traits<T>::assign(data[length], T());
    set_size(length);
  }

 private:
  // Allow StringBuilder to directly set length_ when doing string operations.
  friend class StringBuilder;

  // Provide this constant for static_assert checks. If the capacity is unknown,
  // use the maximum value so that compile-time capacity checks pass. If
  // overflow occurs, the operation triggers a PW_ASSERT at runtime.
  static constexpr size_type kCapacity = string_impl::kGeneric;

  size_type capacity_;
  size_type length_;
};

// Class template argument deduction guides

#ifdef __cpp_deduction_guides

// In C++17, the capacity of the string may be deduced from a string literal or
// array. For example, the following deduces a character type of char and a
// capacity of 4 (which does not include the null terminator).
//
//   InlineBasicString my_string = "1234";
//
// In C++20, the template parameters for the pw::InlineString alias may be
// deduced similarly:
//
//   InlineString my_string = "abc";  // deduces capacity of 3.
//
template <typename T, size_t kCharArraySize>
InlineBasicString(const T (&)[kCharArraySize])
    -> InlineBasicString<T, kCharArraySize - 1>;

#endif  // __cpp_deduction_guides

// Operators

// TODO(b/239996007): Implement operator+

template <typename T,
          string_impl::size_type kLhsCapacity,
          string_impl::size_type kRhsCapacity>
constexpr bool operator==(
    const InlineBasicString<T, kLhsCapacity>& lhs,
    const InlineBasicString<T, kRhsCapacity>& rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

template <typename T,
          string_impl::size_type kLhsCapacity,
          string_impl::size_type kRhsCapacity>
constexpr bool operator!=(
    const InlineBasicString<T, kLhsCapacity>& lhs,
    const InlineBasicString<T, kRhsCapacity>& rhs) noexcept {
  return lhs.compare(rhs) != 0;
}

template <typename T,
          string_impl::size_type kLhsCapacity,
          string_impl::size_type kRhsCapacity>
constexpr bool operator<(
    const InlineBasicString<T, kLhsCapacity>& lhs,
    const InlineBasicString<T, kRhsCapacity>& rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template <typename T,
          string_impl::size_type kLhsCapacity,
          string_impl::size_type kRhsCapacity>
constexpr bool operator<=(
    const InlineBasicString<T, kLhsCapacity>& lhs,
    const InlineBasicString<T, kRhsCapacity>& rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template <typename T,
          string_impl::size_type kLhsCapacity,
          string_impl::size_type kRhsCapacity>
constexpr bool operator>(
    const InlineBasicString<T, kLhsCapacity>& lhs,
    const InlineBasicString<T, kRhsCapacity>& rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template <typename T,
          string_impl::size_type kLhsCapacity,
          string_impl::size_type kRhsCapacity>
constexpr bool operator>=(
    const InlineBasicString<T, kLhsCapacity>& lhs,
    const InlineBasicString<T, kRhsCapacity>& rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template <typename T, string_impl::size_type kLhsCapacity>
constexpr bool operator==(const InlineBasicString<T, kLhsCapacity>& lhs,
                          const T* rhs) {
  return lhs.compare(rhs) == 0;
}

template <typename T, string_impl::size_type kRhsCapacity>
constexpr bool operator==(const T* lhs,
                          const InlineBasicString<T, kRhsCapacity>& rhs) {
  return rhs.compare(lhs) == 0;
}

template <typename T, string_impl::size_type kLhsCapacity>
constexpr bool operator!=(const InlineBasicString<T, kLhsCapacity>& lhs,
                          const T* rhs) {
  return lhs.compare(rhs) != 0;
}

template <typename T, string_impl::size_type kRhsCapacity>
constexpr bool operator!=(const T* lhs,
                          const InlineBasicString<T, kRhsCapacity>& rhs) {
  return rhs.compare(lhs) != 0;
}

template <typename T, string_impl::size_type kLhsCapacity>
constexpr bool operator<(const InlineBasicString<T, kLhsCapacity>& lhs,
                         const T* rhs) {
  return lhs.compare(rhs) < 0;
}

template <typename T, string_impl::size_type kRhsCapacity>
constexpr bool operator<(const T* lhs,
                         const InlineBasicString<T, kRhsCapacity>& rhs) {
  return rhs.compare(lhs) >= 0;
}

template <typename T, string_impl::size_type kLhsCapacity>
constexpr bool operator<=(const InlineBasicString<T, kLhsCapacity>& lhs,
                          const T* rhs) {
  return lhs.compare(rhs) <= 0;
}

template <typename T, string_impl::size_type kRhsCapacity>
constexpr bool operator<=(const T* lhs,
                          const InlineBasicString<T, kRhsCapacity>& rhs) {
  return rhs.compare(lhs) >= 0;
}

template <typename T, string_impl::size_type kLhsCapacity>
constexpr bool operator>(const InlineBasicString<T, kLhsCapacity>& lhs,
                         const T* rhs) {
  return lhs.compare(rhs) > 0;
}

template <typename T, string_impl::size_type kRhsCapacity>
constexpr bool operator>(const T* lhs,
                         const InlineBasicString<T, kRhsCapacity>& rhs) {
  return rhs.compare(lhs) <= 0;
}

template <typename T, string_impl::size_type kLhsCapacity>
constexpr bool operator>=(const InlineBasicString<T, kLhsCapacity>& lhs,
                          const T* rhs) {
  return lhs.compare(rhs) >= 0;
}

template <typename T, string_impl::size_type kRhsCapacity>
constexpr bool operator>=(const T* lhs,
                          const InlineBasicString<T, kRhsCapacity>& rhs) {
  return rhs.compare(lhs) <= 0;
}

// TODO(b/239996007): Implement other comparison operator overloads.

// Aliases
template <string_impl::size_type kCapacity = string_impl::kGeneric>
using InlineString = InlineBasicString<char, kCapacity>;

// Function implementations

template <typename T>
constexpr void InlineBasicString<T, string_impl::kGeneric>::PushBack(T* data,
                                                                     T ch) {
  PW_ASSERT(size() < max_size());
  string_impl::char_traits<T>::assign(data[size()], ch);
  SetSizeAndTerminate(data, size() + 1);
}

template <typename T>
constexpr InlineBasicString<T, string_impl::kGeneric>&
InlineBasicString<T, string_impl::kGeneric>::Copy(T* data,
                                                  const T* source,
                                                  size_type new_size) {
  PW_ASSERT(new_size <= max_size());
  string_impl::char_traits<T>::copy(data, source, new_size);
  SetSizeAndTerminate(data, new_size);
  return *this;
}

template <typename T>
constexpr InlineBasicString<T, string_impl::kGeneric>&
InlineBasicString<T, string_impl::kGeneric>::CopySubstr(T* data,
                                                        const T* source,
                                                        size_type source_size,
                                                        size_type index,
                                                        size_type count) {
  PW_ASSERT(index <= source_size);
  return Copy(data,
              source + index,
              std::min(count, static_cast<size_type>(source_size - index)));
}

template <typename T>
constexpr InlineBasicString<T, string_impl::kGeneric>&
InlineBasicString<T, string_impl::kGeneric>::Fill(T* data,
                                                  T fill_char,
                                                  size_type new_size) {
  PW_ASSERT(new_size <= max_size());
  string_impl::char_traits<T>::assign(data, new_size, fill_char);
  SetSizeAndTerminate(data, new_size);
  return *this;
}

template <typename T>
constexpr InlineBasicString<T, string_impl::kGeneric>&
InlineBasicString<T, string_impl::kGeneric>::CopyExtend(T* data,
                                                        const T* source,
                                                        size_type count) {
  PW_ASSERT(count <= max_size() - size());
  string_impl::char_traits<T>::copy(data + size(), source, count);
  SetSizeAndTerminate(data, size() + count);
  return *this;
}

template <typename T>
constexpr InlineBasicString<T, string_impl::kGeneric>&
InlineBasicString<T, string_impl::kGeneric>::CopyExtendSubstr(
    T* data,
    const T* source,
    size_type source_size,
    size_type index,
    size_type count) {
  PW_ASSERT(index <= source_size);
  return CopyExtend(
      data,
      source + index,
      std::min(count, static_cast<size_type>(source_size - index)));
  return *this;
}

template <typename T>
constexpr InlineBasicString<T, string_impl::kGeneric>&
InlineBasicString<T, string_impl::kGeneric>::FillExtend(T* data,
                                                        T fill_char,
                                                        size_type count) {
  PW_ASSERT(count <= max_size() - size());
  string_impl::char_traits<T>::assign(data + size(), count, fill_char);
  SetSizeAndTerminate(data, size() + count);
  return *this;
}

template <typename T>
constexpr void InlineBasicString<T, string_impl::kGeneric>::Resize(
    T* data, size_type new_size, T ch) {
  PW_ASSERT(new_size <= max_size());

  if (new_size > size()) {
    string_impl::char_traits<T>::assign(data + size(), new_size - size(), ch);
  }

  SetSizeAndTerminate(data, new_size);
}

}  // namespace pw

#undef _PW_STRING_CAPACITY_TOO_SMALL_FOR_ARRAY
#undef _PW_STRING_CAPACITY_TOO_SMALL_FOR_STRING
