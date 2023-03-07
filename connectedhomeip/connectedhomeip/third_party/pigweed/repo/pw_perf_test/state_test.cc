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

#include "gtest/gtest.h"
#include "pw_perf_test/event_handler.h"
#include "pw_perf_test/perf_test.h"

namespace pw::perf_test {
namespace {

class EmptyEventHandler : public EventHandler {
 public:
  void RunAllTestsStart(const TestRunInfo&) override {}
  void TestCaseStart(const TestCase&) override {}
  void TestCaseEnd(const TestCase&, const Results&) override {}
  void TestCaseIteration(const IterationResult&) override {}
  void RunAllTestsEnd() override {}
};

EmptyEventHandler handler;

void TestFunction() {
  for (volatile int i = 0; i < 100000; i = i + 1) {
  }
}

TEST(StateTest, KeepRunningTest) {
  constexpr int test_iterations = 10;
  State state_obj = internal::CreateState(test_iterations, handler, "");
  int total_iterations = 0;
  while (state_obj.KeepRunning()) {
    ++total_iterations;
    TestFunction();
  }
  EXPECT_EQ(total_iterations, test_iterations);
}

TEST(StateTest, SingleTest) {
  constexpr int test_iterations = 1;
  State state_obj = internal::CreateState(test_iterations, handler, "");
  int total_iterations = 0;
  while (state_obj.KeepRunning()) {
    ++total_iterations;
    TestFunction();
  }
  EXPECT_EQ(total_iterations, test_iterations);
}

}  // namespace
}  // namespace pw::perf_test
