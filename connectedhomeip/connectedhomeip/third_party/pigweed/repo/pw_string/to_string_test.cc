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

#include "pw_string/to_string.h"

#include <array>
#include <cinttypes>
#include <cmath>
#include <cstring>
#include <string>

#include "gtest/gtest.h"
#include "pw_status/status.h"
#include "pw_string/internal/config.h"
#include "pw_string/type_to_string.h"

namespace pw {

struct CustomType {
  unsigned a;
  unsigned b;

  static constexpr const char* kToString = "This is a CustomType";

  CustomType() : a(0), b(0) {}

  // Non-copyable to verify that ToString doesn't copy it.
  CustomType(const CustomType&) = delete;
  CustomType& operator=(const CustomType&) = delete;
};

StatusWithSize ToString(const CustomType&, span<char> buffer) {
  int result =
      std::snprintf(buffer.data(), buffer.size(), CustomType::kToString);
  if (result < 0) {
    return StatusWithSize::Unknown();
  }
  if (static_cast<size_t>(result) < buffer.size()) {
    return StatusWithSize(result);
  }
  return StatusWithSize::ResourceExhausted(buffer.empty() ? 0u
                                                          : buffer.size() - 1);
}

namespace {

char buffer[128] = {};
char expected[128] = {};

TEST(ToString, Bool) {
  const volatile bool b = true;
  EXPECT_EQ(4u, ToString(b, buffer).size());
  EXPECT_STREQ("true", buffer);
  EXPECT_EQ(5u, ToString(false, buffer).size());
  EXPECT_STREQ("false", buffer);
}

TEST(ToString, Char) {
  EXPECT_EQ(1u, ToString('%', buffer).size());
  EXPECT_STREQ("%", buffer);
}

template <typename T>
constexpr T kInteger = 127;

TEST(ToString, Integer_AllTypesAreSupported) {
  EXPECT_EQ(3u, ToString(kInteger<unsigned char>, buffer).size());
  EXPECT_STREQ("127", buffer);
  EXPECT_EQ(3u, ToString(kInteger<signed char>, buffer).size());
  EXPECT_STREQ("127", buffer);
  EXPECT_EQ(3u, ToString(kInteger<unsigned short>, buffer).size());
  EXPECT_STREQ("127", buffer);
  EXPECT_EQ(3u, ToString(kInteger<signed short>, buffer).size());
  EXPECT_STREQ("127", buffer);
  EXPECT_EQ(3u, ToString(kInteger<unsigned int>, buffer).size());
  EXPECT_STREQ("127", buffer);
  EXPECT_EQ(3u, ToString(kInteger<signed int>, buffer).size());
  EXPECT_STREQ("127", buffer);
  EXPECT_EQ(3u, ToString(kInteger<unsigned long>, buffer).size());
  EXPECT_STREQ("127", buffer);
  EXPECT_EQ(3u, ToString(kInteger<signed long>, buffer).size());
  EXPECT_STREQ("127", buffer);
  EXPECT_EQ(3u, ToString(kInteger<unsigned long long>, buffer).size());
  EXPECT_STREQ("127", buffer);
  EXPECT_EQ(3u, ToString(kInteger<signed long long>, buffer).size());
  EXPECT_STREQ("127", buffer);
}

TEST(ToString, ScopedEnum) {
  enum class MyEnum : short { kLuckyNumber = 8 };

  auto result = ToString(MyEnum::kLuckyNumber, buffer);
  EXPECT_EQ(1u, result.size());
  EXPECT_EQ(OkStatus(), result.status());
  EXPECT_STREQ("8", buffer);
}

TEST(ToString, Integer_EmptyBuffer_WritesNothing) {
  auto result = ToString(-1234, span(buffer, 0));
  EXPECT_EQ(0u, result.size());
  EXPECT_EQ(Status::ResourceExhausted(), result.status());
}

TEST(ToString, Integer_BufferTooSmall_WritesNullTerminator) {
  auto result = ToString(-1234, span(buffer, 5));
  EXPECT_EQ(0u, result.size());
  EXPECT_FALSE(result.ok());
  EXPECT_STREQ("", buffer);
}

TEST(ToString, Float) {
  if (string::internal::config::kEnableDecimalFloatExpansion) {
    EXPECT_EQ(5u, ToString(0.0f, buffer).size());
    EXPECT_STREQ("0.000", buffer);
    EXPECT_EQ(6u, ToString(33.444, buffer).size());
    EXPECT_STREQ("33.444", buffer);
    EXPECT_EQ(3u, ToString(INFINITY, buffer).size());
    EXPECT_STREQ("inf", buffer);
    EXPECT_EQ(3u, ToString(NAN, buffer).size());
    EXPECT_STREQ("nan", buffer);
  } else {
    EXPECT_EQ(1u, ToString(0.0f, buffer).size());
    EXPECT_STREQ("0", buffer);
    EXPECT_EQ(3u, ToString(INFINITY, buffer).size());
    EXPECT_STREQ("inf", buffer);
    EXPECT_EQ(4u, ToString(-NAN, buffer).size());
    EXPECT_STREQ("-NaN", buffer);
  }
}

TEST(ToString, Pointer_NonNull_WritesValue) {
  CustomType custom;
  const size_t length = std::snprintf(expected,
                                      sizeof(expected),
                                      "%" PRIxPTR,
                                      reinterpret_cast<intptr_t>(&custom));

  EXPECT_EQ(length, ToString(&custom, buffer).size());
  EXPECT_STREQ(expected, buffer);
  EXPECT_EQ(length, ToString(static_cast<void*>(&custom), buffer).size());
  EXPECT_STREQ(expected, buffer);
  EXPECT_EQ(1u, ToString(reinterpret_cast<int*>(4), buffer).size());
  EXPECT_STREQ("4", buffer);
}

TEST(ToString, Pointer_Nullptr_WritesNull) {
  EXPECT_EQ(string::kNullPointerString.size(),
            ToString(nullptr, buffer).size());
  EXPECT_EQ(string::kNullPointerString, buffer);
}

TEST(ToString, Pointer_NullValuedPointer_WritesNull) {
  EXPECT_EQ(string::kNullPointerString.size(),
            ToString(static_cast<const CustomType*>(nullptr), buffer).size());
  EXPECT_EQ(string::kNullPointerString, buffer);
}

TEST(ToString, Pointer_NullValuedCString_WritesNull) {
  EXPECT_EQ(string::kNullPointerString.size(),
            ToString(static_cast<char*>(nullptr), buffer).size());
  EXPECT_EQ(string::kNullPointerString, buffer);

  EXPECT_EQ(string::kNullPointerString.size(),
            ToString(static_cast<const char*>(nullptr), buffer).size());
  EXPECT_EQ(string::kNullPointerString, buffer);
}

TEST(ToString, String_Literal) {
  EXPECT_EQ(0u, ToString("", buffer).size());
  EXPECT_STREQ("", buffer);
  EXPECT_EQ(5u, ToString("hello", buffer).size());
  EXPECT_STREQ("hello", buffer);
}

TEST(ToString, String_Pointer) {
  EXPECT_EQ(0u, ToString(static_cast<const char*>(""), buffer).size());
  EXPECT_STREQ("", buffer);
  EXPECT_EQ(5u, ToString(static_cast<const char*>("hello"), buffer).size());
  EXPECT_STREQ("hello", buffer);
}

TEST(ToString, String_MutableBuffer) {
  char chars[] = {'C', 'o', 'o', 'l', '\0'};
  EXPECT_EQ(sizeof(chars) - 1, ToString(chars, buffer).size());
  EXPECT_STREQ("Cool", buffer);
}

TEST(ToString, String_MutablePointer) {
  char chars[] = {'b', 'o', 'o', 'l', '\0'};
  EXPECT_EQ(sizeof(chars) - 1,
            ToString(static_cast<char*>(chars), buffer).size());
  EXPECT_STREQ("bool", buffer);
}

TEST(ToString, Object) {
  CustomType custom;
  EXPECT_EQ(std::strlen(CustomType::kToString),
            ToString(custom, buffer).size());
  EXPECT_STREQ(CustomType::kToString, buffer);
}

enum Foo : uint8_t {
  BAR = 32,
  BAZ = 100,
};

TEST(ToString, Enum) {
  EXPECT_EQ(2u, ToString(Foo::BAR, buffer).size());
  EXPECT_STREQ("32", buffer);

  EXPECT_EQ(3u, ToString(Foo::BAZ, buffer).size());
  EXPECT_STREQ("100", buffer);
}

TEST(ToString, Status) {
  EXPECT_EQ(2u, ToString(Status(), buffer).size());
  EXPECT_STREQ(Status().str(), buffer);
}

TEST(ToString, StatusCode) {
  EXPECT_EQ(sizeof("UNAVAILABLE") - 1,
            ToString(Status::Unavailable(), buffer).size());
  EXPECT_STREQ("UNAVAILABLE", buffer);
}

TEST(ToString, StdArrayAsBuffer) {
  std::array<char, 128> test_buffer;
  EXPECT_EQ(5u, ToString(false, test_buffer).size());
  EXPECT_STREQ("false", test_buffer.data());
  EXPECT_EQ(2u, ToString("Hi", test_buffer).size());
  EXPECT_STREQ("Hi", test_buffer.data());
  EXPECT_EQ(string::kNullPointerString.size(),
            ToString(static_cast<void*>(nullptr), test_buffer).size());
  EXPECT_EQ(string::kNullPointerString, test_buffer.data());
}

TEST(ToString, StringView) {
  std::string_view view = "cool";
  EXPECT_EQ(4u, ToString(view, buffer).size());
  EXPECT_STREQ("cool", buffer);
}

TEST(ToString, StringView_TooSmall_Truncates) {
  std::string_view view = "kale!";
  EXPECT_EQ(3u, ToString(view, span(buffer, 4)).size());
  EXPECT_STREQ("kal", buffer);
}

TEST(ToString, StringView_EmptyBuffer_WritesNothing) {
  constexpr char kOriginal[] = {'@', '#', '$', '%'};
  char test_buffer[sizeof(kOriginal)];
  std::memcpy(test_buffer, kOriginal, sizeof(kOriginal));

  EXPECT_EQ(0u,
            ToString(std::string_view("Hello!"), span(test_buffer, 0)).size());
  ASSERT_EQ(0, std::memcmp(kOriginal, test_buffer, sizeof(kOriginal)));
}

TEST(ToString, StdString) {
  EXPECT_EQ(5u, ToString(std::string("Whoa!"), buffer).size());
  EXPECT_STREQ("Whoa!", buffer);

  EXPECT_EQ(0u, ToString(std::string(), buffer).size());
  EXPECT_STREQ("", buffer);
}

}  // namespace
}  // namespace pw
