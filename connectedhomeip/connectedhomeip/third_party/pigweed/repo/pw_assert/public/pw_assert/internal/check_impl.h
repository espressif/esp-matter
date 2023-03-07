// Copyright 2020 The Pigweed Authors
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

#ifdef __cplusplus
#include <type_traits>
#else
#include <stddef.h>
#endif  // __cplusplus

// Note: This file depends on the backend header already being included.

#include "pw_assert/config.h"
#include "pw_preprocessor/compiler.h"

// PW_CRASH - Crash the system, with a message.
#define PW_CRASH PW_HANDLE_CRASH

// PW_CHECK - If condition evaluates to false, crash. Message optional.
#define PW_CHECK(condition, ...)                            \
  do {                                                      \
    if (!(condition)) {                                     \
      PW_HANDLE_ASSERT_FAILURE(#condition, "" __VA_ARGS__); \
    }                                                       \
  } while (0)

#define PW_DCHECK(...)            \
  do {                            \
    if (PW_ASSERT_ENABLE_DEBUG) { \
      PW_CHECK(__VA_ARGS__);      \
    }                             \
  } while (0)

// PW_D?CHECK_<type>_<comparison> macros - Binary comparison asserts.
//
// The below blocks are structured in table form, violating the 80-column
// Pigweed style, in order to make it clearer what is common and what isn't
// between the multitude of assert macro instantiations. To best view this
// section, turn off editor wrapping or make your editor wide.
//
// clang-format off

// Checks for int: LE, LT, GE, GT, EQ.
#define PW_CHECK_INT_LE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, <=, argb, int, "%d", __VA_ARGS__)
#define PW_CHECK_INT_LT(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, < , argb, int, "%d", __VA_ARGS__)
#define PW_CHECK_INT_GE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, >=, argb, int, "%d", __VA_ARGS__)
#define PW_CHECK_INT_GT(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, > , argb, int, "%d", __VA_ARGS__)
#define PW_CHECK_INT_EQ(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, ==, argb, int, "%d", __VA_ARGS__)
#define PW_CHECK_INT_NE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, !=, argb, int, "%d", __VA_ARGS__)

// Debug checks for int: LE, LT, GE, GT, EQ.
#define PW_DCHECK_INT_LE(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_INT_LE(__VA_ARGS__)
#define PW_DCHECK_INT_LT(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_INT_LT(__VA_ARGS__)
#define PW_DCHECK_INT_GE(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_INT_GE(__VA_ARGS__)
#define PW_DCHECK_INT_GT(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_INT_GT(__VA_ARGS__)
#define PW_DCHECK_INT_EQ(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_INT_EQ(__VA_ARGS__)
#define PW_DCHECK_INT_NE(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_INT_NE(__VA_ARGS__)

// Checks for unsigned int: LE, LT, GE, GT, EQ.
#define PW_CHECK_UINT_LE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, <=, argb, unsigned int, "%u", __VA_ARGS__)
#define PW_CHECK_UINT_LT(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, < , argb, unsigned int, "%u", __VA_ARGS__)
#define PW_CHECK_UINT_GE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, >=, argb, unsigned int, "%u", __VA_ARGS__)
#define PW_CHECK_UINT_GT(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, > , argb, unsigned int, "%u", __VA_ARGS__)
#define PW_CHECK_UINT_EQ(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, ==, argb, unsigned int, "%u", __VA_ARGS__)
#define PW_CHECK_UINT_NE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, !=, argb, unsigned int, "%u", __VA_ARGS__)

// Debug checks for unsigned int: LE, LT, GE, GT, EQ.
#define PW_DCHECK_UINT_LE(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_UINT_LE(__VA_ARGS__)
#define PW_DCHECK_UINT_LT(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_UINT_LT(__VA_ARGS__)
#define PW_DCHECK_UINT_GE(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_UINT_GE(__VA_ARGS__)
#define PW_DCHECK_UINT_GT(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_UINT_GT(__VA_ARGS__)
#define PW_DCHECK_UINT_EQ(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_UINT_EQ(__VA_ARGS__)
#define PW_DCHECK_UINT_NE(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_UINT_NE(__VA_ARGS__)

// Checks for pointer: LE, LT, GE, GT, EQ, NE.
#define PW_CHECK_PTR_LE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, <=, argb, const void*, "%p", __VA_ARGS__)
#define PW_CHECK_PTR_LT(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, < , argb, const void*, "%p", __VA_ARGS__)
#define PW_CHECK_PTR_GE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, >=, argb, const void*, "%p", __VA_ARGS__)
#define PW_CHECK_PTR_GT(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, > , argb, const void*, "%p", __VA_ARGS__)
#define PW_CHECK_PTR_EQ(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, ==, argb, const void*, "%p", __VA_ARGS__)
#define PW_CHECK_PTR_NE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, !=, argb, const void*, "%p", __VA_ARGS__)

// Check for pointer: NOTNULL. Use "nullptr" in C++, "NULL" in C.
#ifdef __cplusplus
#define PW_CHECK_NOTNULL(arga, ...) \
  _PW_CHECK_BINARY_CMP_IMPL(arga, !=, nullptr, const void*, "%p", __VA_ARGS__)
#else  // __cplusplus
#define PW_CHECK_NOTNULL(arga, ...) \
  _PW_CHECK_BINARY_CMP_IMPL(arga, !=, NULL, const void*, "%p", __VA_ARGS__)
#endif  // __cplusplus

// Debug checks for pointer: LE, LT, GE, GT, EQ, NE, and NOTNULL.
#define PW_DCHECK_PTR_LE(...)  if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_PTR_LE(__VA_ARGS__)
#define PW_DCHECK_PTR_LT(...)  if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_PTR_LT(__VA_ARGS__)
#define PW_DCHECK_PTR_GE(...)  if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_PTR_GE(__VA_ARGS__)
#define PW_DCHECK_PTR_GT(...)  if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_PTR_GT(__VA_ARGS__)
#define PW_DCHECK_PTR_EQ(...)  if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_PTR_EQ(__VA_ARGS__)
#define PW_DCHECK_PTR_NE(...)  if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_PTR_NE(__VA_ARGS__)
#define PW_DCHECK_NOTNULL(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_NOTNULL(__VA_ARGS__)

// Checks for float: EXACT_LE, EXACT_LT, EXACT_GE, EXACT_GT, EXACT_EQ, EXACT_NE,
// NEAR.
#define PW_CHECK_FLOAT_NEAR(arga, argb, abs_tolerance, ...) \
  _PW_CHECK_FLOAT_NEAR(arga, argb, abs_tolerance, __VA_ARGS__)
#define PW_CHECK_FLOAT_EXACT_LE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, <=, argb, float, "%f", __VA_ARGS__)
#define PW_CHECK_FLOAT_EXACT_LT(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, < , argb, float, "%f", __VA_ARGS__)
#define PW_CHECK_FLOAT_EXACT_GE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, >=, argb, float, "%f", __VA_ARGS__)
#define PW_CHECK_FLOAT_EXACT_GT(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, > , argb, float, "%f", __VA_ARGS__)
#define PW_CHECK_FLOAT_EXACT_EQ(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, ==, argb, float, "%f", __VA_ARGS__)
#define PW_CHECK_FLOAT_EXACT_NE(arga, argb, ...) _PW_CHECK_BINARY_CMP_IMPL(arga, !=, argb, float, "%f", __VA_ARGS__)

// Debug checks for float: NEAR, EXACT_LE, EXACT_LT, EXACT_GE, EXACT_GT,
// EXACT_EQ.
#define PW_DCHECK_FLOAT_NEAR(...)     if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_FLOAT_NEAR(__VA_ARGS__)
#define PW_DCHECK_FLOAT_EXACT_LE(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_FLOAT_EXACT_LE(__VA_ARGS__)
#define PW_DCHECK_FLOAT_EXACT_LT(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_FLOAT_EXACT_LT(__VA_ARGS__)
#define PW_DCHECK_FLOAT_EXACT_GE(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_FLOAT_EXACT_GE(__VA_ARGS__)
#define PW_DCHECK_FLOAT_EXACT_GT(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_FLOAT_EXACT_GT(__VA_ARGS__)
#define PW_DCHECK_FLOAT_EXACT_EQ(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_FLOAT_EXACT_EQ(__VA_ARGS__)
#define PW_DCHECK_FLOAT_EXACT_NE(...) if (!(PW_ASSERT_ENABLE_DEBUG)) {} else PW_CHECK_FLOAT_EXACT_NE(__VA_ARGS__)

// clang-format on

// PW_CHECK_OK - If condition does not evaluate to PW_STATUS_OK, crash. Message
// optional.
#define PW_CHECK_OK(expression, ...)                                     \
  do {                                                                   \
    const _PW_CHECK_OK_STATUS _pw_assert_check_ok_status = (expression); \
    if (_pw_assert_check_ok_status != PW_STATUS_OK) {                    \
      _PW_CHECK_BINARY_ARG_HANDLER(                                      \
          #expression,                                                   \
          pw_StatusString(_pw_assert_check_ok_status),                   \
          "==",                                                          \
          "OkStatus()",                                                  \
          "OK",                                                          \
          "%s",                                                          \
          "" __VA_ARGS__);                                               \
    }                                                                    \
  } while (0)

#ifdef __cplusplus
#define _PW_CHECK_OK_STATUS ::pw::Status
#else
#define _PW_CHECK_OK_STATUS pw_Status
#endif  // __cplusplus

#define PW_DCHECK_OK(...)          \
  if (!(PW_ASSERT_ENABLE_DEBUG)) { \
  } else                           \
    PW_CHECK_OK(__VA_ARGS__)

// Use a static_cast in C++ to avoid accidental comparisons between e.g. an
// integer and the CHECK message const char*.
#if defined(__cplusplus) && __cplusplus >= 201703L

namespace pw::assert::internal {

template <typename T, typename U>
constexpr const void* ConvertToType(U* value) {
  if constexpr (std::is_function<U>()) {
    return reinterpret_cast<const void*>(value);
  } else {
    return static_cast<const void*>(value);
  }
}

template <typename T, typename U>
constexpr T ConvertToType(const U& value) {
  return static_cast<T>(value);
}

}  // namespace pw::assert::internal

#define _PW_CHECK_CONVERT(type, name, arg) \
  type name = ::pw::assert::internal::ConvertToType<type>(arg)
#else
#define _PW_CHECK_CONVERT(type, name, arg) type name = (type)(arg)
#endif  // __cplusplus

// For the binary assertions, this private macro is re-used for almost all of
// the variants. Due to limitations of C formatting, it is necessary to have
// separate macros for the types.
//
// The macro avoids evaluating the arguments multiple times at the cost of some
// macro complexity.
#define _PW_CHECK_BINARY_CMP_IMPL(                                    \
    arg_a, comparison_op, arg_b, type_decl, type_fmt, ...)            \
  do {                                                                \
    _PW_CHECK_CONVERT(type_decl, evaluated_argument_a, arg_a);        \
    _PW_CHECK_CONVERT(type_decl, evaluated_argument_b, arg_b);        \
    if (!(evaluated_argument_a comparison_op evaluated_argument_b)) { \
      _PW_CHECK_BINARY_ARG_HANDLER(#arg_a,                            \
                                   evaluated_argument_a,              \
                                   #comparison_op,                    \
                                   #arg_b,                            \
                                   evaluated_argument_b,              \
                                   type_fmt,                          \
                                   "" __VA_ARGS__);                   \
    }                                                                 \
  } while (0)

// All binary comparison CHECK macros are directed to this handler before
// hitting the CHECK backend. This controls whether evaluated values are
// captured.
#if PW_ASSERT_CAPTURE_VALUES
#define _PW_CHECK_BINARY_ARG_HANDLER(arg_a_str,              \
                                     arg_a_val,              \
                                     comparison_op_str,      \
                                     arg_b_str,              \
                                     arg_b_val,              \
                                     type_fmt,               \
                                     ...)                    \
  PW_HANDLE_ASSERT_BINARY_COMPARE_FAILURE(arg_a_str,         \
                                          arg_a_val,         \
                                          comparison_op_str, \
                                          arg_b_str,         \
                                          arg_b_val,         \
                                          type_fmt,          \
                                          __VA_ARGS__)
#else
#define _PW_CHECK_BINARY_ARG_HANDLER(arg_a_str,                           \
                                     arg_a_val,                           \
                                     comparison_op_str,                   \
                                     arg_b_str,                           \
                                     arg_b_val,                           \
                                     type_fmt,                            \
                                     ...)                                 \
  PW_HANDLE_ASSERT_FAILURE(arg_a_str " " comparison_op_str " " arg_b_str, \
                           __VA_ARGS__)
#endif  // PW_ASSERT_CAPTURE_VALUES

// Custom implementation for FLOAT_NEAR which is implemented through two
// underlying checks which are not trivially replaced through the use of
// FLOAT_EXACT_LE & FLOAT_EXACT_GE.
#define _PW_CHECK_FLOAT_NEAR(argument_a, argument_b, abs_tolerance, ...)  \
  do {                                                                    \
    PW_CHECK_FLOAT_EXACT_GE(abs_tolerance, 0.0f);                         \
    float evaluated_argument_a = (float)(argument_a);                     \
    float evaluated_argument_b_min = (float)(argument_b)-abs_tolerance;   \
    float evaluated_argument_b_max = (float)(argument_b) + abs_tolerance; \
    if (!(evaluated_argument_a >= evaluated_argument_b_min)) {            \
      _PW_CHECK_BINARY_ARG_HANDLER(#argument_a,                           \
                                   evaluated_argument_a,                  \
                                   ">=",                                  \
                                   #argument_b " - abs_tolerance",        \
                                   evaluated_argument_b_min,              \
                                   "%f",                                  \
                                   "" __VA_ARGS__);                       \
    } else if (!(evaluated_argument_a <= evaluated_argument_b_max)) {     \
      _PW_CHECK_BINARY_ARG_HANDLER(#argument_a,                           \
                                   evaluated_argument_a,                  \
                                   "<=",                                  \
                                   #argument_b " + abs_tolerance",        \
                                   evaluated_argument_b_max,              \
                                   "%f",                                  \
                                   "" __VA_ARGS__);                       \
    }                                                                     \
  } while (0)
