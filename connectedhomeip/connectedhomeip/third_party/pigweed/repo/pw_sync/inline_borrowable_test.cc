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

#include "pw_sync/inline_borrowable.h"

#include <array>
#include <chrono>
#include <tuple>

#include "gtest/gtest.h"
#include "pw_sync/interrupt_spin_lock.h"
#include "pw_sync/lock_annotations.h"
#include "pw_sync/mutex.h"

namespace pw::sync {
namespace {

using namespace std::chrono_literals;

// A trivial type that is copyable and movable.
struct TrivialType {
  bool yes() const { return true; }
};

// A custom type that is neither copyable nor movable.
class CustomType {
 public:
  explicit constexpr CustomType(int z) : x_(z), y_(-z) {}
  constexpr CustomType(int x, int y) : x_(x), y_(y) {}

  CustomType(const CustomType&) = delete;
  CustomType& operator=(const CustomType&) = delete;
  CustomType(CustomType&&) = delete;
  CustomType&& operator=(CustomType&&) = delete;

  std::pair<int, int> data() const { return std::make_pair(x_, y_); }

 private:
  int x_, y_;
};

// A custom lockable interface.
class PW_LOCKABLE("VirtualCustomLocakble") VirtualCustomLockable {
 public:
  virtual ~VirtualCustomLockable() {}

  virtual void lock() PW_EXCLUSIVE_LOCK_FUNCTION() = 0;
  virtual void unlock() PW_UNLOCK_FUNCTION() = 0;
};

// A custom mutex type that requires a constructor parameter.
class PW_LOCKABLE("VirtualCustomMutex") VirtualCustomMutex
    : public VirtualCustomLockable {
 public:
  explicit VirtualCustomMutex(int id) : mutex_{}, id_{id} {}

  void lock() override PW_EXCLUSIVE_LOCK_FUNCTION() { mutex_.lock(); }
  void unlock() override PW_UNLOCK_FUNCTION() { mutex_.unlock(); }

  int id() const { return id_; }

 private:
  pw::sync::Mutex mutex_;
  int id_;
};

TEST(InlineBorrowableTest, TestTrivialType) {
  InlineBorrowable<TrivialType> trivial;
  EXPECT_TRUE(trivial.acquire()->yes());
}

TEST(InlineBorrowableTest, TestCustomTypeInPlace1Arg) {
  InlineBorrowable<CustomType> custom(std::in_place, 1);
  EXPECT_EQ(custom.acquire()->data(), std::make_pair(1, -1));
}

TEST(InlineBorrowableTest, TestCustomTypeInPlace1ArgLValue) {
  int x = 1;
  InlineBorrowable<CustomType> custom(std::in_place, x);
  EXPECT_EQ(custom.acquire()->data(), std::make_pair(x, -x));
}

TEST(InlineBorrowableTest, TestCustomTypeInPlace2Arg) {
  InlineBorrowable<CustomType> custom(std::in_place, 1, 2);
  EXPECT_EQ(custom.acquire()->data(), std::make_pair(1, 2));
}

TEST(InlineBorrowableTest, TestCustomTypeFromTuple) {
  InlineBorrowable<CustomType> custom(std::make_tuple(1, 2));
  EXPECT_EQ(custom.acquire()->data(), std::make_pair(1, 2));
}

TEST(InlineBorrowableTest, TestCustomTypeFromFactory) {
  InlineBorrowable<CustomType> custom([] { return CustomType(1, 2); });
  EXPECT_EQ(custom.acquire()->data(), std::make_pair(1, 2));
}

TEST(InlineBorrowableTest, TestCustomTypeFromMutableFactory) {
  int i = 0;
  auto factory = [&i]() mutable {
    i++;
    return CustomType(1, 2);
  };
  InlineBorrowable<CustomType> custom(factory);
  EXPECT_EQ(custom.acquire()->data(), std::make_pair(1, 2));
}

TEST(InlineBorrowableTest, TestTrivialTypeWithInterruptSpinLock) {
  InlineBorrowable<TrivialType, VirtualInterruptSpinLock>
      trivial_interrupt_safe;
  EXPECT_TRUE(trivial_interrupt_safe.acquire()->yes());
}

TEST(InlineBorrowableTest, TestCustomTypeWithInterruptSpinLock) {
  InlineBorrowable<CustomType, VirtualInterruptSpinLock> custom_interrupt_safe(
      std::in_place, 1, 2);
  EXPECT_EQ(custom_interrupt_safe.acquire()->data(), std::make_pair(1, 2));
}

TEST(InlineBorrowableTest, TestCustomTypeWithCustomMutexFromTuple) {
  InlineBorrowable<CustomType, VirtualCustomMutex, VirtualCustomLockable>
      custom_mutex(std::make_tuple(1, 2), std::make_tuple(42));
  EXPECT_EQ(custom_mutex.acquire()->data(), std::make_pair(1, 2));
}

TEST(InlineBorrowableTest, TestCustomTypeWithCustomMutexFromFactory) {
  InlineBorrowable<CustomType, VirtualCustomMutex, VirtualCustomLockable>
      custom_mutex([] { return CustomType(1, 2); },
                   [] { return VirtualCustomMutex(42); });
  EXPECT_EQ(custom_mutex.acquire()->data(), std::make_pair(1, 2));
}

TEST(InlineBorrowableTest, TestArrayAggregateInitializationInPlace) {
  using ArrayAggregate = std::array<int, 2>;
  InlineBorrowable<ArrayAggregate> aggregate{std::in_place, 1, 2};
  EXPECT_EQ((*aggregate.acquire())[0], 1);
  EXPECT_EQ((*aggregate.acquire())[1], 2);
}

struct StructAggregate {
  int a;
  int b;
};

TEST(InlineBorrowableTest, TestStructAggregateInitializationInPlace) {
  InlineBorrowable<StructAggregate> aggregate{std::in_place, 1, 2};
  EXPECT_EQ(aggregate.acquire()->a, 1);
  EXPECT_EQ(aggregate.acquire()->b, 2);
}

TEST(InlineBorrowableTest, TestStructAggregateInitializationFromFactory) {
  InlineBorrowable<StructAggregate> aggregate(
      []() -> StructAggregate { return {.a = 1, .b = 2}; });
  EXPECT_EQ(aggregate.acquire()->a, 1);
  EXPECT_EQ(aggregate.acquire()->b, 2);
}

TEST(InlineBorrowableTest,
     TestStructAggregateInitializationFromMutableFactory) {
  int i = 0;
  auto factory = [&i]() mutable -> StructAggregate {
    i++;
    return {.a = 1, .b = 2};
  };
  InlineBorrowable<StructAggregate> aggregate(factory);
  EXPECT_EQ(aggregate.acquire()->a, 1);
  EXPECT_EQ(aggregate.acquire()->b, 2);
}

struct ReferenceTypes {
  ReferenceTypes(const int& a, int& b, BorrowedPointer<int>&& c)
      : in(a), out(b), borrowed(std::move(c)) {}
  const int& in;
  int& out;
  BorrowedPointer<int> borrowed;  // move-only type
};

class InlineBorrowableReferenceTypesTest : public ::testing::Test {
 protected:
  int input_ = 1;
  int output_ = 2;
  InlineBorrowable<int> borrowable_{std::in_place, 3};

  void Validate(BorrowedPointer<ReferenceTypes>&& references) {
    EXPECT_EQ(references->in, 1);
    EXPECT_EQ(references->out, 2);
    EXPECT_EQ(*references->borrowed, 3);

    references->out = -2;
    EXPECT_EQ(output_, -2);
  }
};

TEST_F(InlineBorrowableReferenceTypesTest, TestInPlace) {
  InlineBorrowable<ReferenceTypes> references(
      std::in_place, input_, output_, borrowable_.acquire());
  Validate(references.acquire());
}

TEST_F(InlineBorrowableReferenceTypesTest, TestFromTuple) {
  InlineBorrowable<ReferenceTypes> references(
      std::forward_as_tuple(input_, output_, borrowable_.acquire()));
  Validate(references.acquire());
}

TEST_F(InlineBorrowableReferenceTypesTest, TestFromFactory) {
  InlineBorrowable<ReferenceTypes> references(
      [&] { return ReferenceTypes(input_, output_, borrowable_.acquire()); });
  Validate(references.acquire());
}

}  // namespace
}  // namespace pw::sync
