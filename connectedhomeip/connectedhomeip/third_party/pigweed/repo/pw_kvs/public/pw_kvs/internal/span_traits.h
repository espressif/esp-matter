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

#include <iterator>
#include <type_traits>

namespace pw {
namespace kvs {
namespace internal {

// This borrows the `make_span` function from Chromium and uses to see if a type
// can be represented as a span. See:
// https://chromium.googlesource.com/chromium/src/+/main/base/containers/span.h

// Simplified implementation of C++20's std::iter_reference_t.
// As opposed to std::iter_reference_t, this implementation does not restrict
// the type of `Iter`.
//
// Reference: https://wg21.link/iterator.synopsis#:~:text=iter_reference_t
template <typename Iter>
using iter_reference_t = decltype(*std::declval<Iter&>());

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

// Type-deducing helpers for constructing a span.
template <int&... ExplicitArgumentBarrier, typename It, typename EndOrSize>
constexpr auto make_span(It it, EndOrSize end_or_size) noexcept {
  using T = std::remove_reference_t<iter_reference_t<It>>;
  return span<T>(it, end_or_size);
}

// make_span utility function that deduces both the span's value_type and extent
// from the passed in argument.
//
// Usage: auto span = base::make_span(...);
template <int&... ExplicitArgumentBarrier,
          typename Container,
          typename T = std::remove_pointer_t<
              decltype(std::data(std::declval<Container>()))>>
constexpr auto make_span(Container&& container) noexcept {
  return span<T, Extent<Container>::value>(std::forward<Container>(container));
}

// The make_span functions above don't seem to work correctly with arrays of
// non-const values, so add const to the type. That is fine for KVS's Put
// method, since the values can be accepted as const.
template <typename T,
          typename = decltype(make_span(std::declval<std::add_const_t<T>>()))>
constexpr bool ConvertsToSpan(int) {
  return true;
}

// If the expression span(T) fails, then the type can't be converted to a
// span.
template <typename T>
constexpr bool ConvertsToSpan(...) {
  return false;
}

}  // namespace internal

// Traits class to detect if the type converts to a span.
template <typename T>
struct ConvertsToSpan
    : public std::bool_constant<
          internal::ConvertsToSpan<std::remove_reference_t<T>>(0)> {};

}  // namespace kvs
}  // namespace pw
