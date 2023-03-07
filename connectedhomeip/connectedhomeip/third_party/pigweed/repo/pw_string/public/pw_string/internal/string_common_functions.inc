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

// This file contains the definitions of most pw::InlineBasicString functions.
// The file is included inline in the fixed-capacity pw::InlineBasicString<T,
// kCapacity> and generic-capacity pw::InlineBasicString<T> specialization
// classes.
//
// These functions cannot simply be defined in the pw::InlineBasicString<T> base
// class because:
//
//   1. Many functions return a *this reference. The functions should to return
//      a reference to the exact type they are called on rather than the
//      generic-capacity base class.
//   2. Operations on the generic base class cannot be constexpr unless the
//      class is an InlineBasicString<T, 0>. The functions must be defined in
//      the fixed-capacity dervied classes to support constexpr operations.
//
// These functions have no logic and simply defer to shared, length-agnostic
// implementations so they do not increase code size.

PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC_GCC(ignored, "-Wtype-limits");

// Assignment functions

constexpr InlineBasicString& assign(size_type count, T ch) {
  return static_cast<InlineBasicString&>(Fill(data(), ch, count));
}

// Checks capacity rather than current size.
template <size_type kOtherCapacity>
constexpr InlineBasicString& assign(
    const InlineBasicString<T, kOtherCapacity>& other) {
  static_assert(
      kOtherCapacity == string_impl::kGeneric || kOtherCapacity <= kCapacity,
      _PW_STRING_CAPACITY_TOO_SMALL_FOR_STRING);
  return static_cast<InlineBasicString&>(
      Copy(data(), other.data(), other.size()));
}

constexpr InlineBasicString& assign(const InlineBasicString& other,
                                    size_type index,
                                    size_type count = npos) {
  return static_cast<InlineBasicString&>(
      CopySubstr(data(), other.data(), other.size(), index, count));
}

constexpr InlineBasicString& assign(const T* string, size_type count) {
  return static_cast<InlineBasicString&>(Copy(data(), string, count));
}

template <typename U, typename = string_impl::EnableIfNonArrayCharPointer<T, U>>
constexpr InlineBasicString& assign(U c_string) {
  return assign(c_string,
                string_impl::BoundedStringLength(c_string, max_size()));
}

// Assignment from character array or string literal. For known-size strings,
// the capacity is checked against the string/array size at compile time.
template <size_t kCharArraySize>
constexpr InlineBasicString& assign(const T (&array)[kCharArraySize]) {
  static_assert(
      string_impl::NullTerminatedArrayFitsInString(kCharArraySize, kCapacity),
      _PW_STRING_CAPACITY_TOO_SMALL_FOR_ARRAY);
  return static_cast<InlineBasicString&>(
      Copy(data(), array, string_impl::ArrayStringLength(array, max_size())));
}

template <typename InputIterator,
          typename = string_impl::EnableIfInputIterator<InputIterator>>
constexpr InlineBasicString& assign(InputIterator start, InputIterator finish) {
  return static_cast<InlineBasicString&>(
      IteratorCopy(start, finish, data(), data() + max_size()));
}

constexpr InlineBasicString& assign(std::initializer_list<T> list) {
  return assign(list.begin(), list.size());
}

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // std::string_view is a C++17 feature

template <typename StringView,
          typename = string_impl::EnableIfStringViewLike<T, StringView>>
constexpr InlineBasicString& assign(const StringView& string) {
  const std::basic_string_view<T> view = string;
  PW_ASSERT(view.size() < npos);
  return assign(view.data(), view.size());
}

template <typename StringView,
          typename = string_impl::EnableIfStringViewLike<T, StringView>>
constexpr InlineBasicString& assign(const StringView& string,
                                    size_type index,
                                    size_type count = npos) {
  const std::basic_string_view<T> view = string;
  PW_ASSERT(view.size() < npos);
  return static_cast<InlineBasicString&>(CopySubstr(
      data(), view.data(), static_cast<size_type>(view.size()), index, count));
}

#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

constexpr InlineBasicString& assign(std::nullptr_t) = delete;

// Element access

constexpr reference at(size_type index) {
  PW_ASSERT(index < length());
  return data()[index];
}
constexpr const_reference at(size_type index) const {
  PW_ASSERT(index < length());
  return data()[index];
}

constexpr reference operator[](size_type index) { return data()[index]; }
constexpr const_reference operator[](size_type index) const {
  return data()[index];
}

constexpr reference front() { return data()[0]; }
constexpr const_reference front() const { return data()[0]; }

constexpr reference back() { return data()[size() - 1]; }
constexpr const_reference back() const { return data()[size() - 1]; }

constexpr const_pointer c_str() const noexcept { return data(); }

constexpr operator std::basic_string_view<T>() const noexcept {
  return std::basic_string_view<T>(data(), size());
}

// Iterators

constexpr iterator begin() noexcept { return &data()[0]; }
constexpr const_iterator begin() const noexcept { return &data()[0]; }
constexpr const_iterator cbegin() const noexcept { return &data()[0]; }

constexpr iterator end() noexcept { return &data()[size()]; }
constexpr const_iterator end() const noexcept { return &data()[size()]; }
constexpr const_iterator cend() const noexcept { return &data()[size()]; }

constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
constexpr const_reverse_iterator rbegin() const noexcept {
  return const_reverse_iterator(end());
}
constexpr const_reverse_iterator crbegin() const noexcept {
  return const_reverse_iterator(cend());
}

constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
constexpr const_reverse_iterator rend() const noexcept {
  return const_reverse_iterator(begin());
}
constexpr const_reverse_iterator crend() const noexcept {
  return const_reverse_iterator(cbegin());
}

// Capacity

[[nodiscard]] constexpr bool empty() const noexcept { return size() == 0u; }

// The number of characters in the string.
constexpr size_type length() const noexcept { return size(); }

constexpr size_type capacity() const noexcept { return max_size(); }

// Operations

constexpr void clear() { SetSizeAndTerminate(data(), 0); }

// TODO(b/239996007): Implement insert and erase.

constexpr void push_back(value_type ch) {
  static_assert(kCapacity != 0,
                "Cannot add a character to pw::InlineString<0>");
  PushBack(data(), ch);
}

constexpr void pop_back() {
  static_assert(kCapacity != 0,
                "Cannot remove a character from pw::InlineString<0>");
  PopBack(data());
}

constexpr InlineBasicString& append(size_type count, T ch) {
  return static_cast<InlineBasicString&>(FillExtend(data(), ch, count));
}

template <size_type kOtherCapacity>
constexpr InlineBasicString& append(
    const InlineBasicString<T, kOtherCapacity>& string) {
  static_assert(
      kOtherCapacity == string_impl::kGeneric || kOtherCapacity <= kCapacity,
      _PW_STRING_CAPACITY_TOO_SMALL_FOR_STRING);
  return append(string.data(), string.size());
}

template <size_type kOtherCapacity>
constexpr InlineBasicString& append(
    const InlineBasicString<T, kOtherCapacity>& other,
    size_type index,
    size_type count = npos) {
  return static_cast<InlineBasicString&>(
      CopyExtendSubstr(data(), other.data(), other.size(), index, count));
}

constexpr InlineBasicString& append(const T* string, size_type count) {
  return static_cast<InlineBasicString&>(CopyExtend(data(), string, count));
}

template <size_t kCharArraySize>
constexpr InlineBasicString& append(const T (&array)[kCharArraySize]) {
  static_assert(
      string_impl::NullTerminatedArrayFitsInString(kCharArraySize, kCapacity),
      _PW_STRING_CAPACITY_TOO_SMALL_FOR_ARRAY);
  return append(array, string_impl::ArrayStringLength(array, max_size()));
}

template <typename U, typename = string_impl::EnableIfNonArrayCharPointer<T, U>>
constexpr InlineBasicString& append(U c_string) {
  return append(c_string,
                string_impl::BoundedStringLength(c_string, max_size()));
}

template <typename InputIterator,
          typename = string_impl::EnableIfInputIterator<InputIterator>>
constexpr InlineBasicString& append(InputIterator first, InputIterator last) {
  return static_cast<InlineBasicString&>(
      IteratorExtend(first, last, data() + size(), data() + max_size()));
}

constexpr InlineBasicString& append(std::initializer_list<T> list) {
  return append(list.begin(), list.size());
}

#if PW_CXX_STANDARD_IS_SUPPORTED(17)  // std::string_view is a C++17 feature

template <typename StringView,
          typename = string_impl::EnableIfStringViewLike<T, StringView>>
constexpr InlineBasicString& append(const StringView& string) {
  const std::basic_string_view<T> view = string;
  PW_ASSERT(view.size() < npos);
  return append(view.data(), view.size());
}

template <typename StringView,
          typename = string_impl::EnableIfStringViewLike<T, StringView>>
constexpr InlineBasicString& append(const StringView& string,
                                    size_type index,
                                    size_type count = npos) {
  const std::basic_string_view<T> view = string;
  PW_ASSERT(view.size() < npos);
  return static_cast<InlineBasicString&>(CopyExtendSubstr(
      data(), view.data(), static_cast<size_type>(view.size()), index, count));
}

#endif  // PW_CXX_STANDARD_IS_SUPPORTED(17)

template <size_type kOtherCapacity>
constexpr int compare(
    const InlineBasicString<T, kOtherCapacity>& other) const noexcept {
  return string_impl::Compare(data(), size(), other.data(), other.size());
}

constexpr int compare(const T* other) const {
  return string_impl::Compare(
      data(),
      size(),
      other,
      string_impl::BoundedStringLength(other, max_size()));
}

// TODO(b/239996007): Implement other compare overloads.

// TODO(b/239996007): Implement other std::string functions:
//
//   - starts_with
//   - ends_with
//   - replace
//   - substr
//   - copy

constexpr void resize(size_type new_size) { resize(new_size, T()); }

constexpr void resize(size_type new_size, T ch) {
  return Resize(data(), new_size, ch);
}

// resize_and_overwrite() only takes the callable object since the underlying
// buffer has a fixed size.
template <typename Operation>
constexpr void resize_and_overwrite(Operation operation) {
  const auto new_size = std::move(operation)(data(), max_size());
  PW_ASSERT(static_cast<size_t>(new_size) <= max_size());
  SetSizeAndTerminate(data(), new_size);
}

// TODO(b/239996007): Implement swap

// Search

// TODO(b/239996007): Implement std::string search functions:
//
// - find
// - rfind
// - find_first_of
// - find_first_not_of
// - find_last_of
// - find_last_not_of

PW_MODIFY_DIAGNOSTICS_POP();
