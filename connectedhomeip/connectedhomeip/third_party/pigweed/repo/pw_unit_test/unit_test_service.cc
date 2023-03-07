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

#include "pw_unit_test/unit_test_service.h"

#include "gtest/gtest.h"
#include "pw_containers/vector.h"
#include "pw_log/log.h"
#include "pw_protobuf/decoder.h"

namespace pw::unit_test {

void UnitTestService::Run(ConstByteSpan request, RawServerWriter& writer) {
  writer_ = std::move(writer);
  verbose_ = false;

  // List of test suite names to run. The string views in this vector point to
  // data in the raw protobuf request message, so it is only valid for the
  // duration of this function.
  pw::Vector<std::string_view, 16> suites_to_run;

  protobuf::Decoder decoder(request);

  Status status;
  while ((status = decoder.Next()).ok()) {
    switch (static_cast<TestRunRequest::Fields>(decoder.FieldNumber())) {
      case TestRunRequest::Fields::REPORT_PASSED_EXPECTATIONS:
        decoder.ReadBool(&verbose_)
            .IgnoreError();  // TODO(b/242598609): Handle Status properly
        break;

      case TestRunRequest::Fields::TEST_SUITE: {
        std::string_view suite_name;
        if (!decoder.ReadString(&suite_name).ok()) {
          break;
        }

        if (!suites_to_run.full()) {
          suites_to_run.push_back(suite_name);
        } else {
          PW_LOG_ERROR("Maximum of %u test suite filters supported",
                       static_cast<unsigned>(suites_to_run.max_size()));
          writer_.Finish(Status::InvalidArgument())
              .IgnoreError();  // TODO(b/242598609): Handle Status properly
          return;
        }

        break;
      }
    }
  }

  if (status != Status::OutOfRange()) {
    writer_.Finish(status)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    return;
  }

  PW_LOG_INFO("Starting unit test run");
  handler_.ExecuteTests(suites_to_run);
  PW_LOG_INFO("Unit test run complete");

  writer_.Finish().IgnoreError();  // TODO(b/242598609): Handle Status properly
}

void UnitTestService::WriteTestRunStart() {
  // Write out the key for the start field (even though the message is empty).
  WriteEvent(
      [&](Event::StreamEncoder& event) { event.GetTestRunStartEncoder(); });
}

void UnitTestService::WriteTestRunEnd(const RunTestsSummary& summary) {
  WriteEvent([&](Event::StreamEncoder& event) {
    TestRunEnd::StreamEncoder test_run_end = event.GetTestRunEndEncoder();
    test_run_end.WritePassed(summary.passed_tests)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    test_run_end.WriteFailed(summary.failed_tests)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    test_run_end.WriteSkipped(summary.skipped_tests)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    test_run_end.WriteDisabled(summary.disabled_tests)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  });
}

void UnitTestService::WriteTestCaseStart(const TestCase& test_case) {
  WriteEvent([&](Event::StreamEncoder& event) {
    TestCaseDescriptor::StreamEncoder descriptor =
        event.GetTestCaseStartEncoder();
    descriptor.WriteSuiteName(test_case.suite_name)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    descriptor.WriteTestName(test_case.test_name)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    descriptor.WriteFileName(test_case.file_name)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  });
}

void UnitTestService::WriteTestCaseEnd(TestResult result) {
  WriteEvent([&](Event::StreamEncoder& event) {
    event.WriteTestCaseEnd(static_cast<TestCaseResult>(result))
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  });
}

void UnitTestService::WriteTestCaseDisabled(const TestCase& test_case) {
  WriteEvent([&](Event::StreamEncoder& event) {
    TestCaseDescriptor::StreamEncoder descriptor =
        event.GetTestCaseDisabledEncoder();
    descriptor.WriteSuiteName(test_case.suite_name)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    descriptor.WriteTestName(test_case.test_name)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    descriptor.WriteFileName(test_case.file_name)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  });
}

void UnitTestService::WriteTestCaseExpectation(
    const TestExpectation& expectation) {
  if (!verbose_ && expectation.success) {
    return;
  }

  WriteEvent([&](Event::StreamEncoder& event) {
    TestCaseExpectation::StreamEncoder test_case_expectation =
        event.GetTestCaseExpectationEncoder();
    test_case_expectation.WriteExpression(expectation.expression)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    test_case_expectation
        .WriteEvaluatedExpression(expectation.evaluated_expression)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    test_case_expectation.WriteLineNumber(expectation.line_number)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    test_case_expectation.WriteSuccess(expectation.success)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
  });
}

}  // namespace pw::unit_test
