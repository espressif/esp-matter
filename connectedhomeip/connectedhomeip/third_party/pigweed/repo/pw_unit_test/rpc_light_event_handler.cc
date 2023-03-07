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

#include "pw_unit_test/internal/rpc_event_handler.h"
#include "pw_unit_test/unit_test_service.h"

namespace pw::unit_test::internal {

RpcEventHandler::RpcEventHandler(UnitTestService& service)
    : service_(service) {}

void RpcEventHandler::ExecuteTests(span<std::string_view> suites_to_run) {
  RegisterEventHandler(this);
  SetTestSuitesToRun(suites_to_run);

  PW_LOG_DEBUG("%u test suite filters applied",
               static_cast<unsigned>(suites_to_run.size()));

  RUN_ALL_TESTS();

  RegisterEventHandler(nullptr);
  SetTestSuitesToRun({});
}

void RpcEventHandler::RunAllTestsStart() { service_.WriteTestRunStart(); }

void RpcEventHandler::RunAllTestsEnd(const RunTestsSummary& run_tests_summary) {
  service_.WriteTestRunEnd(run_tests_summary);
}

void RpcEventHandler::TestCaseStart(const TestCase& test_case) {
  service_.WriteTestCaseStart(test_case);
}

void RpcEventHandler::TestCaseEnd(const TestCase&, TestResult result) {
  service_.WriteTestCaseEnd(result);
}

void RpcEventHandler::TestCaseExpect(const TestCase&,
                                     const TestExpectation& expectation) {
  service_.WriteTestCaseExpectation(expectation);
}

void RpcEventHandler::TestCaseDisabled(const TestCase& test_case) {
  service_.WriteTestCaseDisabled(test_case);
}

}  // namespace pw::unit_test::internal
