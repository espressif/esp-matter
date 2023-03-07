// Copyright 2022 The Pigweed Authors
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
//
// These tests were forked from
// https://cs.opensource.google/abseil/abseil-cpp/+/main:absl/algorithm/algorithm_test.cc;drc=38b704384cd2f17590b3922b97744be0b43622c9
// However they were modified to work with containers available in Pigweed
// without dynamic allocations, i.e. no std::vector, std::map, std::list, etc.
#include "pw_containers/algorithm.h"

#include <algorithm>
#include <iterator>

#include "gtest/gtest.h"
#include "pw_containers/intrusive_list.h"
#include "pw_containers/vector.h"
#include "pw_span/span.h"

namespace {

class TestItem : public pw::IntrusiveList<TestItem>::Item {
 public:
  TestItem() : number_(0) {}
  TestItem(int number) : number_(number) {}

  // Add equality comparison to ensure comparisons are done by identity rather
  // than equality for the remove function.
  bool operator==(const TestItem& other) const {
    return number_ == other.number_;
  }

 private:
  int number_;
};

// Most of these tests just check that the code compiles, not that it
// does the right thing. That's fine since the functions just forward
// to the STL implementation.
class NonMutatingTest : public testing::Test {
 protected:
  std::array<int, 3> span_array_ = {1, 2, 3};
  pw::span<int> span_{NonMutatingTest::span_array_};
  pw::Vector<int, 3> vector_ = {1, 2, 3};
  int array_[3] = {1, 2, 3};
};

struct AccumulateCalls {
  void operator()(int value) { calls.push_back(value); }
  pw::Vector<int, 10> calls;
};

bool Predicate(int value) { return value < 3; }
bool BinPredicate(int v1, int v2) { return v1 < v2; }
bool Equals(int v1, int v2) { return v1 == v2; }

}  // namespace

TEST_F(NonMutatingTest, AllOf) {
  const pw::Vector<int>& v = vector_;
  EXPECT_FALSE(pw::containers::AllOf(v, [](int x) { return x > 1; }));
  EXPECT_TRUE(pw::containers::AllOf(v, [](int x) { return x > 0; }));
}

TEST_F(NonMutatingTest, AnyOf) {
  const pw::Vector<int>& v = vector_;
  EXPECT_TRUE(pw::containers::AnyOf(v, [](int x) { return x > 2; }));
  EXPECT_FALSE(pw::containers::AnyOf(v, [](int x) { return x > 5; }));
}

TEST_F(NonMutatingTest, NoneOf) {
  const pw::Vector<int>& v = vector_;
  EXPECT_FALSE(pw::containers::NoneOf(v, [](int x) { return x > 2; }));
  EXPECT_TRUE(pw::containers::NoneOf(v, [](int x) { return x > 5; }));
}

TEST_F(NonMutatingTest, ForEach) {
  AccumulateCalls c = pw::containers::ForEach(span_, AccumulateCalls());
  std::sort(c.calls.begin(), c.calls.end());
  EXPECT_EQ(vector_, c.calls);
}

TEST_F(NonMutatingTest, FindReturnsCorrectType) {
  auto it = pw::containers::Find(span_, 3);
  EXPECT_EQ(3, *it);
  pw::containers::Find(vector_, 3);
}

TEST_F(NonMutatingTest, FindIf) { pw::containers::FindIf(span_, Predicate); }

TEST_F(NonMutatingTest, FindIfNot) {
  pw::containers::FindIfNot(span_, Predicate);
}

TEST_F(NonMutatingTest, FindEnd) {
  pw::containers::FindEnd(array_, vector_);
  pw::containers::FindEnd(vector_, array_);
}

TEST_F(NonMutatingTest, FindEndWithPredicate) {
  pw::containers::FindEnd(array_, vector_, BinPredicate);
  pw::containers::FindEnd(vector_, array_, BinPredicate);
}

TEST_F(NonMutatingTest, FindFirstOf) {
  pw::containers::FindFirstOf(span_, array_);
  pw::containers::FindFirstOf(array_, span_);
}

TEST_F(NonMutatingTest, FindFirstOfWithPredicate) {
  pw::containers::FindFirstOf(span_, array_, BinPredicate);
  pw::containers::FindFirstOf(array_, span_, BinPredicate);
}

TEST_F(NonMutatingTest, AdjacentFind) { pw::containers::AdjacentFind(array_); }

TEST_F(NonMutatingTest, AdjacentFindWithPredicate) {
  pw::containers::AdjacentFind(array_, BinPredicate);
}

TEST_F(NonMutatingTest, Count) {
  EXPECT_EQ(1, pw::containers::Count(span_, 3));
}

TEST_F(NonMutatingTest, CountIf) {
  EXPECT_EQ(2, pw::containers::CountIf(span_, Predicate));
}

TEST_F(NonMutatingTest, Mismatch) {
  // Testing necessary as pw::containers::Mismatch executes logic.
  {
    auto result = pw::containers::Mismatch(span_, vector_);
    EXPECT_EQ(result.first, span_.end());
    EXPECT_EQ(result.second, vector_.end());
  }
  {
    auto result = pw::containers::Mismatch(vector_, span_);
    EXPECT_EQ(result.first, vector_.end());
    EXPECT_EQ(result.second, span_.end());
  }

  vector_.back() = 5;
  {
    auto result = pw::containers::Mismatch(span_, vector_);
    EXPECT_EQ(result.first, std::prev(span_.end()));
    EXPECT_EQ(result.second, std::prev(vector_.end()));
  }
  {
    auto result = pw::containers::Mismatch(vector_, span_);
    EXPECT_EQ(result.first, std::prev(vector_.end()));
    EXPECT_EQ(result.second, std::prev(span_.end()));
  }

  vector_.pop_back();
  {
    auto result = pw::containers::Mismatch(span_, vector_);
    EXPECT_EQ(result.first, std::prev(span_.end()));
    EXPECT_EQ(result.second, vector_.end());
  }
  {
    auto result = pw::containers::Mismatch(vector_, span_);
    EXPECT_EQ(result.first, vector_.end());
    EXPECT_EQ(result.second, std::prev(span_.end()));
  }
  {
    struct NoNotEquals {
      constexpr bool operator==(NoNotEquals) const { return true; }
      constexpr bool operator!=(NoNotEquals) const = delete;
    };
    pw::Vector<NoNotEquals, 1> first;
    std::array<NoNotEquals, 1> second;

    // Check this still compiles.
    pw::containers::Mismatch(first, second);
  }
}

TEST_F(NonMutatingTest, MismatchWithPredicate) {
  // Testing necessary as pw::containers::Mismatch executes logic.
  {
    auto result = pw::containers::Mismatch(span_, vector_, BinPredicate);
    EXPECT_EQ(result.first, span_.begin());
    EXPECT_EQ(result.second, vector_.begin());
  }
  {
    auto result = pw::containers::Mismatch(vector_, span_, BinPredicate);
    EXPECT_EQ(result.first, vector_.begin());
    EXPECT_EQ(result.second, span_.begin());
  }

  vector_.front() = 0;
  {
    auto result = pw::containers::Mismatch(span_, vector_, BinPredicate);
    EXPECT_EQ(result.first, span_.begin());
    EXPECT_EQ(result.second, vector_.begin());
  }
  {
    auto result = pw::containers::Mismatch(vector_, span_, BinPredicate);
    EXPECT_EQ(result.first, std::next(vector_.begin()));
    EXPECT_EQ(result.second, std::next(span_.begin()));
  }

  vector_.clear();
  {
    auto result = pw::containers::Mismatch(span_, vector_, BinPredicate);
    EXPECT_EQ(result.first, span_.begin());
    EXPECT_EQ(result.second, vector_.end());
  }
  {
    auto result = pw::containers::Mismatch(vector_, span_, BinPredicate);
    EXPECT_EQ(result.first, vector_.end());
    EXPECT_EQ(result.second, span_.begin());
  }
}

TEST_F(NonMutatingTest, Equal) {
  EXPECT_TRUE(pw::containers::Equal(vector_, span_));
  EXPECT_TRUE(pw::containers::Equal(span_, vector_));
  EXPECT_TRUE(pw::containers::Equal(span_, array_));
  EXPECT_TRUE(pw::containers::Equal(array_, vector_));

  // Test that behavior appropriately differs from that of equal().
  pw::Vector<int, 4> vector_plus = {1, 2, 3};
  vector_plus.push_back(4);
  EXPECT_FALSE(pw::containers::Equal(vector_plus, span_));
  EXPECT_FALSE(pw::containers::Equal(span_, vector_plus));
  EXPECT_FALSE(pw::containers::Equal(array_, vector_plus));
}

TEST_F(NonMutatingTest, EqualWithPredicate) {
  EXPECT_TRUE(pw::containers::Equal(vector_, span_, Equals));
  EXPECT_TRUE(pw::containers::Equal(span_, vector_, Equals));
  EXPECT_TRUE(pw::containers::Equal(array_, span_, Equals));
  EXPECT_TRUE(pw::containers::Equal(vector_, array_, Equals));

  // Test that behavior appropriately differs from that of equal().
  pw::Vector<int, 4> vector_plus = {1, 2, 3};
  vector_plus.push_back(4);
  EXPECT_FALSE(pw::containers::Equal(vector_plus, span_, Equals));
  EXPECT_FALSE(pw::containers::Equal(span_, vector_plus, Equals));
  EXPECT_FALSE(pw::containers::Equal(vector_plus, array_, Equals));
}

TEST_F(NonMutatingTest, IsPermutation) {
  auto vector_permut_ = vector_;
  std::next_permutation(vector_permut_.begin(), vector_permut_.end());
  EXPECT_TRUE(pw::containers::IsPermutation(vector_permut_, span_));
  EXPECT_TRUE(pw::containers::IsPermutation(span_, vector_permut_));

  // Test that behavior appropriately differs from that of is_permutation().
  pw::Vector<int, 4> vector_plus = {1, 2, 3};
  vector_plus.push_back(4);
  EXPECT_FALSE(pw::containers::IsPermutation(vector_plus, span_));
  EXPECT_FALSE(pw::containers::IsPermutation(span_, vector_plus));
}

TEST_F(NonMutatingTest, IsPermutationWithPredicate) {
  auto vector_permut_ = vector_;
  std::next_permutation(vector_permut_.begin(), vector_permut_.end());
  EXPECT_TRUE(pw::containers::IsPermutation(vector_permut_, span_, Equals));
  EXPECT_TRUE(pw::containers::IsPermutation(span_, vector_permut_, Equals));

  // Test that behavior appropriately differs from that of is_permutation().
  pw::Vector<int, 4> vector_plus = {1, 2, 3};
  vector_plus.push_back(4);
  EXPECT_FALSE(pw::containers::IsPermutation(vector_plus, span_, Equals));
  EXPECT_FALSE(pw::containers::IsPermutation(span_, vector_plus, Equals));
}

TEST_F(NonMutatingTest, Search) {
  pw::containers::Search(span_, vector_);
  pw::containers::Search(vector_, span_);
  pw::containers::Search(array_, span_);
}

TEST_F(NonMutatingTest, SearchWithPredicate) {
  pw::containers::Search(span_, vector_, BinPredicate);
  pw::containers::Search(vector_, span_, BinPredicate);
}

TEST_F(NonMutatingTest, SearchN) { pw::containers::SearchN(span_, 3, 1); }

TEST_F(NonMutatingTest, SearchNWithPredicate) {
  pw::containers::SearchN(span_, 3, 1, BinPredicate);
}
