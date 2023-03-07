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

#include "gtest/gtest.h"

namespace pw::unit_test {

extern int test_3_executions_not_expected;
extern int test_4_executions_not_expected;

namespace {

TEST(StaticLibraryArchivedTest, ShouldNotRunTest3) {
  test_3_executions_not_expected += 1;
}

TEST(StaticLibraryArchivedTest, ShouldNotRunTest4) {
  test_4_executions_not_expected += 1;
}

TEST(StaticLibraryArchivedTest, Fails) { FAIL(); }

}  // namespace
}  // namespace pw::unit_test
