// Copyright 2019 The Pigweed Authors
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

#include <cstring>

#include "gtest/gtest.h"
#include "pw_assert/check.h"

namespace pw {
namespace {

TEST(PigweedTest, ExpectBool) {
  EXPECT_TRUE(true);
  EXPECT_FALSE(false);

  EXPECT_TRUE(1);
  EXPECT_TRUE(1203492);
  EXPECT_TRUE(-1);
  EXPECT_TRUE(0.1f);

  EXPECT_FALSE(0);
  EXPECT_FALSE(0.0f);
  EXPECT_FALSE(-0.0f);
}

TEST(PigweedTest, ExpectBasicComparisons) {
  EXPECT_EQ(1, 1 + 0);
  ASSERT_EQ(1, 1 + 0);

  EXPECT_EQ(0.0f, -0.0f);
  ASSERT_EQ(0.0f, -0.0f);

  EXPECT_NE(-1, 0);
  ASSERT_NE(-1, 0);

  EXPECT_GT(2, 1);
  ASSERT_GT(3, 0);

  EXPECT_GE(1, 1);
  ASSERT_GE(3, 0);

  EXPECT_LT(0, 1);
  ASSERT_LT(-2, 1209);

  EXPECT_LE(-1, 0);
  ASSERT_LE(-2, -2);
}

TEST(PigweedTest, ExpectStringEquality) {
  EXPECT_STREQ("", "");
  EXPECT_STREQ("Yes", "Yes");

  char no[] = {'N', 'o', '\0'};
  ASSERT_STREQ("No", no);

  EXPECT_STRNE("NO", "no");
  ASSERT_STRNE("yes", no);
}

TEST(PigweedTest, SucceedAndFailMacros) {
  SUCCEED();

  // The ADD_FAILURE() and FAIL() macros cause a test to fail if they are
  // reached. Use them, but don't let them run so that this test still passes.
  if (false) {
    ADD_FAILURE();
    FAIL();
  }
}

TEST(PigweedTest, SkipMacro) {
  GTEST_SKIP();
  // This code should not run.
  EXPECT_TRUE(false);
}

class SkipOnSetUpTest : public ::testing::Test {
 public:
  void SetUp() override { GTEST_SKIP(); }
};

TEST_F(SkipOnSetUpTest, FailTest) {
  // This code should not run because the test was skipped in SetUp().
  EXPECT_TRUE(false);
}

class NonCopyable {
 public:
  NonCopyable(int value) : value_(value) {}

  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;

  bool operator==(const NonCopyable& rhs) const { return value_ == rhs.value_; }
  bool operator!=(const NonCopyable& rhs) const { return value_ != rhs.value_; }

  operator bool() const { return value_ > 0; }

 private:
  const int value_;
};

TEST(PigweedTest, NonCopyableType) {
  EXPECT_TRUE(NonCopyable(6));
  EXPECT_FALSE(NonCopyable(-1));

  const NonCopyable this_one(100);
  EXPECT_EQ(this_one, this_one);
  EXPECT_TRUE(this_one);

  EXPECT_EQ(NonCopyable(5), NonCopyable(5));
  EXPECT_NE(NonCopyable(5), NonCopyable(6));
}

bool Increment(int* i) {
  (*i)++;
  return true;
}

TEST(PigweedTest, MacroArgumentsOnlyAreEvaluatedOnce) {
  int i = 1;

  EXPECT_TRUE(Increment(&i));
  EXPECT_EQ(i, 2);
  ASSERT_TRUE(Increment(&i));
  EXPECT_EQ(i, 3);

  EXPECT_EQ(0x600dbeef, [&i]() {
    i += 1;
    return 0x600dbeef;
  }());

  EXPECT_EQ(i, 4);
}

class FixtureTest : public ::testing::Test {
 public:
  FixtureTest() : string_("hello world") {}

  bool ReturnTrue() { return true; }
  int StringLength() { return std::strlen(string_); }

 protected:
  const char* string_;
};

TEST_F(FixtureTest, CustomFixture) {
  EXPECT_TRUE(ReturnTrue());
  EXPECT_EQ(StringLength(), 11);
}

class PigweedTestFixture : public ::testing::Test {
 protected:
  PigweedTestFixture() : cool_number_(35) {}

  int cool_number_;
};

TEST_F(PigweedTestFixture, TheNumberIs35) {
  EXPECT_EQ(cool_number_, 35);
  cool_number_ += 1;
  EXPECT_EQ(cool_number_, 36);
}

TEST_F(PigweedTestFixture, YupTheNumberIs35) {
  EXPECT_EQ(cool_number_, 35);
  cool_number_ *= 100;
  EXPECT_EQ(cool_number_, 3500);
}

class Expectations : public ::testing::Test {
 protected:
  Expectations() : cool_number_(3) { PW_CHECK_INT_EQ(cool_number_, 3); }

  ~Expectations() override { PW_CHECK_INT_EQ(cool_number_, 14159); }

  int cool_number_;
};

TEST_F(Expectations, SetCoolNumber) { cool_number_ = 14159; }

class SetUpAndTearDown : public ::testing::Test {
 protected:
  SetUpAndTearDown() : value_(0) { EXPECT_EQ(value_, 0); }

  ~SetUpAndTearDown() override { EXPECT_EQ(value_, 1); }

  void SetUp() override { value_ = 1337; }

  void TearDown() override { value_ = 1; }

  int value_;
};

TEST_F(SetUpAndTearDown, MakeSureItIsSet) {
  EXPECT_EQ(value_, 1337);
  value_ = 3210;
}

}  // namespace
}  // namespace pw
