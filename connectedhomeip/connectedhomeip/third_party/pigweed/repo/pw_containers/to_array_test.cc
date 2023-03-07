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

#include "pw_containers/to_array.h"

#include "gtest/gtest.h"

namespace pw {
namespace containers {
namespace {

TEST(Array, ToArray_StringLiteral) {
  std::array<char, sizeof("literally!")> array = to_array("literally!");
  EXPECT_EQ(std::strcmp(array.data(), "literally!"), 0);
}

TEST(Array, ToArray_Inline) {
  constexpr std::array<int, 3> kArray = to_array({1, 2, 3});
  static_assert(kArray.size() == 3, "Size should be 3 as initialized");
  EXPECT_EQ(kArray[0], 1);
}

TEST(Array, ToArray_Array) {
  char c_array[] = "array!";
  std::array<char, sizeof("array!")> array = to_array(c_array);
  EXPECT_EQ(std::strcmp(array.data(), "array!"), 0);
}

struct MoveOnly {
  MoveOnly(char ch) : value(ch) {}

  MoveOnly(const MoveOnly&) = delete;
  MoveOnly& operator=(const MoveOnly&) = delete;

  MoveOnly(MoveOnly&&) = default;
  MoveOnly& operator=(MoveOnly&&) = default;

  char value;
};

TEST(Array, ToArray_MoveOnly) {
  MoveOnly c_array[]{MoveOnly('a'), MoveOnly('b')};
  std::array<MoveOnly, 2> array = to_array(std::move(c_array));
  EXPECT_EQ(array[0].value, 'a');
  EXPECT_EQ(array[1].value, 'b');
}

}  // namespace
}  // namespace containers
}  // namespace pw
