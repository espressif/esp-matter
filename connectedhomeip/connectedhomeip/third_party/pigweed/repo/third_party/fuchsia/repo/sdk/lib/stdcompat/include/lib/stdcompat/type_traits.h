// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_TYPE_TRAITS_H_
#define LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_TYPE_TRAITS_H_

#include <cstddef>
#include <tuple>
#include <type_traits>

#include "internal/type_traits.h"
#include "version.h"

namespace cpp17 {

#if defined(__cpp_lib_void_t) && __cpp_lib_void_t >= 201411L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)
using std::void_t;
#else   // Provide std::void_t polyfill.
template <typename... T>
using void_t = void;
#endif  // __cpp_lib_void_t >= 201411L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#if defined(__cpp_lib_logical_traits) && __cpp_lib_logical_traits >= 201510L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::conjunction;
using std::conjunction_v;
using std::disjunction;
using std::disjunction_v;
using std::negation;
using std::negation_v;

#else  // Provide polyfills for std::{negation, conjunction, disjunction} and the *_v helpers.

template <typename... Ts>
struct conjunction : std::true_type {};
template <typename T>
struct conjunction<T> : T {};
template <typename First, typename... Rest>
struct conjunction<First, Rest...>
    : std::conditional_t<bool(First::value), conjunction<Rest...>, First> {};

template <typename... Ts>
static constexpr bool conjunction_v = conjunction<Ts...>::value;

template <typename... Ts>
struct disjunction : std::false_type {};
template <typename T>
struct disjunction<T> : T {};
template <typename First, typename... Rest>
struct disjunction<First, Rest...>
    : std::conditional_t<bool(First::value), First, disjunction<Rest...>> {};

template <typename... Ts>
static constexpr bool disjunction_v = disjunction<Ts...>::value;

// Utility type that negates its truth-like parameter type.
template <typename T>
struct negation : std::integral_constant<bool, !bool(T::value)> {};

template <typename T>
static constexpr bool negation_v = negation<T>::value;

#endif  // __cpp_lib_logical_traits >= 201510L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#if defined(__cpp_lib_bool_constant) && __cpp_lib_bool_constant >= 201505L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::bool_constant;

#else  // Provide polyfill for std::bool_constant

template <bool B>
using bool_constant = std::integral_constant<bool, B>;

#endif  // __cpp_lib_bool_constant >= 201505L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#if defined(__cpp_lib_type_trait_variable_templates) && \
    __cpp_lib_type_trait_variable_templates >= 201510L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::is_array_v;
using std::is_class_v;
using std::is_enum_v;
using std::is_floating_point_v;
using std::is_function_v;
using std::is_integral_v;
using std::is_lvalue_reference_v;
using std::is_member_function_pointer_v;
using std::is_member_object_pointer_v;
using std::is_null_pointer_v;
using std::is_pointer_v;
using std::is_rvalue_reference_v;
using std::is_union_v;
using std::is_void_v;

using std::is_arithmetic_v;
using std::is_compound_v;
using std::is_fundamental_v;
using std::is_member_pointer_v;
using std::is_object_v;
using std::is_reference_v;
using std::is_scalar_v;

using std::is_abstract_v;
using std::is_const_v;
using std::is_empty_v;
using std::is_final_v;
using std::is_pod_v;
using std::is_polymorphic_v;
using std::is_signed_v;
using std::is_standard_layout_v;
using std::is_trivial_v;
using std::is_trivially_copyable_v;
using std::is_unsigned_v;
using std::is_volatile_v;

using std::is_constructible_v;
using std::is_nothrow_constructible_v;
using std::is_trivially_constructible_v;

using std::is_default_constructible_v;
using std::is_nothrow_default_constructible_v;
using std::is_trivially_default_constructible_v;

using std::is_copy_constructible_v;
using std::is_nothrow_copy_constructible_v;
using std::is_trivially_copy_constructible_v;

using std::is_move_constructible_v;
using std::is_nothrow_move_constructible_v;
using std::is_trivially_move_constructible_v;

using std::is_assignable_v;
using std::is_nothrow_assignable_v;
using std::is_trivially_assignable_v;

using std::is_copy_assignable_v;
using std::is_nothrow_copy_assignable_v;
using std::is_trivially_copy_assignable_v;

using std::is_move_assignable_v;
using std::is_nothrow_move_assignable_v;
using std::is_trivially_move_assignable_v;

using std::is_destructible_v;
using std::is_nothrow_destructible_v;
using std::is_trivially_destructible_v;

using std::has_virtual_destructor_v;

using std::alignment_of_v;
using std::extent_v;
using std::rank_v;

using std::is_base_of_v;
using std::is_convertible_v;
using std::is_same_v;

#else  // Provide polyfills for the bulk of the *_v helpers

template <typename T>
static constexpr bool is_void_v = std::is_void<T>::value;
template <typename T>
static constexpr bool is_null_pointer_v = std::is_null_pointer<T>::value;
template <typename T>
static constexpr bool is_integral_v = std::is_integral<T>::value;
template <typename T>
static constexpr bool is_floating_point_v = std::is_floating_point<T>::value;
template <typename T>
static constexpr bool is_array_v = std::is_array<T>::value;
template <typename T>
static constexpr bool is_enum_v = std::is_enum<T>::value;
template <typename T>
static constexpr bool is_union_v = std::is_union<T>::value;
template <typename T>
static constexpr bool is_class_v = std::is_class<T>::value;
template <typename T>
static constexpr bool is_function_v = std::is_function<T>::value;
template <typename T>
static constexpr bool is_pointer_v = std::is_pointer<T>::value;
template <typename T>
static constexpr bool is_lvalue_reference_v = std::is_lvalue_reference<T>::value;
template <typename T>
static constexpr bool is_rvalue_reference_v = std::is_rvalue_reference<T>::value;
template <typename T>
static constexpr bool is_member_object_pointer_v = std::is_member_object_pointer<T>::value;
template <typename T>
static constexpr bool is_member_function_pointer_v = std::is_member_function_pointer<T>::value;

template <typename T>
static constexpr bool is_fundamental_v = std::is_fundamental<T>::value;
template <typename T>
static constexpr bool is_arithmetic_v = std::is_arithmetic<T>::value;
template <typename T>
static constexpr bool is_scalar_v = std::is_scalar<T>::value;
template <typename T>
static constexpr bool is_object_v = std::is_object<T>::value;
template <typename T>
static constexpr bool is_compound_v = std::is_compound<T>::value;
template <typename T>
static constexpr bool is_reference_v = std::is_reference<T>::value;
template <typename T>
static constexpr bool is_member_pointer_v = std::is_member_pointer<T>::value;

template <typename T>
static constexpr bool is_const_v = std::is_const<T>::value;
template <typename T>
static constexpr bool is_volatile_v = std::is_volatile<T>::value;
template <typename T>
static constexpr bool is_trivial_v = std::is_trivial<T>::value;
template <typename T>
static constexpr bool is_trivially_copyable_v = std::is_trivially_copyable<T>::value;
template <typename T>
static constexpr bool is_standard_layout_v = std::is_standard_layout<T>::value;
template <typename T>
[[deprecated]] static constexpr bool is_pod_v = std::is_pod<T>::value;
template <typename T>
static constexpr bool is_empty_v = std::is_empty<T>::value;
template <typename T>
static constexpr bool is_polymorphic_v = std::is_polymorphic<T>::value;
template <typename T>
static constexpr bool is_abstract_v = std::is_abstract<T>::value;
template <typename T>
static constexpr bool is_final_v = std::is_final<T>::value;
template <typename T>
static constexpr bool is_signed_v = std::is_signed<T>::value;
template <typename T>
static constexpr bool is_unsigned_v = std::is_unsigned<T>::value;

template <typename T, typename... Args>
static constexpr bool is_constructible_v = std::is_constructible<T, Args...>::value;
template <typename T, typename... Args>
static constexpr bool is_trivially_constructible_v =
    std::is_trivially_constructible<T, Args...>::value;
template <typename T, typename... Args>
static constexpr bool is_nothrow_constructible_v = std::is_nothrow_constructible<T, Args...>::value;

template <typename T>
static constexpr bool is_default_constructible_v = std::is_default_constructible<T>::value;
template <typename T>
static constexpr bool is_trivially_default_constructible_v =
    std::is_trivially_default_constructible<T>::value;
template <typename T>
static constexpr bool is_nothrow_default_constructible_v =
    std::is_nothrow_default_constructible<T>::value;

template <typename T>
static constexpr bool is_copy_constructible_v = std::is_copy_constructible<T>::value;
template <typename T>
static constexpr bool is_trivially_copy_constructible_v =
    std::is_trivially_copy_constructible<T>::value;
template <typename T>
static constexpr bool is_nothrow_copy_constructible_v =
    std::is_nothrow_copy_constructible<T>::value;

template <typename T>
static constexpr bool is_move_constructible_v = std::is_move_constructible<T>::value;
template <typename T>
static constexpr bool is_trivially_move_constructible_v =
    std::is_trivially_move_constructible<T>::value;
template <typename T>
static constexpr bool is_nothrow_move_constructible_v =
    std::is_nothrow_move_constructible<T>::value;

template <typename T, typename U>
static constexpr bool is_assignable_v = std::is_assignable<T, U>::value;
template <typename T, typename U>
static constexpr bool is_trivially_assignable_v = std::is_trivially_assignable<T, U>::value;
template <typename T, typename U>
static constexpr bool is_nothrow_assignable_v = std::is_nothrow_assignable<T, U>::value;

template <typename T>
static constexpr bool is_copy_assignable_v = std::is_copy_assignable<T>::value;
template <typename T>
static constexpr bool is_trivially_copy_assignable_v = std::is_trivially_copy_assignable<T>::value;
template <typename T>
static constexpr bool is_nothrow_copy_assignable_v = std::is_nothrow_copy_assignable<T>::value;

template <typename T>
static constexpr bool is_move_assignable_v = std::is_move_assignable<T>::value;
template <typename T>
static constexpr bool is_trivially_move_assignable_v = std::is_trivially_move_assignable<T>::value;
template <typename T>
static constexpr bool is_nothrow_move_assignable_v = std::is_nothrow_move_assignable<T>::value;

template <typename T>
static constexpr bool is_destructible_v = std::is_destructible<T>::value;
template <typename T>
static constexpr bool is_trivially_destructible_v = std::is_trivially_destructible<T>::value;
template <typename T>
static constexpr bool is_nothrow_destructible_v = std::is_nothrow_destructible<T>::value;

template <typename T>
static constexpr bool has_virtual_destructor_v = std::has_virtual_destructor<T>::value;

template <typename T>
static constexpr bool alignment_of_v = std::alignment_of<T>::value;
template <typename T>
static constexpr bool rank_v = std::rank<T>::value;
template <typename T, unsigned N = 0>
static constexpr bool extent_v = std::extent<T, N>::value;

template <typename T, typename U>
static constexpr bool is_same_v = std::is_same<T, U>::value;
template <typename T, typename U>
static constexpr bool is_base_of_v = std::is_base_of<T, U>::value;
template <typename T, typename U>
static constexpr bool is_convertible_v = std::is_convertible<T, U>::value;

#endif  // __cpp_lib_type_trait_variable_templates >= 201510L &&
        // !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#if defined(__cpp_lib_is_aggregate) && __cpp_lib_is_aggregate >= 201703L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::is_aggregate;
using std::is_aggregate_v;

#else  // Provide std::is_aggregate polyfill

template <typename T>
struct is_aggregate : bool_constant<__is_aggregate(T)> {};

template <typename T>
static constexpr bool is_aggregate_v = is_aggregate<T>::value;

#endif  // __cpp_lib_is_aggregate >= 201703L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#if defined(__cpp_lib_is_invocable) && __cpp_lib_is_invocable >= 201703L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::is_invocable;
using std::is_invocable_r;
using std::is_nothrow_invocable;
using std::is_nothrow_invocable_r;

using std::is_invocable_r_v;
using std::is_invocable_v;
using std::is_nothrow_invocable_r_v;
using std::is_nothrow_invocable_v;

using std::invoke_result;
using std::invoke_result_t;

#else

template <typename R, typename F, typename... Args>
struct is_invocable_r : decltype(::cpp17::internal::is_valid_invoke<R, F, Args...>(nullptr)) {};

template <typename R, typename F, typename... Args>
static constexpr bool is_invocable_r_v = is_invocable_r<R, F, Args...>::value;

// INVOKE() is a subexpression of INVOKE<R>()
// INVOKE<void>(f, t1, t2, ..., tn) results in a call to
// static_cast<void>(INVOKE(f, t1, t2, ..., tn)) per [func.require] ¶ 2
template <typename F, typename... Args>
struct is_invocable : is_invocable_r<void, F, Args...> {};

template <typename F, typename... Args>
static constexpr bool is_invocable_v = is_invocable<F, Args...>::value;

template <typename F, typename... Args>
struct is_nothrow_invocable : bool_constant<is_invocable_v<F, Args...> &&
                                            noexcept(::cpp17::internal::invoke(
                                                std::declval<F>(), std::declval<Args>()...))> {};

template <typename F, typename... Args>
static constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<F, Args...>::value;

template <typename R, typename F, typename... Args>
struct is_nothrow_invocable_r : bool_constant<is_invocable_r_v<R, F, Args...> &&
                                              noexcept(::cpp17::internal::invoke_r<R>(
                                                  std::declval<F>(), std::declval<Args>()...))> {};

template <typename R, typename F, typename... Args>
static constexpr bool is_nothrow_invocable_r_v = is_nothrow_invocable_r<R, F, Args...>::value;

template <typename F, typename... Args>
struct invoke_result : ::cpp17::internal::invoke_result<is_invocable_v<F, Args...>, F, Args...> {};

template <typename F, typename... Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;

#endif  // __cpp_lib_is_invocable >= 201703L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

}  // namespace cpp17

namespace cpp20 {

#if defined(__cpp_lib_bounded_array_traits) && __cpp_lib_bounded_array_traits >= 201902L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::is_bounded_array;
using std::is_bounded_array_v;

using std::is_unbounded_array;
using std::is_unbounded_array_v;

#else  // Provide polyfills for std::is_{,un}bounded_array{,_v}

template <typename T>
struct is_bounded_array : std::false_type {};
template <typename T, std::size_t N>
struct is_bounded_array<T[N]> : std::true_type {};

template <typename T>
static constexpr bool is_bounded_array_v = is_bounded_array<T>::value;

template <typename T>
struct is_unbounded_array : std::false_type {};
template <typename T>
struct is_unbounded_array<T[]> : std::true_type {};

template <typename T>
static constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;

#endif  // __cpp_lib_bounded_array_traits >= 201902L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#if defined(__cpp_lib_remove_cvref) && __cpp_lib_remove_cvref >= 201711L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::remove_cvref;
using std::remove_cvref_t;

#else  // Provide polyfill for std::remove_cvref{,_t}

template <typename T>
struct remove_cvref {
  using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

template <typename T>
using remove_cvref_t = typename remove_cvref<T>::type;

#endif  // __cpp_lib_remove_cvref >= 201711L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#if defined(__cpp_lib_type_identity) && __cpp_lib_type_identity >= 201806L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::type_identity;
using std::type_identity_t;

#else  // Provide polyfill for std::type_identity{,_t}

template <typename T>
struct type_identity {
  using type = T;
};

template <typename T>
using type_identity_t = typename type_identity<T>::type;

#endif  // __cpp_lib_type_identity >= 201806L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#if defined(__cpp_lib_is_constant_evaluated) && __cpp_lib_is_constant_evaluated >= 201811L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#define LIB_STDCOMPAT_CONSTEVAL_SUPPORT 1
using std::is_constant_evaluated;

#else  // Provide polyfill for std::is_constant_evaluated

#ifdef __has_builtin
#if __has_builtin(__builtin_is_constant_evaluated)

#define LIB_STDCOMPAT_CONSTEVAL_SUPPORT 1
inline constexpr bool is_constant_evaluated() noexcept { return __builtin_is_constant_evaluated(); }

#endif  // __has_builtin(__builtin_is_constant_evaluated)
#endif  // __has_builtin

#ifndef LIB_STDCOMPAT_CONSTEVAL_SUPPORT

#define LIB_STDCOMPAT_CONSTEVAL_SUPPORT 0
inline constexpr bool is_constant_evaluated() noexcept { return false; }

#endif  // LIB_STDCOMPAT_CONSTEVAL_SUPPORT

#endif  // __cpp_lib_is_constant_evaluated >= 201811L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

}  // namespace cpp20

namespace cpp23 {

#if defined(__cpp_lib_is_scoped_enum) && __cpp_lib_is_scoped_enum >= 202011L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::is_scoped_enum;
using std::is_scoped_enum_v;

#else  // Provide polyfill for std::is_scoped_enum{,_v}

template <typename T, typename = void>
struct is_scoped_enum : std::false_type {};

template <typename T>
struct is_scoped_enum<T, std::enable_if_t<cpp17::is_enum_v<T>>>
    : cpp17::bool_constant<!cpp17::is_convertible_v<T, std::underlying_type_t<T>>> {};

template <typename T>
static constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;

#endif  // __cpp_lib_is_scoped_enum >= 202011L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

}  // namespace cpp23

#endif  // LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_TYPE_TRAITS_H_
