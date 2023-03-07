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

#include <cstdint>

#include "pw_perf_test/event_handler.h"

namespace pw::perf_test {

// An event handler that depends on the pw_log module. This event handler acts
// as the default for perf tests, and follows a GTEST-style format of messaging.
class LoggingEventHandler : public EventHandler {
 public:
  void RunAllTestsStart(const TestRunInfo& summary) override;
  void RunAllTestsEnd() override;
  void TestCaseStart(const TestCase& info) override;
  void TestCaseEnd(const TestCase& info, const Results& end_result) override;
  void TestCaseIteration(const IterationResult& result) override;
};

}  // namespace pw::perf_test
