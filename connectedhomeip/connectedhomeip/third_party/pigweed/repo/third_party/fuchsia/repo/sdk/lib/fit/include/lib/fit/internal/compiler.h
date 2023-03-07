// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_COMPILER_H_
#define LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_COMPILER_H_

// Annotate a class or function with C++17's [[nodiscard]] or similar where supported by the
// compiler.
//
// C++14 doesn't support [[nodiscard]], but Clang allows __attribute__((warn_unused_result))
// to be placed on class declarations. GCC only allows the attribute to be used on methods.
#if __cplusplus >= 201703L
#define LIB_FIT_NODISCARD [[nodiscard]]
#elif defined(__clang__)
#define LIB_FIT_NODISCARD __attribute__((__warn_unused_result__))
#else
#define LIB_FIT_NODISCARD /* nothing */
#endif

#endif  // LIB_FIT_INCLUDE_LIB_FIT_INTERNAL_COMPILER_H_
