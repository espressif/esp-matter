// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_RESULT_H_
#define LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_RESULT_H_

#include <lib/fit/internal/compiler.h>
#include <lib/stdcompat/type_traits.h>

#include <cstddef>
#include <new>
#include <tuple>
#include <type_traits>
#include <utility>

namespace fit {

// Forward declarations.
template <typename E>
class error;

template <typename... Ts>
class success;

template <typename E, typename... Ts>
class result;

namespace internal {

// Determines whether T has an operator-> overload and provides a method that
// forwards its argument by reference when T has the overload, or by pointer
// otherwise.
template <typename T, typename = void>
struct arrow_operator {
  static constexpr T* forward(T& value) { return &value; }
  static constexpr const T* forward(const T& value) { return &value; }
};
template <typename T>
struct arrow_operator<T, cpp17::void_t<decltype(std::declval<T>().operator->())>> {
  static constexpr T& forward(T& value) { return value; }
  static constexpr const T& forward(const T& value) { return value; }
};

// Concept helper for constructor, method, and operator overloads.
template <typename... Conditions>
using requires_conditions = std::enable_if_t<cpp17::conjunction_v<Conditions...>, bool>;

// Detects whether the given expression evaluates to an instance of the template T.
template <template <typename...> class T>
struct template_matcher {
  template <typename... Args>
  static constexpr std::true_type match(const T<Args...>&);
  static constexpr std::false_type match(...);
};

template <typename T, template <typename...> class U, typename = bool>
struct is_match : decltype(template_matcher<U>::match(std::declval<T>())) {};

template <typename T, template <typename...> class U>
struct is_match<T, U, requires_conditions<std::is_void<T>>> : std::false_type {};

template <typename T, template <typename...> class U>
static constexpr bool is_match_v = is_match<T, U>::value;

// Predicate indicating whether type T is an instantiation of fit::error.
template <typename T>
struct is_error : is_match<T, ::fit::error>::type {};

template <typename T>
static constexpr bool is_error_v = is_error<T>::value;

// Predicate indicating whether type T is not an instantiation of fit::error.
template <typename T>
struct not_error_type : cpp17::negation<is_error<T>>::type {};

// Predicate indicating whether type T is an instantiation of fit::success.
template <typename T>
struct is_success : is_match<T, ::fit::success>::type {};

template <typename T>
static constexpr bool is_success_v = is_success<T>::value;

// Predicate indicating whether type T is an instantiation of fit::result.
template <typename T>
struct is_result : is_match<T, ::fit::result>::type {};

template <typename T>
static constexpr bool is_result_v = is_result<T>::value;

// Predicate indicating whether type T is not an instantiation of fit::result.
template <typename T>
struct not_result_type : cpp17::negation<is_result<T>>::type {};

// Determines whether T += U is well defined.
template <typename T, typename U, typename = void>
struct has_plus_equals : std::false_type {};
template <typename T, typename U>
struct has_plus_equals<T, U, cpp17::void_t<decltype(std::declval<T>() += std::declval<U>())>>
    : std::true_type {};

// Enable if relational operator is convertible to bool and the optional
// conditions are true.
template <typename Op, typename... Conditions>
using enable_rel_op =
    std::enable_if_t<(cpp17::is_convertible_v<Op, bool> && cpp17::conjunction_v<Conditions...>),
                     bool>;

// Specifies whether a type is trivially or non-trivially destructible.
enum class storage_class_e {
  trivial,
  non_trivial,
};

// Evaluates to storage_class_e::trivial if all of the types in Ts are trivially
// destructible, storage_class_e::non_trivial otherwise.
template <typename... Ts>
static constexpr storage_class_e storage_class_trait =
    cpp17::conjunction_v<std::is_trivially_destructible<Ts>...> ? storage_class_e::trivial
                                                                : storage_class_e::non_trivial;

// Trivial type for the default variant of the union below.
struct empty_type {};

// Type tags to discriminate between empty, error, and value constructors,
// avoiding ambiguity with copy/move constructors.
enum empty_t { empty_v };
enum error_t { error_v };
enum value_t { value_v };

// Union that stores either nothing, an error of type E, or a value of type T.
// This type is specialized for trivially and non-trivially destructible types
// to support multi-register return values for trivial types.
template <typename E, typename T, storage_class_e = storage_class_trait<E, T>>
union error_or_value_type {
  constexpr error_or_value_type() : empty{} {}

  constexpr error_or_value_type(const error_or_value_type&) = default;
  constexpr error_or_value_type& operator=(const error_or_value_type&) = default;
  constexpr error_or_value_type(error_or_value_type&&) = default;
  constexpr error_or_value_type& operator=(error_or_value_type&&) = default;

  template <typename F>
  constexpr error_or_value_type(error_t, F&& error) : error(std::forward<F>(error)) {}

  template <typename U>
  constexpr error_or_value_type(value_t, U&& value) : value(std::forward<U>(value)) {}

  ~error_or_value_type() = default;

  constexpr void destroy(error_t) {}
  constexpr void destroy(value_t) {}

  empty_type empty;
  E error;
  T value;
};
template <typename E, typename T>
union error_or_value_type<E, T, storage_class_e::non_trivial> {
  constexpr error_or_value_type() : empty{} {}

  constexpr error_or_value_type(const error_or_value_type&) = default;
  constexpr error_or_value_type& operator=(const error_or_value_type&) = default;
  constexpr error_or_value_type(error_or_value_type&&) = default;
  constexpr error_or_value_type& operator=(error_or_value_type&&) = default;

  template <typename F>
  constexpr error_or_value_type(error_t, F&& error) : error(std::forward<F>(error)) {}

  template <typename U>
  constexpr error_or_value_type(value_t, U&& value) : value(std::forward<U>(value)) {}

  ~error_or_value_type() {}

  // The caller must manually destroy() if overwriting an existing value.
  constexpr void copy_from(error_t, const E& e) { new (&error) E(e); }
  constexpr void copy_from(value_t, const T& t) { new (&value) T(t); }
  constexpr void move_from(error_t, E&& e) { new (&error) E(std::move(e)); }
  constexpr void move_from(value_t, T&& t) { new (&value) T(std::move(t)); }

  constexpr void destroy(error_t) { error.E::~E(); }
  constexpr void destroy(value_t) { value.T::~T(); }

  empty_type empty;
  E error;
  T value;
};

// Specifies whether the storage is empty, contains an error, or contains a
// a value.
enum class state_e {
  empty,
  has_error,
  has_value,
};

// Storage type is either empty, holds an error, or holds a set of values. This
// type is specialized for trivially and non-trivially destructible types. When
// E and all of the elements of Ts are trivially destructible, this type
// provides a trivial destructor, which is necessary for multi-register return
// value optimization.
template <storage_class_e storage_class, typename E, typename... Ts>
struct storage_type;

template <storage_class_e storage_class, typename E, typename T>
struct storage_type<storage_class, E, T> {
  using value_type = error_or_value_type<E, T>;

  constexpr storage_type() = default;

  constexpr storage_type(const storage_type&) = default;
  constexpr storage_type& operator=(const storage_type&) = default;
  constexpr storage_type(storage_type&&) = default;
  constexpr storage_type& operator=(storage_type&&) = default;

  constexpr void destroy() {}

  constexpr void reset() { state = state_e::empty; }

  ~storage_type() = default;

  explicit constexpr storage_type(empty_t) {}

  template <typename F>
  constexpr storage_type(error_t, F&& error)
      : state{state_e::has_error}, error_or_value{error_v, std::forward<F>(error)} {}

  template <typename U>
  explicit constexpr storage_type(value_t, U&& value)
      : state{state_e::has_value}, error_or_value{value_v, std::forward<U>(value)} {}

  template <storage_class_e other_storage_class, typename F, typename U>
  explicit constexpr storage_type(storage_type<other_storage_class, F, U>&& other)
      : state{other.state},
        error_or_value{other.state == state_e::empty ? value_type{}
                       : other.state == state_e::has_error
                           ? value_type{error_v, std::move(other.error_or_value.error)}
                           : value_type{value_v, std::move(other.error_or_value.value)}} {}

  state_e state{state_e::empty};
  value_type error_or_value;
};
template <typename E, typename T>
struct storage_type<storage_class_e::non_trivial, E, T> {
  using value_type = error_or_value_type<E, T>;

  constexpr storage_type() = default;

  constexpr storage_type(const storage_type& other) { copy_from(other); }
  constexpr storage_type& operator=(const storage_type& other) {
    destroy();
    copy_from(other);
    return *this;
  }

  constexpr storage_type(storage_type&& other) noexcept(
      std::is_nothrow_move_constructible<E>::value&& std::is_nothrow_move_constructible<T>::value) {
    move_from(std::move(other));
  }
  constexpr storage_type& operator=(storage_type&& other) noexcept(
      std::is_nothrow_move_assignable<E>::value&& std::is_nothrow_move_assignable<T>::value) {
    destroy();
    move_from(std::move(other));
    return *this;
  }

  // Copy/move-constructs over this object's value. If there could be a previous value, callers must
  // call destroy() first.
  constexpr void copy_from(const storage_type& other) {
    state = other.state;
    if (state == state_e::has_value) {
      error_or_value.copy_from(value_v, other.error_or_value.value);
    } else if (state == state_e::has_error) {
      error_or_value.copy_from(error_v, other.error_or_value.error);
    }
  }
  constexpr void move_from(storage_type&& other) {
    state = other.state;
    if (state == state_e::has_value) {
      error_or_value.move_from(value_v, std::move(other.error_or_value.value));
    } else if (state == state_e::has_error) {
      error_or_value.move_from(error_v, std::move(other.error_or_value.error));
    }
  }

  constexpr void destroy() {
    if (state == state_e::has_value) {
      error_or_value.destroy(value_v);
    } else if (state == state_e::has_error) {
      error_or_value.destroy(error_v);
    }
  }

  constexpr void reset() {
    destroy();
    state = state_e::empty;
  }

  ~storage_type() { destroy(); }

  explicit constexpr storage_type(empty_t) {}

  template <typename F>
  constexpr storage_type(error_t, F&& error)
      : state{state_e::has_error}, error_or_value{error_v, std::forward<F>(error)} {}

  template <typename U>
  explicit constexpr storage_type(value_t, U&& value)
      : state{state_e::has_value}, error_or_value{value_v, std::forward<U>(value)} {}

  template <storage_class_e other_storage_class, typename F, typename U>
  explicit constexpr storage_type(storage_type<other_storage_class, F, U>&& other)
      : state{other.state},
        error_or_value{other.state == state_e::empty ? value_type{}
                       : other.state == state_e::has_error
                           ? value_type{error_v, std::move(other.error_or_value.error)}
                           : value_type{value_v, std::move(other.error_or_value.value)}} {}

  state_e state{state_e::empty};
  value_type error_or_value;
};

template <storage_class_e storage_class, typename E>
struct storage_type<storage_class, E> {
  using value_type = error_or_value_type<E, empty_type>;

  constexpr storage_type() = default;

  constexpr storage_type(const storage_type&) = default;
  constexpr storage_type& operator=(const storage_type&) = default;
  constexpr storage_type(storage_type&&) = default;
  constexpr storage_type& operator=(storage_type&&) = default;

  constexpr void destroy() {}

  constexpr void reset() { state = state_e::empty; }

  ~storage_type() = default;

  explicit constexpr storage_type(empty_t) {}

  explicit constexpr storage_type(value_t)
      : state{state_e::has_value}, error_or_value{value_v, empty_type{}} {}

  template <typename F>
  constexpr storage_type(error_t, F&& error)
      : state{state_e::has_error}, error_or_value{error_v, std::forward<F>(error)} {}

  template <storage_class_e other_storage_class, typename F>
  explicit constexpr storage_type(storage_type<other_storage_class, F>&& other)
      : state{other.state},
        error_or_value{other.state == state_e::empty ? value_type{}
                       : other.state == state_e::has_error
                           ? value_type{error_v, std::move(other.error_or_value.error)}
                           : value_type{value_v, std::move(other.error_or_value.value)}} {}

  state_e state{state_e::empty};
  value_type error_or_value;
};
template <typename E>
struct storage_type<storage_class_e::non_trivial, E> {
  using value_type = error_or_value_type<E, empty_type>;

  constexpr storage_type() = default;

  constexpr storage_type(const storage_type& other) { copy_from(other); }
  constexpr storage_type& operator=(const storage_type& other) {
    destroy();
    copy_from(other);
    return *this;
  }

  constexpr storage_type(storage_type&& other) noexcept(
      std::is_nothrow_move_constructible<E>::value) {
    move_from(std::move(other));
  }
  constexpr storage_type& operator=(storage_type&& other) noexcept(
      std::is_nothrow_move_assignable<E>::value) {
    destroy();
    move_from(std::move(other));
    return *this;
  }

  // Copy/move-constructs over this object's value. If there could be a previous value, callers must
  // call destroy() first.
  constexpr void copy_from(const storage_type& other) {
    state = other.state;
    if (state == state_e::has_error) {
      error_or_value.copy_from(error_v, other.error_or_value.error);
    }
  }
  constexpr void move_from(storage_type&& other) {
    state = other.state;
    if (state == state_e::has_error) {
      error_or_value.move_from(error_v, std::move(other.error_or_value.error));
    }
  }

  constexpr void destroy() {
    if (state == state_e::has_error) {
      error_or_value.destroy(error_v);
    }
  }

  constexpr void reset() {
    destroy();
    state = state_e::empty;
  }

  ~storage_type() { destroy(); }

  explicit constexpr storage_type(empty_t) {}

  explicit constexpr storage_type(value_t)
      : state{state_e::has_value}, error_or_value{value_v, empty_type{}} {}

  template <typename F>
  constexpr storage_type(error_t, F&& error)
      : state{state_e::has_error}, error_or_value{error_v, std::forward<F>(error)} {}

  template <storage_class_e other_storage_class, typename F>
  explicit constexpr storage_type(storage_type<other_storage_class, F>&& other)
      : state{other.state},
        error_or_value{other.state == state_e::empty ? value_type{}
                       : other.state == state_e::has_error
                           ? value_type{error_v, std::move(other.error_or_value.error)}
                           : value_type{value_v, std::move(other.error_or_value.value)}} {}

  state_e state{state_e::empty};
  value_type error_or_value;
};

// Simplified alias of storage_type.
template <typename E, typename... Ts>
using storage = storage_type<storage_class_trait<E, Ts...>, E, Ts...>;

}  // namespace internal
}  // namespace fit

#endif  // LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_RESULT_H_
