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

#include "pw_containers/flat_map.h"

#include <limits>

#include "gtest/gtest.h"

namespace pw::containers {
namespace {
constexpr FlatMap<int, char, 5> kOddMap({{
    {-3, 'a'},
    {0, 'b'},
    {1, 'c'},
    {50, 'd'},
    {100, 'e'},
}});
}  // namespace

TEST(FlatMap, Size) { EXPECT_EQ(kOddMap.size(), static_cast<uint32_t>(5)); }

TEST(FlatMap, EmptyFlatMapSize) {
  constexpr FlatMap<int, char, 0> kEmpty({{}});
  EXPECT_EQ(kEmpty.size(), static_cast<uint32_t>(0));
}

TEST(FlatMap, Empty) {
  constexpr FlatMap<int, char, 0> kEmpty({{}});
  EXPECT_TRUE(kEmpty.empty());
}

TEST(FlatMap, NotEmpty) {
  constexpr FlatMap<int, char, 1> kNotEmpty({{}});
  EXPECT_FALSE(kNotEmpty.empty());
}

TEST(FlatMap, EmptyFlatMapFind) {
  constexpr FlatMap<int, char, 0> kEmpty({{}});
  EXPECT_EQ(kEmpty.find(0), kEmpty.end());
}

TEST(FlatMap, EmptyFlatMapLowerBound) {
  constexpr FlatMap<int, char, 0> kEmpty({{}});
  EXPECT_EQ(kEmpty.lower_bound(0), kEmpty.end());
}

TEST(FlatMap, EmptyFlatMapUpperBound) {
  constexpr FlatMap<int, char, 0> kEmpty({{}});
  EXPECT_EQ(kEmpty.upper_bound(0), kEmpty.end());
}

TEST(FlatMap, EmptyEqualRange) {
  constexpr FlatMap<int, char, 0> kEmpty({{}});
  EXPECT_EQ(kEmpty.equal_range(0).first, kEmpty.end());
  EXPECT_EQ(kEmpty.equal_range(0).second, kEmpty.end());
}

TEST(FlatMap, Contains) {
  EXPECT_TRUE(kOddMap.contains(0));
  EXPECT_FALSE(kOddMap.contains(10));
}

TEST(FlatMap, Iterate) {
  char value = 'a';
  for (const auto& item : kOddMap) {
    EXPECT_EQ(value, item.second);
    EXPECT_EQ(&item, kOddMap.find(item.first));
    value += 1;
  }
}

TEST(FlatMap, EqualRange) {
  auto pair = kOddMap.equal_range(1);
  EXPECT_EQ(1, pair.first->first);
  EXPECT_EQ(50, pair.second->first);

  pair = kOddMap.equal_range(75);
  EXPECT_EQ(100, pair.first->first);
  EXPECT_EQ(100, pair.second->first);
}

TEST(FlatMap, Find) {
  auto it = kOddMap.find(50);
  EXPECT_EQ(50, it->first);
  EXPECT_EQ('d', it->second);

  auto not_found = kOddMap.find(-1);
  EXPECT_EQ(kOddMap.cend(), not_found);
}

TEST(FlatMap, UpperBoundLessThanSmallestKey) {
  EXPECT_EQ(-3, kOddMap.upper_bound(std::numeric_limits<int>::min())->first);
  EXPECT_EQ(-3, kOddMap.upper_bound(-123)->first);
  EXPECT_EQ(-3, kOddMap.upper_bound(-4)->first);
}

TEST(FlatMap, UpperBoundBetweenTheTwoSmallestKeys) {
  EXPECT_EQ(0, kOddMap.upper_bound(-3)->first);
  EXPECT_EQ(0, kOddMap.upper_bound(-2)->first);
  EXPECT_EQ(0, kOddMap.upper_bound(-1)->first);
}

TEST(FlatMap, UpperBoundIntermediateKeys) {
  EXPECT_EQ(1, kOddMap.upper_bound(0)->first);
  EXPECT_EQ('c', kOddMap.upper_bound(0)->second);
  EXPECT_EQ(50, kOddMap.upper_bound(1)->first);
  EXPECT_EQ('d', kOddMap.upper_bound(1)->second);
  EXPECT_EQ(50, kOddMap.upper_bound(2)->first);
  EXPECT_EQ(50, kOddMap.upper_bound(49)->first);
  EXPECT_EQ(100, kOddMap.upper_bound(51)->first);
}

TEST(FlatMap, UpperBoundGreaterThanLargestKey) {
  EXPECT_EQ(kOddMap.end(), kOddMap.upper_bound(100));
  EXPECT_EQ(kOddMap.end(), kOddMap.upper_bound(2384924));
  EXPECT_EQ(kOddMap.end(),
            kOddMap.upper_bound(std::numeric_limits<int>::max()));
}

TEST(FlatMap, LowerBoundLessThanSmallestKey) {
  EXPECT_EQ(-3, kOddMap.lower_bound(std::numeric_limits<int>::min())->first);
  EXPECT_EQ(-3, kOddMap.lower_bound(-123)->first);
  EXPECT_EQ(-3, kOddMap.lower_bound(-4)->first);
}

TEST(FlatMap, LowerBoundBetweenTwoSmallestKeys) {
  EXPECT_EQ(-3, kOddMap.lower_bound(-3)->first);
  EXPECT_EQ(0, kOddMap.lower_bound(-2)->first);
  EXPECT_EQ(0, kOddMap.lower_bound(-1)->first);
}

TEST(FlatMap, LowerBoundIntermediateKeys) {
  EXPECT_EQ(0, kOddMap.lower_bound(0)->first);
  EXPECT_EQ('b', kOddMap.lower_bound(0)->second);
  EXPECT_EQ(1, kOddMap.lower_bound(1)->first);
  EXPECT_EQ('c', kOddMap.lower_bound(1)->second);
  EXPECT_EQ(50, kOddMap.lower_bound(2)->first);
  EXPECT_EQ(50, kOddMap.lower_bound(49)->first);
  EXPECT_EQ(100, kOddMap.lower_bound(51)->first);
}

TEST(FlatMap, LowerBoundGreaterThanLargestKey) {
  EXPECT_EQ(100, kOddMap.lower_bound(100)->first);
  EXPECT_EQ(kOddMap.end(), kOddMap.lower_bound(2384924));
  EXPECT_EQ(kOddMap.end(),
            kOddMap.lower_bound(std::numeric_limits<int>::max()));
}

TEST(FlatMap, ForEachIteration) {
  for (const auto& item : kOddMap) {
    EXPECT_NE(item.first, 2);
  }
}

TEST(FlatMap, MapsWithUnsortedKeys) {
  constexpr FlatMap<int, const char*, 2> bad_array({{
      {2, "hello"},
      {1, "goodbye"},
  }});

  EXPECT_EQ(bad_array.begin()->first, 1);

  constexpr FlatMap<int, const char*, 2> too_short({{
      {1, "goodbye"},
  }});
  EXPECT_EQ(too_short.begin()->first, 0);
}

TEST(FlatMap, DontDereferenceEnd) {
  constexpr FlatMap<int, const char*, 2> unsorted_array({{
      {2, "hello"},
      {1, "goodbye"},
  }});

  EXPECT_EQ(unsorted_array.contains(3), false);
}

}  // namespace pw::containers
