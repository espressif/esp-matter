// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_VERSION_H_
#define LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_VERSION_H_

// This <version> polyfills is meant to provide the feature testing macros for the rest of
// the stdcompat library. It is not meant to be a full polyfill of <version>.

#if __has_include(<version>) && !defined(LIB_STDCOMPAT_USE_POLYFILLS)
#include <version>
#elif __cplusplus > 201703L && !defined(LIB_STDCOMPAT_USE_POLYFILLS)
#error "cpp=std20 must provide a '<version>' header."
#else

#if __has_include(<optional>) && !defined(__cpp_lib_optional) && __cplusplus >= 201606L
#define __cpp_lib_optional 201606L
#endif

#if __has_include(<string_view>) && !defined(__cpp_lib_string_view) && __cplusplus >= 201606L
#define __cpp_lib_string_view 201606L
#endif

#if __has_include(<variant>) && !defined(__cpp_lib_variant) && __cplusplus >= 201606L
#define __cpp_lib_variant 201606L
#endif

#if !defined(__cpp_lib_void_t) && __cplusplus >= 201411L
#define __cpp_lib_void_t 201411L
#endif

#if !defined(__cpp_lib_logical_traits) && __cplusplus >= 201510L
#define __cpp_lib_logical_traits 201510L
#endif

#if !defined(__cpp_lib_addressof_constexpr) && __cplusplus >= 201603L
#define __cpp_lib_addressof_constexpr 201603L
#endif

#if !defined(__cpp_lib_nonmember_container_access) && __cplusplus >= 201411L
#define __cpp_lib_nonmember_container_access 201411L
#endif

#if !defined(__cpp_lib_byte) && __cplusplus >= 201603L
#define __cpp_lib_byte 201603L
#endif

#if !defined(__cpp_lib_bool_constant) && __cplusplus >= 201505L
#define __cpp_lib_bool_constant 201505L
#endif

#if !defined(__cpp_lib_type_trait_variable_templates) && __cplusplus >= 201510L
#define __cpp_lib_type_trait_variable_templates 201510L
#endif

#if !defined(__cpp_lib_is_aggregate) && __cplusplus >= 201703L
#define __cpp_lib_is_aggregate 201703L
#endif

#if !defined(__cpp_lib_is_invocable) && __cplusplus >= 201703L
#define __cpp_lib_is_invocable 201703L
#endif

#if !defined(__cpp_lib_invoke) && __cplusplus >= 201411L
#define __cpp_lib_invoke 201411L
#endif

#if !defined(__cpp_lib_apply) && __cplusplus >= 201603L
#define __cpp_lib_apply 201603L
#endif

#if !defined(__cpp_lib_as_const) && __cplusplus >= 201510L
#define __cpp_lib_as_const 201510L
#endif

#endif  // __has_include(<version>) && !defined(LIB_STDCOMPAT_USE_POLYFILLS)

#endif  // LIB_STDCOMPAT_INCLUDE_LIB_STDCOMPAT_VERSION_H_
