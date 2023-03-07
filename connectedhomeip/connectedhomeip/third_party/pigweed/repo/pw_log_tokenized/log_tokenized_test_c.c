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

#define PW_LOG_MODULE_NAME "log module name!"

// Configure the module so that the test runs against known values.
#undef PW_LOG_TOKENIZED_LEVEL_BITS
#undef PW_LOG_TOKENIZED_MODULE_BITS
#undef PW_LOG_TOKENIZED_FLAG_BITS
#undef PW_LOG_TOKENIZED_LINE_BITS

#define PW_LOG_TOKENIZED_LEVEL_BITS 3
#define PW_LOG_TOKENIZED_MODULE_BITS 16
#define PW_LOG_TOKENIZED_FLAG_BITS 2
#define PW_LOG_TOKENIZED_LINE_BITS 11

#include "pw_log_tokenized/log_tokenized.h"
#include "pw_log_tokenized_private/test_utils.h"

pw_log_tokenized_CapturedLog last_log;

void pw_log_tokenized_CaptureArgs(uintptr_t payload,
                                  size_t arg_count,
                                  const char* message,
                                  ...) {
  last_log.metadata = payload;
  last_log.format_string = message;
  last_log.arg_count = arg_count;
}

// These functions correspond to tests in log_tokenized_test.cc. The tests call
// these functions and check the results.
void pw_log_tokenized_Test_LogMetadata_LevelTooLarge_Clamps(void) {
  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(8, 0, "hello");
}

void pw_log_tokenized_Test_LogMetadata_TooManyFlags_Truncates(void) {
  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(1, 0xFFFFFFFF, "hello");
}

void pw_log_tokenized_Test_LogMetadata_LogMetadata_VariousValues(void) {
  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(6, 3, "hello%s", "?");
}

void pw_log_tokenized_Test_LogMetadata_LogMetadata_Zero(void) {
  PW_LOG_TOKENIZED_TO_GLOBAL_HANDLER_WITH_PAYLOAD(0, 0, "hello");
}
