// Copyright 2021 The Pigweed Authors
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

#include <zephyr/sys/__assert.h>

#include "pw_preprocessor/compiler.h"

#define PW_HANDLE_CRASH(...)        \
  {                                 \
    __ASSERT_LOC("PW_CRASH()");     \
    __ASSERT_MSG_INFO(__VA_ARGS__); \
    __ASSERT_POST_ACTION();         \
    PW_UNREACHABLE;                 \
  }

#define PW_HANDLE_ASSERT_FAILURE(condition_string, ...)                    \
  {                                                                        \
    __ASSERT_LOC("PW_CHECK() or PW_DCHECK() failure");                     \
    __ASSERT_MSG_INFO("Check failed: " condition_string ". " __VA_ARGS__); \
    __ASSERT_POST_ACTION();                                                \
    PW_UNREACHABLE;                                                        \
  }

#define PW_HANDLE_ASSERT_BINARY_COMPARE_FAILURE(arg_a_str,                 \
                                                arg_a_val,                 \
                                                comparison_op_str,         \
                                                arg_b_str,                 \
                                                arg_b_val,                 \
                                                type_fmt,                  \
                                                message,                   \
                                                ...)                       \
  PW_HANDLE_ASSERT_FAILURE(arg_a_str " (=" type_fmt ") " comparison_op_str \
                                     " " arg_b_str " (=" type_fmt ")",     \
                           message,                                        \
                           arg_a_val,                                      \
                           arg_b_val PW_COMMA_ARGS(__VA_ARGS__))
