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

#include "pw_bluetooth/result.h"

#include "gtest/gtest.h"

namespace pw::bluetooth {
namespace {

enum class TestError { kFailure0, kFailure1 };

TEST(ResultTest, NoValue) {
  EXPECT_TRUE(Result<TestError>().ok());

  Result<TestError> error_result(TestError::kFailure0);
  ASSERT_FALSE(error_result.ok());
  EXPECT_EQ(error_result.error(), TestError::kFailure0);
}

TEST(ResultTest, Value) {
  Result<TestError, int> ok_result(42);
  ASSERT_TRUE(ok_result.ok());
  EXPECT_EQ(ok_result.value(), 42);

  Result<TestError, int> error_result(TestError::kFailure1);
  ASSERT_FALSE(error_result.ok());
  EXPECT_EQ(error_result.error(), TestError::kFailure1);
}

struct NonTrivialDestructor {
  explicit NonTrivialDestructor(int* destructor_counter)
      : destructor_counter_(destructor_counter) {}
  ~NonTrivialDestructor() {
    EXPECT_NE(nullptr, destructor_counter_);
    (*destructor_counter_)++;
  }
  NonTrivialDestructor(const NonTrivialDestructor&) = delete;
  NonTrivialDestructor(NonTrivialDestructor&&) = delete;

  int* destructor_counter_;
};

TEST(ResultTest, NonTrivialDestructorTest) {
  int counter = 0;
  {
    Result<TestError, NonTrivialDestructor> ret(std::in_place, &counter);
    ASSERT_TRUE(ret.ok());
  }
  EXPECT_EQ(counter, 1);
  {
    Result<TestError, NonTrivialDestructor> ret(TestError::kFailure0);
    ASSERT_FALSE(ret.ok());
  }
  EXPECT_EQ(counter, 1);
}

}  // namespace
}  // namespace pw::bluetooth
