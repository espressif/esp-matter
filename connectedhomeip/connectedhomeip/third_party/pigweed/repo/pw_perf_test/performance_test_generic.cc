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

#include "pw_perf_test/perf_test.h"

constexpr int kGlobalVariablePerfTest = 4;

namespace pw::perf_test {
namespace {

void SimpleTestingFunction(pw::perf_test::State& state) {
  [[maybe_unused]] int p = 0;
  while (state.KeepRunning()) {
    ++p;
  }
}

void FunctionWithDelay(pw::perf_test::State& state, int a, int b) {
  while (state.KeepRunning()) {
    for (volatile int i = 0; i < a * b * 100000; i = i + 1) {
    }
  }
}

int TestSimple(int a, int b) { return a + b; }

PW_PERF_TEST(IntialTest, SimpleTestingFunction);

PW_PERF_TEST(FunctionWithParameters, FunctionWithDelay, 5, 5);

PW_PERF_TEST(LambdaFunction, [](pw::perf_test::State& state_) {
  FunctionWithDelay(state_, kGlobalVariablePerfTest, 4);
});

PW_PERF_TEST_SIMPLE(SimpleTest, TestSimple, 2, 4);
PW_PERF_TEST_SIMPLE(
    SimpleLambda, [](int a, int b) { return a + b; }, 1, 3);
}  // namespace
}  // namespace pw::perf_test
