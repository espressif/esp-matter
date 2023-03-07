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

#include "pw_string/string_builder.h"

#include <cinttypes>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string_view>

#include "gtest/gtest.h"
#include "pw_span/span.h"
#include "pw_string/format.h"

namespace this_pw_test {

struct CustomType {
  uint32_t a;
  uint32_t b;

  static constexpr const char* kToString = "This is a CustomType";

  CustomType() = default;

  // Non-copyable to verify StringBuffer's << operator doesn't copy it.
  CustomType(const CustomType&) = delete;
  CustomType& operator=(const CustomType&) = delete;
};

}  // namespace this_pw_test

namespace pw {

template <>
StatusWithSize ToString<this_pw_test::CustomType>(
    const this_pw_test::CustomType&, span<char> buffer) {
  return string::Format(buffer, this_pw_test::CustomType::kToString);
}

}  // namespace pw

namespace pw {
namespace {

using this_pw_test::CustomType;

TEST(StringBuilder, EmptyBuffer_SizeAndMaxSizeAreCorrect) {
  StringBuilder sb(span<char>{});

  EXPECT_TRUE(sb.empty());
  EXPECT_EQ(0u, sb.size());
  EXPECT_EQ(0u, sb.max_size());
}

using namespace std::literals::string_view_literals;

constexpr std::string_view kNoTouch = "DO NOT TOUCH\0VALUE SHOULD NOT CHANGE"sv;

TEST(StringBuilder, EmptyBuffer_StreamOutput_WritesNothing) {
  char buffer[kNoTouch.size()];
  std::memcpy(buffer, kNoTouch.data(), sizeof(buffer));

  StringBuilder sb(span(buffer, 0));

  sb << CustomType() << " is " << 12345;
  EXPECT_EQ(Status::ResourceExhausted(), sb.status());
  EXPECT_EQ(kNoTouch, std::string_view(buffer, sizeof(buffer)));
}

TEST(StringBuilder, EmptyBuffer_Append_WritesNothing) {
  char buffer[kNoTouch.size()];
  std::memcpy(buffer, kNoTouch.data(), sizeof(buffer));

  StringBuilder sb(span(buffer, 0));

  EXPECT_FALSE(sb.append("Hello").ok());
  EXPECT_EQ(kNoTouch, std::string_view(buffer, sizeof(buffer)));
}

TEST(StringBuilder, EmptyBuffer_Resize_WritesNothing) {
  char buffer[kNoTouch.size()];
  std::memcpy(buffer, kNoTouch.data(), sizeof(buffer));

  StringBuilder sb(span(buffer, 0));

  sb.resize(0);
  EXPECT_TRUE(sb.ok());
  EXPECT_EQ(kNoTouch, std::string_view(buffer, sizeof(buffer)));
}

TEST(StringBuilder, EmptyBuffer_AppendEmpty_ResourceExhausted) {
  StringBuilder sb(span<char>{});
  EXPECT_EQ(OkStatus(), sb.last_status());
  EXPECT_EQ(OkStatus(), sb.status());

  sb << "";

  EXPECT_EQ(Status::ResourceExhausted(), sb.last_status());
  EXPECT_EQ(Status::ResourceExhausted(), sb.status());
}

TEST(StringBuilder, Status_StartsOk) {
  StringBuffer<16> sb;
  EXPECT_EQ(OkStatus(), sb.status());
  EXPECT_EQ(OkStatus(), sb.last_status());
}

TEST(StringBuilder, Status_StatusAndLastStatusUpdate) {
  StringBuffer<16> sb;
  sb << "Well, if only there were enough room in here for this string";
  EXPECT_EQ(Status::ResourceExhausted(), sb.status());
  EXPECT_EQ(Status::ResourceExhausted(), sb.last_status());

  sb.resize(1029);
  EXPECT_EQ(Status::OutOfRange(), sb.status());
  EXPECT_EQ(Status::OutOfRange(), sb.last_status());

  sb << "";
  EXPECT_EQ(Status::OutOfRange(), sb.status());
  EXPECT_EQ(OkStatus(), sb.last_status());
}

TEST(StringBuilder, Status_ClearStatus_SetsStatuesToOk) {
  StringBuffer<2> sb = MakeString<2>("Won't fit!!!!!");
  EXPECT_EQ(Status::ResourceExhausted(), sb.status());
  EXPECT_EQ(Status::ResourceExhausted(), sb.last_status());

  sb.clear_status();
  EXPECT_EQ(OkStatus(), sb.status());
  EXPECT_EQ(OkStatus(), sb.last_status());
}

TEST(StringBuilder, StreamOutput_OutputSelf) {
  auto sb = MakeString<32>("echo!");
  sb << sb;

  EXPECT_STREQ("echo!echo!", sb.data());
  EXPECT_EQ(10u, sb.size());
}

TEST(StringBuilder, PushBack) {
  StringBuffer<12> sb;
  sb.push_back('?');
  EXPECT_EQ(OkStatus(), sb.last_status());
  EXPECT_EQ(1u, sb.size());
  EXPECT_STREQ("?", sb.data());
}

TEST(StringBuilder, PushBack_Full) {
  StringBuffer<1> sb;
  sb.push_back('!');
  EXPECT_EQ(Status::ResourceExhausted(), sb.last_status());
  EXPECT_EQ(0u, sb.size());
}

TEST(StringBuilder, PopBack) {
  auto sb = MakeString<12>("Welcome!");
  sb.pop_back();
  EXPECT_EQ(OkStatus(), sb.last_status());
  EXPECT_EQ(7u, sb.size());
  EXPECT_STREQ("Welcome", sb.data());
}

TEST(StringBuilder, PopBack_Empty) {
  StringBuffer<12> sb;
  sb.pop_back();
  EXPECT_EQ(Status::OutOfRange(), sb.last_status());
  EXPECT_EQ(0u, sb.size());
}

TEST(StringBuilder, Append_NonTerminatedString) {
  static char bad_string[256];
  std::memset(bad_string, '?', sizeof(bad_string));

  StringBuffer<6> sb;
  EXPECT_EQ(Status::ResourceExhausted(), sb.append(bad_string).last_status());
  EXPECT_STREQ("?????", sb.data());
}

TEST(StringBuilder, Append_Chars) {
  StringBuffer<8> sb;

  EXPECT_TRUE(sb.append(7, '?').ok());
  EXPECT_STREQ("???????", sb.data());
}

TEST(StringBuilder, Append_Chars_Full) {
  StringBuffer<8> sb;

  EXPECT_EQ(Status::ResourceExhausted(), sb.append(8, '?').last_status());
  EXPECT_STREQ("???????", sb.data());
}

TEST(StringBuilder, Append_Chars_ToEmpty) {
  StringBuilder sb(span<char>{});

  EXPECT_EQ(Status::ResourceExhausted(), sb.append(1, '?').last_status());
}

TEST(StringBuilder, Append_PartialCString) {
  StringBuffer<12> sb;
  EXPECT_TRUE(sb.append("123456", 4).ok());
  EXPECT_EQ(4u, sb.size());
  EXPECT_STREQ("1234", sb.data());
}

TEST(StringBuilder, Append_CString) {
  auto sb = MakeString("hello");
  EXPECT_TRUE(sb.append(" goodbye").ok());
  EXPECT_STREQ("hello goodbye", sb.data());
  EXPECT_EQ(13u, sb.size());
}

TEST(StringBuilder, Append_CString_Full) {
  auto sb = MakeString<6>("hello");
  EXPECT_EQ(Status::ResourceExhausted(), sb.append("890123", 1).last_status());
  EXPECT_EQ(Status::ResourceExhausted(), sb.status());
  EXPECT_EQ(sb.max_size(), sb.size());
  EXPECT_STREQ("hello", sb.data());
}

TEST(StringBuilder, Append_StringView) {
  auto sb = MakeString<32>("hello");
  EXPECT_TRUE(sb.append("???"sv).ok());
  EXPECT_EQ("hello???"sv, sb);
}

TEST(StringBuilder, Append_StringView_Substring) {
  auto sb = MakeString<32>("I like ");
  EXPECT_TRUE(sb.append("your shoes!!!"sv, 5, 5).ok());
  EXPECT_EQ("I like shoes"sv, sb);
}

TEST(StringBuilder, Append_StringView_RemainingSubstring) {
  auto sb = MakeString<32>("I like ");
  EXPECT_TRUE(sb.append("your shoes!!!"sv, 5).ok());
  EXPECT_EQ("I like shoes!!!"sv, sb);
}

TEST(StringBuilder, Resize_Smaller) {
  auto sb = MakeString<12>("Four");
  sb.resize(2);
  EXPECT_TRUE(sb.ok());
  EXPECT_EQ(2u, sb.size());
  EXPECT_STREQ("Fo", sb.data());
}

TEST(StringBuilder, Resize_Clear) {
  auto sb = MakeString<12>("Four");
  sb.resize(0);
  EXPECT_TRUE(sb.ok());
  EXPECT_EQ(0u, sb.size());
  EXPECT_STREQ("", sb.data());
}

TEST(StringBuilder, Resize_Larger_Fails) {
  auto sb = MakeString<12>("Four");
  EXPECT_EQ(4u, sb.size());
  sb.resize(10);
  EXPECT_EQ(sb.status(), Status::OutOfRange());
  EXPECT_EQ(4u, sb.size());
}

TEST(StringBuilder, Resize_LargerThanCapacity_Fails) {
  auto sb = MakeString<12>("Four");
  sb.resize(1234);
  EXPECT_EQ(sb.status(), Status::OutOfRange());
  EXPECT_EQ(4u, sb.size());
  EXPECT_STREQ("Four", sb.data());
}

TEST(StringBuilder, Format_Normal) {
  std::byte buffer[64];
  StringBuilder sb(buffer);
  EXPECT_TRUE(sb.Format("0x%x", 0xabc).ok());
  EXPECT_STREQ("0xabc", sb.data());

  sb << "def";

  EXPECT_TRUE(sb.Format("GHI").ok());
  EXPECT_STREQ("0xabcdefGHI", sb.data());
}

TEST(StringBuilder, Format_ExhaustBuffer) {
  StringBuffer<6> sb;
  EXPECT_EQ(Status::ResourceExhausted(), sb.Format("012345").status());

  EXPECT_STREQ("01234", sb.data());
  EXPECT_EQ(Status::ResourceExhausted(), sb.status());
}

TEST(StringBuilder, StreamOutput_MultipleTypes) {
  constexpr const char* kExpected = "This is -1true example\n of this";
  constexpr const char* kExample = "example";

  StringBuffer<64> sb;
  sb << "This is " << -1 << true << ' ' << kExample << '\n' << " of this";

  EXPECT_STREQ(kExpected, sb.data());
  EXPECT_EQ(std::strlen(kExpected), sb.size());
}

TEST(StringBuilder, StreamOutput_FullBufferIgnoresExtraStrings) {
  StringBuffer<6> sb;
  EXPECT_EQ(5u, sb.max_size());  // max_size() excludes the null terminator

  sb << 1 - 1;
  EXPECT_TRUE(sb.ok());
  EXPECT_STREQ("0", sb.data());

  sb << true << "Now it's way " << static_cast<unsigned char>(2) << " long";
  EXPECT_FALSE(sb.ok());
  EXPECT_EQ(Status::ResourceExhausted(), sb.status());
  EXPECT_STREQ("0true", sb.data());
}

TEST(StringBuilder, StreamOutput_ExhaustBuffer_InOneString) {
  StringBuffer<9> sb;
  EXPECT_EQ(8u, sb.max_size());

  sb << "0123456789";  // write 10 chars
  EXPECT_FALSE(sb.ok());
  EXPECT_STREQ("01234567", sb.data());  // only can fit 8
  EXPECT_EQ(8u, sb.size());

  sb << "no"
     << " more "
     << "room" << '?';
  EXPECT_STREQ("01234567", sb.data());
}

TEST(StringBuilder, StreamOutput_ExhaustBuffer_InTwoStrings) {
  StringBuffer<4> sb;

  sb << "01";  // fill 3/4 of buffer
  EXPECT_EQ(2u, sb.size());
  sb << "234";
  EXPECT_STREQ("012", sb.data());
  EXPECT_EQ(Status::ResourceExhausted(), sb.status());
  EXPECT_EQ(3u, sb.size());
}

TEST(StringBuilder, StreamOutput_NonTerminatedString) {
  static char bad_string[256];
  std::memset(bad_string, '?', sizeof(bad_string));

  StringBuffer<6> sb;
  sb << "hey" << bad_string;

  EXPECT_EQ(Status::ResourceExhausted(), sb.status());
  EXPECT_STREQ("hey??", sb.data());
}

TEST(StringBuilder, SteamOutput_StringView) {
  StringBuffer<6> buffer;
  constexpr std::string_view hello("hello");

  buffer << hello;
  EXPECT_EQ(OkStatus(), buffer.status());
  EXPECT_STREQ("hello", buffer.data());
}

TEST(StringBuilder, StreamOutput_EmptyStringView) {
  StringBuffer<4> buffer;
  buffer << "hi" << std::string_view() << "!";
  EXPECT_TRUE(buffer.ok());
  EXPECT_STREQ("hi!", buffer.data());
}

TEST(StringBuilder, StreamOutput_ByteArray) {
  StringBuffer<7> buffer;
  std::array<std::byte, 3> data{
      {std::byte(0xc8), std::byte(0x02), std::byte(0x41)}};
  buffer << data;
  EXPECT_EQ(buffer.status(), OkStatus());
  EXPECT_STREQ("c80241", buffer.data());
}

TEST(StringBuilder, StreamOutput_ByteSpan) {
  StringBuffer<11> buffer;
  std::array<std::byte, 5> data{{std::byte(0),
                                 std::byte(0xc8),
                                 std::byte(0x02),
                                 std::byte(0x41),
                                 std::byte(0xe0)}};
  buffer << as_bytes(span(data));
  EXPECT_EQ(buffer.status(), OkStatus());
  EXPECT_STREQ("00c80241e0", buffer.data());
}

TEST(StringBuilder, StreamOutput_ByteSpanOutOfSpace) {
  StringBuffer<4> buffer;
  std::array<uint8_t, 3> data{{0xc8, 0x02, 0x41}};
  buffer << as_bytes(span(data));
  EXPECT_EQ(buffer.status(), Status::ResourceExhausted());
  EXPECT_STREQ("", buffer.data());
}

TEST(StringBuffer, Assign) {
  StringBuffer<10> one;
  StringBuffer<10> two;

  one << "What";
  ASSERT_STREQ("What", one.data());
  two = one;
  EXPECT_STREQ("What", two.data());
  EXPECT_NE(one.data(), two.data());
  one << " the";
  two << " heck";

  EXPECT_STREQ("What the", one.data());
  EXPECT_STREQ("What heck", two.data());

  two << "0123456789";
  ASSERT_STREQ("What heck", two.data());
  ASSERT_EQ(Status::ResourceExhausted(), two.status());
  ASSERT_EQ(Status::ResourceExhausted(), two.last_status());

  one = two;
  EXPECT_STREQ("What heck", one.data());
  EXPECT_EQ(Status::ResourceExhausted(), one.status());
  EXPECT_EQ(Status::ResourceExhausted(), one.last_status());

  StringBuffer<12> three;
  three = two;
  EXPECT_STREQ(three.data(), two.data());
  EXPECT_EQ(three.size(), two.size());
}

TEST(StringBuffer, CopyConstructFromSameSize) {
  StringBuffer<10> one;

  one << "What";
  ASSERT_STREQ("What", one.data());
  StringBuffer<10> two(one);
  EXPECT_STREQ("What", two.data());
  EXPECT_NE(one.data(), two.data());
  one << " the";
  two << " heck";

  EXPECT_STREQ("What the", one.data());
  EXPECT_STREQ("What heck", two.data());

  two << "0123456789";
  two << "";
  ASSERT_STREQ("What heck", two.data());
  ASSERT_EQ(Status::ResourceExhausted(), two.status());
  ASSERT_EQ(OkStatus(), two.last_status());
}

TEST(StringBuffer, CopyConstructFromSmaller) {
  StringBuffer<10> one = MakeString<10>("You are the chosen one.");
  StringBuffer<12> two(one);

  EXPECT_STREQ("You are t", two.data());
  EXPECT_EQ(Status::ResourceExhausted(), two.status());
}

TEST(StringBuilder, Object) {
  StringBuffer<64> sb;
  sb << CustomType();

  EXPECT_STREQ(CustomType::kToString, sb.data());
  EXPECT_EQ(std::strlen(CustomType::kToString), sb.size());
}

TEST(StringBuilder, UseStringAsBuffer) {
  InlineString<32> string;
  StringBuilder sb(string);

  sb << 123 << "456";

  EXPECT_EQ(sb.data(), string.data());
  EXPECT_EQ(sb.size(), string.size());
  EXPECT_EQ(6u, string.size());

  EXPECT_STREQ(sb.c_str(), "123456");
  EXPECT_STREQ(string.c_str(), "123456");
}

TEST(StringBuilder, OverflowStringAsBuffer) {
  InlineString<5> string;
  StringBuilder sb(string);

  sb << 123 << "456";

  EXPECT_EQ(sb.status(), Status::ResourceExhausted());
  EXPECT_EQ(string.size(), 5u);
  EXPECT_STREQ(string.c_str(), "12345");
}

TEST(MakeString, Object) {
  CustomType custom;
  const auto sb = MakeString<64>(custom);

  EXPECT_STREQ(CustomType::kToString, sb.data());
  EXPECT_EQ(std::strlen(CustomType::kToString), sb.size());
}

TEST(MakeString, IntegerTypes) {
  EXPECT_STREQ("0123-4567",
               MakeString(0ll,
                          1u,
                          2l,
                          3,
                          -4,
                          static_cast<unsigned short>(5),
                          static_cast<short>(6),
                          static_cast<unsigned char>(7))
                   .data());
}

TEST(MakeString, Char) {
  EXPECT_STREQ("a b c", MakeString('a', ' ', 'b', ' ', 'c').data());
}

TEST(MakeString, Float) { EXPECT_STREQ("-inf", MakeString(-INFINITY).data()); }

TEST(MakeString, Pointer_Null) {
  EXPECT_STREQ("(null)", MakeString(nullptr).data());
  EXPECT_STREQ("(null)", MakeString(static_cast<void*>(nullptr)).data());
}

TEST(MakeString, Pointer_NonNull) {
  EXPECT_STREQ("1", MakeString(reinterpret_cast<void*>(0x1)).data());
  EXPECT_STREQ("123", MakeString(reinterpret_cast<int*>(0x123)).data());
}

TEST(MakeString, Pointer_CustomType) {
  char expected[32] = {};

  CustomType custom;
  std::snprintf(expected,
                sizeof(expected),
                "%" PRIxPTR,
                reinterpret_cast<uintptr_t>(&custom));

  EXPECT_STREQ(expected, MakeString(&custom).data());
}

TEST(MakeString, Bool) {
  EXPECT_STREQ("true", MakeString(true).data());
  EXPECT_STREQ("false", MakeString(false).data());
}

TEST(MakeString, MutableString) {
  char chars[] = {'C', 'o', 'o', 'l', '\0'};
  EXPECT_STREQ("Cool?", MakeString(chars, "?").data());
}

TEST(MakeString, Empty_IsEmpty) { EXPECT_TRUE(MakeString().empty()); }

constexpr char kLongestString[] = "18446744073709551615";  // largest uint64_t

TEST(MakeString, DefaultSizeString_FitsWholeString) {
  EXPECT_STREQ(
      kLongestString,
      MakeString(184, "467", u'\x04', "40", '7', '3', '7', "0", "", 955ul, 1615)
          .data());
}

TEST(MakeString, LargerThanDefaultSize_Truncates) {
  auto sb = MakeString("1844674407", 3709551615, 123456);

  EXPECT_EQ(Status::ResourceExhausted(), sb.status());
  EXPECT_STREQ(kLongestString, sb.data());
}

TEST(MakeString, StringLiteral_ResizesToFitWholeLiteral) {
  EXPECT_STREQ("", MakeString().data());

  [[maybe_unused]] auto normal = MakeString("");
  static_assert(normal.max_size() == decltype(MakeString(1))::max_size());
  EXPECT_EQ(normal.max_size(), decltype(MakeString(1))::max_size());

  [[maybe_unused]] auto resized =
      MakeString("This string is reeeeeeeeeaaaaallly long!!!!!");
  static_assert(resized.max_size() > decltype(MakeString(1))::max_size());
  static_assert(resized.max_size() ==
                sizeof("This string is reeeeeeeeeaaaaallly long!!!!!") - 1);
  EXPECT_GT(resized.max_size(), decltype(MakeString(1))::max_size());
  EXPECT_EQ(resized.max_size(),
            sizeof("This string is reeeeeeeeeaaaaallly long!!!!!") - 1);
}

TEST(MakeString, StringLiteral_UsesLongerFixedSize) {
  auto fixed_size = MakeString<64>("");
  static_assert(fixed_size.max_size() == 63u);
  EXPECT_EQ(fixed_size.max_size(), 63u);
  EXPECT_STREQ("", fixed_size.data());
}

TEST(MakeString, StringLiteral_TruncatesShorterFixedSize) {
  EXPECT_STREQ("Goo", MakeString<4>("Google").data());
  EXPECT_STREQ("Google", MakeString<7>("Google").data());
  EXPECT_EQ(MakeString().max_size(), MakeString("Google").max_size());
  EXPECT_STREQ("Google", MakeString("Google").data());
}

TEST(MakeString, DefaultSize_FitsMaxAndMinInts) {
  EXPECT_STREQ("-9223372036854775808",
               MakeString(std::numeric_limits<int64_t>::min()).data());
  EXPECT_STREQ("18446744073709551615",
               MakeString(std::numeric_limits<uint64_t>::max()).data());
}

TEST(MakeString, OutputToTemporaryStringBuffer) {
  EXPECT_STREQ("hello", (MakeString<6>("hello ") << "world").data());
  EXPECT_STREQ("hello world", (MakeString("hello ") << "world").data());
}

// Test MakeString's default size calculations.
template <typename... Args>
constexpr size_t DefaultStringBufferSize(Args&&...) {
  return string_internal::DefaultStringBufferSize<Args...>();
}

// Default sizes are rounded up to 24 bytes.
static_assert(DefaultStringBufferSize("") == 24);
static_assert(DefaultStringBufferSize("123") == 24);
static_assert(DefaultStringBufferSize("123", "456", "78901234567890") == 24);
static_assert(DefaultStringBufferSize("1234567890", "1234567890", "123") == 24);
static_assert(DefaultStringBufferSize(1234, 5678, 9012) == 24);

// The buffer is sized to fix strings needing more than 24 bytes.
static_assert(DefaultStringBufferSize("1234567890", "1234567890", "1234") ==
              25);
static_assert(DefaultStringBufferSize("1234567890", "1234567890", "12345") ==
              26);
static_assert(DefaultStringBufferSize("1234567890", "1234567890", "12345678") ==
              29);

// Four bytes are allocated for each non-string argument.
static_assert(DefaultStringBufferSize(1234, 5678, 9012, 3456, 7890, 1234) ==
              25);
static_assert(DefaultStringBufferSize('a', nullptr, 'b', 4, 5, 6, 7, 8) == 33);

struct SomeCustomType {};

StringBuilder& operator<<(StringBuilder& sb, const SomeCustomType&) {
  return sb << "SomeCustomType was here!";
}

TEST(StringBuilder, ShiftOperatorOverload_SameNamsepace) {
  pw::StringBuffer<48> buffer;
  buffer << SomeCustomType{};

  EXPECT_STREQ("SomeCustomType was here!", buffer.c_str());
}

}  // namespace
}  // namespace pw

namespace some_other_ns {

struct MyCustomType {
  int item;
};

pw::StringBuilder& operator<<(pw::StringBuilder& sb,
                              const MyCustomType& value) {
  return sb << "MyCustomType(" << value.item << ')';
}

}  // namespace some_other_ns

namespace pw_test_namespace {

TEST(StringBuilder, ShiftOperatorOverload_DifferentNamsepace) {
  pw::StringBuffer<48> buffer;
  buffer << "This is " << some_other_ns::MyCustomType{1138};

  EXPECT_STREQ("This is MyCustomType(1138)", buffer.data());
}

}  // namespace pw_test_namespace
