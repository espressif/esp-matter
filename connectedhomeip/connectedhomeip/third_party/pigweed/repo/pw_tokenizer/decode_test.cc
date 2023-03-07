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

#include "pw_tokenizer/internal/decode.h"

#include <cstdio>
#include <string>
#include <string_view>

#include "gtest/gtest.h"
#include "pw_tokenizer_private/tokenized_string_decoding_test_data.h"
#include "pw_tokenizer_private/varint_decoding_test_data.h"
#include "pw_varint/varint.h"

#define ERR PW_TOKENIZER_ARG_DECODING_ERROR

namespace pw::tokenizer {
namespace {

using namespace std::literals::string_view_literals;

const FormatString kOneArg("Hello %s");
const FormatString kTwoArgs("The %d %s");

const bool kSupportsC99Printf = []() {
  char buf[16] = {};
  std::snprintf(buf, sizeof(buf), "%zu", sizeof(char));
  return buf[0] == '1' && buf[1] == '\0';
}();

const bool kSupportsFloatPrintf = []() {
  char buf[16] = {};
  std::snprintf(buf, sizeof(buf), "%.1f", 3.5f);
  return buf[0] == '3' && buf[1] == '.' && buf[2] == '5' && buf[3] == '\0';
}();

bool FormatIsSupported(std::string_view value) {
  return (kSupportsC99Printf || (value.find("%hh") == std::string_view::npos &&
                                 value.find("%ll") == std::string_view::npos &&
                                 value.find("%j") == std::string_view::npos &&
                                 value.find("%z") == std::string_view::npos &&
                                 value.find("%t") == std::string_view::npos)) &&
         // To make absolutely certain there are no floating point numbers, skip
         // anything with an f or e in it.
         (kSupportsFloatPrintf || (value.find('f') == std::string_view::npos &&
                                   value.find('e') == std::string_view::npos));
}

TEST(TokenizedStringDecode, TokenizedStringDecodingTestCases) {
  const auto& test_data = test::tokenized_string_decoding::kTestData;
  static_assert(sizeof(test_data) / sizeof(*test_data) > 100u);

  size_t skipped = 0;

  for (const auto& [format, expected, args] : test_data) {
    if (FormatIsSupported(format)) {
      ASSERT_EQ(FormatString(format).Format(args).value_with_errors(),
                expected);
    } else {
      skipped += 1;
    }
  }

  if (sizeof(void*) == 8) {  // 64-bit systems should have full snprintf.
    ASSERT_EQ(skipped, 0u);
  }
}

TEST(TokenizedStringDecode, FullyDecodeInput_ZeroRemainingBytes) {
  auto result = kOneArg.Format("\5hello");
  EXPECT_EQ(result.value(), "Hello hello");
  EXPECT_EQ(result.remaining_bytes(), 0u);
  EXPECT_EQ(result.decoding_errors(), 0u);
}

TEST(TokenizedStringDecode, PartiallyDecodeInput_HasRemainingBytes) {
  auto result = kOneArg.Format("\5helloworld");
  EXPECT_EQ(result.value(), "Hello hello");
  EXPECT_EQ(result.remaining_bytes(), 5u);
  EXPECT_EQ(result.decoding_errors(), 0u);
}

TEST(TokenizedStringDecode, Truncation_NotAnError) {
  auto result = kTwoArgs.Format("\6\x89musketeer");
  EXPECT_EQ(result.value(), "The 3 musketeer[...]");
  EXPECT_EQ(result.value_with_errors(), "The 3 musketeer[...]");
  EXPECT_EQ(result.remaining_bytes(), 0u);
  EXPECT_EQ(result.decoding_errors(), 0u);
}

TEST(TokenizedStringDecode, WrongStringLenth_IsErrorAndConsumesRestOfString) {
  auto result = kTwoArgs.Format("\6\x0amusketeer");
  EXPECT_EQ(result.value(), "The 3 %s");
  EXPECT_EQ(result.value_with_errors(), "The 3 " ERR("%s ERROR (musketeer)"));
  EXPECT_EQ(result.remaining_bytes(), 0u);
  EXPECT_EQ(result.decoding_errors(), 1u);
}

TEST(TokenizedStringDecode, UnterminatedVarint_IsError) {
  auto result = kTwoArgs.Format("\x80");
  EXPECT_EQ(result.value(), "The %d %s");
  EXPECT_EQ(result.value_with_errors(),
            "The " ERR("%d ERROR") " " ERR("%s SKIPPED"));
  EXPECT_EQ(result.remaining_bytes(), 0u);
  EXPECT_EQ(result.decoding_errors(), 2u);
}

TEST(TokenizedStringDecode, UnterminatedVarint_ConsumesUpToMaxVarintSize) {
  std::string_view data = "\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80";
  auto result = kTwoArgs.Format(data);
  EXPECT_EQ(result.value(), "The %d %s");
  EXPECT_EQ(result.value_with_errors(),
            "The " ERR("%d ERROR") " " ERR("%s SKIPPED"));
  EXPECT_EQ(result.remaining_bytes(),
            data.size() - varint::kMaxVarint64SizeBytes);
  EXPECT_EQ(result.decoding_errors(), 2u);
}

TEST(TokenizedStringDecode, MissingArguments_IsDecodeError) {
  auto result = kTwoArgs.Format("");
  EXPECT_EQ(result.value(), "The %d %s");
  EXPECT_EQ(result.value_with_errors(),
            "The " ERR("%d MISSING") " " ERR("%s SKIPPED"));
  EXPECT_EQ(result.remaining_bytes(), 0u);
  EXPECT_EQ(result.decoding_errors(), 2u);
}

TEST(VarintDecode, VarintDecodeTestCases) {
  const auto& test_data = test::varint_decoding::kTestData;
  static_assert(sizeof(test_data) / sizeof(*test_data) > 100u);

  size_t skipped = 0;

  for (const auto& [d_fmt, d_expected, u_fmt, u_expected, data] : test_data) {
    if (FormatIsSupported(d_fmt)) {
      ASSERT_EQ(FormatString(d_fmt).Format(data).value(), d_expected);
      ASSERT_EQ(FormatString(u_fmt).Format(data).value(), u_expected);
    } else {
      skipped += 1;
    }

    if (sizeof(void*) == 8) {  // 64-bit systems should have full snprintf.
      ASSERT_EQ(skipped, 0u);
    }
  }
}

class DecodedFormatStringTest : public ::testing::Test {
 protected:
  DecodedFormatStringTest()
      : no_args_("Give 110%% sometimes"),
        one_arg_("so you can give %d%%"),
        two_args_("%d.%d%% of the time") {}

  const FormatString no_args_;
  const FormatString one_arg_;
  const FormatString two_args_;
};

TEST_F(DecodedFormatStringTest, Value) {
  EXPECT_EQ("Give 110% sometimes", no_args_.Format("").value());
  EXPECT_EQ("so you can give 0%", one_arg_.Format("\0"sv).value());
  EXPECT_EQ("90.9% of the time", two_args_.Format("\xB4\x01\x12").value());
}

TEST_F(DecodedFormatStringTest, FormatSuccessfully_IsOk) {
  EXPECT_TRUE(no_args_.Format("").ok());
  EXPECT_TRUE(one_arg_.Format("\0"sv).ok());
  EXPECT_TRUE(two_args_.Format("\xB4\x01\x12").ok());
}

TEST_F(DecodedFormatStringTest, FormatWithDecodingErrors_IsNotOkay) {
  EXPECT_FALSE(one_arg_.Format("").ok());
  EXPECT_FALSE(two_args_.Format("\x80").ok());
}

TEST_F(DecodedFormatStringTest, FormatWithRemainingBytes_IsNotOkay) {
  EXPECT_FALSE(no_args_.Format("Hello").ok());
  EXPECT_FALSE(one_arg_.Format("\0\0"sv).ok());
  EXPECT_FALSE(two_args_.Format("\xB4\x01\x12?").ok());
}

TEST_F(DecodedFormatStringTest, FormatWithRemainingBytesAndError_IsNotOkay) {
  EXPECT_FALSE(
      one_arg_.Format("\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x00"sv).ok());
}

TEST_F(DecodedFormatStringTest, RemainingBytes_NoData_IsZero) {
  EXPECT_EQ(0u, no_args_.Format("").remaining_bytes());
  EXPECT_EQ(0u, one_arg_.Format("").remaining_bytes());
  EXPECT_EQ(0u, two_args_.Format("").remaining_bytes());
}

TEST_F(DecodedFormatStringTest, RemainingBytes_WithData_MatchesNumberOfArgs) {
  EXPECT_EQ(2u, no_args_.Format("\x02\x02").remaining_bytes());
  EXPECT_EQ(1u, one_arg_.Format("\x02\x02").remaining_bytes());
  EXPECT_EQ(0u, two_args_.Format("\x02\x02").remaining_bytes());
}

TEST_F(DecodedFormatStringTest, ArgumentCount_NoData_MatchesNumberOfArgs) {
  EXPECT_EQ(0u, no_args_.Format("").argument_count());
  EXPECT_EQ(1u, one_arg_.Format("").argument_count());
  EXPECT_EQ(2u, two_args_.Format("").argument_count());
}

TEST_F(DecodedFormatStringTest, ArgumentCount_WithData_MatchesNumberOfArgs) {
  EXPECT_EQ(0u, no_args_.Format("\x02\x02").argument_count());
  EXPECT_EQ(1u, one_arg_.Format("\x02\x02").argument_count());
  EXPECT_EQ(2u, two_args_.Format("\x02\x02").argument_count());
}

TEST_F(DecodedFormatStringTest, DecodingErrors_NoData_MatchesArgumentCount) {
  EXPECT_EQ(0u, no_args_.Format("").decoding_errors());
  EXPECT_EQ(1u, one_arg_.Format("").decoding_errors());
  EXPECT_EQ(2u, two_args_.Format("").decoding_errors());
}

TEST_F(DecodedFormatStringTest, DecodingErrors_OneArg_AllRemainingAreErrors) {
  EXPECT_EQ(0u, no_args_.Format("\x02").decoding_errors());
  EXPECT_EQ(0u, one_arg_.Format("\x02").decoding_errors());
  EXPECT_EQ(1u, two_args_.Format("\x02").decoding_errors());
}

TEST_F(DecodedFormatStringTest, DecodingErrors_TwoArgs_IsZero) {
  EXPECT_EQ(0u, no_args_.Format("\x02\x02").decoding_errors());
  EXPECT_EQ(0u, one_arg_.Format("\x02\x02").decoding_errors());
  EXPECT_EQ(0u, two_args_.Format("\x02\x02").decoding_errors());
}

}  // namespace
}  // namespace pw::tokenizer
