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

#include "gtest/gtest.h"
#include "pw_span/span.h"

namespace pw::unit_test {

class UnitTestService;

namespace internal {

// Unit test event handler that streams test events through an RPC service.
class RpcEventHandler : public EventHandler {
 public:
  RpcEventHandler(UnitTestService& service);
  void ExecuteTests(span<std::string_view> suites_to_run);

  void RunAllTestsStart() override;
  void RunAllTestsEnd(const RunTestsSummary& run_tests_summary) override;
  void TestCaseStart(const TestCase& test_case) override;
  void TestCaseEnd(const TestCase& test_case, TestResult result) override;
  void TestCaseExpect(const TestCase& test_case,
                      const TestExpectation& expectation) override;
  void TestCaseDisabled(const TestCase& test_case) override;

 private:
  UnitTestService& service_;
};

}  // namespace internal
}  // namespace pw::unit_test
