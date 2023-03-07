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

#include <stdio.h>

#include "pw_assert/internal/print_and_abort.h"

// Implement the pw_assert backend by forwarding to the print_and_abort.h
// macros, which use printf and abort.
#define PW_HANDLE_CRASH(...)                         \
  PW_ASSERT_PRINT_EXPRESSION("CRASH", "PW_CRASH()"); \
  _PW_ASSERT_PRINT_MESSAGE_AND_ABORT(__VA_ARGS__)

#define PW_HANDLE_ASSERT_FAILURE(condition_string, ...)  \
  PW_ASSERT_PRINT_EXPRESSION("CHECK", condition_string); \
  _PW_ASSERT_PRINT_MESSAGE_AND_ABORT(__VA_ARGS__)

#define PW_HANDLE_ASSERT_BINARY_COMPARE_FAILURE(arg_a_str,                   \
                                                arg_a_val,                   \
                                                comparison_op_str,           \
                                                arg_b_str,                   \
                                                arg_b_val,                   \
                                                type_fmt,                    \
                                                ...)                         \
  PW_ASSERT_PRINT_EXPRESSION("CHECK",                                        \
                             arg_a_str " " comparison_op_str " " arg_b_str); \
  fprintf(stderr,                                                            \
          "  EVALUATED CONDITION\n\n    " arg_a_str " (=" type_fmt           \
          ") " comparison_op_str " " arg_b_str " (=" type_fmt                \
          ")"                                                                \
          ".\n\n",                                                           \
          arg_a_val,                                                         \
          arg_b_val);                                                        \
  _PW_ASSERT_PRINT_MESSAGE_AND_ABORT(__VA_ARGS__)

#define _PW_ASSERT_PRINT_MESSAGE_AND_ABORT(...)     \
  fprintf(stderr, "  MESSAGE\n\n    " __VA_ARGS__); \
  fprintf(stderr, "\n\n");                          \
  fflush(stderr);                                   \
  abort()
