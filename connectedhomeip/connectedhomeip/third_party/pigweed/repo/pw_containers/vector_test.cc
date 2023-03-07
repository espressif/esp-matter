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

#include "pw_containers/vector.h"

#include <cstddef>

#include "gtest/gtest.h"

namespace pw {
namespace {

using namespace std::literals::string_view_literals;

// Since pw::Vector<T, N> downcasts to a pw::Vector<T, 0>, ensure that the
// alignment doesn't change.
static_assert(alignof(Vector<std::max_align_t, 0>) ==
              alignof(Vector<std::max_align_t, 1>));

struct CopyOnly {
  explicit CopyOnly(int val) : value(val) {}

  CopyOnly(const CopyOnly& other) { value = other.value; }

  CopyOnly& operator=(const CopyOnly& other) {
    value = other.value;
    return *this;
  }

  CopyOnly(CopyOnly&&) = delete;

  int value;
};

struct MoveOnly {
  explicit MoveOnly(int val) : value(val) {}

  MoveOnly(const MoveOnly&) = delete;

  MoveOnly(MoveOnly&& other) {
    value = other.value;
    other.value = kDeleted;
  }

  static constexpr int kDeleted = -1138;

  int value;
};

struct Counter {
  static int created;
  static int destroyed;
  static int moved;

  static void Reset() { created = destroyed = moved = 0; }

  Counter() : value(0) { created += 1; }

  Counter(int val) : value(val) { created += 1; }

  Counter(const Counter& other) : value(other.value) { created += 1; }

  Counter(Counter&& other) : value(other.value) {
    other.value = 0;
    moved += 1;
  }

  ~Counter() { destroyed += 1; }

  int value;
};

int Counter::created = 0;
int Counter::destroyed = 0;
int Counter::moved = 0;

TEST(Vector, Construct_NoArg) {
  Vector<int, 3> vector;
  EXPECT_TRUE(vector.empty());
}

TEST(Vector, Construct_MultipleCopies) {
  Vector<int, 3> vector(3, 123);

  EXPECT_EQ(vector.size(), 3u);
  EXPECT_EQ(vector[0], 123);
  EXPECT_EQ(vector[1], 123);
  EXPECT_EQ(vector[2], 123);
}

TEST(Vector, Construct_DefaultSize) {
  Vector<int, 3> vector(3);

  EXPECT_EQ(vector.size(), 3u);
  EXPECT_EQ(vector[0], 0);
  EXPECT_EQ(vector[1], 0);
  EXPECT_EQ(vector[2], 0);
}

TEST(Vector, Construct_Iterators) {
  std::array array{1, 2, 3, 4, 5};

  Vector<int, 64> vector(array.begin(), array.end());

  EXPECT_EQ(vector.size(), array.size());
  for (size_t i = 0; i < array.size(); ++i) {
    EXPECT_EQ(vector[i], array[i]);
  }
}

TEST(Vector, Construct_Copy) {
  CopyOnly origin(5);
  Vector<CopyOnly, 10> origin_vector(3, origin);

  Vector<CopyOnly, 100> vector(origin_vector);

  EXPECT_EQ(3u, vector.size());

  for (size_t i = 0; i < vector.size(); ++i) {
    EXPECT_EQ(vector[i].value, origin.value);
  }
}

TEST(Vector, Construct_Move) {
  Vector<MoveOnly, 10> origin_vector;

  for (int i = 0; i < 5; ++i) {
    origin_vector.emplace_back(421);
  }

  Vector<MoveOnly, 100> vector(std::move(origin_vector));

  EXPECT_EQ(5u, vector.size());

  for (size_t i = 0; i < vector.size(); ++i) {
    EXPECT_EQ(vector[i].value, 421);
  }

  // NOLINTNEXTLINE(bugprone-use-after-move)
  for (size_t i = 0; i < origin_vector.size(); ++i) {
    EXPECT_EQ(origin_vector[i].value, MoveOnly::kDeleted);
  }
}

TEST(Vector, Construct_InitializerList) {
  Vector<int, 3> vector{100, 200};
  EXPECT_EQ(vector.size(), 2u);
  EXPECT_EQ(vector[0], 100);
  EXPECT_EQ(vector[1], 200);
}

struct Aggregate {
  int integer;
  Vector<char, 8> vector;
};

TEST(Vector, Construct_String) {
  const auto vector = Vector<char, 8>{"Hello"};
  EXPECT_EQ(5u, vector.size());
  EXPECT_EQ("Hello"sv, std::string_view(vector.data(), vector.size()));
}

TEST(Vector, Construct_StringTruncates) {
  const auto vector = Vector<char, 8>{"Hello from a long string"};
  EXPECT_EQ(8u, vector.size());
  EXPECT_EQ("Hello fr"sv, std::string_view(vector.data(), vector.size()));
}

TEST(Vector, Construct_AssignFromString) {
  Vector<char, 8> vector = "Hello";
  EXPECT_EQ(5u, vector.size());
  EXPECT_EQ("Hello"sv, std::string_view(vector.data(), vector.size()));
}

TEST(Vector, Construct_AggregateString) {
  Aggregate aggregate = {.integer = 42, .vector = "Hello"};
  EXPECT_EQ(5u, aggregate.vector.size());
  EXPECT_EQ("Hello"sv,
            std::string_view(aggregate.vector.data(), aggregate.vector.size()));
}

TEST(Vector, Construct_StringView) {
  const auto vector = Vector<char, 8>{"Hello"sv};
  EXPECT_EQ(5u, vector.size());
  EXPECT_EQ("Hello"sv, std::string_view(vector.data(), vector.size()));
}

TEST(Vector, Construct_StringViewTruncates) {
  const auto vector = Vector<char, 8>{"Hello from a long string"sv};
  EXPECT_EQ(8u, vector.size());
  EXPECT_EQ("Hello fr"sv, std::string_view(vector.data(), vector.size()));
}

TEST(Vector, Construct_AssignFromStringView) {
  Vector<char, 8> vector = "Hello"sv;
  EXPECT_EQ(5u, vector.size());
  EXPECT_EQ("Hello"sv, std::string_view(vector.data(), vector.size()));
}

TEST(Vector, Construct_AggregateStringView) {
  Aggregate aggregate = {.integer = 42, .vector = "Hello"sv};
  EXPECT_EQ(5u, aggregate.vector.size());
  EXPECT_EQ("Hello"sv,
            std::string_view(aggregate.vector.data(), aggregate.vector.size()));
}

TEST(Vector, Destruct_ZeroLength) {
  Counter::Reset();

  { Vector<Counter, 0> destroyed; }
  EXPECT_EQ(Counter::created, 0);
  EXPECT_EQ(Counter::destroyed, 0);
}

TEST(Vector, Destruct_Empty) {
  Counter::Reset();

  { Vector<Counter, 128> destroyed; }
  EXPECT_EQ(Counter::created, 0);
  EXPECT_EQ(Counter::destroyed, 0);
}

TEST(Vector, Destruct_MultpileEntries) {
  Counter value;
  Counter::Reset();

  { Vector<Counter, 128> destroyed(100, value); }

  EXPECT_EQ(Counter::created, 100);
  EXPECT_EQ(Counter::destroyed, 100);
}

TEST(Vector, Assign_Copy_SmallerToLarger) {
  CopyOnly origin(5);
  Vector<CopyOnly, 3> origin_vector(3, origin);

  Vector<CopyOnly, 2> vector;
  vector = origin_vector;

  EXPECT_EQ(2u, vector.size());

  for (size_t i = 0; i < vector.size(); ++i) {
    EXPECT_EQ(vector[i].value, origin.value);
  }
}

TEST(Vector, Assign_DifferentMaxSize_Copy) {
  const Vector<int, 10> origin_vector = {1, 1, 2, 3};

  Vector<int, 100> vector;
  vector = origin_vector;

  ASSERT_EQ(4u, vector.size());
  EXPECT_EQ(1, vector[0]);
  EXPECT_EQ(1, vector[1]);
  EXPECT_EQ(2, vector[2]);
  EXPECT_EQ(3, vector[3]);
}

TEST(Vector, Assign_SameMaxSize_Copy) {
  const Vector<int, 10> origin_vector = {1, 1, 2, 3};

  Vector<int, 10> vector;
  vector = origin_vector;

  ASSERT_EQ(4u, vector.size());
  EXPECT_EQ(1, vector[0]);
  EXPECT_EQ(1, vector[1]);
  EXPECT_EQ(2, vector[2]);
  EXPECT_EQ(3, vector[3]);
}

TEST(Vector, Assign_Generic_Copy) {
  const Vector<int, 10> origin_vector = {1, 1, 2, 3};

  Vector<int, 10> vector;
  Vector<int>& ref = vector;
  ref = static_cast<const Vector<int>&>(origin_vector);

  ASSERT_EQ(4u, vector.size());
  EXPECT_EQ(1, vector[0]);
  EXPECT_EQ(1, vector[1]);
  EXPECT_EQ(2, vector[2]);
  EXPECT_EQ(3, vector[3]);
}

TEST(Vector, Assign_Move) {
  Vector<MoveOnly, 10> origin_vector;

  for (int i = 0; i < 5; ++i) {
    origin_vector.emplace_back(421);
  }

  Vector<MoveOnly, 10> vector;
  vector = std::move(origin_vector);

  EXPECT_EQ(5u, vector.size());

  for (size_t i = 0; i < vector.size(); ++i) {
    EXPECT_EQ(vector[i].value, 421);
  }

  // NOLINTNEXTLINE(bugprone-use-after-move)
  for (size_t i = 0; i < origin_vector.size(); ++i) {
    EXPECT_EQ(origin_vector[i].value, MoveOnly::kDeleted);
  }
}

TEST(Vector, Assign_InitializerList) {
  Vector<int, 4> vector;
  vector = {1, 3, 5, 7, 9};

  EXPECT_EQ(4u, vector.size());

  EXPECT_EQ(1, vector[0]);
  EXPECT_EQ(3, vector[1]);
  EXPECT_EQ(5, vector[2]);
  EXPECT_EQ(7, vector[3]);
}

TEST(Vector, Access_ZeroLength) {
  Vector<Counter, 0> vector;

  EXPECT_EQ(0u, vector.size());
  EXPECT_EQ(0u, vector.max_size());
  EXPECT_TRUE(vector.empty());
  EXPECT_TRUE(vector.full());

  for (auto& item : vector) {
    (void)item;
    FAIL();
  }
}

TEST(Vector, Access_Data_ArrayLocationIsIndependentOfMaxSize) {
  Vector<int, 10> vector;
  Vector<int>& base = static_cast<Vector<int>&>(vector);

  EXPECT_EQ(vector.data(), base.data());
  EXPECT_EQ(vector.data(), (static_cast<Vector<int, 0>&>(base).data()));
  EXPECT_EQ(vector.data(), (static_cast<Vector<int, 1>&>(base).data()));
}

TEST(Vector, Modify_Clear) {
  Counter::Reset();

  Vector<Counter, 100> vector;
  vector.emplace_back();
  vector.emplace_back();
  vector.emplace_back();

  vector.clear();

  EXPECT_EQ(3, Counter::created);
  EXPECT_EQ(3, Counter::destroyed);
}

TEST(Vector, Modify_PushBack_Copy) {
  Counter value(99);
  Counter::Reset();

  {
    Vector<Counter, 10> vector;
    vector.push_back(value);

    EXPECT_EQ(vector.size(), 1u);
    EXPECT_EQ(vector.front().value, 99);
  }

  EXPECT_EQ(Counter::created, 1);
  EXPECT_EQ(Counter::destroyed, 1);
}

TEST(Vector, Modify_PushBack_Move) {
  Counter::Reset();

  {
    Counter value(99);
    Vector<Counter, 10> vector;
    vector.push_back(std::move(value));

    EXPECT_EQ(vector.size(), 1u);
    EXPECT_EQ(vector.front().value, 99);
    // NOLINTNEXTLINE(bugprone-use-after-move)
    EXPECT_EQ(value.value, 0);
  }

  EXPECT_EQ(Counter::created, 1);
  EXPECT_EQ(Counter::destroyed, 2);
  EXPECT_EQ(Counter::moved, 1);
}

TEST(Vector, Modify_EmplaceBack) {
  Counter::Reset();

  {
    Vector<Counter, 10> vector;
    vector.emplace_back(314);

    EXPECT_EQ(vector.size(), 1u);
    EXPECT_EQ(vector.front().value, 314);
  }

  EXPECT_EQ(Counter::created, 1);
  EXPECT_EQ(Counter::destroyed, 1);
}

TEST(Vector, Modify_Resize_Larger) {
  Vector<CopyOnly, 10> vector(1, CopyOnly(123));
  vector.resize(3, CopyOnly(123));

  EXPECT_EQ(vector.size(), 3u);
  for (auto& i : vector) {
    EXPECT_EQ(i.value, 123);
  }
}

TEST(Vector, Modify_Resize_LargerThanMax) {
  Vector<CopyOnly, 10> vector;
  vector.resize(1000, CopyOnly(123));

  EXPECT_EQ(vector.size(), 10u);
  for (auto& i : vector) {
    EXPECT_EQ(i.value, 123);
  }
}

TEST(Vector, Modify_Resize_Smaller) {
  Vector<CopyOnly, 10> vector(9, CopyOnly(123));
  vector.resize(3, CopyOnly(123));

  EXPECT_EQ(vector.size(), 3u);
  for (auto& i : vector) {
    EXPECT_EQ(i.value, 123);
  }
}

TEST(Vector, Modify_PopBack) {
  Vector<Counter, 10> vector({Counter(1), Counter(2), Counter(3)});
  Counter::Reset();

  vector.pop_back();

  EXPECT_EQ(vector.size(), 2u);
  EXPECT_EQ(vector[0].value, 1);
  EXPECT_EQ(vector[1].value, 2);

  EXPECT_EQ(Counter::created, 0);
  EXPECT_EQ(Counter::destroyed, 1);
}

TEST(Vector, Modify_Resize_Zero) {
  Vector<CopyOnly, 10> vector(10, CopyOnly(123));
  vector.resize(0, CopyOnly(123));

  EXPECT_EQ(vector.size(), 0u);
}

TEST(Vector, Generic) {
  Vector<int, 10> vector{1, 2, 3, 4, 5};

  Vector<int>& generic_vector(vector);

  EXPECT_EQ(generic_vector.size(), vector.size());
  EXPECT_EQ(generic_vector.max_size(), vector.max_size());

  int i = 0;
  for (int value : vector) {
    EXPECT_EQ(value, generic_vector[i]);
    i += 1;
  }

  i = 0;
  for (int value : generic_vector) {
    EXPECT_EQ(vector[i], value);
    i += 1;
  }
}

TEST(Vector, ConstexprMaxSize) {
  Vector<int, 10> vector;
  Vector<int, vector.max_size()> vector2;

  EXPECT_EQ(vector.max_size(), vector2.max_size());

  // The following code would fail with the following compiler error:
  // "non-type template argument is not a constant expression"
  // Reason: the generic_vector doesn't return a constexpr max_size value.
  // Vector<int>& generic_vector(vector);
  // Vector<int, generic_vector.max_size()> vector3;
}

// Test that Vector<T> is trivially destructible when its type is.
static_assert(std::is_trivially_destructible_v<Vector<int>>);
static_assert(std::is_trivially_destructible_v<Vector<int, 4>>);

static_assert(std::is_trivially_destructible_v<MoveOnly>);
static_assert(std::is_trivially_destructible_v<Vector<MoveOnly>>);
static_assert(std::is_trivially_destructible_v<Vector<MoveOnly, 1>>);

static_assert(std::is_trivially_destructible_v<CopyOnly>);
static_assert(std::is_trivially_destructible_v<Vector<CopyOnly>>);
static_assert(std::is_trivially_destructible_v<Vector<CopyOnly, 99>>);

static_assert(!std::is_trivially_destructible_v<Counter>);
static_assert(!std::is_trivially_destructible_v<Vector<Counter>>);
static_assert(!std::is_trivially_destructible_v<Vector<Counter, 99>>);

}  // namespace
}  // namespace pw
