// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_UTILITY_H_
#define LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_UTILITY_H_

#include <cstddef>
#include <utility>

#include "../type_traits.h"

namespace cpp17 {
namespace internal {

template <typename Tag>
struct instantiate_templated_tag {
  static constexpr const Tag storage{};
};

template <typename Tag>
constexpr const Tag instantiate_templated_tag<Tag>::storage;

template <typename T, typename ValueType, ValueType Value>
struct inline_storage {
  static constexpr const ValueType storage{Value};
};

template <typename T, typename ValueType, ValueType Value>
constexpr const ValueType inline_storage<T, ValueType, Value>::storage;

// Utility to return the first type in a parameter pack.
template <typename... Ts>
struct first;
template <typename First, typename... Rest>
struct first<First, Rest...> {
  using type = First;
};

template <typename... Ts>
using first_t = typename first<Ts...>::type;

// Utility to count the occurences of type T in the parameter pack Ts.
template <typename T, typename... Ts>
struct occurences_of : std::integral_constant<std::size_t, 0> {};
template <typename T, typename U>
struct occurences_of<T, U> : std::integral_constant<std::size_t, std::is_same<T, U>::value> {};
template <typename T, typename First, typename... Rest>
struct occurences_of<T, First, Rest...>
    : std::integral_constant<std::size_t,
                             occurences_of<T, First>::value + occurences_of<T, Rest...>::value> {};

template <typename T, typename... Ts>
constexpr std::size_t occurences_of_v = occurences_of<T, Ts...>::value;

// Evaluates to truth-like when type T matches type U with cv-reference removed.
template <typename T, typename U>
using not_same_type = negation<std::is_same<T, ::cpp20::remove_cvref_t<U>>>;

// Concept helper for constructors.
template <typename... Conditions>
using requires_conditions = std::enable_if_t<conjunction_v<Conditions...>, bool>;

// Concept helper for assignment operators.
template <typename Return, typename... Conditions>
using assignment_requires_conditions =
    std::enable_if_t<conjunction_v<Conditions...>, std::add_lvalue_reference_t<Return>>;

// Evaluates to true when every element type of Ts is trivially destructible.
template <typename... Ts>
constexpr bool is_trivially_destructible_v = conjunction_v<std::is_trivially_destructible<Ts>...>;

// Evaluates to true when every element type of Ts is trivially copyable.
template <typename... Ts>
constexpr bool is_trivially_copyable_v =
    (conjunction_v<std::is_trivially_copy_assignable<Ts>...> &&
     conjunction_v<std::is_trivially_copy_constructible<Ts>...>);

// Evaluates to true when every element type of Ts is trivially movable.
template <typename... Ts>
constexpr bool is_trivially_movable_v =
    (conjunction_v<std::is_trivially_move_assignable<Ts>...> &&
     conjunction_v<std::is_trivially_move_constructible<Ts>...>);

// Enable if relational operator is convertible to bool and the optional
// conditions are true.
template <typename Op, typename... Conditions>
using enable_relop_t =
    std::enable_if_t<(std::is_convertible<Op, bool>::value && conjunction_v<Conditions...>), bool>;

// Returns true when T is a complete type or an unbounded array.
template <typename T, std::size_t = sizeof(T)>
constexpr bool is_complete_or_unbounded_array(::cpp20::type_identity<T>) {
  return true;
}
template <typename Identity, typename T = typename Identity::type>
constexpr bool is_complete_or_unbounded_array(Identity) {
  return disjunction<std::is_reference<T>, std::is_function<T>, std::is_void<T>,
                     ::cpp20::is_unbounded_array<T>>::value;
}

// Using swap for ADL. This directive is contained within the cpp17::internal
// namespace, which prevents leaking std::swap into user namespaces. Doing this
// at namespace scope is necessary to lookup swap via ADL while preserving the
// noexcept() specification of the resulting lookup.
using std::swap;

// Evaluates to true when T is swappable.
template <typename T, typename = void>
struct is_swappable : std::false_type {
  static_assert(is_complete_or_unbounded_array(::cpp20::type_identity<T>{}),
                "T must be a complete type or an unbounded array!");
};
template <typename T>
struct is_swappable<T, void_t<decltype(swap(std::declval<T&>(), std::declval<T&>()))>>
    : std::true_type {
  static_assert(is_complete_or_unbounded_array(::cpp20::type_identity<T>{}),
                "T must be a complete type or an unbounded array!");
};

// Evaluates to true when T is nothrow swappable.
template <typename T, typename = void>
struct is_nothrow_swappable : std::false_type {
  static_assert(is_complete_or_unbounded_array(::cpp20::type_identity<T>{}),
                "T must be a complete type or an unbounded array!");
};
template <typename T>
struct is_nothrow_swappable<T, void_t<decltype(swap(std::declval<T&>(), std::declval<T&>()))>>
    : std::integral_constant<bool, noexcept(swap(std::declval<T&>(), std::declval<T&>()))> {
  static_assert(is_complete_or_unbounded_array(::cpp20::type_identity<T>{}),
                "T must be a complete type or an unbounded array!");
};

}  // namespace internal
}  // namespace cpp17

#endif  // LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_UTILITY_H_
