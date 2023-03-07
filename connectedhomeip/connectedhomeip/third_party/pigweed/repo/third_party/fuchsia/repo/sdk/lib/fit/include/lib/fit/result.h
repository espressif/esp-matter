// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_FIT_INCLUDE_LIB_FIT_RESULT_H_
#define LIB_FIT_INCLUDE_LIB_FIT_RESULT_H_

#include <lib/fit/internal/compiler.h>
#include <lib/fit/internal/result.h>

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

// General purpose fit::result type for Zircon kernel, system, and above.
//
// fit::result is an efficient C++ implementation of the result pattern found in many languages and
// vocabulary libraries. This implementation supports returning either an error value or zero/one
// non-error values from a function or method.
//
// To make a fit::result:
//
//   fit::success(success_value)  // Success for fit::result<E, V>.
//   fit::success()               // Success for fit::result<E> (no success value).
//   fit::ok(success_value)       // Success for fit::result<E, V>.
//   fit::ok()                    // Success for fit::result<E> (no success value).
//
//   fit::error(error_value)      // Failure.
//   fit::as_error(error_value)   // Failure.
//   fit::failed()                // Failure for fit::result<>.
//
// General functions that can always be called:
//
//   bool is_ok()
//   bool is_error()
//   T value_or(default_value)   // Returns value on success, or default on failure.
//
// Available only when is_ok() (will assert otherwise).
//
//   T& value()                  // Accesses the value.
//   T&& value()                 // Moves the value.
//   T& operator*()              // Accesses the value.
//   T&& operator*()             // Moves the value.
//   T* operator->()             // Accesses the value.
//   success<T> take_value()     // Generates a fit::success() which can be implicitly converted to
//                               // another fit::result with the same "success" type.
//
// Available only when is_error() (will assert otherwise):
//
//   E& error_value()            // Error value.
//   E&& error_value()           // Error value.
//   error<E> take_error()       // Generates a fit::error() which can be implicitly converted to a
//                               // fit::result with a different "success" vluae type (or
//                               // fit::result<E>).

#include "pw_assert/assert.h"

namespace fit {

// Convenience type to indicate failure without elaboration.
//
// Example:
//
//   fit::result<fit::failed> Contains(const char* string, const char* find) {
//     if (string == nullptr || find == nullptr ||
//         strstr(string, find) == nullptr) {
//       return fit::failed();
//     }
//     return fit::ok();
//   }
//
struct failed {};

// Type representing an error value of type E to return as a result. Returning an error through
// fit::result always requires using fit::error to disambiguate errors from values.
//
// fit::result<E, Ts...> is implicitly constructible from any fit::error<F>, where E is
// constructible from F. This simplifies returning errors when the E has converting constructors.
//
// Example usage:
//
//   fit::result<std::string, size_t> StringLength(const char* string) {
//     if (string == nullptr) {
//       return fit::error("Argument to StringLength is nullptr!");
//     }
//     return fit::success(strlen(string));
//   }
//
template <typename E>
class error {
 public:
  using error_type = E;

  // Constructs an error with the given arguments.
  template <typename... Args,
            ::fit::internal::requires_conditions<std::is_constructible<E, Args...>> = true>
  explicit constexpr error(Args&&... args) : value_(std::forward<Args>(args)...) {}

  ~error() = default;

  // Error has the same copyability and moveability as the underlying type E.
  constexpr error(const error&) = default;
  constexpr error& operator=(const error&) = default;
  constexpr error(error&&) = default;
  constexpr error& operator=(error&&) = default;

 private:
  template <typename F, typename... Ts>
  friend class result;

  E value_;
};

#if __cplusplus >= 201703L

// Deduction guide to simplify single argument error expressions in C++17.
template <typename T>
error(T) -> error<T>;

#endif

// Returns fit::error<E> for the given value, where E is deduced from the argument type. This
// utility is a C++14 compatible alternative to the C++17 deduction guide above.
//
// Example:
//
//   fit::result<std::string, std::string> MakeString(const char* string) {
//     if (string == nullptr) {
//       return fit::as_error("String is nullptr!");
//     } else if (strlen(string) == 0) {
//       return fit::as_error("String is empty!");
//     }
//     return fit::ok(string);
//   }
//
template <typename E>
constexpr error<std::decay_t<E>> as_error(E&& error_value) {
  return error<std::decay_t<E>>(std::forward<E>(error_value));
}

// Type representing success with zero or one value.
//
// Base type.
template <typename... Ts>
class success;

// Type representing a success value of type T to return as a result. Returning a value through
// fit::result always requires using fit::success to disambiguate errors from values.
//
// fit::result<E, T> is implicitly constructible from any fit::success<U>, where T is
// constructible from U. This simplifies returning values when T has converting constructors.
template <typename T>
class success<T> {
 public:
  using value_type = T;

  // Constructs a success value with the given arguments.
  template <typename... Args,
            ::fit::internal::requires_conditions<std::is_constructible<T, Args...>> = true>
  explicit constexpr success(Args&&... args) : value_(std::forward<Args>(args)...) {}

  ~success() = default;

  // Error has the same copyability and moveability as the underlying type E.
  constexpr success(const success&) = default;
  constexpr success& operator=(const success&) = default;
  constexpr success(success&&) = default;
  constexpr success& operator=(success&&) = default;

 private:
  template <typename E, typename... Ts>
  friend class result;

  T value_;
};

// Specialization of success for empty values.
template <>
class success<> {
 public:
  constexpr success() = default;
  ~success() = default;

  constexpr success(const success&) = default;
  constexpr success& operator=(const success&) = default;
  constexpr success(success&&) = default;
  constexpr success& operator=(success&&) = default;
};

#if __cplusplus >= 201703L

// Deduction guides to simplify zero and single argument success expressions in C++17.
success()->success<>;

template <typename T>
success(T) -> success<T>;

#endif

// Returns fit::success<T> for the given value, where T is deduced from the argument type. This
// utility is a C++14 compatible alternative to the C++17 deduction guide above.
//
// Example:
//
//   fit::result<std::string, std::string> MakeString(const char* string) {
//     if (string == nullptr) {
//       return fit::as_error("String is nullptr!");
//     } else if (strlen(string) == 0) {
//       return fit::as_error("String is empty!");
//     }
//     return fit::ok(string);
//   }
template <typename T>
constexpr success<std::decay_t<T>> ok(T&& value) {
  return success<std::decay_t<T>>(std::forward<T>(value));
}

// Overload for empty value success.
constexpr success<> ok() { return success<>{}; }

// Result type representing either an error or zero/one return values.
//
// Base type.
template <typename E, typename... Ts>
class result;

// Specialization of result for one value type.
template <typename E, typename T>
class LIB_FIT_NODISCARD result<E, T> {
  static_assert(!::fit::internal::is_success_v<E>,
                "fit::success may not be used as the error type of fit::result!");
  static_assert(!cpp17::is_same_v<failed, std::decay_t<T>>,
                "fit::failed may not be used as a value type of fit::result!");

  template <typename U>
  using not_same = cpp17::negation<std::is_same<result, U>>;

  struct none {};
  using failed_or_none = std::conditional_t<cpp17::is_same_v<failed, E>, failed, none>;

 public:
  using error_type = E;
  using value_type = T;

  constexpr result(const result&) = default;
  constexpr result& operator=(const result&) = default;
  constexpr result(result&&) = default;
  constexpr result& operator=(result&&) = default;

  // Implicit conversion from fit::failed. This overload is only enabled when the error type E is
  // fit::failed.
  constexpr result(failed_or_none) : storage_{::fit::internal::error_v, failed{}} {}

  // Implicit conversion from success<U>, where T is constructible from U.
  template <typename U, ::fit::internal::requires_conditions<std::is_constructible<T, U>> = true>
  constexpr result(success<U> success)
      : storage_{::fit::internal::value_v, std::move(success.value_)} {}

  // Implicit conversion from error<F>, where E is constructible from F.
  template <typename F, ::fit::internal::requires_conditions<std::is_constructible<E, F>> = true>
  constexpr result(error<F> error) : storage_{::fit::internal::error_v, std::move(error.value_)} {}

  // Implicitly constructs a result from another result with compatible types.
  template <
      typename F, typename U,
      ::fit::internal::requires_conditions<not_same<result<F, U>>, std::is_constructible<E, F>,
                                           std::is_constructible<T, U>> = true>
  constexpr result(result<F, U> other) : storage_{std::move(other.storage_)} {}

  // Predicates indicating whether the result contains a value or an error. The positive values are
  // mutually exclusive, however, both predicates are negative when the result is default
  // constructed to the empty state.
  constexpr bool is_ok() const { return storage_.state == ::fit::internal::state_e::has_value; }
  constexpr bool is_error() const { return storage_.state == ::fit::internal::state_e::has_error; }

  // Accessors for the underlying error.
  //
  // May only be called when the result contains an error.
  constexpr E& error_value() & {
    if (is_error()) {
      return storage_.error_or_value.error;
    }
    PW_ASSERT(false);
  }
  constexpr const E& error_value() const& {
    if (is_error()) {
      return storage_.error_or_value.error;
    }
    PW_ASSERT(false);
  }
  constexpr E&& error_value() && {
    if (is_error()) {
      return std::move(storage_.error_or_value.error);
    }
    PW_ASSERT(false);
  }
  constexpr const E&& error_value() const&& {
    if (is_error()) {
      return std::move(storage_.error_or_value.error);
    }
    PW_ASSERT(false);
  }

  // Moves the underlying error and returns it as an instance of fit::error, simplifying
  // propagating the error to another fit::result.
  //
  // May only be called when the result contains an error.
  constexpr error<E> take_error() {
    if (is_error()) {
      return error<E>(std::move(storage_.error_or_value.error));
    }
    PW_ASSERT(false);
  }

  // Accessors for the underlying value.
  //
  // May only be called when the result contains a value.
  constexpr T& value() & {
    if (is_ok()) {
      return storage_.error_or_value.value;
    }
    PW_ASSERT(false);
  }
  constexpr const T& value() const& {
    if (is_ok()) {
      return storage_.error_or_value.value;
    }
    PW_ASSERT(false);
  }
  constexpr T&& value() && {
    if (is_ok()) {
      return std::move(storage_.error_or_value.value);
    }
    PW_ASSERT(false);
  }
  constexpr const T&& value() const&& {
    if (is_ok()) {
      return std::move(storage_.error_or_value.value);
    }
    PW_ASSERT(false);
  }

  // Moves the underlying value and returns it as an instance of fit::success, simplifying
  // propagating the value to another fit::result.
  //
  // May only be called when the result contains a value.
  constexpr success<T> take_value() {
    if (is_ok()) {
      return success<T>(std::move(storage_.error_or_value.value));
    }
    PW_ASSERT(false);
  }

  // Contingent accessors for the underlying value.
  //
  // Returns the value when the result has a value, otherwise returns the given default value.
  template <typename U, ::fit::internal::requires_conditions<std::is_constructible<T, U>> = true>
  constexpr T value_or(U&& default_value) const& {
    if (is_ok()) {
      return storage_.error_or_value.value;
    }
    return static_cast<T>(std::forward<U>(default_value));
  }
  template <typename U, ::fit::internal::requires_conditions<std::is_constructible<T, U>> = true>
  constexpr T value_or(U&& default_value) && {
    if (is_ok()) {
      return std::move(storage_.error_or_value.value);
    }
    return static_cast<T>(std::forward<U>(default_value));
  }

  // Accessors for the members of the underlying value. These operators forward to T::operator->()
  // when defined, otherwise they provide direct access to T*.
  //
  // May only be called when the result contains a value.
  constexpr decltype(auto) operator->() {
    if (is_ok()) {
      return ::fit::internal::arrow_operator<T>::forward(storage_.error_or_value.value);
    }
    PW_ASSERT(false);
  }
  constexpr decltype(auto) operator->() const {
    if (is_ok()) {
      return ::fit::internal::arrow_operator<T>::forward(storage_.error_or_value.value);
    }
    PW_ASSERT(false);
  }

  // Accessors for the underlying value. This is a syntax sugar for value().
  //
  // May only be called when the result contains a value.
  constexpr T& operator*() & { return value(); }
  constexpr const T& operator*() const& { return value(); }
  constexpr T&& operator*() && { return std::move(value()); }
  constexpr const T&& operator*() const&& { return std::move(value()); }

  // Augments the error value of the result with the given error value. The operator
  // E::operator+=(F) must be defined. Additionally, E may not be a pointer, primitive, or enum
  // type.
  //
  // May only be called when the result contains an error.
  template <typename F, ::fit::internal::requires_conditions<
                            std::is_class<E>, ::fit::internal::has_plus_equals<E, F>> = true>
  constexpr result& operator+=(error<F> error) {
    if (is_error()) {
      storage_.error_or_value.error += std::move(error.value_);
      return *this;
    }
    PW_ASSERT(false);
  }

  constexpr void swap(result& other) {
    if (&other != this) {
      using std::swap;
      swap(storage_, other.storage_);
    }
  }

 protected:
  // Default constructs a result in empty state.
  constexpr result() = default;

  // Reset is not a recommended operation for the general result pattern. This method is provided
  // for derived types that need it for specific use cases.
  constexpr void reset() { storage_.reset(); }

 private:
  template <typename, typename...>
  friend class result;

  ::fit::internal::storage<E, T> storage_;
};

// Specialization of the result type for zero values.
template <typename E>
class LIB_FIT_NODISCARD result<E> {
  static_assert(!::fit::internal::is_success_v<E>,
                "fit::success may not be used as the error type of fit::result!");

  template <typename U>
  using not_same = cpp17::negation<std::is_same<result, U>>;

  template <size_t>
  struct none {};
  using failure_or_none = std::conditional_t<cpp17::is_same_v<failed, E>, failed, none<1>>;

 public:
  using error_type = E;

  constexpr result(const result&) = default;
  constexpr result& operator=(const result&) = default;
  constexpr result(result&&) = default;
  constexpr result& operator=(result&&) = default;

  // Implicit conversion from fit::failure. This overload is only enabled when the error type E is
  // fit::failed.
  constexpr result(failure_or_none) : storage_{::fit::internal::error_v, failed{}} {}

  // Implicit conversion from fit::success<>.
  constexpr result(success<>) : storage_{::fit::internal::value_v} {}

  // Implicit conversion from error<F>, where E is constructible from F.
  template <typename F, ::fit::internal::requires_conditions<std::is_constructible<E, F>> = true>
  constexpr result(error<F> error) : storage_{::fit::internal::error_v, std::move(error.value_)} {}

  // Implicitly constructs a result from another result with compatible types.
  template <typename F, ::fit::internal::requires_conditions<not_same<result<F>>,
                                                             std::is_constructible<E, F>> = true>
  constexpr result(result<F> other) : storage_{std::move(other.storage_)} {}

  // Predicates indicating whether the result contains a value or an error.
  constexpr bool is_ok() const { return storage_.state == ::fit::internal::state_e::has_value; }
  constexpr bool is_error() const { return storage_.state == ::fit::internal::state_e::has_error; }

  // Accessors for the underlying error.
  //
  // May only be called when the result contains an error.
  constexpr E& error_value() & {
    if (is_error()) {
      return storage_.error_or_value.error;
    }
    PW_ASSERT(false);
  }
  constexpr const E& error_value() const& {
    if (is_error()) {
      return storage_.error_or_value.error;
    }
    PW_ASSERT(false);
  }
  constexpr E&& error_value() && {
    if (is_error()) {
      return std::move(storage_.error_or_value.error);
    }
    PW_ASSERT(false);
  }
  constexpr const E&& error_value() const&& {
    if (is_error()) {
      return std::move(storage_.error_or_value.error);
    }
    PW_ASSERT(false);
  }

  // Moves the underlying error and returns it as an instance of fit::error, simplifying
  // propagating the error to another fit::result.
  //
  // May only be called when the result contains an error.
  constexpr error<E> take_error() {
    if (is_error()) {
      return error<E>(std::move(storage_.error_or_value.error));
    }
    PW_ASSERT(false);
  }

  // Augments the error value of the result with the given value. The operator E::operator+=(F) must
  // be defined. Additionally, E may not be a pointer, primitive, or enum type.
  //
  // May only be called when the result contains an error.
  template <typename F, ::fit::internal::requires_conditions<
                            std::is_class<E>, ::fit::internal::has_plus_equals<E, F>> = true>
  constexpr result& operator+=(error<F> error) {
    if (is_error()) {
      storage_.error_or_value.error += std::move(error.value_);
      return *this;
    }
    PW_ASSERT(false);
  }

  constexpr void swap(result& other) {
    if (&other != this) {
      using std::swap;
      swap(storage_, other.storage_);
    }
  }

 protected:
  // Default constructs a result in empty state.
  constexpr result() = default;

  // Reset is not a recommended operation for the general result pattern. This method is provided
  // for derived types that need it for specific use cases.
  constexpr void reset() { storage_.reset(); }

 private:
  template <typename, typename...>
  friend class result;

  ::fit::internal::storage<E> storage_;
};

template <typename E, typename... Ts>
constexpr void swap(result<E, Ts...>& r, result<E, Ts...>& s) {
  return r.swap(s);
}

// Relational Operators.
//
// Results are comparable to the follownig types:
//  * Other results with the same arity when the value types are comparable.
//  * Any type that is comparable to the value type when the arity is 1.
//  * Any instance of fit::success<> (i.e. fit::ok()).
//  * Any instance of fit::failed.
//
// Result comparisons behave similarly to std::optional<T>, having the same empty and non-empty
// lexicographic ordering. A non-value result behaves like an empty std::optional, regardless of the
// value of the actual error. Error values are never compared, only the is_ok() predicate and result
// values are considered in comparisons.

// Equal/not equal to fit::success.
template <typename E, typename... Ts>
constexpr bool operator==(const result<E, Ts...>& lhs, const success<>&) {
  return lhs.is_ok();
}
template <typename E, typename... Ts>
constexpr bool operator!=(const result<E, Ts...>& lhs, const success<>&) {
  return !lhs.is_ok();
}

template <typename E, typename... Ts>
constexpr bool operator==(const success<>&, const result<E, Ts...>& rhs) {
  return rhs.is_ok();
}
template <typename E, typename... Ts>
constexpr bool operator!=(const success<>&, const result<E, Ts...>& rhs) {
  return !rhs.is_ok();
}

// Equal/not equal to fit::failed.
template <typename E, typename... Ts>
constexpr bool operator==(const result<E, Ts...>& lhs, failed) {
  return lhs.is_error();
}
template <typename E, typename... Ts>
constexpr bool operator!=(const result<E, Ts...>& lhs, failed) {
  return !lhs.is_error();
}

template <typename E, typename... Ts>
constexpr bool operator==(failed, const result<E, Ts...>& rhs) {
  return rhs.is_error();
}
template <typename E, typename... Ts>
constexpr bool operator!=(failed, const result<E, Ts...>& rhs) {
  return !rhs.is_error();
}

// Equal/not equal.
template <typename E, typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() == std::declval<U>())> = true>
constexpr bool operator==(const result<E, T>& lhs, const result<F, U>& rhs) {
  return (lhs.is_ok() == rhs.is_ok()) && (!lhs.is_ok() || lhs.value() == rhs.value());
}
template <typename E, typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() != std::declval<U>())> = true>
constexpr bool operator!=(const result<E, T>& lhs, const result<F, U>& rhs) {
  return (lhs.is_ok() != rhs.is_ok()) || (lhs.is_ok() && lhs.value() != rhs.value());
}

template <typename E, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() == std::declval<U>()),
                                         ::fit::internal::not_result_type<U>> = true>
constexpr bool operator==(const result<E, T>& lhs, const U& rhs) {
  return lhs.is_ok() && lhs.value() == rhs;
}
template <typename E, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() != std::declval<U>()),
                                         ::fit::internal::not_result_type<U>> = true>
constexpr bool operator!=(const result<E, T>& lhs, const U& rhs) {
  return !lhs.is_ok() || lhs.value() != rhs;
}

template <typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() == std::declval<U>()),
                                         ::fit::internal::not_result_type<T>> = true>
constexpr bool operator==(const T& lhs, const result<F, U>& rhs) {
  return rhs.is_ok() && lhs == rhs.value();
}
template <typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() != std::declval<U>()),
                                         ::fit::internal::not_result_type<T>> = true>
constexpr bool operator!=(const T& lhs, const result<F, U>& rhs) {
  return !rhs.is_ok() || lhs != rhs.value();
}

// Less than/greater than.
template <typename E, typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() < std::declval<U>())> = true>
constexpr bool operator<(const result<E, T>& lhs, const result<F, U>& rhs) {
  return rhs.is_ok() && (!lhs.is_ok() || lhs.value() < rhs.value());
}
template <typename E, typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() > std::declval<U>())> = true>
constexpr bool operator>(const result<E, T>& lhs, const result<F, U>& rhs) {
  return lhs.is_ok() && (!rhs.is_ok() || lhs.value() > rhs.value());
}

template <typename E, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() < std::declval<U>()),
                                         ::fit::internal::not_result_type<U>> = true>
constexpr bool operator<(const result<E, T>& lhs, const U& rhs) {
  return !lhs.is_ok() || lhs.value() < rhs;
}
template <typename E, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() > std::declval<U>()),
                                         ::fit::internal::not_result_type<U>> = true>
constexpr bool operator>(const result<E, T>& lhs, const U& rhs) {
  return lhs.is_ok() && lhs.value() > rhs;
}

template <typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() < std::declval<U>()),
                                         ::fit::internal::not_result_type<T>> = true>
constexpr bool operator<(const T& lhs, const result<F, U>& rhs) {
  return rhs.is_ok() && lhs < rhs.value();
}
template <typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() > std::declval<U>()),
                                         ::fit::internal::not_result_type<T>> = true>
constexpr bool operator>(const T& lhs, const result<F, U>& rhs) {
  return !rhs.is_ok() || lhs > rhs.value();
}

// Less than or equal/greater than or equal.
template <typename E, typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() <= std::declval<U>())> = true>
constexpr bool operator<=(const result<E, T>& lhs, const result<F, U>& rhs) {
  return !lhs.is_ok() || (rhs.is_ok() && lhs.value() <= rhs.value());
}
template <typename E, typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() >= std::declval<U>())> = true>
constexpr bool operator>=(const result<E, T>& lhs, const result<F, U>& rhs) {
  return !rhs.is_ok() || (lhs.is_ok() && lhs.value() >= rhs.value());
}

template <typename E, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() <= std::declval<U>()),
                                         ::fit::internal::not_result_type<U>> = true>
constexpr bool operator<=(const result<E, T>& lhs, const U& rhs) {
  return !lhs.is_ok() || lhs.value() <= rhs;
}
template <typename E, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() >= std::declval<U>()),
                                         ::fit::internal::not_result_type<U>> = true>
constexpr bool operator>=(const result<E, T>& lhs, const U& rhs) {
  return lhs.is_ok() && lhs.value() >= rhs;
}

template <typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() <= std::declval<U>()),
                                         ::fit::internal::not_result_type<T>> = true>
constexpr bool operator<=(const T& lhs, const result<F, U>& rhs) {
  return rhs.is_ok() && lhs <= rhs.value();
}
template <typename F, typename T, typename U,
          ::fit::internal::enable_rel_op<decltype(std::declval<T>() >= std::declval<U>()),
                                         ::fit::internal::not_result_type<T>> = true>
constexpr bool operator>=(const T& lhs, const result<F, U>& rhs) {
  return !rhs.is_ok() || lhs >= rhs.value();
}

}  // namespace fit

#endif  // LIB_FIT_INCLUDE_LIB_FIT_RESULT_H_
