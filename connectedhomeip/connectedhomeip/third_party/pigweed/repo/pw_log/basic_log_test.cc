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

// This is mostly a compile test to verify that the log backend is able to
// compile the constructs  promised by the logging facade; and that when run,
// there is no crash.
//
// TODO(b/235289499): Add verification of the actually logged statements.

// clang-format off
#define PW_LOG_MODULE_NAME "TST"
#define PW_LOG_LEVEL PW_LOG_LEVEL_DEBUG
#include "pw_log/log.h"
#include "pw_log/short.h"
#include "pw_log/shorter.h"
// clang-format on

#include "gtest/gtest.h"

// TODO(b/235291136): Test unsigned integer logging (32 and 64 bit); test
// pointer logging.

void LoggingFromFunction() { PW_LOG_INFO("From a function!"); }

const int N = 3;

TEST(BasicLog, DebugLevel) {
  PW_LOG_DEBUG("This log statement should be at DEBUG level; no args");
  for (int i = 0; i < N; ++i) {
    PW_LOG_DEBUG("Counting: %d", i);
  }
  PW_LOG_DEBUG("Here is a string: %s; with another string %s", "foo", "bar");
}

TEST(BasicLog, InfoLevel) {
  PW_LOG_INFO("This log statement should be at INFO level; no args");
  for (int i = 0; i < N; ++i) {
    PW_LOG_INFO("Counting: %d", i);
  }
  PW_LOG_INFO("Here is a string: %s; with another string %s", "foo", "bar");
}

TEST(BasicLog, WarnLevel) {
  PW_LOG_WARN("This log statement should be at WARN level; no args");
  for (int i = 0; i < N; ++i) {
    PW_LOG_WARN("Counting: %d", i);
  }
  PW_LOG_WARN("Here is a string: %s; with another string %s", "foo", "bar");
}

TEST(BasicLog, ErrorLevel) {
  PW_LOG_ERROR("This log statement should be at ERROR level; no args");
  for (int i = 0; i < N; ++i) {
    PW_LOG_ERROR("Counting: %d", i);
  }
  PW_LOG_ERROR("Here is a string: %s; with another string %s", "foo", "bar");
}

TEST(BasicLog, CriticalLevel) {
  PW_LOG_CRITICAL("Critical, emergency log. Device should not reboot");
}

TEST(BasicLog, ManualLevel) {
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
}

TEST(BasicLog, FromAFunction) { LoggingFromFunction(); }

TEST(BasicLog, CustomLogLevels) {
  // Log levels other than the standard ones work; what each backend does is
  // implementation defined.
  PW_LOG(0, 0, "Custom log level: 0");
  PW_LOG(1, 0, "Custom log level: 1");
  PW_LOG(2, 0, "Custom log level: 2");
  PW_LOG(3, 0, "Custom log level: 3");
  PW_LOG(100, 0, "Custom log level: 100");
}

#define TEST_FAILED_LOG "IF THIS MESSAGE WAS LOGGED, THE TEST FAILED"

TEST(BasicLog, FilteringByLevel) {
#undef PW_LOG_SKIP_LOGS_WITH_LEVEL_LT
#define PW_LOG_SKIP_LOGS_WITH_LEVEL_LT PW_LOG_LEVEL_ERROR

  PW_LOG_DEBUG(TEST_FAILED_LOG);
  PW_LOG_INFO(TEST_FAILED_LOG);
  PW_LOG_WARN(TEST_FAILED_LOG);

  PW_LOG_ERROR("This log should appear as error status (and that's good)");

#undef PW_LOG_SKIP_LOGS_WITH_LEVEL_LT
#define PW_LOG_SKIP_LOGS_WITH_LEVEL_LT 0
}

TEST(BasicLog, FilteringByFlags) {
#undef PW_LOG_SKIP_LOGS_WITH_FLAGS
#define PW_LOG_SKIP_LOGS_WITH_FLAGS 1

  // Flag is set so these should all get zapped.
  PW_LOG(PW_LOG_LEVEL_INFO, 1, TEST_FAILED_LOG);
  PW_LOG(PW_LOG_LEVEL_ERROR, 1, TEST_FAILED_LOG);

  // However, a different flag bit should still log.
  PW_LOG(PW_LOG_LEVEL_INFO, 1 << 1, "This flagged log is intended to appear");
  PW_LOG(PW_LOG_LEVEL_ERROR, 1 << 1, "This flagged log is intended to appear");

#undef PW_LOG_SKIP_LOGS_WITH_FLAGS
#define PW_LOG_SKIP_LOGS_WITH_FLAGS 0
}

TEST(BasicLog, ChangingTheModuleName) {
#undef PW_LOG_MODULE_NAME
#define PW_LOG_MODULE_NAME "PQR"
  PW_LOG_INFO("This has a custom module name");
  PW_LOG_INFO("So does this");
}

TEST(BasicLog, ShortNames) {
  LOG(PW_LOG_LEVEL_INFO, 0, "Shrt lg");
  LOG_DEBUG("A debug log: %d", 1);
  LOG_INFO("An info log: %d", 2);
  LOG_WARN("A warning log: %d", 3);
  LOG_ERROR("An error log: %d", 4);
  LOG_CRITICAL("A critical log: %d", 4);
}

TEST(BasicLog, UltraShortNames) {
  LOG(PW_LOG_LEVEL_INFO, 0, "Shrt lg");
  DBG("A debug log: %d", 1);
  INF("An info log: %d", 2);
  WRN("A warning log: %d", 3);
  ERR("An error log: %d", 4);
  CRT("A critical log: %d", 4);
}

extern "C" void BasicLogTestPlainC();

TEST(BasicLog, FromPlainC) { BasicLogTestPlainC(); }

// Test that adding to the format string compiles correctly. If PW_COMMA_ARGS is
// used in PW_LOG_INFO and the other wrappers in pw_log/log.h, then these
// functions tests fail to compile, because the arguments end up out-of-order.

#undef PW_LOG
#define PW_LOG(level, flags, message, ...)                               \
  DoNothingFakeFunction("%d/%d/%d: incoming transmission [" message "]", \
                        level,                                           \
                        __LINE__,                                        \
                        flags PW_COMMA_ARGS(__VA_ARGS__))

void DoNothingFakeFunction(const char*, ...) PW_PRINTF_FORMAT(1, 2);

void DoNothingFakeFunction(const char*, ...) {}

TEST(CustomFormatString, DebugLevel) {
  PW_LOG_DEBUG("This log statement should be at DEBUG level; no args");
  for (int i = 0; i < N; ++i) {
    PW_LOG_DEBUG("Counting: %d", i);
  }
  PW_LOG_DEBUG("Here is a string: %s; with another string %s", "foo", "bar");
}

TEST(CustomFormatString, InfoLevel) {
  PW_LOG_INFO("This log statement should be at INFO level; no args");
  for (int i = 0; i < N; ++i) {
    PW_LOG_INFO("Counting: %d", i);
  }
  PW_LOG_INFO("Here is a string: %s; with another string %s", "foo", "bar");
}

TEST(CustomFormatString, WarnLevel) {
  PW_LOG_WARN("This log statement should be at WARN level; no args");
  for (int i = 0; i < N; ++i) {
    PW_LOG_WARN("Counting: %d", i);
  }
  PW_LOG_WARN("Here is a string: %s; with another string %s", "foo", "bar");
}

TEST(CustomFormatString, ErrorLevel) {
  PW_LOG_ERROR("This log statement should be at ERROR level; no args");
  for (int i = 0; i < N; ++i) {
    PW_LOG_ERROR("Counting: %d", i);
  }
  PW_LOG_ERROR("Here is a string: %s; with another string %s", "foo", "bar");
}

TEST(CustomFormatString, CriticalLevel) {
  PW_LOG_CRITICAL("Critical, emergency log. Device should not reboot");
}
