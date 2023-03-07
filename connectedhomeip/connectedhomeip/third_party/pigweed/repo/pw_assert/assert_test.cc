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

#include "pw_assert/assert.h"

#include "gtest/gtest.h"

// PW_ASSERT() should always be enabled, and always evaluate the expression.
TEST(Assert, AssertTrue) {
  int evaluated = 1;
  PW_ASSERT(++evaluated);
  EXPECT_EQ(evaluated, 2);
}

// PW_DASSERT() might be disabled sometimes.
TEST(Assert, DebugAssertTrue) {
  int evaluated = 1;
  PW_DASSERT(++evaluated);
  if (PW_ASSERT_ENABLE_DEBUG == 1) {
    EXPECT_EQ(evaluated, 2);
  } else {
    EXPECT_EQ(evaluated, 1);
  }
}

TEST(Assert, AssertOkEvaluatesExpressionAndDoesNotCrashOnOk) {
  int evaluated = 1;
  PW_ASSERT_OK(([&]() {
    ++evaluated;
    return pw::OkStatus();
  })());
  EXPECT_EQ(evaluated, 2);
}

// Unfortunately, we don't have the infrastructure to test failure handling
// automatically, since the harness crashes in the process of running this
// test. The unsatisfying alternative is to test the functionality manually,
// then disable the test.

TEST(Assert, AssertFalse) {
  if (false) {
    PW_ASSERT(false);
  }
}

TEST(Assert, DebugAssertFalse) {
  if (false) {
    PW_DASSERT(false);
  }
}
