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

#include "pw_sync/borrow.h"

#include <chrono>
#include <ratio>

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_sync/virtual_basic_lockable.h"

namespace pw::sync {
namespace {

template <typename Lock>
class BorrowableTest : public ::testing::Test {
 protected:
  static constexpr int kInitialValue = 42;

  BorrowableTest()
      : foo_{.value = kInitialValue}, borrowable_foo_(foo_, lock_) {}

  void SetUp() override {
    EXPECT_FALSE(lock_.locked());  // Ensure it's not locked on construction.
  }

  struct Foo {
    int value;
  };
  Lock lock_;
  Foo foo_;
  Borrowable<Foo, Lock> borrowable_foo_;
};

class BasicLockable : public VirtualBasicLockable {
 public:
  virtual ~BasicLockable() = default;

  bool locked() const { return locked_; }

 protected:
  bool locked_ = false;

 private:
  void DoLockOperation(Operation operation) override {
    switch (operation) {
      case Operation::kLock:
        PW_CHECK(!locked_, "Recursive lock detected");
        locked_ = true;
        return;

      case Operation::kUnlock:
      default:
        PW_CHECK(locked_, "Unlock while unlocked detected");
        locked_ = false;
        return;
    }
  }
};

using BorrowableBasicLockableTest = BorrowableTest<BasicLockable>;

TEST_F(BorrowableBasicLockableTest, Acquire) {
  {
    BorrowedPointer<Foo, BasicLockable> borrowed_foo =
        borrowable_foo_.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, kInitialValue);
    borrowed_foo->value = 13;
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
  EXPECT_EQ(foo_.value, 13);
}

TEST_F(BorrowableBasicLockableTest, RepeatedAcquire) {
  {
    BorrowedPointer<Foo, BasicLockable> borrowed_foo =
        borrowable_foo_.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, kInitialValue);
    borrowed_foo->value = 13;
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
  {
    BorrowedPointer<Foo, BasicLockable> borrowed_foo =
        borrowable_foo_.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, 13);
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
}

TEST_F(BorrowableBasicLockableTest, Moveable) {
  Borrowable<Foo, BasicLockable> borrowable_foo = std::move(borrowable_foo_);
  {
    BorrowedPointer<Foo, BasicLockable> borrowed_foo = borrowable_foo.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, kInitialValue);
    borrowed_foo->value = 13;
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
}

TEST_F(BorrowableBasicLockableTest, Copyable) {
  const Borrowable<Foo, BasicLockable>& other = borrowable_foo_;
  Borrowable<Foo, BasicLockable> borrowable_foo(other);
  {
    BorrowedPointer<Foo, BasicLockable> borrowed_foo = borrowable_foo.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, kInitialValue);
    borrowed_foo->value = 13;
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
}

class Lockable : public BasicLockable {
 public:
  bool try_lock() {
    if (locked()) {
      return false;
    }
    locked_ = true;
    return true;
  }
};

using BorrowableLockableTest = BorrowableTest<Lockable>;

TEST_F(BorrowableLockableTest, Acquire) {
  {
    BorrowedPointer<Foo, Lockable> borrowed_foo = borrowable_foo_.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, kInitialValue);
    borrowed_foo->value = 13;
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
  EXPECT_EQ(foo_.value, 13);
}

TEST_F(BorrowableLockableTest, RepeatedAcquire) {
  {
    BorrowedPointer<Foo, Lockable> borrowed_foo = borrowable_foo_.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, kInitialValue);
    borrowed_foo->value = 13;
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
  {
    BorrowedPointer<Foo, Lockable> borrowed_foo = borrowable_foo_.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, 13);
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
}

TEST_F(BorrowableLockableTest, TryAcquireSuccess) {
  {
    std::optional<BorrowedPointer<Foo, Lockable>> maybe_borrowed_foo =
        borrowable_foo_.try_acquire();
    ASSERT_TRUE(maybe_borrowed_foo.has_value());
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(maybe_borrowed_foo.value()->value, kInitialValue);
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
}

TEST_F(BorrowableLockableTest, TryAcquireFailure) {
  lock_.lock();
  EXPECT_TRUE(lock_.locked());
  {
    std::optional<BorrowedPointer<Foo, Lockable>> maybe_borrowed_foo =
        borrowable_foo_.try_acquire();
    EXPECT_FALSE(maybe_borrowed_foo.has_value());
  }
  EXPECT_TRUE(lock_.locked());
  lock_.unlock();
}

struct Clock {
  using rep = int64_t;
  using period = std::micro;
  using duration = std::chrono::duration<rep, period>;
  using time_point = std::chrono::time_point<Clock>;
};

class TimedLockable : public Lockable {
 public:
  bool try_lock() {
    if (locked()) {
      return false;
    }
    locked_ = true;
    return true;
  }

  bool try_lock_for(const Clock::duration&) { return try_lock(); }
  bool try_lock_until(const Clock::time_point&) { return try_lock(); }
};

using BorrowableTimedLockableTest = BorrowableTest<TimedLockable>;

TEST_F(BorrowableTimedLockableTest, Acquire) {
  {
    BorrowedPointer<Foo, TimedLockable> borrowed_foo =
        borrowable_foo_.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, kInitialValue);
    borrowed_foo->value = 13;
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
  EXPECT_EQ(foo_.value, 13);
}

TEST_F(BorrowableTimedLockableTest, RepeatedAcquire) {
  {
    BorrowedPointer<Foo, TimedLockable> borrowed_foo =
        borrowable_foo_.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, kInitialValue);
    borrowed_foo->value = 13;
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
  {
    BorrowedPointer<Foo, TimedLockable> borrowed_foo =
        borrowable_foo_.acquire();
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(borrowed_foo->value, 13);
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
}

TEST_F(BorrowableTimedLockableTest, TryAcquireSuccess) {
  {
    std::optional<BorrowedPointer<Foo, TimedLockable>> maybe_borrowed_foo =
        borrowable_foo_.try_acquire();
    ASSERT_TRUE(maybe_borrowed_foo.has_value());
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(maybe_borrowed_foo.value()->value, kInitialValue);
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
}

TEST_F(BorrowableTimedLockableTest, TryAcquireFailure) {
  lock_.lock();
  EXPECT_TRUE(lock_.locked());
  {
    std::optional<BorrowedPointer<Foo, TimedLockable>> maybe_borrowed_foo =
        borrowable_foo_.try_acquire();
    EXPECT_FALSE(maybe_borrowed_foo.has_value());
  }
  EXPECT_TRUE(lock_.locked());
  lock_.unlock();
}

TEST_F(BorrowableTimedLockableTest, TryAcquireForSuccess) {
  {
    std::optional<BorrowedPointer<Foo, TimedLockable>> maybe_borrowed_foo =
        borrowable_foo_.try_acquire_for(std::chrono::seconds(0));
    ASSERT_TRUE(maybe_borrowed_foo.has_value());
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(maybe_borrowed_foo.value()->value, kInitialValue);
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
}

TEST_F(BorrowableTimedLockableTest, TryAcquireForFailure) {
  lock_.lock();
  EXPECT_TRUE(lock_.locked());
  {
    std::optional<BorrowedPointer<Foo, TimedLockable>> maybe_borrowed_foo =
        borrowable_foo_.try_acquire_for(std::chrono::seconds(0));
    EXPECT_FALSE(maybe_borrowed_foo.has_value());
  }
  EXPECT_TRUE(lock_.locked());
  lock_.unlock();
}

TEST_F(BorrowableTimedLockableTest, TryAcquireUntilSuccess) {
  {
    std::optional<BorrowedPointer<Foo, TimedLockable>> maybe_borrowed_foo =
        borrowable_foo_.try_acquire_until(
            Clock::time_point(std::chrono::seconds(0)));
    ASSERT_TRUE(maybe_borrowed_foo.has_value());
    EXPECT_TRUE(lock_.locked());  // Ensure the lock is held.
    EXPECT_EQ(maybe_borrowed_foo.value()->value, kInitialValue);
  }
  EXPECT_FALSE(lock_.locked());  // Ensure the lock is released.
}

TEST_F(BorrowableTimedLockableTest, TryAcquireUntilFailure) {
  lock_.lock();
  EXPECT_TRUE(lock_.locked());
  {
    std::optional<BorrowedPointer<Foo, TimedLockable>> maybe_borrowed_foo =
        borrowable_foo_.try_acquire_until(
            Clock::time_point(std::chrono::seconds(0)));
    EXPECT_FALSE(maybe_borrowed_foo.has_value());
  }
  EXPECT_TRUE(lock_.locked());
  lock_.unlock();
}

}  // namespace
}  // namespace pw::sync
