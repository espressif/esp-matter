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
#include <limits>

#include "pw_assert/assert.h"
#include "pw_perf_test/event_handler.h"
#include "pw_perf_test/internal/duration_unit.h"
#include "pw_perf_test/internal/timer.h"

#define PW_PERF_TEST(Name, Function, ...)                  \
  ::pw::perf_test::internal::TestInfo PwPerfTest_##Name(   \
      #Name, [](::pw::perf_test::State& state) {           \
        static_cast<void>(Function(state, ##__VA_ARGS__)); \
      });                                                  \
  static_assert(true, "Perftest calls must end with a semicolon")

#define PW_PERF_TEST_SIMPLE(name, /*function,*/...) \
  PW_PERF_TEST(name, ::pw::perf_test::internal::RunSimpleFunction, __VA_ARGS__)

namespace pw::perf_test {

class State;

namespace internal {

class TestInfo;

// Allows access to the private State object constructor
State CreateState(int durations,
                  EventHandler& event_handler,
                  const char* test_name);

class Framework {
 public:
  constexpr Framework()
      : event_handler_(nullptr),
        tests_(nullptr),
        run_info_{.total_tests = 0, .default_iterations = kDefaultIterations} {}
  static Framework& Get() { return framework_; }

  void RegisterEventHandler(EventHandler& event_handler) {
    event_handler_ = &event_handler;
  }

  void RegisterTest(TestInfo&);

  int RunAllTests();

 private:
  static constexpr int kDefaultIterations = 10;

  EventHandler* event_handler_;

  // Pointer to the list of tests
  TestInfo* tests_;

  TestRunInfo run_info_;

  static Framework framework_;
};

class TestInfo {
 public:
  constexpr TestInfo(const char* test_name, void (*function_body)(State&))
      : run_(function_body), test_name_(test_name) {
    // Once a TestInfo object is created by the macro, this adds itself to the
    // list of registered tests
    Framework::Get().RegisterTest(*this);
  }

  // Returns the next registered test
  TestInfo* next() const { return next_; }

  void SetNext(TestInfo* next) { next_ = next; }

  void Run(State& state) const { run_(state); }

  const char* test_name() const { return test_name_; }

 private:
  // Function pointer to the code that will be measured
  void (*run_)(State&);

  // Intrusively linked list, this acts as a pointer to the next test
  TestInfo* next_ = nullptr;

  const char* test_name_;
};

}  // namespace internal

class State {
 public:
  // KeepRunning() should be called in a while loop. Responsible for managing
  // iterations and timestamps.
  bool KeepRunning();

 private:
  // Allows the framework to create state objects and unit tests for the state
  // class
  friend State internal::CreateState(int durations,
                                     EventHandler& event_handler,
                                     const char* test_name);

  // Privated constructor to prevent unauthorized instances of the state class.
  constexpr State(int iterations,
                  EventHandler& event_handler,
                  const char* test_name)
      : mean_(-1),
        test_iterations_(iterations),
        total_duration_(0),
        min_(std::numeric_limits<int64_t>::max()),
        max_(std::numeric_limits<int64_t>::min()),
        iteration_start_(),
        current_iteration_(-1),
        event_handler_(&event_handler),
        test_info{.name = test_name} {
    PW_ASSERT(test_iterations_ > 0);
  }
  // Set public after deciding how exactly to set user-defined iterations
  void SetIterations(int iterations) {
    PW_ASSERT(current_iteration_ == -1);
    test_iterations_ = iterations;
    PW_ASSERT(test_iterations_ > 0);
  }

  int64_t mean_;

  // Stores the total number of iterations wanted
  int test_iterations_;

  // Stores the total duration of the tests.
  int64_t total_duration_;

  // Smallest value of the iterations
  int64_t min_;

  // Largest value of the iterations
  int64_t max_;

  // Time at the start of the iteration
  internal::Timestamp iteration_start_;

  // The current iteration
  int current_iteration_;

  EventHandler* event_handler_;

  TestCase test_info;
};

namespace internal {

template <typename Function, typename... Args>
void RunSimpleFunction(::pw::perf_test::State& state,
                       Function function,
                       const Args&... args) {
  while (state.KeepRunning()) {
    static_cast<void>(function(args...));
  }
}

}  // namespace internal

void RunAllTests(pw::perf_test::EventHandler& handler);

}  // namespace pw::perf_test
