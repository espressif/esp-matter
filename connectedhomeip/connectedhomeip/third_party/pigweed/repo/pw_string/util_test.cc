// Copyright 2021 The Pigweed Authors
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

#include "pw_string/util.h"

#include "gtest/gtest.h"

namespace pw::string {
namespace {

using namespace std::literals::string_view_literals;

TEST(ClampedLength, Nullptr_Returns0) {
  EXPECT_EQ(0u, internal::ClampedLength(nullptr, 100));
}

TEST(ClampedLength, EmptyString_Returns0) {
  EXPECT_EQ(0u, internal::ClampedLength("", 0));
  EXPECT_EQ(0u, internal::ClampedLength("", 100));
}

TEST(ClampedLength, MaxLongerThanString_ReturnsStrlen) {
  EXPECT_EQ(5u, internal::ClampedLength("12345", 100));
}

TEST(ClampedLength, StringMaxLongerThanMax_ReturnsMax) {
  EXPECT_EQ(0u, internal::ClampedLength("12345", 0));
  EXPECT_EQ(4u, internal::ClampedLength("12345", 4));
}

TEST(ClampedLength, LengthEqualsMax) {
  EXPECT_EQ(5u, internal::ClampedLength("12345", 5));
}

TEST(ClampedCString, NullPtr_ReturnsEmpty) {
  EXPECT_TRUE(ClampedCString(nullptr, 100).empty());
}

TEST(ClampedCString, EmptyString_Returns0) {
  EXPECT_TRUE(ClampedCString("", 0).empty());
  EXPECT_TRUE(ClampedCString("", 100).empty());
}

TEST(ClampedCString, MaxLongerThanString_ReturnsStr) {
  static constexpr char kInput[] = "12345";
  const std::string_view result = ClampedCString(kInput, 100);
  EXPECT_EQ(result.size(), strlen(kInput));
  EXPECT_EQ(result.data(), &kInput[0]);
}

TEST(ClampedCString, StringMaxLongerThanMax_ClampsView) {
  static constexpr char kInput[] = "12345";

  EXPECT_TRUE(ClampedCString(kInput, 0).empty());

  const std::string_view result = ClampedCString(kInput, 4);
  EXPECT_EQ(result.size(), 4u);
  EXPECT_EQ(result.data(), &kInput[0]);
}

TEST(ClampedCString, FullStringView) {
  static constexpr char kInput[] = "12345";
  const std::string_view result = ClampedCString(kInput);
  EXPECT_EQ(result.size(), strlen(kInput));
  EXPECT_EQ(result.data(), &kInput[0]);
}

TEST(NullTerminatedLength, EmptyString_RequiresNullTerminator) {
  EXPECT_TRUE(NullTerminatedLength("", 0).status().IsOutOfRange());

  ASSERT_TRUE(NullTerminatedLength("", 100).status().ok());
  EXPECT_EQ(0u, NullTerminatedLength("", 100).value());
}

TEST(NullTerminatedLength, MaxLongerThanString_ReturnsStrlen) {
  ASSERT_TRUE(NullTerminatedLength("12345", 100).status().ok());
  EXPECT_EQ(5u, NullTerminatedLength("12345", 100).value());
}

TEST(NullTerminatedLength, StringMaxLongerThanMax_Fails) {
  EXPECT_TRUE(NullTerminatedLength("12345", 0).status().IsOutOfRange());
  EXPECT_TRUE(NullTerminatedLength("12345", 4).status().IsOutOfRange());
}

TEST(NullTerminatedLength, LengthEqualsMax) {
  static constexpr char kInput[] = "12345";
  ASSERT_TRUE(NullTerminatedLength(kInput).ok());
  EXPECT_EQ(5u, NullTerminatedLength(kInput).value());
}

class TestWithBuffer : public ::testing::Test {
 protected:
  static constexpr char kStartingString[] = "!@#$%^&*()!@#$%^&*()";

  TestWithBuffer() { std::memcpy(buffer_, kStartingString, sizeof(buffer_)); }

  char buffer_[sizeof(kStartingString)];
};

class CopyTest : public TestWithBuffer {};

TEST_F(CopyTest, EmptyStringView_WritesNullTerminator) {
  EXPECT_EQ(0u, Copy("", buffer_).size());
  EXPECT_EQ('\0', buffer_[0]);
}

TEST_F(CopyTest, EmptyBuffer_WritesNothing) {
  auto result = Copy("Hello", span(buffer_, 0));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ(kStartingString, buffer_);
}

TEST_F(CopyTest, TooSmall_Truncates) {
  auto result = Copy("Hi!", span(buffer_, 3));
  EXPECT_EQ(2u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("Hi", buffer_);
}

TEST_F(CopyTest, ExactFit) {
  auto result = Copy("Hi!", span(buffer_, 4));
  EXPECT_EQ(3u, result.size());
  EXPECT_TRUE(result.ok());
  EXPECT_STREQ("Hi!", buffer_);
}

TEST_F(CopyTest, NullTerminatorsInString) {
  ASSERT_EQ(4u, Copy("\0!\0\0"sv, span(buffer_, 5)).size());
  EXPECT_EQ("\0!\0\0"sv, std::string_view(buffer_, 4));
}

class InlineStringUtilTest : public ::testing::Test {
 protected:
  InlineString<5> string_;
};

TEST_F(InlineStringUtilTest, Assign_EmptyStringView_WritesNullTerminator) {
  EXPECT_EQ(OkStatus(), Assign(string_, ""));
  EXPECT_EQ(string_, "");
}

TEST_F(InlineStringUtilTest, Assign_EmptyBuffer_WritesNothing) {
  InlineString<0> zero_capacity;
  EXPECT_EQ(Status::ResourceExhausted(), Assign(zero_capacity, "Hello"));
  EXPECT_TRUE(zero_capacity.empty());
  EXPECT_EQ(zero_capacity.c_str()[0], '\0');
}

TEST_F(InlineStringUtilTest, Assign_TooSmall_Truncates) {
  EXPECT_EQ(Status::ResourceExhausted(), Assign(string_, "12345HELLO?"));
  EXPECT_EQ("12345", string_);
}

TEST_F(InlineStringUtilTest, Assign_ExactFit) {
  EXPECT_EQ(OkStatus(), Assign(string_, "12345"));
  EXPECT_EQ("12345", string_);
}

TEST_F(InlineStringUtilTest, Assign_NullTerminatorsInString) {
  EXPECT_EQ(OkStatus(), Assign(string_, "\0!\0\0\0"sv));
  EXPECT_EQ("\0!\0\0\0"sv, string_);
}

TEST_F(InlineStringUtilTest, Assign_ExistingContent_Replaces) {
  string_ = "12345";
  EXPECT_EQ(OkStatus(), Assign(string_, ""));
  EXPECT_EQ("", string_);
}

TEST_F(InlineStringUtilTest, Assign_ExistingContent_ExactFit) {
  string_.append("yo");
  EXPECT_EQ(OkStatus(), Assign(string_, "12345"));
  EXPECT_EQ("12345", string_);
}

TEST_F(InlineStringUtilTest, Assign_ExistingContent_Truncates) {
  string_.append("yo");
  EXPECT_EQ(Status::ResourceExhausted(), Assign(string_, "1234567"));
  EXPECT_EQ("12345", string_);
}

TEST_F(InlineStringUtilTest, Append_EmptyStringView_WritesNullTerminator) {
  EXPECT_EQ(OkStatus(), Append(string_, ""));
  EXPECT_EQ(string_, "");
}

TEST_F(InlineStringUtilTest, Append_EmptyBuffer_WritesNothing) {
  InlineString<0> zero_capacity;
  EXPECT_EQ(Status::ResourceExhausted(), Append(zero_capacity, "Hello"));
  EXPECT_TRUE(zero_capacity.empty());
  EXPECT_EQ(zero_capacity.c_str()[0], '\0');
}

TEST_F(InlineStringUtilTest, Append_TooSmall_Truncates) {
  EXPECT_EQ(Status::ResourceExhausted(), Append(string_, "12345HELLO?"));
  EXPECT_EQ("12345", string_);
}

TEST_F(InlineStringUtilTest, Append_ExactFit) {
  EXPECT_EQ(OkStatus(), Append(string_, "12345"));
  EXPECT_EQ("12345", string_);
}

TEST_F(InlineStringUtilTest, Append_NullTerminatorsInString) {
  EXPECT_EQ(OkStatus(), Append(string_, "\0!\0\0\0"sv));
  EXPECT_EQ("\0!\0\0\0"sv, string_);
}

TEST_F(InlineStringUtilTest, Append_ExistingContent_AppendNothing) {
  string_ = "12345";
  EXPECT_EQ(OkStatus(), Append(string_, ""));
  EXPECT_EQ("12345", string_);
}

TEST_F(InlineStringUtilTest, Append_ExistingContent_ExactFit) {
  string_.append("yo");
  EXPECT_EQ(OkStatus(), Append(string_, "123"));
  EXPECT_EQ("yo123", string_);
}

TEST_F(InlineStringUtilTest, Append_ExistingContent_Truncates) {
  string_.append("yo");
  EXPECT_EQ(Status::ResourceExhausted(), Append(string_, "12345"));
  EXPECT_EQ("yo123", string_);
}

class PrintableCopyTest : public TestWithBuffer {};

TEST_F(PrintableCopyTest, EmptyBuffer_WritesNothing) {
  auto result = PrintableCopy("Hello", span(buffer_, 0));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ(kStartingString, buffer_);
}

TEST_F(PrintableCopyTest, TooSmall_Truncates) {
  auto result = PrintableCopy("Hi!", span(buffer_, 3));
  EXPECT_EQ(2u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("Hi", buffer_);
}

TEST_F(PrintableCopyTest, ExactFit) {
  auto result = PrintableCopy("Hi!", span(buffer_, 4));
  EXPECT_EQ(3u, result.size());
  EXPECT_TRUE(result.ok());
  EXPECT_STREQ("Hi!", buffer_);
}

TEST_F(PrintableCopyTest, StartingString) {
  memset(buffer_, '\0', sizeof(buffer_));
  auto result = PrintableCopy(kStartingString, span(buffer_));
  EXPECT_EQ(sizeof(kStartingString) - 1, result.size());
  EXPECT_TRUE(result.ok());
  EXPECT_STREQ(kStartingString, buffer_);
}

TEST_F(PrintableCopyTest, NullTerminatorsInString) {
  ASSERT_EQ(4u, PrintableCopy("\0!\0\0"sv, span(buffer_, 5)).size());
  EXPECT_STREQ(".!..", buffer_);
}

TEST_F(PrintableCopyTest, ControlCharsInString) {
  ASSERT_EQ(
      14u,
      PrintableCopy("\n!\t\n\x10\x7F\xFF\vabcd\b\r"sv, span(buffer_)).size());
  EXPECT_STREQ(".!......abcd..", buffer_);
}

}  // namespace
}  // namespace pw::string
