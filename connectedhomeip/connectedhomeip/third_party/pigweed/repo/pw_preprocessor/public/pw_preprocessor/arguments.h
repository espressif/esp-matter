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

// Macros for working with arguments to function-like macros.
#pragma once

#include "pw_preprocessor/boolean.h"
#include "pw_preprocessor/internal/arg_count_impl.h"

// Expands to a comma followed by __VA_ARGS__, if __VA_ARGS__ is non-empty.
// Otherwise, expands to nothing. If the final argument is empty, it is omitted.
// This is useful when passing __VA_ARGS__ to a variadic function or template
// parameter list, since it removes the extra comma when no arguments are
// provided. PW_COMMA_ARGS must NOT be used when invoking a macro from another
// macro.
//
// This is a more flexible, standard-compliant version of ##__VA_ARGS__. Unlike
// ##__VA_ARGS__, this can be used to eliminate an unwanted comma when
// __VA_ARGS__ expands to an empty argument because an outer macro was called
// with __VA_ARGS__ instead of ##__VA_ARGS__. Also, since PW_COMMA_ARGS drops
// the last argument if it is empty, both MY_MACRO(1, 2) and MY_MACRO(1, 2, )
// can work correctly.
//
// PW_COMMA_ARGS must NOT be used to conditionally include a comma when invoking
// a macro from another macro. PW_COMMA_ARGS only functions correctly when the
// macro expands to C or C++ code! Using it with intermediate macros can result
// in out-of-order parameters. When invoking one macro from another, simply pass
// __VA_ARGS__. Only the final macro that expands to C/C++ code should use
// PW_COMMA_ARGS.
//
// For example, the following does NOT work:
/*
     #define MY_MACRO(fmt, ...) \
         NESTED_MACRO(fmt PW_COMMA_ARGS(__VA_ARGS__))  // BAD! Do not do this!
*/
// Instead, only use PW_COMMA_ARGS when the macro expands to C/C++ code:
/*
     #define MY_MACRO(fmt, ...) \
         NESTED_MACRO(fmt, __VA_ARGS__)  // Pass __VA_ARGS__ to nested macros

     #define NESTED_MACRO(fmt, ...) \
         printf(fmt PW_COMMA_ARGS(__VA_ARGS__))  // PW_COMMA_ARGS is OK here
*/
#define PW_COMMA_ARGS(...)                                       \
  _PW_IF(PW_EMPTY_ARGS(__VA_ARGS__), _PW_EXPAND, _PW_COMMA_ARGS) \
  (PW_DROP_LAST_ARG_IF_EMPTY(__VA_ARGS__))

#define _PW_COMMA_ARGS(...) , __VA_ARGS__

// Allows calling a different function-like macros based on the number of
// arguments. For example:
//
//   #define ARG_PRINT(...)  PW_DELEGATE_BY_ARG_COUNT(_ARG_PRINT, __VA_ARGS__)
//   #define _ARG_PRINT1(a)        LOG_INFO("1 arg: %s", a)
//   #define _ARG_PRINT2(a, b)     LOG_INFO("2 args: %s, %s", a, b)
//   #define _ARG_PRINT3(a, b, c)  LOG_INFO("3 args: %s, %s, %s", a, b, c)
//
// This can the be called from C/C++ code:
//
//    ARG_PRINT("a");            // Outputs: 1 arg: a
//    ARG_PRINT("a", "b");       // Outputs: 2 args: a, b
//    ARG_PRINT("a", "b", "c");  // Outputs: 3 args: a, b, c
//
#define PW_DELEGATE_BY_ARG_COUNT(function, ...)                 \
  _PW_DELEGATE_BY_ARG_COUNT(                                    \
      _PW_PASTE2(function, PW_FUNCTION_ARG_COUNT(__VA_ARGS__)), \
      PW_DROP_LAST_ARG_IF_EMPTY(__VA_ARGS__))

#define _PW_DELEGATE_BY_ARG_COUNT(function, ...) function(__VA_ARGS__)

// PW_MACRO_ARG_COUNT counts the number of arguments it was called with. It
// evalulates to an integer literal in the range 0 to 64. Counting more than 64
// arguments is not currently supported.
//
// PW_MACRO_ARG_COUNT is most commonly used to count __VA_ARGS__ in a variadic
// macro. For example, the following code counts the number of arguments passed
// to a logging macro:
//
/*   #define LOG_INFO(format, ...) {                                   \
         static const int kArgCount = PW_MACRO_ARG_COUNT(__VA_ARGS__); \
         SendLog(kArgCount, format, ##__VA_ARGS__);                    \
       }
*/
// clang-format off
#define PW_MACRO_ARG_COUNT(...)                      \
  _PW_MACRO_ARG_COUNT_IMPL(__VA_ARGS__,              \
                     64, 63, 62, 61, 60, 59, 58, 57, \
                     56, 55, 54, 53, 52, 51, 50, 49, \
                     48, 47, 46, 45, 44, 43, 42, 41, \
                     40, 39, 38, 37, 36, 35, 34, 33, \
                     32, 31, 30, 29, 28, 27, 26, 25, \
                     24, 23, 22, 21, 20, 19, 18, 17, \
                     16, 15, 14, 13, 12, 11, 10,  9, \
                      8,  7,  6,  5, 4,  3,  2,  PW_HAS_ARGS(__VA_ARGS__))
// clang-format on

// Argument count for using with a C/C++ function or template parameter list.
// The difference from PW_MACRO_ARG_COUNT is that the last argument is not
// counted if it is empty. This makes it easier to drop the final comma when
// expanding to C/C++ code.
#define PW_FUNCTION_ARG_COUNT(...) \
  _PW_FUNCTION_ARG_COUNT(PW_LAST_ARG(__VA_ARGS__), __VA_ARGS__)

#define _PW_FUNCTION_ARG_COUNT(last_arg, ...) \
  _PW_PASTE2(_PW_FUNCTION_ARG_COUNT_, PW_EMPTY_ARGS(last_arg))(__VA_ARGS__)

#define _PW_FUNCTION_ARG_COUNT_0 PW_MACRO_ARG_COUNT
#define _PW_FUNCTION_ARG_COUNT_1(...) \
  PW_MACRO_ARG_COUNT(PW_DROP_LAST_ARG(__VA_ARGS__))

// Evaluates to the last argument in the provided arguments.
#define PW_LAST_ARG(...) \
  _PW_PASTE2(_PW_LAST_ARG_, PW_MACRO_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

// Evaluates to the provided arguments, excluding the final argument.
#define PW_DROP_LAST_ARG(...) \
  _PW_PASTE2(_PW_DROP_LAST_ARG_, PW_MACRO_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

// Evaluates to the arguments, excluding the final argument if it is empty.
#define PW_DROP_LAST_ARG_IF_EMPTY(...)                                       \
  _PW_IF(                                                                    \
      PW_EMPTY_ARGS(PW_LAST_ARG(__VA_ARGS__)), PW_DROP_LAST_ARG, _PW_EXPAND) \
  (__VA_ARGS__)

// Expands to 1 if one or more arguments are provided, 0 otherwise.
#define PW_HAS_ARGS(...) PW_NOT(PW_EMPTY_ARGS(__VA_ARGS__))

// Expands to 0 if one or more arguments are provided, 1 otherwise. This
// approach is from Jens Gustedt's blog:
//   https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
//
// Normally, with a standard-compliant C preprocessor, it's impossible to tell
// whether a variadic macro was called with no arguments or with one argument.
// A macro invoked with no arguments is actually passed one empty argument.
//
// This macro works by checking for the presence of a comma in four situations.
// These situations give the following information about __VA_ARGS__:
//
//   1. It is two or more variadic arguments.
//   2. It expands to one argument surrounded by parentheses.
//   3. It is a function-like macro that produces a comma when invoked.
//   4. It does not interfere with calling a macro when placed between it and
//      parentheses.
//
// If a comma is not present in 1, 2, 3, but is present in 4, then __VA_ARGS__
// is empty. For this case (0001), and only this case, a corresponding macro
// that expands to a comma is defined. The presence of this comma determines
// whether any arguments were passed in.
//
// C++20 introduces __VA_OPT__, which would greatly simplify this macro.
#define PW_EMPTY_ARGS(...)                                             \
  _PW_HAS_NO_ARGS(_PW_HAS_COMMA(__VA_ARGS__),                          \
                  _PW_HAS_COMMA(_PW_MAKE_COMMA_IF_CALLED __VA_ARGS__), \
                  _PW_HAS_COMMA(__VA_ARGS__()),                        \
                  _PW_HAS_COMMA(_PW_MAKE_COMMA_IF_CALLED __VA_ARGS__()))

// clang-format off

#define _PW_HAS_COMMA(...)                                           \
  _PW_MACRO_ARG_COUNT_IMPL(__VA_ARGS__,                              \
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)

#define _PW_MACRO_ARG_COUNT_IMPL(a64, a63, a62, a61, a60, a59, a58, a57, \
                                 a56, a55, a54, a53, a52, a51, a50, a49, \
                                 a48, a47, a46, a45, a44, a43, a42, a41, \
                                 a40, a39, a38, a37, a36, a35, a34, a33, \
                                 a32, a31, a30, a29, a28, a27, a26, a25, \
                                 a24, a23, a22, a21, a20, a19, a18, a17, \
                                 a16, a15, a14, a13, a12, a11, a10, a09, \
                                 a08, a07, a06, a05, a04, a03, a02, a01, \
                                 count, ...)                             \
  count

// clang-format on

#define _PW_HAS_NO_ARGS(a1, a2, a3, a4) \
  _PW_HAS_COMMA(_PW_PASTE_RESULTS(a1, a2, a3, a4))
#define _PW_PASTE_RESULTS(a1, a2, a3, a4) _PW_HAS_COMMA_CASE_##a1##a2##a3##a4
#define _PW_HAS_COMMA_CASE_0001 ,
#define _PW_MAKE_COMMA_IF_CALLED(...) ,
