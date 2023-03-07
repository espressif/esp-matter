// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_UTILITY_H_
#define LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_UTILITY_H_

#include <lib/stdcompat/type_traits.h>

#include <type_traits>
#include <utility>

namespace fit {
namespace internal {

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
struct occurences_of : std::integral_constant<size_t, 0> {};
template <typename T, typename U>
struct occurences_of<T, U> : std::integral_constant<size_t, std::is_same<T, U>::value> {};
template <typename T, typename First, typename... Rest>
struct occurences_of<T, First, Rest...>
    : std::integral_constant<size_t,
                             occurences_of<T, First>::value + occurences_of<T, Rest...>::value> {};

template <typename T, typename... Ts>
constexpr size_t occurences_of_v = occurences_of<T, Ts...>::value;

// Utility to remove const, volatile, and reference qualifiers.
template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

// Evaluates to truth-like when type T matches type U with cv-reference removed.
template <typename T, typename U>
using not_same_type = cpp17::negation<std::is_same<T, remove_cvref_t<U>>>;

// Concept helper for constructors.
template <typename... Conditions>
using requires_conditions = std::enable_if_t<cpp17::conjunction_v<Conditions...>, bool>;

// Concept helper for assignment operators.
template <typename Return, typename... Conditions>
using assignment_requires_conditions =
    std::enable_if_t<cpp17::conjunction_v<Conditions...>, std::add_lvalue_reference_t<Return>>;

// Evaluates to true when every element type of Ts is trivially destructible.
template <typename... Ts>
constexpr bool is_trivially_destructible_v =
    cpp17::conjunction_v<std::is_trivially_destructible<Ts>...>;

// Evaluates to true when every element type of Ts is trivially copyable.
template <typename... Ts>
constexpr bool is_trivially_copyable_v =
    (cpp17::conjunction_v<std::is_trivially_copy_assignable<Ts>...> &&
     cpp17::conjunction_v<std::is_trivially_copy_constructible<Ts>...>);

// Evaluates to true when every element type of Ts is trivially movable.
template <typename... Ts>
constexpr bool is_trivially_movable_v =
    (cpp17::conjunction_v<std::is_trivially_move_assignable<Ts>...> &&
     cpp17::conjunction_v<std::is_trivially_move_constructible<Ts>...>);

// Enable if relational operator is convertible to bool and the optional
// conditions are true.
template <typename Op, typename... Conditions>
using enable_relop_t =
    std::enable_if_t<(std::is_convertible<Op, bool>::value && cpp17::conjunction_v<Conditions...>),
                     bool>;

template <typename T>
struct identity {
  using type = T;
};

// Evaluates to true when T is an unbounded array.
template <typename T>
struct is_unbounded_array : cpp17::conjunction<std::is_array<T>, cpp17::negation<std::extent<T>>> {
};

// Returns true when T is a complete type or an unbounded array.
template <typename T, size_t = sizeof(T)>
constexpr bool is_complete_or_unbounded_array(identity<T>) {
  return true;
}
template <typename Identity, typename T = typename Identity::type>
constexpr bool is_complete_or_unbounded_array(Identity) {
  return cpp17::disjunction<std::is_reference<T>, std::is_function<T>, std::is_void<T>,
                            is_unbounded_array<T>>::value;
}

// Using swap for ADL. This directive is contained within the fit::internal
// namespace, which prevents leaking std::swap into user namespaces. Doing this
// at namespace scope is necessary to lookup swap via ADL while preserving the
// noexcept() specification of the resulting lookup.
using std::swap;

// Evaluates to true when T is swappable.
template <typename T, typename = void>
struct is_swappable : std::false_type {
  static_assert(is_complete_or_unbounded_array(identity<T>{}),
                "T must be a complete type or an unbounded array!");
};
template <typename T>
struct is_swappable<T, cpp17::void_t<decltype(swap(std::declval<T&>(), std::declval<T&>()))>>
    : std::true_type {
  static_assert(is_complete_or_unbounded_array(identity<T>{}),
                "T must be a complete type or an unbounded array!");
};

// Evaluates to true when T is nothrow swappable.
template <typename T, typename = void>
struct is_nothrow_swappable : std::false_type {
  static_assert(is_complete_or_unbounded_array(identity<T>{}),
                "T must be a complete type or an unbounded array!");
};
template <typename T>
struct is_nothrow_swappable<T,
                            cpp17::void_t<decltype(swap(std::declval<T&>(), std::declval<T&>()))>>
    : std::integral_constant<bool, noexcept(swap(std::declval<T&>(), std::declval<T&>()))> {
  static_assert(is_complete_or_unbounded_array(identity<T>{}),
                "T must be a complete type or an unbounded array!");
};

}  // namespace internal
}  // namespace fit

#endif  // LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_UTILITY_H_
