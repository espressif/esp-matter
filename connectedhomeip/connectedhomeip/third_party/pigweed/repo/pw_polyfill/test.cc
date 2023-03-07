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

#include <array>

#include "gtest/gtest.h"
#include "pw_polyfill/language_feature_macros.h"
#include "pw_polyfill/standard.h"
#include "pw_polyfill/standard_library/cstddef.h"
#include "pw_polyfill/standard_library/iterator.h"

namespace pw {
namespace polyfill {
namespace {

PW_INLINE_VARIABLE constexpr int foo = 42;

static_assert(foo == 42, "Error!");

static_assert(PW_CXX_STANDARD_IS_SUPPORTED(98), "C++98 must be supported");
static_assert(PW_CXX_STANDARD_IS_SUPPORTED(11), "C++11 must be supported");
static_assert(PW_CXX_STANDARD_IS_SUPPORTED(14), "C++14 must be supported");

#if __cplusplus >= 201703L
static_assert(PW_CXX_STANDARD_IS_SUPPORTED(17), "C++17 must be not supported");
#else
static_assert(!PW_CXX_STANDARD_IS_SUPPORTED(17), "C++17 must be supported");
#endif  // __cplusplus >= 201703L

#if __cplusplus >= 202002L
static_assert(PW_CXX_STANDARD_IS_SUPPORTED(20), "C++20 must be supported");
#else
static_assert(!PW_CXX_STANDARD_IS_SUPPORTED(20), "C++20 must not be supported");
#endif  // __cplusplus >= 202002L

TEST(Cstddef, Byte_Operators) {
  std::byte value = std::byte(0);
  EXPECT_EQ((value | std::byte(0x12)), std::byte(0x12));
  EXPECT_EQ((value & std::byte(0x12)), std::byte(0));
  EXPECT_EQ((value ^ std::byte(0x12)), std::byte(0x12));
  EXPECT_EQ(~std::byte(0), std::byte(-1));
  EXPECT_EQ((std::byte(1) << 3), std::byte(0x8));
  EXPECT_EQ((std::byte(0x8) >> 3), std::byte(1));
}

TEST(Cstddef, Byte_AssignmentOperators) {
  std::byte value = std::byte(0);
  EXPECT_EQ((value |= std::byte(0x12)), std::byte(0x12));
  EXPECT_EQ((value &= std::byte(0x0F)), std::byte(0x02));
  EXPECT_EQ((value ^= std::byte(0xFF)), std::byte(0xFD));
  EXPECT_EQ((value <<= 4), std::byte(0xD0));
  EXPECT_EQ((value >>= 5), std::byte(0x6));
}

// Check that consteval is at least equivalent to constexpr.
PW_CONSTEVAL int ConstevalFunction() { return 123; }
static_assert(ConstevalFunction() == 123,
              "Function should work in static_assert");

int c_array[5423] = {};
std::array<int, 32> array;

TEST(Iterator, Size) {
  EXPECT_EQ(std::size(c_array), sizeof(c_array) / sizeof(*c_array));
  EXPECT_EQ(std::size(array), array.size());
}

TEST(Iterator, Data) {
  EXPECT_EQ(std::data(c_array), c_array);
  EXPECT_EQ(std::data(array), array.data());
}

PW_CONSTINIT bool mutable_value = true;

TEST(Constinit, ValueIsMutable) {
  ASSERT_TRUE(mutable_value);
  mutable_value = false;
  ASSERT_FALSE(mutable_value);
  mutable_value = true;
}

}  // namespace
}  // namespace polyfill
}  // namespace pw
