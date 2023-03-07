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

#include "pw_string/type_to_string.h"

#include <cmath>
#include <cstring>
#include <limits>
#include <string_view>

#include "gtest/gtest.h"

namespace pw::string {
namespace {

TEST(Digits, DecimalDigits_AllOneDigit) {
  for (uint64_t i = 0; i < 10; ++i) {
    ASSERT_EQ(1u, DecimalDigitCount(i));
  }
}

TEST(Digits, DecimalDigits_AllTwoDigit) {
  for (uint64_t i = 10; i < 100u; ++i) {
    ASSERT_EQ(2u, DecimalDigitCount(i));
  }
}

TEST(Digits, DecimalDigits_1To19Digits) {
  uint64_t value = 1;
  for (unsigned digits = 1; digits <= 19u; ++digits) {
    ASSERT_EQ(digits, DecimalDigitCount(value));
    ASSERT_EQ(digits, DecimalDigitCount(value + 1));

    value *= 10;
    ASSERT_EQ(digits, DecimalDigitCount(value - 1));
  }
}

TEST(Digits, DecimalDigits_20) {
  for (uint64_t i : {
           10'000'000'000'000'000'000llu,
           10'000'000'000'000'000'001llu,
           std::numeric_limits<unsigned long long>::max(),
       }) {
    ASSERT_EQ(20u, DecimalDigitCount(i));
  }
}

TEST(Digits, HexDigits_AllOneDigit) {
  for (uint64_t i = 0; i < 0x10; ++i) {
    ASSERT_EQ(1u, HexDigitCount(i));
  }
}

TEST(Digits, HexDigits_AllTwoDigit) {
  for (uint64_t i = 0x10; i < 0x100u; ++i) {
    ASSERT_EQ(2u, HexDigitCount(i));
  }
}

TEST(Digits, HexDigits_1To15Digits) {
  uint64_t value = 1;
  for (unsigned digits = 1; digits <= 15u; ++digits) {
    ASSERT_EQ(digits, HexDigitCount(value));
    ASSERT_EQ(digits, HexDigitCount(value + 1));

    value *= 0x10;
    ASSERT_EQ(digits, HexDigitCount(value - 1));
  }
}

TEST(Digits, HexDigits_16) {
  for (uint64_t i : {
           0x1000000000000000llu,
           0x1000000000000001llu,
           std::numeric_limits<unsigned long long>::max(),
       }) {
    ASSERT_EQ(16u, HexDigitCount(i));
  }
}

class TestWithBuffer : public ::testing::Test {
 protected:
  static constexpr char kStartingString[] = "!@#$%^&*()!@#$%^&*()";
  static constexpr char kUint64Max[] = "18446744073709551615";
  static constexpr char kInt64Min[] = "-9223372036854775808";
  static constexpr char kInt64Max[] = "9223372036854775807";

  static_assert(sizeof(kStartingString) == sizeof(kUint64Max));

  TestWithBuffer() { std::memcpy(buffer_, kStartingString, sizeof(buffer_)); }

  char buffer_[sizeof(kUint64Max)];
};

class IntToStringTest : public TestWithBuffer {};

TEST_F(IntToStringTest, Unsigned_EmptyBuffer_WritesNothing) {
  auto result = IntToString(9u, span(buffer_, 0));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ(kStartingString, buffer_);
}

TEST_F(IntToStringTest, Unsigned_TooSmall_1Char_OnlyNullTerminates) {
  auto result = IntToString(9u, span(buffer_, 1));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

TEST_F(IntToStringTest, Unsigned_TooSmall_2Chars_OnlyNullTerminates) {
  auto result = IntToString(10u, span(buffer_, 2));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

TEST_F(IntToStringTest, Unsigned_TooSmall_3Chars_OnlyNullTerminates) {
  auto result = IntToString(123u, span(buffer_, 3));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

TEST_F(IntToStringTest, Unsigned_1Char_FitsExactly) {
  auto result = IntToString(0u, span(buffer_, 2));
  EXPECT_EQ(1u, result.size());
  EXPECT_TRUE(result.ok());
  EXPECT_STREQ("0", buffer_);

  result = IntToString(9u, span(buffer_, 2));
  EXPECT_EQ(1u, result.size());
  EXPECT_TRUE(result.ok());
  EXPECT_STREQ("9", buffer_);
}

TEST_F(IntToStringTest, Unsigned_2Chars_FitsExactly) {
  auto result = IntToString(10u, span(buffer_, 3));
  EXPECT_EQ(2u, result.size());
  EXPECT_STREQ("10", buffer_);
}

TEST_F(IntToStringTest, Unsigned_MaxFitsExactly) {
  EXPECT_EQ(20u,
            IntToString(std::numeric_limits<uint64_t>::max(),
                        span(buffer_, sizeof(kUint64Max)))
                .size());
  EXPECT_STREQ(kUint64Max, buffer_);
}

TEST_F(IntToStringTest, SignedPositive_EmptyBuffer_WritesNothing) {
  auto result = IntToString(9, span(buffer_, 0));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ(kStartingString, buffer_);
}

TEST_F(IntToStringTest, SignedPositive_TooSmall_NullTerminates) {
  auto result = IntToString(9, span(buffer_, 1));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

TEST_F(IntToStringTest, SignedPositive_TooSmall_DoesNotWritePastEnd) {
  EXPECT_EQ(0u, IntToString(9, span(buffer_, 1)).size());
  EXPECT_EQ(0, std::memcmp("\0@#$%^&*()!@#$%^&*()", buffer_, sizeof(buffer_)));
}

TEST_F(IntToStringTest, SignedPositive_1Char_FitsExactly) {
  auto result = IntToString(0, span(buffer_, 2));
  EXPECT_EQ(1u, result.size());
  EXPECT_TRUE(result.ok());
  EXPECT_STREQ("0", buffer_);

  result = IntToString(9, span(buffer_, 2));
  EXPECT_EQ(1u, result.size());
  EXPECT_TRUE(result.ok());
  EXPECT_STREQ("9", buffer_);
}

TEST_F(IntToStringTest, SignedPositive_2Chars_FitsExactly) {
  auto result = IntToString(10, span(buffer_, 4));
  EXPECT_EQ(2u, result.size());
  EXPECT_TRUE(result.ok());
  EXPECT_STREQ("10", buffer_);
}

TEST_F(IntToStringTest, SignedPositive_MaxFitsExactly) {
  auto result = IntToString(std::numeric_limits<int64_t>::max(),
                            span(buffer_, sizeof(kInt64Min)));
  EXPECT_EQ(19u, result.size());
  EXPECT_STREQ(kInt64Max, buffer_);
}

TEST_F(IntToStringTest, SignedNegative_EmptyBuffer_WritesNothing) {
  auto result = IntToString(-9, span(buffer_, 0));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ(kStartingString, buffer_);
}

TEST_F(IntToStringTest, SignedNegative_TooSmall_NullTerminates) {
  auto result = IntToString(-9, span(buffer_, 1));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

TEST_F(IntToStringTest, SignedNegative_TooSmall_DoesNotWritePastEnd) {
  // Note that two \0 are written due to the unsigned IntToString call.
  EXPECT_EQ(0u, IntToString(-9, span(buffer_, 2)).size());
  EXPECT_EQ(0, std::memcmp("\0\0#$%^&*()!@#$%^&*()", buffer_, sizeof(buffer_)));
}

TEST_F(IntToStringTest, SignedNegative_FitsExactly) {
  auto result = IntToString(-9, span(buffer_, 3));
  EXPECT_EQ(2u, result.size());
  EXPECT_STREQ("-9", buffer_);
  result = IntToString(-99, span(buffer_, 4));
  EXPECT_EQ(3u, result.size());
  EXPECT_STREQ("-99", buffer_);
  result = IntToString(-123, span(buffer_, 5));
  EXPECT_EQ(4u, result.size());
  EXPECT_STREQ("-123", buffer_);
}

TEST_F(IntToStringTest, SignedNegative_MinFitsExactly) {
  auto result = IntToString(std::numeric_limits<int64_t>::min(),
                            span(buffer_, sizeof(kInt64Min)));
  EXPECT_EQ(20u, result.size());
  EXPECT_STREQ(kInt64Min, buffer_);
}

TEST(IntToString, SignedSweep) {
  for (int i = -1002; i <= 1002; ++i) {
    char buffer[6];
    char printf_buffer[6];
    int written = std::snprintf(printf_buffer, sizeof(printf_buffer), "%d", i);
    auto result = IntToString(i, buffer);
    ASSERT_EQ(static_cast<size_t>(written), result.size());
    ASSERT_STREQ(printf_buffer, buffer);
  }
}

TEST(IntToString, UnsignedSweep) {
  for (unsigned i = 0; i <= 1002u; ++i) {
    char buffer[5];
    char printf_buffer[5];
    int written = std::snprintf(printf_buffer, sizeof(printf_buffer), "%u", i);
    auto result = IntToString(i, buffer);
    ASSERT_EQ(static_cast<size_t>(written), result.size());
    ASSERT_STREQ(printf_buffer, buffer);
  }
}

class IntToHexStringTest : public TestWithBuffer {};

TEST_F(IntToHexStringTest, Sweep) {
  for (unsigned i = 0; i < 1030; ++i) {
    char hex[16];
    int bytes = std::snprintf(hex, sizeof(hex), "%x", static_cast<unsigned>(i));

    auto result = IntToHexString(i, buffer_);
    EXPECT_EQ(static_cast<size_t>(bytes), result.size());
    EXPECT_TRUE(result.ok());
    EXPECT_STREQ(hex, buffer_);
  }
}

TEST_F(IntToHexStringTest, MinWidth) {
  unsigned val = 0xbeef;
  EXPECT_TRUE(IntToHexString(val, buffer_, 8).ok());
  EXPECT_STREQ("0000beef", buffer_);
}

TEST_F(IntToHexStringTest, Uint32Max) {
  EXPECT_EQ(
      8u,
      IntToHexString(std::numeric_limits<uint32_t>::max() - 1, buffer_).size());
  EXPECT_STREQ("fffffffe", buffer_);

  EXPECT_EQ(
      8u, IntToHexString(std::numeric_limits<uint32_t>::max(), buffer_).size());
  EXPECT_STREQ("ffffffff", buffer_);
}

TEST_F(IntToHexStringTest, Uint64Max) {
  EXPECT_EQ(
      16u,
      IntToHexString(std::numeric_limits<uint64_t>::max() - 1, buffer_).size());
  EXPECT_STREQ("fffffffffffffffe", buffer_);

  EXPECT_EQ(
      16u,
      IntToHexString(std::numeric_limits<uint64_t>::max(), buffer_).size());
  EXPECT_STREQ("ffffffffffffffff", buffer_);
}

TEST_F(IntToHexStringTest, EmptyBuffer_WritesNothing) {
  auto result = IntToHexString(0xbeef, span(buffer_, 0));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ(kStartingString, buffer_);
}

TEST_F(IntToHexStringTest, TooSmall_Truncates) {
  auto result = IntToHexString(0xbeef, span(buffer_, 3));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

class FloatAsIntToStringTest : public TestWithBuffer {};

TEST_F(FloatAsIntToStringTest, PositiveInfinity) {
  EXPECT_EQ(3u, FloatAsIntToString(INFINITY, buffer_).size());
  EXPECT_STREQ("inf", buffer_);
}

TEST_F(FloatAsIntToStringTest, NegativeInfinity) {
  EXPECT_EQ(4u, FloatAsIntToString(-INFINITY, buffer_).size());
  EXPECT_STREQ("-inf", buffer_);
}

TEST_F(FloatAsIntToStringTest, PositiveNan) {
  EXPECT_EQ(3u, FloatAsIntToString(NAN, buffer_).size());
  EXPECT_STREQ("NaN", buffer_);
}

TEST_F(FloatAsIntToStringTest, NegativeNan) {
  EXPECT_EQ(4u, FloatAsIntToString(-NAN, buffer_).size());
  EXPECT_STREQ("-NaN", buffer_);
}

TEST_F(FloatAsIntToStringTest, RoundDown_PrintsNearestInt) {
  EXPECT_EQ(1u, FloatAsIntToString(1.23, buffer_).size());
  EXPECT_STREQ("1", buffer_);
}

TEST_F(FloatAsIntToStringTest, RoundUp_PrintsNearestInt) {
  EXPECT_EQ(4u, FloatAsIntToString(1234.5, buffer_).size());
  EXPECT_STREQ("1235", buffer_);
}

TEST_F(FloatAsIntToStringTest, RoundsToNegativeZero_PrintsZero) {
  EXPECT_EQ(1u, FloatAsIntToString(-3.14e-20f, buffer_).size());
  EXPECT_STREQ("0", buffer_);
}

TEST_F(FloatAsIntToStringTest, RoundsToPositiveZero_PrintsZero) {
  EXPECT_EQ(1u, FloatAsIntToString(3.14e-20f, buffer_).size());
  EXPECT_STREQ("0", buffer_);
}

TEST_F(FloatAsIntToStringTest, RoundDownNegative_PrintsNearestInt) {
  volatile float x = -5.9;
  EXPECT_EQ(2u, FloatAsIntToString(x, buffer_).size());
  EXPECT_STREQ("-6", buffer_);
}

TEST_F(FloatAsIntToStringTest, RoundUpNegative_PrintsNearestInt) {
  EXPECT_EQ(9u, FloatAsIntToString(-50000000.1, buffer_).size());
  EXPECT_STREQ("-50000000", buffer_);
}

TEST_F(FloatAsIntToStringTest, LargerThanInteger) {
  EXPECT_EQ(3u, FloatAsIntToString(3.14e20f, buffer_).size());
  EXPECT_STREQ("inf", buffer_);
}

TEST_F(FloatAsIntToStringTest, SmallerThanInteger) {
  EXPECT_EQ(4u, FloatAsIntToString(-3.14e20f, buffer_).size());
  EXPECT_STREQ("-inf", buffer_);
}

TEST_F(FloatAsIntToStringTest, TooSmall_Numeric_NullTerminates) {
  auto result = FloatAsIntToString(-3.14e20f, span(buffer_, 1));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

TEST_F(FloatAsIntToStringTest, TooSmall_Infinity_NullTerminates) {
  auto result = FloatAsIntToString(-INFINITY, span(buffer_, 3));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

TEST_F(FloatAsIntToStringTest, TooSmall_NaN_NullTerminates) {
  auto result = FloatAsIntToString(NAN, span(buffer_, 2));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

class CopyStringOrNullTest : public TestWithBuffer {};

using namespace std::literals::string_view_literals;

TEST_F(CopyStringOrNullTest, NullSource_WritesNullPointerString) {
  EXPECT_EQ(kNullPointerString.size(),
            CopyStringOrNull(nullptr, buffer_).size());
  EXPECT_EQ(kNullPointerString, buffer_);
}

TEST_F(CopyStringOrNullTest, EmptyStringView_WritesNullTerminator) {
  EXPECT_EQ(0u, CopyStringOrNull("", buffer_).size());
  EXPECT_EQ('\0', buffer_[0]);
}

TEST_F(CopyStringOrNullTest, EmptyBuffer_WritesNothing) {
  auto result = CopyStringOrNull("Hello", span(buffer_, 0));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ(kStartingString, buffer_);
}

TEST_F(CopyStringOrNullTest, TooSmall_Truncates) {
  auto result = CopyStringOrNull("Hi!", span(buffer_, 3));
  EXPECT_EQ(2u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("Hi", buffer_);
}

TEST_F(CopyStringOrNullTest, ExactFit) {
  auto result = CopyStringOrNull("Hi!", span(buffer_, 4));
  EXPECT_EQ(3u, result.size());
  EXPECT_TRUE(result.ok());
  EXPECT_STREQ("Hi!", buffer_);
}

TEST_F(CopyStringOrNullTest, NullTerminatorsInString) {
  ASSERT_EQ(4u, CopyStringOrNull("\0!\0\0"sv, span(buffer_, 5)).size());
  EXPECT_EQ("\0!\0\0"sv, std::string_view(buffer_, 4));
}

class CopyEntireStringOrNullTest : public TestWithBuffer {};

TEST_F(CopyEntireStringOrNullTest, NullSource_WritesNullPointerString) {
  EXPECT_EQ(kNullPointerString.size(),
            CopyEntireStringOrNull(nullptr, buffer_).size());
  EXPECT_EQ(kNullPointerString, buffer_);
}

TEST_F(CopyEntireStringOrNullTest, EmptyStringView_WritesNullTerminator) {
  EXPECT_EQ(0u, CopyEntireStringOrNull("", buffer_).size());
  EXPECT_EQ('\0', buffer_[0]);
}

TEST_F(CopyEntireStringOrNullTest, EmptyBuffer_WritesNothing) {
  auto result = CopyEntireStringOrNull("Hello", span(buffer_, 0));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ(kStartingString, buffer_);
}

TEST_F(CopyEntireStringOrNullTest, TooSmall_WritesNothing) {
  auto result = CopyEntireStringOrNull("Hi!", span(buffer_, 3));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

TEST_F(CopyEntireStringOrNullTest, ExactFit) {
  auto result = CopyEntireStringOrNull("Hi!", span(buffer_, 4));
  EXPECT_EQ(3u, result.size());
  EXPECT_TRUE(result.ok());
  EXPECT_STREQ("Hi!", buffer_);
}

TEST_F(CopyEntireStringOrNullTest, NullTerminatorsInString) {
  ASSERT_EQ(4u, CopyEntireStringOrNull("\0!\0\0"sv, span(buffer_, 5)).size());
  EXPECT_EQ("\0!\0\0"sv, std::string_view(buffer_, 4));
}

class PointerToStringTest : public TestWithBuffer {};

TEST_F(PointerToStringTest, Nullptr_WritesNull) {
  EXPECT_EQ(6u, PointerToString(nullptr, span(buffer_, 7)).size());
  EXPECT_STREQ("(null)", buffer_);
}

TEST_F(PointerToStringTest, WritesAddress) {
  const void* pointer = reinterpret_cast<void*>(0xbeef);
  EXPECT_EQ(4u, PointerToString(pointer, buffer_).size());
  EXPECT_STREQ("beef", buffer_);
}

class BoolToStringTest : public TestWithBuffer {};

TEST_F(BoolToStringTest, ExactFit) {
  EXPECT_EQ(4u, BoolToString(true, span(buffer_, 5)).size());
  EXPECT_STREQ("true", buffer_);

  EXPECT_EQ(5u, BoolToString(false, span(buffer_, 6)).size());
  EXPECT_STREQ("false", buffer_);
}

TEST_F(BoolToStringTest, True_TooSmall_WritesNullTerminator) {
  auto result = BoolToString(true, span(buffer_, 4));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

TEST_F(BoolToStringTest, False_TooSmall_WritesNullTerminator) {
  auto result = BoolToString(false, span(buffer_, 5));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer_);
}

TEST_F(BoolToStringTest, EmptyBuffer_WritesNothing) {
  EXPECT_EQ(0u, BoolToString(true, span(buffer_, 0)).size());
  EXPECT_STREQ(kStartingString, buffer_);

  EXPECT_EQ(0u, BoolToString(false, span(buffer_, 0)).size());
  EXPECT_STREQ(kStartingString, buffer_);
}

}  // namespace
}  // namespace pw::string
