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

#include <type_traits>

#include "pw_polyfill/standard_library/namespace.h"

_PW_POLYFILL_BEGIN_NAMESPACE_STD

template <typename T>
constexpr remove_reference_t<T>&& move(T&& object) {
  return (remove_reference_t<T> &&) object;
}

// Forward declare these classes, which are specialized in other headers.
template <decltype(sizeof(0)), typename>
struct tuple_element;

template <typename>
struct tuple_size;

template <typename T, T... kSequence>
class integer_sequence;

template <size_t... kSequence>
using index_sequence = integer_sequence<decltype(sizeof(int)), kSequence...>;

template <typename T, T kEnd>
#if __has_builtin(__make_integer_seq)
using make_integer_sequence = __make_integer_seq<integer_sequence, T, kEnd>;
#elif __has_builtin(__integer_pack)
using make_integer_sequence = integer_sequence<T, __integer_pack(kEnd)...>;
#endif  // make_integer_sequence

template <size_t kEnd>
using make_index_sequence = make_integer_sequence<size_t, kEnd>;

struct in_place_t {
  explicit constexpr in_place_t() = default;
};

inline constexpr in_place_t in_place{};

_PW_POLYFILL_END_NAMESPACE_STD
