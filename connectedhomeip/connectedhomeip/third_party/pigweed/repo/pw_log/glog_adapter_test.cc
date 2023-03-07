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
// TODO(b/235289499): Add verification of the actually logged statements.

// clang-format off
#define PW_LOG_MODULE_NAME "TST"
#define PW_LOG_LEVEL PW_LOG_LEVEL_DEBUG

#include "pw_log/glog_adapter.h"

#include "gtest/gtest.h"
// clang-format on

namespace pw::log {
namespace {

volatile bool conditional;

TEST(Glog, Debug) { LOG(DEBUG) << "LOG(DEBUG) works"; }

TEST(Glog, ConditionalDebug) {
  conditional = true;
  LOG_IF(DEBUG, conditional) << "LOG_IF(DEBUG, true) works";
  conditional = false;
  LOG_IF(DEBUG, conditional) << "You should not see this log";
}

TEST(Glog, Info) { LOG(INFO) << "LOG(INFO) works"; }

TEST(Glog, ConditionalInfo) {
  conditional = true;
  LOG_IF(INFO, conditional) << "LOG_IF(INFO, true) works";
  conditional = false;
  LOG_IF(INFO, conditional) << "You should not see this log";
}

TEST(Glog, Warning) { LOG(WARNING) << "LOG(WARNING) works"; }

TEST(Glog, ConditionalWarning) {
  conditional = true;
  LOG_IF(WARNING, conditional) << "LOG_IF(WARNING, true) works";
  conditional = false;
  LOG_IF(WARNING, conditional) << "You should not see this log";
}

TEST(Glog, Error) { LOG(ERROR) << "LOG(ERROR) works"; }

TEST(Glog, ConditionalError) {
  conditional = true;
  LOG_IF(ERROR, conditional) << "LOG_IF(ERROR, true) works";
  conditional = false;
  LOG_IF(ERROR, conditional) << "You should not see this log";
}

TEST(Glog, Fatal) {
  conditional = false;
  if (conditional) {
    LOG(FATAL) << "LOG(FATAL) compiles but you should not see this log";
  }
}

TEST(Glog, ConditionalFatal) {
  conditional = false;
  LOG_IF(FATAL, conditional) << "LOG_IF(FATAL, false) compiles but you should "
                             << "not see this log";
}

TEST(Glog, Dfatal) {
  conditional = false;
  if (conditional) {
#if defined(NDEBUG)
    LOG(DFATAL) << "LOG(DFATAL) works through PW_LOG_ERROR as NDEBUG is set";
#else   // !defined(NDEBUG)
    LOG(DFATAL) << "LOG(DFATAL) compiles but you should not see this log";
#endif  // defined(NDEBUG)
  }
}

TEST(Glog, ConditionalDfatal) {
#if defined(NDEBUG)
  conditional = true;
  LOG_IF(DFATAL, conditional) << "LOG_IF(DFATAL, true) works through "
                              << "PW_LOG_ERROR as NDEBUG is set";
#endif  // defined(NDEBUG)
  conditional = false;
  LOG_IF(DFATAL, conditional) << "LOG_IF(DFATAL, false) compiles but you "
                              << "should not see this log";
}

}  // namespace
}  // namespace pw::log
