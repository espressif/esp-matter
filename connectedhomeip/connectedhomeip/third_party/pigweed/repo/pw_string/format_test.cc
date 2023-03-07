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

#include "pw_string/format.h"

#include <cstdarg>

#include "gtest/gtest.h"
#include "pw_span/span.h"

namespace pw::string {
namespace {

TEST(Format, ValidFormatString_Succeeds) {
  char buffer[32];
  auto result = Format(buffer, "-_-");

  EXPECT_EQ(OkStatus(), result.status());
  EXPECT_EQ(3u, result.size());
  EXPECT_STREQ("-_-", buffer);
}

TEST(Format, ValidFormatStringAndArguments_Succeeds) {
  char buffer[32];
  auto result = Format(buffer, "%d4%s", 123, "5");

  EXPECT_EQ(OkStatus(), result.status());
  EXPECT_EQ(5u, result.size());
  EXPECT_STREQ("12345", buffer);
}

TEST(Format, EmptyBuffer_ReturnsResourceExhausted) {
  auto result = Format(span<char>(), "?");

  EXPECT_EQ(Status::ResourceExhausted(), result.status());
  EXPECT_EQ(0u, result.size());
}

TEST(Format, FormatLargerThanBuffer_ReturnsResourceExhausted) {
  char buffer[5];
  auto result = Format(buffer, "2big!");

  EXPECT_EQ(Status::ResourceExhausted(), result.status());
  EXPECT_EQ(4u, result.size());
  EXPECT_STREQ("2big", buffer);
}

TEST(Format, ArgumentLargerThanBuffer_ReturnsResourceExhausted) {
  char buffer[5];
  auto result = Format(buffer, "%s", "2big!");

  EXPECT_EQ(Status::ResourceExhausted(), result.status());
  EXPECT_EQ(4u, result.size());
  EXPECT_STREQ("2big", buffer);
}

StatusWithSize CallFormatWithVaList(span<char> buffer, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  StatusWithSize result = FormatVaList(buffer, fmt, args);

  va_end(args);
  return result;
}

TEST(Format, CallFormatWithVaList_CallsCorrectFormatOverload) {
  char buffer[8];
  auto result = CallFormatWithVaList(buffer, "Yo%s", "?!");

  EXPECT_EQ(OkStatus(), result.status());
  EXPECT_EQ(4u, result.size());
  EXPECT_STREQ("Yo?!", buffer);
}

}  // namespace
}  // namespace pw::string
