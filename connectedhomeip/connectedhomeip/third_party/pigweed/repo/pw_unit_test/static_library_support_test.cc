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

#include "pw_unit_test/static_library_support.h"

#include "pw_assert/check.h"

namespace pw::unit_test {

// Refer to one test in static_library_archived_tests.cc. Do not refer to any
// tests in static_library_missing_archived_tests.cc, though; those tests are
// expected to be lost because they are not referenced.
PW_UNIT_TEST_LINK_FILE_CONTAINING_TEST(StaticLibraryArchivedTest, Test1);

// Count the number of times each test executes.
int test_1_executions = 0;
int test_2_executions = 0;

int test_3_executions_not_expected = 0;
int test_4_executions_not_expected = 0;

class CheckThatTestsRanWhenDestructed {
 public:
  ~CheckThatTestsRanWhenDestructed() {
    PW_CHECK_INT_EQ(test_1_executions, 1);
    PW_CHECK_INT_EQ(test_2_executions, 1);

    PW_CHECK_INT_EQ(test_3_executions_not_expected, 0);
    PW_CHECK_INT_EQ(test_4_executions_not_expected, 0);
  }
} check_that_tests_ran;

// Test that linking fails if these macros refer to tests that do not exist.
// These cannot be a negative compilation tests because they compile
// successfully, but fail to link.
#if 0

PW_UNIT_TEST_LINK_FILE_CONTAINING_TEST(NotARealSuite, NotARealTest);

#elif 0

PW_UNIT_TEST_LINK_FILE_CONTAINING_TEST(StaticLibraryArchivedTest, NotARealTest);

#endif  // negative linking tests

}  // namespace pw::unit_test
