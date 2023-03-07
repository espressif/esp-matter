// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_OPTIONAL_H_
#define LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_OPTIONAL_H_

#include "utility.h"
#include "version.h"

#if defined(__cpp_lib_optional) && __cpp_lib_optional >= 201606L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#include <optional>

namespace cpp17 {

using std::bad_optional_access;
using std::make_optional;
using std::nullopt;
using std::nullopt_t;
using std::optional;

}  // namespace cpp17

#else  // Provide std::optional and std::nullopt_t polyfill.

#include <cstdlib>
#include <exception>
#include <new>

#include "internal/constructors.h"
#include "internal/exception.h"
#include "internal/storage.h"
#include "internal/utility.h"
#include "type_traits.h"

namespace cpp17 {

// A sentinel value for indicating that it contains no value.
struct nullopt_t {
  explicit constexpr nullopt_t(int) {}
};
static constexpr nullopt_t nullopt{0};

// Exception type to report bad accesses to optional.
class bad_optional_access : public std::exception {
 public:
  bad_optional_access() noexcept {}

  const char* what() const noexcept override { return reason_; }

 private:
  template <typename T>
  friend class optional;

  bad_optional_access(const char* reason) noexcept : reason_{reason} {}

  // String describing the reason for the bad access. Must point to a string
  // with static storage duration.
  const char* reason_;

  template <typename T,
            typename std::enable_if<std::is_base_of<std::exception, T>::value, bool>::type>
  friend constexpr void cpp17::internal::throw_or_abort(const char*);
};

// A reasonably complete implementation of std::optional compatible with C++14.
template <typename T>
class optional : private ::cpp17::internal::modulate_copy_and_move<T> {
 private:
  // Helper types and values for SFINAE and noexcept rules.
  static constexpr bool nothrow_move_constructible = std::is_nothrow_move_constructible<T>::value;

  static constexpr bool nothrow_swappable = std::is_nothrow_move_constructible<T>::value &&
                                            ::cpp17::internal::is_nothrow_swappable<T>::value;

  static constexpr auto trivial_init_v = ::cpp17::internal::trivial_init_v;
  static constexpr auto maybe_init_v = ::cpp17::internal::maybe_init_v;
  using type_tag = ::cpp17::internal::type_tag<T>;

  template <typename U, typename V>
  using converts_from_optional = disjunction<
      std::is_constructible<U, const optional<V>&>, std::is_constructible<U, optional<V>&>,
      std::is_constructible<U, const optional<V>&&>, std::is_constructible<U, optional<V>&&>,
      std::is_convertible<const optional<V>&, U>, std::is_convertible<optional<V>&, U>,
      std::is_convertible<const optional<V>&&, U>, std::is_convertible<optional<V>&&, U>>;

  template <typename U, typename V>
  using assigns_from_optional =
      disjunction<std::is_assignable<U&, const optional<V>&>, std::is_assignable<U&, optional<V>&>,
                  std::is_assignable<U&, const optional<V>&&>,
                  std::is_assignable<U&, optional<V>&&>>;

  template <typename U>
  using not_self_type = ::cpp17::internal::not_same_type<optional, U>;

  template <typename U>
  using not_in_place = ::cpp17::internal::not_same_type<in_place_t, U>;

  template <typename... Conditions>
  using requires_conditions = ::cpp17::internal::requires_conditions<Conditions...>;

  template <typename... Conditions>
  using assignment_requires_conditions =
      ::cpp17::internal::assignment_requires_conditions<optional&, Conditions...>;

  template <typename... Args>
  using emplace_constructible = std::enable_if_t<std::is_constructible<T, Args...>::value, T&>;

 public:
  using value_type = T;

  // Default constructors.

  constexpr optional() = default;

  constexpr optional(nullopt_t) noexcept {}

  // Copy/move constructors and assignment operators.

  constexpr optional(const optional&) = default;
  constexpr optional& operator=(const optional&) = default;

  constexpr optional(optional&&) = default;
  constexpr optional& operator=(optional&&) = default;

  // Converting constructors.

  template <typename U = T,
            requires_conditions<not_self_type<U>, not_in_place<U>, std::is_constructible<T, U&&>,
                                std::is_convertible<U&&, T>> = true>
  constexpr optional(U&& value) : storage_(type_tag{}, std::forward<U>(value)) {}

  template <typename U = T,
            requires_conditions<not_self_type<U>, not_in_place<U>, std::is_constructible<T, U&&>,
                                negation<std::is_convertible<U&&, T>>> = false>
  explicit constexpr optional(U&& value) : storage_{type_tag{}, std::forward<U>(value)} {}

  template <typename U,
            requires_conditions<negation<std::is_same<T, U>>, std::is_constructible<T, const U&>,
                                std::is_convertible<const U&, T>,
                                negation<converts_from_optional<T, U>>> = true>
  constexpr optional(const optional<U>& other) : storage_{maybe_init_v, other.storage_} {}

  template <typename U,
            requires_conditions<negation<std::is_same<T, U>>, std::is_constructible<T, const U&>,
                                negation<std::is_convertible<const U&, T>>,
                                negation<converts_from_optional<T, U>>> = false>
  explicit constexpr optional(const optional<U>& other) : storage_{maybe_init_v, other.storage_} {}

  template <typename U,
            requires_conditions<negation<std::is_same<T, U>>, std::is_constructible<T, U&&>,
                                std::is_convertible<U&&, T>,
                                negation<converts_from_optional<T, U>>> = true>
  constexpr optional(optional<U>&& other) : storage_{maybe_init_v, std::move(other.storage_)} {}

  template <typename U,
            requires_conditions<negation<std::is_same<T, U>>, std::is_constructible<T, U&&>,
                                negation<std::is_convertible<U&&, T>>,
                                negation<converts_from_optional<T, U>>> = false>
  explicit constexpr optional(optional<U>&& other)
      : storage_{maybe_init_v, std::move(other.storage_)} {}

  template <typename... Args, requires_conditions<std::is_constructible<T, Args&&...>> = false>
  explicit constexpr optional(in_place_t, Args&&... args)
      : storage_(type_tag{}, std::forward<Args>(args)...) {}

  template <
      typename U, typename... Args,
      requires_conditions<std::is_constructible<T, std::initializer_list<U>&, Args&&...>> = false>
  explicit constexpr optional(in_place_t, std::initializer_list<U> init_list, Args&&... args)
      : storage_(type_tag{}, init_list, std::forward<Args>(args)...) {}

  // Destructor.

  ~optional() = default;

  // Checked accessors.

  constexpr T& value() & {
    if (has_value()) {
      return storage_.get(type_tag{});
    }
    internal::throw_or_abort<bad_optional_access>("Accessed value of empty optional!");
  }
  constexpr const T& value() const& {
    if (has_value()) {
      return storage_.get(type_tag{});
    }
    internal::throw_or_abort<bad_optional_access>("Accessed value of empty optional!");
  }
  constexpr T&& value() && {
    if (has_value()) {
      return std::move(storage_.get(type_tag{}));
    }
    internal::throw_or_abort<bad_optional_access>("Accessed value of empty optional!");
  }
  constexpr const T&& value() const&& {
    if (has_value()) {
      return std::move(storage_.get(type_tag{}));
    }
    internal::throw_or_abort<bad_optional_access>("Accessed value of empty optional!");
  }

  template <typename U>
  constexpr T value_or(U&& default_value) const& {
    static_assert(std::is_copy_constructible<T>::value,
                  "value_or() requires copy-constructible value_type!");
    static_assert(std::is_convertible<U&&, T>::value,
                  "Default value must be convertible to value_type!");

    return has_value() ? storage_.get(type_tag{}) : static_cast<T>(std::forward<U>(default_value));
  }
  template <typename U>
  constexpr T value_or(U&& default_value) && {
    static_assert(std::is_move_constructible<T>::value,
                  "value_or() requires move-constructible value_type!");
    static_assert(std::is_convertible<U&&, T>::value,
                  "Default value must be convertible to value_type!");

    return has_value() ? std::move(storage_.get(type_tag{}))
                       : static_cast<T>(std::forward<U>(default_value));
  }

  // Unchecked accessors.

  constexpr T* operator->() { return std::addressof(storage_.get(type_tag{})); }
  constexpr const T* operator->() const { return std::addressof(storage_.get(type_tag{})); }

  constexpr T& operator*() { return storage_.get(type_tag{}); }
  constexpr const T& operator*() const { return storage_.get(type_tag{}); }

  // Availability accessors/operators.

  constexpr bool has_value() const { return !storage_.is_empty(); }
  constexpr explicit operator bool() const { return has_value(); }

  // Assignment operators.

  template <typename U>
  constexpr assignment_requires_conditions<
      not_self_type<U>, negation<conjunction<std::is_scalar<T>, std::is_same<T, std::decay_t<U>>>>,
      std::is_constructible<T, U>, std::is_assignable<T&, U>>
  operator=(U&& value) {
    if (has_value()) {
      storage_.get(type_tag{}) = std::forward<U>(value);
    } else {
      storage_.construct(type_tag{}, std::forward<U>(value));
    }
    return *this;
  }

  template <typename U>
  constexpr assignment_requires_conditions<
      negation<std::is_same<T, U>>, std::is_constructible<T, const U&>, std::is_assignable<T&, U>,
      negation<converts_from_optional<T, U>>, negation<assigns_from_optional<T, U>>>
  operator=(const optional<U>& other) {
    storage_.assign(other.storage_);
    return *this;
  }

  template <typename U>
  constexpr assignment_requires_conditions<
      negation<std::is_same<T, U>>, std::is_constructible<T, U>, std::is_assignable<T&, U>,
      negation<converts_from_optional<T, U>>, negation<assigns_from_optional<T, U>>>
  operator=(optional<U>&& other) {
    storage_.assign(std::move(other.storage_));
    return *this;
  }

  constexpr optional& operator=(nullopt_t) {
    storage_.reset();
    return *this;
  }

  // Swap.

  constexpr void swap(optional& other) noexcept(nothrow_swappable) {
    storage_.swap(other.storage_);
  }

  // Emplacement.

  template <typename... Args>
  constexpr emplace_constructible<Args&&...> emplace(Args&&... args) {
    storage_.reset();
    storage_.construct(type_tag{}, std::forward<Args>(args)...);
    return storage_.get(type_tag{});
  }

  template <typename U, typename... Args>
  constexpr emplace_constructible<std::initializer_list<U>&, Args&&...> emplace(
      std::initializer_list<U> init_list, Args&&... args) {
    storage_.reset();
    storage_.construct(type_tag{}, init_list, std::forward<Args>(args)...);
    return storage_.get(type_tag{});
  }

  // Reset.

  void reset() noexcept { storage_.reset(); }

 private:
  ::cpp17::internal::storage_type<T> storage_;
};

// Swap.
template <typename T>
inline std::enable_if_t<(std::is_move_constructible<T>::value &&
                         ::cpp17::internal::is_swappable<T>::value)>
swap(optional<T>& a, optional<T>& b) noexcept(noexcept(a.swap(b))) {
  a.swap(b);
}
template <typename T>
inline std::enable_if_t<(!std::is_move_constructible<T>::value &&
                         ::cpp17::internal::is_swappable<T>::value)>
swap(optional<T>& a, optional<T>& b) = delete;

// Make optional.
template <typename T>
constexpr optional<std::decay_t<T>> make_optional(T&& value) {
  return optional<std::decay_t<T>>{std::forward<T>(value)};
}
template <typename T, typename... Args>
constexpr optional<T> make_optional(Args&&... args) {
  return optional<T>{in_place, std::forward<Args>(args)...};
}
template <typename T, typename U, typename... Args>
constexpr optional<T> make_optional(std::initializer_list<U> init_list, Args&&... args) {
  return optional<T>{in_place, init_list, std::forward<Args>(args)...};
}

// Empty.
template <typename T>
constexpr bool operator==(const optional<T>& lhs, nullopt_t) {
  return !lhs.has_value();
}
template <typename T>
constexpr bool operator!=(const optional<T>& lhs, nullopt_t) {
  return lhs.has_value();
}

template <typename T>
constexpr bool operator==(nullopt_t, const optional<T>& rhs) {
  return !rhs.has_value();
}
template <typename T>
constexpr bool operator!=(nullopt_t, const optional<T>& rhs) {
  return rhs.has_value();
}

// Equal/not equal.
template <
    typename T, typename U,
    ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() == std::declval<U>())> = true>
constexpr bool operator==(const optional<T>& lhs, const optional<U>& rhs) {
  return (lhs.has_value() == rhs.has_value()) && (!lhs.has_value() || *lhs == *rhs);
}
template <
    typename T, typename U,
    ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() != std::declval<U>())> = true>
constexpr bool operator!=(const optional<T>& lhs, const optional<U>& rhs) {
  return (lhs.has_value() != rhs.has_value()) || (lhs.has_value() && *lhs != *rhs);
}

template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() == std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, U>> = true>
constexpr bool operator==(const optional<T>& lhs, const U& rhs) {
  return lhs.has_value() && *lhs == rhs;
}
template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() != std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, U>> = true>
constexpr bool operator!=(const optional<T>& lhs, const U& rhs) {
  return !lhs.has_value() || *lhs != rhs;
}

template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() == std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, T>> = true>
constexpr bool operator==(const T& lhs, const optional<U>& rhs) {
  return rhs.has_value() && lhs == *rhs;
}
template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() != std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, T>> = true>
constexpr bool operator!=(const T& lhs, const optional<U>& rhs) {
  return !rhs.has_value() || lhs != *rhs;
}

// Less than/greater than.
template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() < std::declval<U>())> = true>
constexpr bool operator<(const optional<T>& lhs, const optional<U>& rhs) {
  return rhs.has_value() && (!lhs.has_value() || *lhs < *rhs);
}
template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() > std::declval<U>())> = true>
constexpr bool operator>(const optional<T>& lhs, const optional<U>& rhs) {
  return lhs.has_value() && (!rhs.has_value() || *lhs > *rhs);
}

template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() < std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, U>> = true>
constexpr bool operator<(const optional<T>& lhs, const U& rhs) {
  return !lhs.has_value() || *lhs < rhs;
}
template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() > std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, U>> = true>
constexpr bool operator>(const optional<T>& lhs, const U& rhs) {
  return lhs.has_value() && *lhs > rhs;
}

template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() < std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, T>> = true>
constexpr bool operator<(const T& lhs, const optional<U>& rhs) {
  return rhs.has_value() && lhs < *rhs;
}
template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() > std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, T>> = true>
constexpr bool operator>(const T& lhs, const optional<U>& rhs) {
  return !rhs.has_value() || lhs > *rhs;
}

// Less than or equal/greater than or equal.
template <
    typename T, typename U,
    ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() <= std::declval<U>())> = true>
constexpr bool operator<=(const optional<T>& lhs, const optional<U>& rhs) {
  return !lhs.has_value() || (rhs.has_value() && *lhs <= *rhs);
}
template <
    typename T, typename U,
    ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() >= std::declval<U>())> = true>
constexpr bool operator>=(const optional<T>& lhs, const optional<U>& rhs) {
  return !rhs.has_value() || (lhs.has_value() && *lhs >= *rhs);
}

template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() <= std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, U>> = true>
constexpr bool operator<=(const optional<T>& lhs, const U& rhs) {
  return !lhs.has_value() || *lhs <= rhs;
}
template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() >= std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, U>> = true>
constexpr bool operator>=(const optional<T>& lhs, const U& rhs) {
  return lhs.has_value() && *lhs >= rhs;
}

template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() <= std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, T>> = true>
constexpr bool operator<=(const T& lhs, const optional<U>& rhs) {
  return rhs.has_value() && lhs <= *rhs;
}
template <typename T, typename U,
          ::cpp17::internal::enable_relop_t<decltype(std::declval<T>() >= std::declval<U>()),
                                            ::cpp17::internal::not_same_type<nullopt_t, T>> = true>
constexpr bool operator>=(const T& lhs, const optional<U>& rhs) {
  return !rhs.has_value() || lhs >= *rhs;
}

}  // namespace cpp17

#endif

#endif  // LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_OPTIONAL_H_
