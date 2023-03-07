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

// This is test verifies that the logging backend:
//
// - Compiles as plain C
// - Runs when compiled as C
//
// Unfortunately, since we do not know where the log sink actually goes, the
// logging functionality itself is not verified beyond compiling and running.

#define PW_LOG_MODULE_NAME "CTS"

#include "pw_log/log.h"

#ifdef __cplusplus
#error "This file must be compiled as plain C to verify C compilation works."
#endif  // __cplusplus

static void LoggingFromFunctionPlainC(void) { PW_LOG_INFO("From a function!"); }

static void CustomFormatStringTest(void);

void BasicLogTestPlainC(void) {
  int n = 3;

  // Debug level
  PW_LOG_DEBUG("This log statement should be at DEBUG level; no args");
  for (int i = 0; i < n; ++i) {
    PW_LOG_DEBUG("Counting: %d", i);
  }
  PW_LOG_DEBUG("Here is a string: %s; with another string %s", "foo", "bar");

  // Info level
  PW_LOG_INFO("This log statement should be at INFO level; no args");
  for (int i = 0; i < n; ++i) {
    PW_LOG_INFO("Counting: %d", i);
  }
  PW_LOG_INFO("Here is a string: %s; with another string %s", "foo", "bar");

  // Warn level
  PW_LOG_WARN("This log statement should be at WARN level; no args");
  for (int i = 0; i < n; ++i) {
    PW_LOG_WARN("Counting: %d", i);
  }
  PW_LOG_WARN("Here is a string: %s; with another string %s", "foo", "bar");

  // Error level.
  PW_LOG_ERROR("This log statement should be at ERROR level; no args");
  for (int i = 0; i < n; ++i) {
    PW_LOG_ERROR("Counting: %d", i);
  }
  PW_LOG_ERROR("Here is a string: %s; with another string %s", "foo", "bar");

  // Critical level.
  PW_LOG_CRITICAL("This log is the last one; device should reboot");

  // Core log macro, with manually specified level and flags.
  PW_LOG(PW_LOG_LEVEL_DEBUG, 0, "A manual DEBUG-level message");
  PW_LOG(PW_LOG_LEVEL_DEBUG, 1, "A manual DEBUG-level message; with a flag");

  PW_LOG(PW_LOG_LEVEL_INFO, 0, "A manual INFO-level message");
  PW_LOG(PW_LOG_LEVEL_INFO, 1, "A manual INFO-level message; with a flag");

  PW_LOG(PW_LOG_LEVEL_WARN, 0, "A manual WARN-level message");
  PW_LOG(PW_LOG_LEVEL_WARN, 1, "A manual WARN-level message; with a flag");

  PW_LOG(PW_LOG_LEVEL_ERROR, 0, "A manual ERROR-level message");
  PW_LOG(PW_LOG_LEVEL_ERROR, 1, "A manual ERROR-level message; with a flag");

  PW_LOG(PW_LOG_LEVEL_CRITICAL, 0, "A manual CRITICAL-level message");
  PW_LOG(
      PW_LOG_LEVEL_CRITICAL, 1, "A manual CRITICAL-level message; with a flag");

  // Log levels other than the standard ones work; what each backend does is
  // implementation defined.
  PW_LOG(0, PW_LOG_FLAGS, "Custom log level: 0");
  PW_LOG(1, PW_LOG_FLAGS, "Custom log level: 1");
  PW_LOG(2, PW_LOG_FLAGS, "Custom log level: 2");
  PW_LOG(3, PW_LOG_FLAGS, "Custom log level: 3");
  PW_LOG(100, PW_LOG_FLAGS, "Custom log level: 100");

  // Logging from a function.
  LoggingFromFunctionPlainC();

  // Changing the module name mid-file.
#undef PW_LOG_MODULE_NAME
#define PW_LOG_MODULE_NAME "XYZ"
  PW_LOG_INFO("This has a custom module name");
  PW_LOG_INFO("So does this");

  CustomFormatStringTest();
}

#undef PW_LOG
#define PW_LOG(level, flags, message, ...)                               \
  DoNothingFakeFunction("%d/%d/%d: incoming transmission [" message "]", \
                        level,                                           \
                        __LINE__,                                        \
                        flags PW_COMMA_ARGS(__VA_ARGS__))

static void DoNothingFakeFunction(const char* f, ...) PW_PRINTF_FORMAT(1, 2);

static void DoNothingFakeFunction(const char* f, ...) { (void)f; }

static void CustomFormatStringTest(void) {
  PW_LOG_DEBUG("Abc");
  PW_LOG_INFO("Abc %d", 123);
  PW_LOG_WARN("Abc %d %s", 123, "four");
}
