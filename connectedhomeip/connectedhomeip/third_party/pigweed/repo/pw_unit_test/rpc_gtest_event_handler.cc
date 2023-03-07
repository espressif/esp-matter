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

#include <tuple>

#include "gtest/gtest.h"
#include "pw_unit_test/internal/rpc_event_handler.h"
#include "pw_unit_test/unit_test_service.h"

namespace pw::unit_test::internal {

RpcEventHandler::RpcEventHandler(UnitTestService& service) : service_(service) {
  // Initialize GoogleTest and disable the default result printer.
  testing::InitGoogleTest();
  auto unit_test = testing::UnitTest::GetInstance();
  auto default_listener = unit_test->listeners().default_result_printer();
  unit_test->listeners().Release(default_listener);
  delete default_listener;
}

void RpcEventHandler::ExecuteTests(span<std::string_view> suites_to_run) {
  if (!suites_to_run.empty()) {
    PW_LOG_WARN(
        "GoogleTest backend does not support test suite filtering. Running all "
        "suites.");
  }
  if (service_.verbose_) {
    PW_LOG_WARN(
        "GoogleTest backend does not support reporting passed expectations.");
  }

  auto unit_test = testing::UnitTest::GetInstance();
  unit_test->listeners().Append(this);

  std::ignore = RUN_ALL_TESTS();

  unit_test->listeners().Release(this);
}

void RpcEventHandler::OnTestProgramStart(const testing::UnitTest&) {
  service_.WriteTestRunStart();
}

void RpcEventHandler::OnTestProgramEnd(const testing::UnitTest& unit_test) {
  RunTestsSummary run_tests_summary{
      .passed_tests = unit_test.successful_test_count(),
      .failed_tests = unit_test.failed_test_count(),
      .skipped_tests = unit_test.skipped_test_count(),
      .disabled_tests = unit_test.disabled_test_count(),
  };
  service_.WriteTestRunEnd(run_tests_summary);
}

void RpcEventHandler::OnTestStart(const testing::TestInfo& test_info) {
  TestCase test_case{
      .suite_name = test_info.test_suite_name(),
      .test_name = test_info.name(),
      .file_name = test_info.file(),
  };
  service_.WriteTestCaseStart(test_case);
}

void RpcEventHandler::OnTestEnd(const testing::TestInfo& test_info) {
  TestResult result;
  if (test_info.result()->Passed()) {
    result = TestResult::kSuccess;
  } else if (test_info.result()->Skipped()) {
    result = TestResult::kSkipped;
  } else {
    result = TestResult::kFailure;
  }

  service_.WriteTestCaseEnd(result);
}

void RpcEventHandler::OnTestPartResult(const testing::TestPartResult& result) {
  TestExpectation expectation{
      .expression = "",
      .evaluated_expression = result.summary(),
      .line_number = result.line_number(),
      .success = result.passed(),
  };
  service_.WriteTestCaseExpectation(expectation);
}

void RpcEventHandler::OnTestDisabled(const testing::TestInfo& test_info) {
  TestCase test_case{
      .suite_name = test_info.test_suite_name(),
      .test_name = test_info.name(),
      .file_name = test_info.file(),
  };
  service_.WriteTestCaseDisabled(test_case);
}

}  // namespace pw::unit_test::internal
