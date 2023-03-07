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

#include "pw_unit_test/googletest_style_event_handler.h"

#include <cstdarg>

namespace pw {
namespace unit_test {

void GoogleTestStyleEventHandler::RunAllTestsStart() {
  WriteLine(PW_UNIT_TEST_GOOGLETEST_RUN_ALL_TESTS_START);
}

void GoogleTestStyleEventHandler::RunAllTestsEnd(
    const RunTestsSummary& run_tests_summary) {
  WriteLine(PW_UNIT_TEST_GOOGLETEST_RUN_ALL_TESTS_END);
  WriteLine(PW_UNIT_TEST_GOOGLETEST_PASSED_SUMMARY,
            run_tests_summary.passed_tests);
  if (run_tests_summary.skipped_tests) {
    WriteLine(PW_UNIT_TEST_GOOGLETEST_SKIPPED_SUMMARY,
              run_tests_summary.skipped_tests);
  }
  if (run_tests_summary.failed_tests) {
    WriteLine(PW_UNIT_TEST_GOOGLETEST_FAILED_SUMMARY,
              run_tests_summary.failed_tests);
  }
}

void GoogleTestStyleEventHandler::TestCaseStart(const TestCase& test_case) {
  WriteLine(PW_UNIT_TEST_GOOGLETEST_CASE_START,
            test_case.suite_name,
            test_case.test_name);
}

void GoogleTestStyleEventHandler::TestCaseEnd(const TestCase& test_case,
                                              TestResult result) {
  // Use a switch with no default to detect changes in the test result enum.
  switch (result) {
    case TestResult::kSuccess:
      WriteLine(PW_UNIT_TEST_GOOGLETEST_CASE_OK,
                test_case.suite_name,
                test_case.test_name);
      break;
    case TestResult::kFailure:
      WriteLine(PW_UNIT_TEST_GOOGLETEST_CASE_FAILED,
                test_case.suite_name,
                test_case.test_name);
      break;
    case TestResult::kSkipped:
      WriteLine(PW_UNIT_TEST_GOOGLETEST_CASE_SKIPPED,
                test_case.suite_name,
                test_case.test_name);
      break;
  }
}

void GoogleTestStyleEventHandler::TestCaseExpect(
    const TestCase& test_case, const TestExpectation& expectation) {
  if (!verbose_ && expectation.success) {
    return;
  }

  const char* result = expectation.success ? "Success" : "Failure";
  WriteLine("%s:%d: %s", test_case.file_name, expectation.line_number, result);
  WriteLine("      Expected: %s", expectation.expression);

  Write("        Actual: ");
  WriteLine("%s", expectation.evaluated_expression);
}

void GoogleTestStyleEventHandler::TestCaseDisabled(const TestCase& test) {
  if (verbose_) {
    WriteLine("Skipping disabled test %s.%s", test.suite_name, test.test_name);
  }
}

}  // namespace unit_test
}  // namespace pw
