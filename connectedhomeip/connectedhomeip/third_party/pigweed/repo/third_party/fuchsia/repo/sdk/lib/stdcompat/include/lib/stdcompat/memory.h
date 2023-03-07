// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_MEMORY_H_
#define LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_MEMORY_H_

#include <memory>

#include "version.h"

namespace cpp17 {

#if defined(__cpp_lib_addressof_constexpr) && __cpp_lib_addressof_constexpr >= 201603L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::addressof;

#else  // Provide constexpr polyfill for addressof.

template <typename T>
constexpr T* addressof(T& arg) noexcept {
  return __builtin_addressof(arg);
}

template <typename T>
const T* addressof(const T&&) = delete;

#endif  // __cpp_lib_addressof_constexpr >= 201603L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

}  // namespace cpp17

namespace cpp20 {

#if defined(__cpp_lib_to_address) && __cpp_lib_to_address >= 201711L && \
    !defined(LIB_STDCOMPAT_USE_POLYFILLS)

using std::to_address;

#else  // Provide to_address polyfill.

template <typename T>
constexpr T* to_address(T* pointer) noexcept {
  static_assert(!std::is_function<T>::value, "Cannot pass function pointers to std::to_address()");
  return pointer;
}

// TODO(fxbug.dev/70523): This std::pointer_traits stuff is only to be bug-compatible with the
// standard library implementations; switch back to auto when the linked bug is resolved.
template <typename T>
constexpr typename std::pointer_traits<T>::element_type* to_address(const T& pointer) noexcept {
  static_assert(
      std::is_same<decltype(pointer.operator->()),
                   typename std::pointer_traits<T>::element_type*>::value,
      "For compatibility with libc++ and libstdc++, operator->() must return "
      "typename std::pointer_traits<T>::element_type*. 'Chaining' operator->() in "
      "cpp20::to_address() will not be permitted until https://fxbug.dev/70523 is resolved.");

  return to_address(pointer.operator->());
}

#endif  // __cpp_lib_to_address >= 201711L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

}  // namespace cpp20

#endif  // LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_MEMORY_H_
