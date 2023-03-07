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

#define PW_PERF_TEST_GOOGLESTYLE_RUN_ALL_TESTS_START \
  "[==========] Running all tests."
#define PW_PERF_TEST_GOOGLESTYLE_BEGINNING_SUMMARY \
  "[ PLANNING ] %d test(s) with %d run(s) each."
#define PW_PERF_TEST_GOOGLESTYLE_RUN_ALL_TESTS_END \
  "[==========] Done running all tests."

#define PW_PERF_TEST_GOOGLESTYLE_CASE_START "[ RUN      ] %s"
#define PW_PERF_TEST_GOOGLESTYLE_CASE_RESULT \
  "[  RESULT  ] MEAN: %ld %s, MIN: %ld %s, MAX: %ld %s, ITERATIONS: %d"
#define PW_PERF_TEST_GOOGLESTYLE_CASE_END "[     DONE ] %s"
#define PW_PERF_TEST_GOOGLESTYLE_ITERATION_REPORT "[ Iteration ] #%ld: %ld %s"
