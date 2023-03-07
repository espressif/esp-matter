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

#include <stddef.h>
#include <stdint.h>

#include "pw_log_tokenized/log_tokenized.h"
#include "pw_preprocessor/arguments.h"
#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"

// Create a fake version of the tokenization macro.
#undef PW_LOG_TOKENIZED_ENCODE_MESSAGE
#define PW_LOG_TOKENIZED_ENCODE_MESSAGE(payload, message, ...)  \
  pw_log_tokenized_CaptureArgs(payload,                         \
                               PW_MACRO_ARG_COUNT(__VA_ARGS__), \
                               message PW_COMMA_ARGS(__VA_ARGS__))

PW_EXTERN_C_START

typedef struct {
  uintptr_t metadata;
  const char* format_string;
  size_t arg_count;
} pw_log_tokenized_CapturedLog;

extern pw_log_tokenized_CapturedLog last_log;

void pw_log_tokenized_CaptureArgs(uintptr_t payload,
                                  size_t arg_count,
                                  const char* message,
                                  ...) PW_PRINTF_FORMAT(3, 4);

// C versions of tests.
void pw_log_tokenized_Test_LogMetadata_LevelTooLarge_Clamps(void);
void pw_log_tokenized_Test_LogMetadata_TooManyFlags_Truncates(void);
void pw_log_tokenized_Test_LogMetadata_LogMetadata_VariousValues(void);
void pw_log_tokenized_Test_LogMetadata_LogMetadata_Zero(void);

PW_EXTERN_C_END
