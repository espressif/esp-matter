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

namespace pw::perf_test {

// The data that will be reported on completion of an iteration.
struct IterationResult {
  int64_t number;
  int64_t result;
};

// The data that will be reported upon the completion of an iteration.
struct Results {
  int64_t mean;
  int64_t max;
  int64_t min;
  int iterations;
};

// Stores information on the upcoming collection of tests.
struct TestRunInfo {
  int total_tests;
  int default_iterations;
};

struct TestCase {
  const char* name;
};

// This is a declaration of the base EventHandler class. An EventHandler
// collects and reports test results. Both the state and the framework classes
// use their functions to report what happens at each stage
class EventHandler {
 public:
  virtual ~EventHandler() = default;

  // Called before all tests are run
  virtual void RunAllTestsStart(const TestRunInfo& summary) = 0;

  // Called after all tests are run
  virtual void RunAllTestsEnd() = 0;

  // Called when a new performance test is started
  virtual void TestCaseStart(const TestCase& info) = 0;

  // Called to output the results of an iteration
  virtual void TestCaseIteration(const IterationResult& result) = 0;

  // Called after a performance test ends
  virtual void TestCaseEnd(const TestCase& info, const Results& end_result) = 0;
};

}  // namespace pw::perf_test
