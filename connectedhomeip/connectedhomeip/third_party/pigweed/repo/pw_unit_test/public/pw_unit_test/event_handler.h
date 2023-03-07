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

namespace pw {
namespace unit_test {

// This file defines the interface through which the pw_unit_test framework
// sends its test data as it runs unit tests. A program wanting to process test
// events must define a class implementing the EventHandler interface and
// register it with the framework. When RUN_ALL_TESTS() is called, pw_unit_test
// will notify the handler of various events which occur in the test process.
//
// For example, consider a file containing the following test definitions:
//
//   TEST(MyTestSuite, MyFirstCase) {
//     EXPECT_TRUE(true);
//   }
//
//   TEST(MyTestSuite, MySecondCase) {
//     EXPECT_TRUE(false);
//   }
//
// In this file, there is one test suite consisting of two test cases.
//
// When pw_unit_test starts running the first test case, it dispatches a
// TestCaseStart event to the event handler. It then runs the body of the test,
// sequentially checking each assertion within. After each assertion, a
// TestCaseExpect event is sent to the event handler with the assertion's
// result. In this case, there is only one, which passes successfully (as
// `true`, is in fact, true). Finally, after the test is finished, a TestCaseEnd
// event is dispatched with the overall result of the test case.
//
// pw_unit_test then runs MySecondCase, still within the same test suite. The
// sequence of events dispatched is the same, except that this TestCaseExpect
// event is marked as a failure. The result passed alongside the TestCaseEnd
// event also indicates that the test case did not complete successfully.

// The result of a complete test run.
enum class TestResult {
  kSuccess = 0,
  kFailure = 1,
  // Test skipped at runtime. This is neither a success nor a failure.
  kSkipped = 2,
};

struct TestCase {
  // Name of the test suite to which this test case belongs.
  const char* suite_name;

  // Name of the test case.
  const char* test_name;

  // Path to the file in which the test case is defined.
  const char* file_name;
};

struct TestExpectation {
  // The source code for the expression which was run.
  const char* expression;

  // The expression with arguments evaluated.
  const char* evaluated_expression;

  // Line number at which the expectation is located.
  int line_number;

  // Whether the expectation succeeded.
  bool success;
};

struct RunTestsSummary {
  // The number of passed tests among the run tests.
  int passed_tests;

  // The number of passed tests among the run tests.
  int failed_tests;

  // The number of tests skipped or filtered out.
  int skipped_tests;

  // The number of disabled tests encountered.
  int disabled_tests;
};

// An event handler is responsible for collecting and processing the results of
// a unit test run. Its interface is called by the unit test framework as tests
// are executed and various test events occur.
class EventHandler {
 public:
  virtual ~EventHandler() = default;

  // Called before all tests are run.
  virtual void RunAllTestsStart() = 0;

  // Called after all tests are run.
  virtual void RunAllTestsEnd(const RunTestsSummary& run_tests_summary) = 0;

  // Called when a new test case is started.
  virtual void TestCaseStart(const TestCase& test_case) = 0;

  // Called when a test case completes. The overall result of the test case is
  // provided.
  virtual void TestCaseEnd(const TestCase& test_case, TestResult result) = 0;

  // Called when a disabled test case is encountered.
  virtual void TestCaseDisabled(const TestCase&) {}

  // Called after each expect/assert statement within a test case with the
  // result of the expectation.
  virtual void TestCaseExpect(const TestCase& test_case,
                              const TestExpectation& expectation) = 0;
};

}  // namespace unit_test
}  // namespace pw
