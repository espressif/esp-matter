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

#include "pw_assert_tokenized/handler.h"
#include "pw_log_tokenized/config.h"
#include "pw_tokenizer/tokenize.h"

#define _PW_ASSERT_TOKENIZED_TO_HANDLER(str)                    \
  do {                                                          \
    const uint32_t token = PW_TOKENIZE_STRING(                  \
        PW_LOG_TOKENIZED_FORMAT_STRING("Check failure: " str)); \
    pw_assert_tokenized_HandleCheckFailure(token, __LINE__);    \
  } while (0)

#define PW_HANDLE_CRASH(...) _PW_ASSERT_TOKENIZED_TO_HANDLER(#__VA_ARGS__)

#define PW_HANDLE_ASSERT_FAILURE(condition_string, message, ...) \
  _PW_ASSERT_TOKENIZED_TO_HANDLER(condition_string ", " message)

#define PW_HANDLE_ASSERT_BINARY_COMPARE_FAILURE(arg_a_str,         \
                                                arg_a_val,         \
                                                comparison_op_str, \
                                                arg_b_str,         \
                                                arg_b_val,         \
                                                type_fmt,          \
                                                message,           \
                                                ...)               \
  _PW_ASSERT_TOKENIZED_TO_HANDLER(                                 \
      arg_a_str " " comparison_op_str " " arg_b_str ", " message #__VA_ARGS__)
