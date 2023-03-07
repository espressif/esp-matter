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

#include <stdbool.h>

#include "pw_preprocessor/arguments.h"
#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"

PW_EXTERN_C_START

struct pw_CapturedAssert {
  // The capturing assert handler will set triggered = 1 upon entry, enabling
  // detecting if an assert triggered by setting it to 0 before; for example
  //
  //   captured_assert_arguments.triggered = 0;
  //   PW_CHECK_NE(1, 2, "Ruh roh!");
  //   EXPECT_TRUE(captured_assert_arguments.triggered);
  //
  int triggered;
  const char* file_name;
  int line_number;
  const char* function_name;
  char message[150];
};

// The assert handler pushes arguments into this global, then returns.
extern struct pw_CapturedAssert pw_captured_assert;

// Crash, including a message with the listed attributes.
void pw_CaptureAssert(const char* file_name,
                      int line_number,
                      const char* function_name,
                      const char* message,
                      ...) PW_PRINTF_FORMAT(4, 5);

PW_EXTERN_C_END

// Capture the crash attributes for testing.
#define PW_HANDLE_CRASH(...) \
  pw_CaptureAssert(__FILE__, __LINE__, __func__ PW_COMMA_ARGS(__VA_ARGS__))

// Capture the crash attributes for testing, including the condition string.
#define PW_HANDLE_ASSERT_FAILURE(condition_string, message, ...) \
  pw_CaptureAssert(__FILE__,                                     \
                   __LINE__,                                     \
                   __func__,                                     \
                   "Check failed: " condition_string             \
                   ". " message PW_COMMA_ARGS(__VA_ARGS__))

// Sample assert failure message produced by the below implementation:
//
//   Check failed: current_sensor (=610) < new_sensor (=50): More details!
//
// Putting the value next to the operand makes the string easier to read.

// clang-format off
// This is too hairy for clang format to handle and retain readability.
#define PW_HANDLE_ASSERT_BINARY_COMPARE_FAILURE(arg_a_str,         \
                                                arg_a_val,         \
                                                comparison_op_str, \
                                                arg_b_str,         \
                                                arg_b_val,         \
                                                type_fmt,          \
                                                message, ...)      \
  pw_CaptureAssert(__FILE__,                                       \
                   __LINE__,                                       \
                   __func__,                                       \
                   "Check failed: "                                \
                       arg_a_str " (=" type_fmt ") "               \
                       comparison_op_str " "                       \
                       arg_b_str " (=" type_fmt ")"                \
                       ". " message,                               \
                   arg_a_val, arg_b_val PW_COMMA_ARGS(__VA_ARGS__))
// clang-format on
