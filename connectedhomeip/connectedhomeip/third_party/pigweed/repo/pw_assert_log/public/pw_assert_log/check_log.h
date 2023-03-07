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

#include "pw_log/levels.h"
#include "pw_log/log.h"
#include "pw_log/options.h"
#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"

// Die with a message with several attributes included. This crash frontend
// funnels everything into the logger, which must then handle the true crash
// behaviour.
#define PW_HANDLE_CRASH(message, ...)                                      \
  do {                                                                     \
    PW_HANDLE_LOG(                                                         \
        PW_LOG_LEVEL_FATAL, PW_LOG_FLAGS, "Crash: " message, __VA_ARGS__); \
    PW_UNREACHABLE;                                                        \
  } while (0)

// Die with a message with several attributes included. This assert frontend
// funnels everything into the logger, which is responsible for displaying the
// log, then crashing/rebooting the device.
#define PW_HANDLE_ASSERT_FAILURE(condition_string, message, ...) \
  do {                                                           \
    PW_LOG(PW_LOG_LEVEL_FATAL,                                   \
           PW_LOG_FLAGS,                                         \
           "Check failed: " condition_string ". " message,       \
           __VA_ARGS__);                                         \
    PW_UNREACHABLE;                                              \
  } while (0)

// Sample assert failure message produced by the below implementation:
//
//   Check failed: old_x (=610) < new_x (=50). Details: foo=10, bar.
//
// Putting the value next to the operand makes the string easier to read.

// clang-format off
// This is too hairy for clang format to handle and retain readability.
#define PW_HANDLE_ASSERT_BINARY_COMPARE_FAILURE(arg_a_str,                \
                                                arg_a_val,                \
                                                comparison_op_str,        \
                                                arg_b_str,                \
                                                arg_b_val,                \
                                                type_fmt,                 \
                                                message, ...)             \
  do {                                                                    \
    PW_LOG(PW_LOG_LEVEL_FATAL,                                            \
           PW_LOG_FLAGS,                                                  \
           "Check failed: "                                               \
                 arg_a_str " (=" type_fmt ") "                            \
                 comparison_op_str " "                                    \
                 arg_b_str " (=" type_fmt ")"                             \
                 ". " message,                                            \
              arg_a_val, arg_b_val, __VA_ARGS__);                         \
    PW_UNREACHABLE;                                                       \
  } while(0)
// clang-format on
