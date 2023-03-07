// Copyright 2022 The Pigweed Authors
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
#include <utility>

#include "pw_assert/assert.h"
#include "pw_status/status.h"

namespace pw {

template <typename T>
class [[nodiscard]] Result;

namespace internal_result {

// Detects whether `U` has conversion operator to `Result<T>`, i.e. `operator
// Result<T>()`.
template <typename T, typename U, typename = void>
struct HasConversionOperatorToResult : std::false_type {};

template <typename T, typename U>
void test(char (*)[sizeof(std::declval<U>().operator Result<T>())]);

template <typename T, typename U>
struct HasConversionOperatorToResult<T, U, decltype(test<T, U>(0))>
    : std::true_type {};

// Detects whether `T` is constructible or convertible from `Result<U>`.
template <typename T, typename U>
using IsConstructibleOrConvertibleFromResult =
    std::disjunction<std::is_constructible<T, Result<U>&>,
                     std::is_constructible<T, const Result<U>&>,
                     std::is_constructible<T, Result<U>&&>,
                     std::is_constructible<T, const Result<U>&&>,
                     std::is_convertible<Result<U>&, T>,
                     std::is_convertible<const Result<U>&, T>,
                     std::is_convertible<Result<U>&&, T>,
                     std::is_convertible<const Result<U>&&, T>>;

// Detects whether `T` is constructible or convertible or assignable from
// `Result<U>`.
template <typename T, typename U>
using IsConstructibleOrConvertibleOrAssignableFromResult =
    std::disjunction<IsConstructibleOrConvertibleFromResult<T, U>,
                     std::is_assignable<T&, Result<U>&>,
                     std::is_assignable<T&, const Result<U>&>,
                     std::is_assignable<T&, Result<U>&&>,
                     std::is_assignable<T&, const Result<U>&&>>;

// Detects whether direct initializing `Result<T>` from `U` is ambiguous, i.e.
// when `U` is `Result<V>` and `T` is constructible or convertible from `V`.
template <typename T, typename U>
struct IsDirectInitializationAmbiguous
    : public std::conditional_t<
          std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, U>::value,
          std::false_type,
          IsDirectInitializationAmbiguous<
              T,
              std::remove_cv_t<std::remove_reference_t<U>>>> {};

template <typename T, typename V>
struct IsDirectInitializationAmbiguous<T, Result<V>>
    : public IsConstructibleOrConvertibleFromResult<T, V> {};

// Checks against the constraints of the direction initialization, i.e. when
// `Result<T>::Result(U&&)` should participate in overload resolution.
template <typename T, typename U>
using IsDirectInitializationValid = std::disjunction<
    // Short circuits if T is basically U.
    std::is_same<T, std::remove_cv_t<std::remove_reference_t<U>>>,
    std::negation<std::disjunction<
        std::is_same<Result<T>, std::remove_cv_t<std::remove_reference_t<U>>>,
        std::is_same<Status, std::remove_cv_t<std::remove_reference_t<U>>>,
        std::is_same<std::in_place_t,
                     std::remove_cv_t<std::remove_reference_t<U>>>,
        IsDirectInitializationAmbiguous<T, U>>>>;

// This trait detects whether `Result<T>::operator=(U&&)` is ambiguous, which
// is equivalent to whether all the following conditions are met:
// 1. `U` is `Result<V>`.
// 2. `T` is constructible and assignable from `V`.
// 3. `T` is constructible and assignable from `U` (i.e. `Result<V>`).
// For example, the following code is considered ambiguous:
// (`T` is `bool`, `U` is `Result<bool>`, `V` is `bool`)
//   Result<bool> s1 = true;  // s1.ok() && s1.ValueOrDie() == true
//   Result<bool> s2 = false;  // s2.ok() && s2.ValueOrDie() == false
//   s1 = s2;  // ambiguous, `s1 = s2.ValueOrDie()` or `s1 = bool(s2)`?
template <typename T, typename U>
struct IsForwardingAssignmentAmbiguous
    : public std::conditional_t<
          std::is_same<std::remove_cv_t<std::remove_reference_t<U>>, U>::value,
          std::false_type,
          IsForwardingAssignmentAmbiguous<
              T,
              std::remove_cv_t<std::remove_reference_t<U>>>> {};

template <typename T, typename U>
struct IsForwardingAssignmentAmbiguous<T, Result<U>>
    : public IsConstructibleOrConvertibleOrAssignableFromResult<T, U> {};

// Checks against the constraints of the forwarding assignment, i.e. whether
// `Result<T>::operator(U&&)` should participate in overload resolution.
template <typename T, typename U>
using IsForwardingAssignmentValid = std::disjunction<
    // Short circuits if T is basically U.
    std::is_same<T, std::remove_cv_t<std::remove_reference_t<U>>>,
    std::negation<std::disjunction<
        std::is_same<Result<T>, std::remove_cv_t<std::remove_reference_t<U>>>,
        std::is_same<Status, std::remove_cv_t<std::remove_reference_t<U>>>,
        std::is_same<std::in_place_t,
                     std::remove_cv_t<std::remove_reference_t<U>>>,
        IsForwardingAssignmentAmbiguous<T, U>>>>;

// This trait is for determining if a given type is a Result.
template <typename T>
constexpr bool IsResult = false;
template <typename T>
constexpr bool IsResult<Result<T>> = true;

// This trait determines the return type of a given function without const,
// volatile or reference qualifiers.
template <typename Fn, typename T>
using InvokeResultType =
    std::remove_cv_t<std::remove_reference_t<std::invoke_result_t<Fn, T>>>;

PW_MODIFY_DIAGNOSTICS_PUSH();
PW_MODIFY_DIAGNOSTIC_GCC(ignored, "-Wmaybe-uninitialized");

// Construct an instance of T in `p` through placement new, passing Args... to
// the constructor.
// This abstraction is here mostly for the gcc performance fix.
template <typename T, typename... Args>
void PlacementNew(void* p, Args&&... args) {
  new (p) T(std::forward<Args>(args)...);
}

// Helper base class to hold the data and all operations.
// We move all this to a base class to allow mixing with the appropriate
// TraitsBase specialization.
//
// Pigweed addition: Specialize StatusOrData for trivially destructible types.
// This makes a Result usable in a constexpr statement.
//
// Note: in C++20, this entire file can be greatly simplfied with the requires
// statement.
template <typename T, bool = std::is_trivially_destructible<T>::value>
class StatusOrData;

// Place the implementation of StatusOrData in a macro so it can be shared
// between both specializations.
#define PW_RESULT_STATUS_OR_DATA_IMPL                                          \
  template <typename U, bool>                                                  \
  friend class StatusOrData;                                                   \
                                                                               \
 public:                                                                       \
  StatusOrData() = delete;                                                     \
                                                                               \
  constexpr StatusOrData(const StatusOrData& other)                            \
      : status_(other.status_), unused_() {                                    \
    if (other.ok()) {                                                          \
      MakeValue(other.data_);                                                  \
    }                                                                          \
  }                                                                            \
                                                                               \
  constexpr StatusOrData(StatusOrData&& other) noexcept                        \
      : status_(std::move(other.status_)), unused_() {                         \
    if (other.ok()) {                                                          \
      MakeValue(std::move(other.data_));                                       \
    }                                                                          \
  }                                                                            \
                                                                               \
  template <typename U>                                                        \
  explicit constexpr StatusOrData(const StatusOrData<U>& other) {              \
    if (other.ok()) {                                                          \
      MakeValue(other.data_);                                                  \
      status_ = OkStatus();                                                    \
    } else {                                                                   \
      status_ = other.status_;                                                 \
    }                                                                          \
  }                                                                            \
                                                                               \
  template <typename U>                                                        \
  explicit constexpr StatusOrData(StatusOrData<U>&& other) {                   \
    if (other.ok()) {                                                          \
      MakeValue(std::move(other.data_));                                       \
      status_ = OkStatus();                                                    \
    } else {                                                                   \
      status_ = std::move(other.status_);                                      \
    }                                                                          \
  }                                                                            \
                                                                               \
  template <typename... Args>                                                  \
  explicit constexpr StatusOrData(std::in_place_t, Args&&... args)             \
      : status_(), data_(std::forward<Args>(args)...) {}                       \
                                                                               \
  explicit constexpr StatusOrData(const T& value) : status_(), data_(value) {} \
  explicit constexpr StatusOrData(T&& value)                                   \
      : status_(), data_(std::move(value)) {}                                  \
                                                                               \
  template <typename U,                                                        \
            std::enable_if_t<std::is_constructible<Status, U&&>::value, int> = \
                0>                                                             \
  explicit constexpr StatusOrData(U&& v)                                       \
      : status_(std::forward<U>(v)), unused_() {                               \
    PW_ASSERT(!status_.ok());                                                  \
  }                                                                            \
                                                                               \
  constexpr StatusOrData& operator=(const StatusOrData& other) {               \
    if (this == &other) {                                                      \
      return *this;                                                            \
    }                                                                          \
                                                                               \
    if (other.ok()) {                                                          \
      Assign(other.data_);                                                     \
    } else {                                                                   \
      AssignStatus(other.status_);                                             \
    }                                                                          \
    return *this;                                                              \
  }                                                                            \
                                                                               \
  constexpr StatusOrData& operator=(StatusOrData&& other) {                    \
    if (this == &other) {                                                      \
      return *this;                                                            \
    }                                                                          \
                                                                               \
    if (other.ok()) {                                                          \
      Assign(std::move(other.data_));                                          \
    } else {                                                                   \
      AssignStatus(std::move(other.status_));                                  \
    }                                                                          \
    return *this;                                                              \
  }                                                                            \
                                                                               \
  template <typename U>                                                        \
  constexpr void Assign(U&& value) {                                           \
    if (ok()) {                                                                \
      data_ = std::forward<U>(value);                                          \
    } else {                                                                   \
      MakeValue(std::forward<U>(value));                                       \
      status_ = OkStatus();                                                    \
    }                                                                          \
  }                                                                            \
                                                                               \
  template <typename U>                                                        \
  constexpr void AssignStatus(U&& v) {                                         \
    Clear();                                                                   \
    status_ = static_cast<Status>(std::forward<U>(v));                         \
    PW_ASSERT(!status_.ok());                                                  \
  }                                                                            \
                                                                               \
  constexpr bool ok() const { return status_.ok(); }                           \
                                                                               \
 protected:                                                                    \
  union {                                                                      \
    Status status_;                                                            \
  };                                                                           \
                                                                               \
  struct Empty {};                                                             \
  union {                                                                      \
    Empty unused_;                                                             \
    T data_;                                                                   \
  };                                                                           \
                                                                               \
  constexpr void Clear() {                                                     \
    if (ok()) {                                                                \
      data_.~T();                                                              \
    }                                                                          \
  }                                                                            \
                                                                               \
  template <typename... Arg>                                                   \
  void MakeValue(Arg&&... arg) {                                               \
    internal_result::PlacementNew<T>(&unused_, std::forward<Arg>(arg)...);     \
  }                                                                            \
  static_assert(true, "Macros must be terminated with a semicolon")

template <typename T>
class StatusOrData<T, true> {
  PW_RESULT_STATUS_OR_DATA_IMPL;
};

template <typename T>
class StatusOrData<T, false> {
  PW_RESULT_STATUS_OR_DATA_IMPL;

 public:
  // Add a destructor since T is not trivially destructible.
  ~StatusOrData() {
    if (ok()) {
      data_.~T();
    }
  }
};

#undef PW_RESULT_STATUS_OR_DATA_IMPL

PW_MODIFY_DIAGNOSTICS_POP();

// Helper base classes to allow implicitly deleted constructors and assignment
// operators in `Result`. For example, `CopyCtorBase` will explicitly delete
// the copy constructor when T is not copy constructible and `Result` will
// inherit that behavior implicitly.
template <typename T, bool = std::is_copy_constructible<T>::value>
struct CopyCtorBase {
  CopyCtorBase() = default;
  CopyCtorBase(const CopyCtorBase&) = default;
  CopyCtorBase(CopyCtorBase&&) = default;
  CopyCtorBase& operator=(const CopyCtorBase&) = default;
  CopyCtorBase& operator=(CopyCtorBase&&) = default;
};

template <typename T>
struct CopyCtorBase<T, false> {
  CopyCtorBase() = default;
  CopyCtorBase(const CopyCtorBase&) = delete;
  CopyCtorBase(CopyCtorBase&&) = default;
  CopyCtorBase& operator=(const CopyCtorBase&) = default;
  CopyCtorBase& operator=(CopyCtorBase&&) = default;
};

template <typename T, bool = std::is_move_constructible<T>::value>
struct MoveCtorBase {
  MoveCtorBase() = default;
  MoveCtorBase(const MoveCtorBase&) = default;
  MoveCtorBase(MoveCtorBase&&) = default;
  MoveCtorBase& operator=(const MoveCtorBase&) = default;
  MoveCtorBase& operator=(MoveCtorBase&&) = default;
};

template <typename T>
struct MoveCtorBase<T, false> {
  MoveCtorBase() = default;
  MoveCtorBase(const MoveCtorBase&) = default;
  MoveCtorBase(MoveCtorBase&&) = delete;
  MoveCtorBase& operator=(const MoveCtorBase&) = default;
  MoveCtorBase& operator=(MoveCtorBase&&) = default;
};

template <typename T,
          bool = std::is_copy_constructible<T>::value&&
              std::is_copy_assignable<T>::value>
struct CopyAssignBase {
  CopyAssignBase() = default;
  CopyAssignBase(const CopyAssignBase&) = default;
  CopyAssignBase(CopyAssignBase&&) = default;
  CopyAssignBase& operator=(const CopyAssignBase&) = default;
  CopyAssignBase& operator=(CopyAssignBase&&) = default;
};

template <typename T>
struct CopyAssignBase<T, false> {
  CopyAssignBase() = default;
  CopyAssignBase(const CopyAssignBase&) = default;
  CopyAssignBase(CopyAssignBase&&) = default;
  CopyAssignBase& operator=(const CopyAssignBase&) = delete;
  CopyAssignBase& operator=(CopyAssignBase&&) = default;
};

template <typename T,
          bool = std::is_move_constructible<T>::value&&
              std::is_move_assignable<T>::value>
struct MoveAssignBase {
  MoveAssignBase() = default;
  MoveAssignBase(const MoveAssignBase&) = default;
  MoveAssignBase(MoveAssignBase&&) = default;
  MoveAssignBase& operator=(const MoveAssignBase&) = default;
  MoveAssignBase& operator=(MoveAssignBase&&) = default;
};

template <typename T>
struct MoveAssignBase<T, false> {
  MoveAssignBase() = default;
  MoveAssignBase(const MoveAssignBase&) = default;
  MoveAssignBase(MoveAssignBase&&) = default;
  MoveAssignBase& operator=(const MoveAssignBase&) = default;
  MoveAssignBase& operator=(MoveAssignBase&&) = delete;
};

}  // namespace internal_result
}  // namespace pw
