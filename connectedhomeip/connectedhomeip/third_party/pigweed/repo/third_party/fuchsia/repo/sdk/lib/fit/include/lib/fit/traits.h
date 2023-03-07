// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_FIT_INCLUDE_LIB_FIT_TRAITS_H_
#define LIB_FIT_INCLUDE_LIB_FIT_TRAITS_H_

#include <lib/stdcompat/type_traits.h>

#include <tuple>
#include <type_traits>

namespace fit {

// Encapsulates capture of a parameter pack. Typical use is to use instances of this empty struct
// for type dispatch in function template deduction/overload resolution.
//
// Example:
//  template <typename Callable, typename... Args>
//  auto inspect_args(Callable c, parameter_pack<Args...>) {
//      // do something with Args...
//  }
//
//  template <typename Callable>
//  auto inspect_args(Callable c) {
//      return inspect_args(std::move(c), typename callable_traits<Callable>::args{});
//  }
template <typename... T>
struct parameter_pack {
  static constexpr size_t size = sizeof...(T);

  template <size_t i>
  using at = typename std::tuple_element_t<i, std::tuple<T...>>;
};

// |callable_traits| captures elements of interest from function-like types (functions, function
// pointers, and functors, including lambdas). Due to common usage patterns, const and non-const
// functors are treated identically.
//
// Member types:
//  |args|        - a |parameter_pack| that captures the parameter types of the function. See
//                  |parameter_pack| for usage and details.
//  |return_type| - the return type of the function.
//  |type|        - the underlying functor or function pointer type. This member is absent if
//                  |callable_traits| are requested for a raw function signature (as opposed to a
//                  function pointer or functor; e.g. |callable_traits<void()>|).
//  |signature|   - the type of the equivalent function.

template <typename T>
struct callable_traits : public callable_traits<decltype(&T::operator())> {};

// Treat mutable call operators the same as const call operators.
//
// It would be equivalent to erase the const instead, but the common case is lambdas, which are
// const, so prefer to nest less deeply for the common const case.
template <typename FunctorType, typename ReturnType, typename... ArgTypes>
struct callable_traits<ReturnType (FunctorType::*)(ArgTypes...)>
    : public callable_traits<ReturnType (FunctorType::*)(ArgTypes...) const> {};

// Common functor specialization.
template <typename FunctorType, typename ReturnType, typename... ArgTypes>
struct callable_traits<ReturnType (FunctorType::*)(ArgTypes...) const>
    : public callable_traits<ReturnType (*)(ArgTypes...)> {
  using type = FunctorType;
};

// Function pointer specialization.
template <typename ReturnType, typename... ArgTypes>
struct callable_traits<ReturnType (*)(ArgTypes...)>
    : public callable_traits<ReturnType(ArgTypes...)> {
  using type = ReturnType (*)(ArgTypes...);
};

// Base specialization.
template <typename ReturnType, typename... ArgTypes>
struct callable_traits<ReturnType(ArgTypes...)> {
  using signature = ReturnType(ArgTypes...);
  using return_type = ReturnType;
  using args = parameter_pack<ArgTypes...>;

  callable_traits() = delete;
};

// Determines whether a type has an operator() that can be invoked.
template <typename T, typename = cpp17::void_t<>>
struct is_callable : public std::false_type {};
template <typename ReturnType, typename... ArgTypes>
struct is_callable<ReturnType (*)(ArgTypes...)> : public std::true_type {};
template <typename FunctorType, typename ReturnType, typename... ArgTypes>
struct is_callable<ReturnType (FunctorType::*)(ArgTypes...)> : public std::true_type {};
template <typename T>
struct is_callable<T, cpp17::void_t<decltype(&T::operator())>> : public std::true_type {};

namespace internal {

template <typename Default, typename AlwaysVoid, template <typename...> class Op, typename... Args>
struct detector {
  using value_t = std::false_type;
  using type = Default;
};

template <typename Default, template <typename...> class Op, typename... Args>
struct detector<Default, cpp17::void_t<Op<Args...>>, Op, Args...> {
  using value_t = std::true_type;
  using type = Op<Args...>;
};

}  // namespace internal

// Default type when detection fails; |void| could be a legitimate result so we need something else.
struct nonesuch {
  constexpr nonesuch() = delete;
  constexpr nonesuch(const nonesuch&) = delete;
  constexpr nonesuch& operator=(const nonesuch&) = delete;
  constexpr nonesuch(nonesuch&&) = delete;
  constexpr nonesuch& operator=(nonesuch&&) = delete;

  // This ensures that no one can actually make a value of this type.
  ~nonesuch() = delete;
};

// Trait for detecting if |Op<Args...>| resolves to a legitimate type. Without this trait,
// metaprogrammers often resort to making their own detectors with |void_t<>|.
//
// With this trait, they can simply make the core part of the detector, like this to detect an
// |operator==|:
//
//     template <typename T>
//     using equality_t = decltype(std::declval<const T&>() == std::declval<const T&>());
//
// And then make their detector like so:
//
//     template <typename T>
//     using has_equality = fit::is_detected<equality_t, T>;
template <template <typename...> class Op, typename... Args>
using is_detected = typename ::fit::internal::detector<nonesuch, void, Op, Args...>::value_t;

template <template <typename...> class Op, typename... Args>
constexpr bool is_detected_v = is_detected<Op, Args...>::value;

// Trait for accessing the result of |Op<Args...>| if it exists, or |fit::nonesuch| otherwise.
//
// This is advantageous because the same "core" of the detector can be used both for finding if the
// prospective type exists at all (with |fit::is_detected|) and what it actually is.
template <template <typename...> class Op, typename... Args>
using detected_t = typename ::fit::internal::detector<nonesuch, void, Op, Args...>::type;

// Trait for detecting whether |Op<Args...>| exists (via |::value_t|, which is either
// |std::true_type| or |std::false_type|) and accessing its type via |::type| if so, which will
// otherwise be |Default|.
template <typename Default, template <typename...> class Op, typename... Args>
using detected_or = typename ::fit::internal::detector<Default, void, Op, Args...>;

// Essentially the same as |fit::detected_t| but with a user-specified default instead of
// |fit::nonesuch|.
template <typename Default, template <typename...> class Op, typename... Args>
using detected_or_t = typename detected_or<Default, Op, Args...>::type;

// Trait for detecting whether |Op<Args...>| exists and is exactly the type |Expected|.
//
// To reuse the previous example of |operator==| and |equality_t|:
//
//     template <typename T>
//     using has_equality_exact = fit::is_detected_exact<bool, equality_t, T>;
template <typename Expected, template <typename...> class Op, typename... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

template <typename Expected, template <typename...> class Op, typename... Args>
constexpr bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;

// Essentially the same as |fit::is_detected_exact| but tests for convertibility instead of exact
// sameness.
template <typename To, template <typename...> class Op, typename... Args>
using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

template <typename To, template <typename...> class Op, typename... Args>
constexpr bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;

}  // namespace fit

#endif  // LIB_FIT_INCLUDE_LIB_FIT_TRAITS_H_
