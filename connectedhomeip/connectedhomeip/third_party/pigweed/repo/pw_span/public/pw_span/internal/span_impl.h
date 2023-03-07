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

// This span implementation is a stand-in for C++20's std::span. Do NOT include
// this header directly; instead, include it as "pw_span/span.h".
//
// A span is a non-owning array view class. It refers to an external array by
// storing a pointer and length. Unlike std::array, the size does not have to be
// a template parameter, so this class can be used to without stating its size.
//
// This file a modified version of base::span from Chromium:
//   https://chromium.googlesource.com/chromium/src/+/d93ae920e4309682deb9352a4637cfc2941c1d1f/base/containers/span.h
//
// In order to minimize changes from the original, this file does NOT fully
// adhere to Pigweed's style guide.
//
// A few changes were made to the Chromium version of span. These include:
//   - Use std::data and std::size instead of base::* versions.
//   - Rename base namespace to pw.
//   - Rename internal namespace to pw_span_internal.
//   - Remove uses of checked_iterators.h and CHECK.
//   - Replace make_span functions with C++17 class template deduction guides.
//   - Use std::byte instead of uint8_t for compatibility with std::span.
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>

#include "pw_polyfill/language_feature_macros.h"
#include "pw_span/internal/config.h"

#if PW_SPAN_ENABLE_ASSERTS

#include "pw_assert/assert.h"

#define _PW_SPAN_ASSERT(arg) PW_ASSERT(arg)

#else
#define _PW_SPAN_ASSERT(arg)
#endif  // PW_SPAN_ENABLE_ASSERTS

namespace pw {

// [views.constants]
constexpr size_t dynamic_extent = std::numeric_limits<size_t>::max();

template <typename T, size_t Extent = dynamic_extent>
class span;

namespace pw_span_internal {

template <typename T>
struct ExtentImpl : std::integral_constant<size_t, dynamic_extent> {};

template <typename T, size_t N>
struct ExtentImpl<T[N]> : std::integral_constant<size_t, N> {};

template <typename T, size_t N>
struct ExtentImpl<std::array<T, N>> : std::integral_constant<size_t, N> {};

template <typename T, size_t N>
struct ExtentImpl<span<T, N>> : std::integral_constant<size_t, N> {};

template <typename T>
using Extent = ExtentImpl<std::remove_cv_t<std::remove_reference_t<T>>>;

template <typename T>
struct IsSpanImpl : std::false_type {};

template <typename T, size_t Extent>
struct IsSpanImpl<span<T, Extent>> : std::true_type {};

template <typename T>
using IsSpan = IsSpanImpl<std::decay_t<T>>;

template <typename T>
struct IsStdArrayImpl : std::false_type {};

template <typename T, size_t N>
struct IsStdArrayImpl<std::array<T, N>> : std::true_type {};

template <typename T>
using IsStdArray = IsStdArrayImpl<std::decay_t<T>>;

template <typename T>
using IsCArray = std::is_array<std::remove_reference_t<T>>;

template <typename From, typename To>
using IsLegalDataConversion = std::is_convertible<From (*)[], To (*)[]>;

template <typename Container, typename T>
using ContainerHasConvertibleData = IsLegalDataConversion<
    std::remove_pointer_t<decltype(std::data(std::declval<Container>()))>,
    T>;

template <typename Container>
using ContainerHasIntegralSize =
    std::is_integral<decltype(std::size(std::declval<Container>()))>;

template <typename From, size_t FromExtent, typename To, size_t ToExtent>
using EnableIfLegalSpanConversion =
    std::enable_if_t<(ToExtent == dynamic_extent || ToExtent == FromExtent) &&
                     IsLegalDataConversion<From, To>::value>;

// SFINAE check if Array can be converted to a span<T>.
template <typename Array, typename T, size_t Extent>
using EnableIfSpanCompatibleArray =
    std::enable_if_t<(Extent == dynamic_extent ||
                      Extent == pw_span_internal::Extent<Array>::value) &&
                     ContainerHasConvertibleData<Array, T>::value>;

// SFINAE check if Container can be converted to a span<T>.
template <typename Container, typename T>
using IsSpanCompatibleContainer =
    std::conditional_t<!IsSpan<Container>::value &&
                           !IsStdArray<Container>::value &&
                           !IsCArray<Container>::value &&
                           ContainerHasConvertibleData<Container, T>::value &&
                           ContainerHasIntegralSize<Container>::value,
                       std::true_type,
                       std::false_type>;

template <typename Container, typename T>
using EnableIfSpanCompatibleContainer =
    std::enable_if_t<IsSpanCompatibleContainer<Container, T>::value>;

template <typename Container, typename T, size_t Extent>
using EnableIfSpanCompatibleContainerAndSpanIsDynamic =
    std::enable_if_t<IsSpanCompatibleContainer<Container, T>::value &&
                     Extent == dynamic_extent>;

// A helper template for storing the size of a span. Spans with static extents
// don't require additional storage, since the extent itself is specified in the
// template parameter.
template <size_t Extent>
class ExtentStorage {
 public:
  constexpr explicit ExtentStorage(size_t /* size */) noexcept {}
  constexpr size_t size() const noexcept { return Extent; }
};

// Specialization of ExtentStorage for dynamic extents, which do require
// explicit storage for the size.
template <>
struct ExtentStorage<dynamic_extent> {
  constexpr explicit ExtentStorage(size_t size) noexcept : size_(size) {}
  constexpr size_t size() const noexcept { return size_; }

 private:
  size_t size_;
};

}  // namespace pw_span_internal

// A span is a value type that represents an array of elements of type T. Since
// it only consists of a pointer to memory with an associated size, it is very
// light-weight. It is cheap to construct, copy, move and use spans, so that
// users are encouraged to use it as a pass-by-value parameter. A span does not
// own the underlying memory, so care must be taken to ensure that a span does
// not outlive the backing store.
//
// span is somewhat analogous to StringPiece, but with arbitrary element types,
// allowing mutation if T is non-const.
//
// span is implicitly convertible from C++ arrays, as well as most [1]
// container-like types that provide a data() and size() method (such as
// std::vector<T>). A mutable span<T> can also be implicitly converted to an
// immutable span<const T>.
//
// Consider using a span for functions that take a data pointer and size
// parameter: it allows the function to still act on an array-like type, while
// allowing the caller code to be a bit more concise.
//
// For read-only data access pass a span<const T>: the caller can supply either
// a span<const T> or a span<T>, while the callee will have a read-only view.
// For read-write access a mutable span<T> is required.
//
// Without span:
//   Read-Only:
//     // std::string HexEncode(const uint8_t* data, size_t size);
//     std::vector<uint8_t> data_buffer = GenerateData();
//     std::string r = HexEncode(data_buffer.data(), data_buffer.size());
//
//  Mutable:
//     // ssize_t SafeSNPrintf(char* buf, size_t N, const char* fmt, Args...);
//     char str_buffer[100];
//     SafeSNPrintf(str_buffer, sizeof(str_buffer), "Pi ~= %lf", 3.14);
//
// With span:
//   Read-Only:
//     // std::string HexEncode(std::span<const uint8_t> data);
//     std::vector<uint8_t> data_buffer = GenerateData();
//     std::string r = HexEncode(data_buffer);
//
//  Mutable:
//     // ssize_t SafeSNPrintf(std::span<char>, const char* fmt, Args...);
//     char str_buffer[100];
//     SafeSNPrintf(str_buffer, "Pi ~= %lf", 3.14);
//
// Spans with "const" and pointers
// -------------------------------
//
// Const and pointers can get confusing. Here are vectors of pointers and their
// corresponding spans:
//
//   const std::vector<int*>        =>  std::span<int* const>
//   std::vector<const int*>        =>  std::span<const int*>
//   const std::vector<const int*>  =>  std::span<const int* const>
//
// Differences from the C++20 draft
// --------------------------------
//
// http://eel.is/c++draft/views contains the latest C++20 draft of std::span.
// Chromium tries to follow the draft as close as possible. Differences between
// the draft and the implementation are documented in subsections below.
//
// Differences from [span.cons]:
// - Constructing a static span (i.e. Extent != dynamic_extent) from a dynamic
//   sized container (e.g. std::vector) requires an explicit conversion (in the
//   C++20 draft this is simply UB)
//
// Furthermore, all constructors and methods are marked noexcept due to the lack
// of exceptions in Chromium.

// [span], class template span
template <typename T, size_t Extent>
class span : public pw_span_internal::ExtentStorage<Extent> {
 private:
  using ExtentStorage = pw_span_internal::ExtentStorage<Extent>;

 public:
  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using pointer = T*;
  using reference = T&;
  using iterator = T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  static constexpr size_t extent = Extent;

  // [span.cons], span constructors, copy, assignment, and destructor
  constexpr span() noexcept : ExtentStorage(0), data_(nullptr) {
    static_assert(Extent == dynamic_extent || Extent == 0, "Invalid Extent");
  }

  constexpr span(T* data, size_t size) noexcept
      : ExtentStorage(size), data_(data) {
    _PW_SPAN_ASSERT(Extent == dynamic_extent || Extent == size);
  }

  // Prevent construction from nullptr, which is disallowed by C++20's std::span
  constexpr span(std::nullptr_t data, size_t size) = delete;

  // Artificially templatized to break ambiguity for span(ptr, 0).
  template <typename = void>
  constexpr span(T* begin, T* end) noexcept : span(begin, end - begin) {
    // Note: CHECK_LE is not constexpr, hence regular CHECK must be used.
    _PW_SPAN_ASSERT(begin <= end);
  }

  template <
      size_t N,
      typename =
          pw_span_internal::EnableIfSpanCompatibleArray<T (&)[N], T, Extent>>
  constexpr span(T (&array)[N]) noexcept : span(std::data(array), N) {}

  template <typename U,
            size_t N,
            typename = pw_span_internal::
                EnableIfSpanCompatibleArray<std::array<U, N>&, T, Extent>>
  constexpr span(std::array<U, N>& array) noexcept
      : span(std::data(array), N) {}

  template <typename U,
            size_t N,
            typename = pw_span_internal::
                EnableIfSpanCompatibleArray<const std::array<U, N>&, T, Extent>>
  constexpr span(const std::array<U, N>& array) noexcept
      : span(std::data(array), N) {}

  // Conversion from a container that has compatible std::data() and integral
  // std::size().
  template <typename Container,
            typename = pw_span_internal::
                EnableIfSpanCompatibleContainerAndSpanIsDynamic<Container&,
                                                                T,
                                                                Extent>>
  constexpr span(Container& container) noexcept
      : span(std::data(container), std::size(container)) {}

  template <
      typename Container,
      typename = pw_span_internal::
          EnableIfSpanCompatibleContainerAndSpanIsDynamic<const Container&,
                                                          T,
                                                          Extent>>
  constexpr span(const Container& container) noexcept
      : span(std::data(container), std::size(container)) {}

  constexpr span(const span& other) noexcept = default;

  // Conversions from spans of compatible types and extents: this allows a
  // span<T> to be seamlessly used as a span<const T>, but not the other way
  // around. If extent is not dynamic, OtherExtent has to be equal to Extent.
  template <typename U,
            size_t OtherExtent,
            typename = pw_span_internal::
                EnableIfLegalSpanConversion<U, OtherExtent, T, Extent>>
  constexpr span(const span<U, OtherExtent>& other)
      : span(other.data(), other.size()) {}

  constexpr span& operator=(const span& other) noexcept = default;
  ~span() noexcept = default;

  // [span.sub], span subviews
  template <size_t Count>
  constexpr span<T, Count> first() const noexcept {
    static_assert(Count <= Extent, "Count must not exceed Extent");
    _PW_SPAN_ASSERT(Extent != dynamic_extent || Count <= size());
    return {data(), Count};
  }

  template <size_t Count>
  constexpr span<T, Count> last() const noexcept {
    static_assert(Count <= Extent, "Count must not exceed Extent");
    _PW_SPAN_ASSERT(Extent != dynamic_extent || Count <= size());
    return {data() + (size() - Count), Count};
  }

  template <size_t Offset, size_t Count = dynamic_extent>
  constexpr span<T,
                 (Count != dynamic_extent
                      ? Count
                      : (Extent != dynamic_extent ? Extent - Offset
                                                  : dynamic_extent))>
  subspan() const noexcept {
    static_assert(Offset <= Extent, "Offset must not exceed Extent");
    static_assert(Count == dynamic_extent || Count <= Extent - Offset,
                  "Count must not exceed Extent - Offset");
    _PW_SPAN_ASSERT(Extent != dynamic_extent || Offset <= size());
    _PW_SPAN_ASSERT(Extent != dynamic_extent || Count == dynamic_extent ||
                    Count <= size() - Offset);
    return {data() + Offset, Count != dynamic_extent ? Count : size() - Offset};
  }

  constexpr span<T, dynamic_extent> first(size_t count) const noexcept {
    // Note: CHECK_LE is not constexpr, hence regular CHECK must be used.
    _PW_SPAN_ASSERT(count <= size());
    return {data(), count};
  }

  constexpr span<T, dynamic_extent> last(size_t count) const noexcept {
    // Note: CHECK_LE is not constexpr, hence regular CHECK must be used.
    _PW_SPAN_ASSERT(count <= size());
    return {data() + (size() - count), count};
  }

  constexpr span<T, dynamic_extent> subspan(
      size_t offset, size_t count = dynamic_extent) const noexcept {
    // Note: CHECK_LE is not constexpr, hence regular CHECK must be used.
    _PW_SPAN_ASSERT(offset <= size());
    _PW_SPAN_ASSERT(count == dynamic_extent || count <= size() - offset);
    return {data() + offset, count != dynamic_extent ? count : size() - offset};
  }

  // [span.obs], span observers
  constexpr size_t size() const noexcept { return ExtentStorage::size(); }
  constexpr size_t size_bytes() const noexcept { return size() * sizeof(T); }
  [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }

  // [span.elem], span element access
  constexpr T& operator[](size_t idx) const noexcept {
    // Note: CHECK_LT is not constexpr, hence regular CHECK must be used.
    _PW_SPAN_ASSERT(idx < size());
    return *(data() + idx);
  }

  constexpr T& front() const noexcept {
    static_assert(Extent == dynamic_extent || Extent > 0,
                  "Extent must not be 0");
    _PW_SPAN_ASSERT(Extent != dynamic_extent || !empty());
    return *data();
  }

  constexpr T& back() const noexcept {
    static_assert(Extent == dynamic_extent || Extent > 0,
                  "Extent must not be 0");
    _PW_SPAN_ASSERT(Extent != dynamic_extent || !empty());
    return *(data() + size() - 1);
  }

  constexpr T* data() const noexcept { return data_; }

  // [span.iter], span iterator support
  constexpr iterator begin() const noexcept { return data_; }
  constexpr iterator end() const noexcept { return data_ + size(); }

  constexpr reverse_iterator rbegin() const noexcept {
    return reverse_iterator(end());
  }
  constexpr reverse_iterator rend() const noexcept {
    return reverse_iterator(begin());
  }

 private:
  T* data_;
};

// span<T, Extent>::extent can not be declared inline prior to C++17, hence this
// definition is required.
// template <class T, size_t Extent>
// constexpr size_t span<T, Extent>::extent;

// [span.objectrep], views of object representation
template <typename T, size_t X>
span<const std::byte, (X == dynamic_extent ? dynamic_extent : sizeof(T) * X)>
as_bytes(span<T, X> s) noexcept {
  return {reinterpret_cast<const std::byte*>(s.data()), s.size_bytes()};
}

template <typename T,
          size_t X,
          typename = std::enable_if_t<!std::is_const<T>::value>>
span<std::byte, (X == dynamic_extent ? dynamic_extent : sizeof(T) * X)>
as_writable_bytes(span<T, X> s) noexcept {
  return {reinterpret_cast<std::byte*>(s.data()), s.size_bytes()};
}

// Type-deducing helpers for constructing a span.
// Pigweed: Instead of a make_span function, provide the deduction guides
//     specified in the C++20 standard.
#ifdef __cpp_deduction_guides

template <class T, std::size_t N>
span(T (&)[N]) -> span<T, N>;

template <class T, std::size_t N>
span(std::array<T, N>&) -> span<T, N>;

template <class T, std::size_t N>
span(const std::array<T, N>&) -> span<const T, N>;

namespace pw_span_internal {

// Containers can be mutable or const and have mutable or const members. Check
// the type of the accessed elements to determine which type of span should be
// created (e.g. span<char> or span<const char>).
template <typename T>
using ValueType = std::remove_reference_t<decltype(std::declval<T>()[0])>;

}  // namespace pw_span_internal

// This diverges a little from the standard, which uses std::ranges.
template <class Container>
span(Container&) -> span<pw_span_internal::ValueType<Container>>;

template <class Container>
span(const Container&) -> span<pw_span_internal::ValueType<const Container>>;

#endif  // __cpp_deduction_guides

}  // namespace pw

#undef _PW_SPAN_ASSERT
