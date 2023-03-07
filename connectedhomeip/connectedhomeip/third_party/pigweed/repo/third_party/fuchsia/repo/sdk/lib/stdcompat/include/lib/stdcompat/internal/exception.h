// Copyright 2021 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_EXCEPTION_H_
#define LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_EXCEPTION_H_

#include <exception>

#include "../type_traits.h"

namespace cpp17 {
namespace internal {

// When exceptions are enabled, will generate an exception of the right type, when disabled will
// simply abort execution.
//
// Note: both clang and gcc support gnu::unused, which makes it a portable alternative for
// [[maybe_unused]].
template <typename T,
          typename std::enable_if<std::is_base_of<std::exception, T>::value, bool>::type = true>
[[noreturn]] inline constexpr void throw_or_abort([[gnu::unused]] const char* reason) {
#if defined(__cpp_exceptions) && __cpp_exceptions >= 199711L
  throw T(reason);
#else
  __builtin_abort();
#endif
}

template <typename T>
inline constexpr void throw_or_abort_if_any_impl(const char* reason, bool should_abort) {
  if (should_abort) {
    throw_or_abort<T>(reason);
  }
}

template <typename T, typename... AbortIf>
inline constexpr void throw_or_abort_if_any_impl(const char* reason, bool head, AbortIf... tail) {
  if (head) {
    throw_or_abort<T>(reason);
  }
  throw_or_abort_if_any_impl<T>(reason, tail...);
}

template <typename T, typename... AbortIf>
inline constexpr void throw_or_abort_if_any(const char* reason, AbortIf... abort_if) {
  static_assert(sizeof...(AbortIf) > 0, "Must provide an |abort_if| clause.");
  static_assert(cpp17::conjunction_v<std::is_same<bool, AbortIf>...>,
                "|abort_if| arguments must be boolean.");
  throw_or_abort_if_any_impl<T>(reason, abort_if...);
}

}  // namespace internal
}  // namespace cpp17

#endif  // LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_INTERNAL_EXCEPTION_H_
