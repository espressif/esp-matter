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
#define PW_LOG_MODULE_NAME "pw_perf_test"
#define PW_LOG_LEVEL PW_LOG_LEVEL_INFO

#include "pw_perf_test/perf_test.h"

#include <cstdint>

#include "pw_log/log.h"
#include "pw_perf_test/event_handler.h"
#include "pw_perf_test/internal/timer.h"

namespace pw::perf_test {
namespace internal {

Framework Framework::framework_;

int Framework::RunAllTests() {
  if (!internal::TimerPrepare()) {
    return false;
  }

  event_handler_->RunAllTestsStart(run_info_);

  for (const TestInfo* test = tests_; test != nullptr; test = test->next()) {
    State test_state =
        CreateState(kDefaultIterations, *event_handler_, test->test_name());
    test->Run(test_state);
  }
  internal::TimerCleanup();
  event_handler_->RunAllTestsEnd();
  return true;
}

void Framework::RegisterTest(TestInfo& new_test) {
  ++run_info_.total_tests;
  if (tests_ == nullptr) {
    tests_ = &new_test;
    return;
  }
  TestInfo* info = tests_;
  for (; info->next() != nullptr; info = info->next()) {
  }
  info->SetNext(&new_test);
}

State CreateState(int durations,
                  EventHandler& event_handler,
                  const char* test_name) {
  return State(durations, event_handler, test_name);
}
}  // namespace internal

bool State::KeepRunning() {
  internal::Timestamp iteration_end = internal::GetCurrentTimestamp();
  if (current_iteration_ == -1) {
    ++current_iteration_;
    event_handler_->TestCaseStart(test_info);
    iteration_start_ = internal::GetCurrentTimestamp();
    return true;
  }
  int64_t duration = internal::GetDuration(iteration_start_, iteration_end);
  if (duration > max_) {
    max_ = duration;
  }
  if (duration < min_) {
    min_ = duration;
  }
  total_duration_ += duration;
  ++current_iteration_;
  PW_LOG_DEBUG("Iteration number: %d - Duration: %ld",
               current_iteration_,
               static_cast<long>(duration));
  event_handler_->TestCaseIteration({current_iteration_, duration});
  if (current_iteration_ == test_iterations_) {
    PW_LOG_DEBUG("Total Duration: %ld  Total Iterations: %d",
                 static_cast<long>(total_duration_),
                 test_iterations_);
    mean_ = total_duration_ / test_iterations_;
    PW_LOG_DEBUG("Mean: %ld: ", static_cast<long>(mean_));
    PW_LOG_DEBUG("Minimum: %ld", static_cast<long>(min_));
    PW_LOG_DEBUG("Maxmimum: %ld", static_cast<long>(max_));
    event_handler_->TestCaseEnd(test_info,
                                Results{mean_, max_, min_, test_iterations_});
    return false;
  }
  iteration_start_ = internal::GetCurrentTimestamp();
  return true;
}

void RunAllTests(EventHandler& handler) {
  internal::Framework::Get().RegisterEventHandler(handler);
  internal::Framework::Get().RunAllTests();
}

}  // namespace pw::perf_test
