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

#include "pw_intrusive_ptr/intrusive_ptr.h"

#include <stdint.h>

#include <utility>

#include "gtest/gtest.h"

namespace pw {
namespace {

class TestItem : public RefCounted<TestItem> {
 public:
  TestItem() { ++instance_counter; }

  explicit TestItem(int32_t f) : TestItem() { first = f; }

  explicit TestItem(int64_t s) : TestItem() { second = s; }

  TestItem(int32_t f, int64_t s) : TestItem() {
    first = f;
    second = s;
  }

  TestItem(const TestItem&) : TestItem() {}
  TestItem(TestItem&&) noexcept : TestItem() {}

  TestItem& operator=(const TestItem& other) {
    if (&other != this) {
      ++instance_counter;
    }
    return *this;
  }

  TestItem& operator=(TestItem&& other) noexcept {
    if (&other != this) {
      ++instance_counter;
    }
    return *this;
  }

  virtual ~TestItem() { --instance_counter; }

  inline static int32_t instance_counter = 0;

  int32_t first = 0;
  int64_t second = 1;
};

class TestItemDerived : public TestItem {
 public:
  TestItemDerived() { ++derived_instance_counter; }
  TestItemDerived(const TestItemDerived&) : TestItemDerived() {}
  TestItemDerived(TestItemDerived&&) noexcept : TestItemDerived() {}

  TestItemDerived& operator=(const TestItemDerived& other) {
    if (&other != this) {
      ++derived_instance_counter;
    }
    return *this;
  }

  TestItemDerived& operator=(TestItemDerived&& other) noexcept {
    if (&other != this) {
      ++derived_instance_counter;
    }
    return *this;
  }

  ~TestItemDerived() override { --derived_instance_counter; }

  inline static int32_t derived_instance_counter = 0;
};

struct FreeTestItem {
  void AddRef() const { ++instance_counter; }

  bool ReleaseRef() const { return --instance_counter < 1; }

  mutable int32_t instance_counter = 0;
};

class IntrusivePtrTest : public ::testing::Test {
 protected:
  void SetUp() override {
    TestItem::instance_counter = 0;
    TestItemDerived::derived_instance_counter = 0;
  }
};

TEST_F(IntrusivePtrTest, DeletingLastPtrDeletesTheObject) {
  {
    IntrusivePtr<TestItem> ptr(new TestItem());
    EXPECT_EQ(TestItem::instance_counter, 1);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
}

TEST_F(IntrusivePtrTest, AssigningToNullptrDeletesTheObject) {
  IntrusivePtr<TestItem> ptr(new TestItem());
  EXPECT_EQ(TestItem::instance_counter, 1);
  ptr = nullptr;
  EXPECT_EQ(TestItem::instance_counter, 0);
}

TEST_F(IntrusivePtrTest, AssigningToEmptyPtrDeletesTheObject) {
  IntrusivePtr<TestItem> ptr(new TestItem());
  IntrusivePtr<TestItem> empty;
  EXPECT_EQ(TestItem::instance_counter, 1);
  ptr = empty;
  EXPECT_EQ(TestItem::instance_counter, 0);
}

TEST_F(IntrusivePtrTest, SwapWithNullptrKeepsTheObject) {
  IntrusivePtr<TestItem> ptr(new TestItem());
  IntrusivePtr<TestItem> empty;
  EXPECT_EQ(TestItem::instance_counter, 1);

  ptr.swap(empty);
  EXPECT_EQ(TestItem::instance_counter, 1);
  EXPECT_EQ(ptr, nullptr);

  empty = nullptr;
  EXPECT_EQ(TestItem::instance_counter, 0);
}

TEST_F(IntrusivePtrTest, CopyingPtrDoesntCreateNewObjects) {
  {
    IntrusivePtr<TestItem> ptr(new TestItem());
    EXPECT_EQ(TestItem::instance_counter, 1);

    {
      IntrusivePtr<TestItem> ptr_2(ptr);
      EXPECT_EQ(TestItem::instance_counter, 1);
    }

    // We still have a ptr here.
    EXPECT_EQ(TestItem::instance_counter, 1);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
}

TEST_F(IntrusivePtrTest, MovingPtrDoesntCreateNewObjects) {
  {
    IntrusivePtr<TestItem> ptr(new TestItem());
    EXPECT_EQ(TestItem::instance_counter, 1);

    {
      IntrusivePtr<TestItem> ptr_2(std::move(ptr));
      EXPECT_EQ(TestItem::instance_counter, 1);
    }

    // ptr was moved away, object should be deleted.
    EXPECT_EQ(TestItem::instance_counter, 0);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
}

TEST_F(IntrusivePtrTest, CopyAssigningPtrDoesntCreateNewObjects) {
  {
    IntrusivePtr<TestItem> ptr(new TestItem());
    EXPECT_EQ(TestItem::instance_counter, 1);

    {
      auto ptr_2 = ptr;
      EXPECT_EQ(TestItem::instance_counter, 1);
    }

    // We still have a ptr here.
    EXPECT_EQ(TestItem::instance_counter, 1);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
}

TEST_F(IntrusivePtrTest, MoveAssigningPtrDoesntCreateNewObjects) {
  {
    IntrusivePtr<TestItem> ptr(new TestItem());
    EXPECT_EQ(TestItem::instance_counter, 1);

    {
      auto ptr_2 = std::move(ptr);
      EXPECT_EQ(TestItem::instance_counter, 1);
    }

    // ptr was moved away, object should be deleted.
    EXPECT_EQ(TestItem::instance_counter, 0);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
}

TEST_F(IntrusivePtrTest, CopyingPtrToBaseClassPtrDoesntCreateNewObjects) {
  {
    IntrusivePtr<TestItemDerived> ptr(new TestItemDerived());
    EXPECT_EQ(TestItem::instance_counter, 1);
    EXPECT_EQ(TestItemDerived::derived_instance_counter, 1);

    {
      IntrusivePtr<TestItem> ptr_2(ptr);
      EXPECT_EQ(TestItem::instance_counter, 1);
      EXPECT_EQ(TestItemDerived::derived_instance_counter, 1);
    }

    // We still have a ptr here.
    EXPECT_EQ(TestItem::instance_counter, 1);
    EXPECT_EQ(TestItemDerived::derived_instance_counter, 1);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
  EXPECT_EQ(TestItemDerived::derived_instance_counter, 0);
}

TEST_F(IntrusivePtrTest, MovingPtrToBaseClassPtrDoesntCreateNewObjects) {
  {
    IntrusivePtr<TestItemDerived> ptr(new TestItemDerived());
    EXPECT_EQ(TestItem::instance_counter, 1);
    EXPECT_EQ(TestItemDerived::derived_instance_counter, 1);

    {
      IntrusivePtr<TestItem> ptr_2(std::move(ptr));
      EXPECT_EQ(TestItem::instance_counter, 1);
      EXPECT_EQ(TestItemDerived::derived_instance_counter, 1);
    }

    // ptr was moved away, object should be deleted.
    EXPECT_EQ(TestItem::instance_counter, 0);
    EXPECT_EQ(TestItemDerived::derived_instance_counter, 0);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
  EXPECT_EQ(TestItemDerived::derived_instance_counter, 0);
}

TEST_F(IntrusivePtrTest, CopyAssigningPtrToBaseClassPtrDoesntCreateNewObjects) {
  {
    IntrusivePtr<TestItemDerived> ptr(new TestItemDerived());
    EXPECT_EQ(TestItem::instance_counter, 1);
    EXPECT_EQ(TestItemDerived::derived_instance_counter, 1);

    {
      IntrusivePtr<TestItem> ptr_2 = ptr;
      EXPECT_EQ(TestItem::instance_counter, 1);
      EXPECT_EQ(TestItemDerived::derived_instance_counter, 1);
    }

    // We still have a ptr here.
    EXPECT_EQ(TestItem::instance_counter, 1);
    EXPECT_EQ(TestItemDerived::derived_instance_counter, 1);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
  EXPECT_EQ(TestItemDerived::derived_instance_counter, 0);
}

TEST_F(IntrusivePtrTest, MoveAssigningPtrToBaseClassPtrDoesntCreateNewObjects) {
  {
    IntrusivePtr<TestItemDerived> ptr(new TestItemDerived());
    EXPECT_EQ(TestItem::instance_counter, 1);
    EXPECT_EQ(TestItemDerived::derived_instance_counter, 1);

    {
      IntrusivePtr<TestItem> ptr_2 = std::move(ptr);
      EXPECT_EQ(TestItem::instance_counter, 1);
      EXPECT_EQ(TestItemDerived::derived_instance_counter, 1);
    }

    // ptr was moved away, object should be deleted.
    EXPECT_EQ(TestItem::instance_counter, 0);
    EXPECT_EQ(TestItemDerived::derived_instance_counter, 0);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
  EXPECT_EQ(TestItemDerived::derived_instance_counter, 0);
}

TEST_F(IntrusivePtrTest, CopyAssigningPtrDeletesOldObjectIfLast) {
  {
    IntrusivePtr<TestItem> ptr(new TestItem());
    EXPECT_EQ(TestItem::instance_counter, 1);

    {
      IntrusivePtr<TestItem> ptr_2(new TestItem());
      EXPECT_EQ(TestItem::instance_counter, 2);

      ptr_2 = ptr;

      // Old object in ptr_2 should be removed.
      EXPECT_EQ(TestItem::instance_counter, 1);
    }

    // We still have a ptr here.
    EXPECT_EQ(TestItem::instance_counter, 1);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
}

TEST_F(IntrusivePtrTest, MoveAssigningPtrDeletesOldObjectIfLast) {
  {
    IntrusivePtr<TestItem> ptr(new TestItem());
    EXPECT_EQ(TestItem::instance_counter, 1);

    {
      IntrusivePtr<TestItem> ptr_2(new TestItem());
      EXPECT_EQ(TestItem::instance_counter, 2);

      ptr_2 = std::move(ptr);

      // Old object in ptr_2 should be removed.
      EXPECT_EQ(TestItem::instance_counter, 1);
    }

    // ptr was moved away, object should be deleted.
    EXPECT_EQ(TestItem::instance_counter, 0);
  }
  EXPECT_EQ(TestItem::instance_counter, 0);
}

// Comparison tests use operators directly to cover == and != and both
// argument orders.
TEST_F(IntrusivePtrTest, PtrsWithDifferentObjectsAreNotEqual) {
  IntrusivePtr<TestItem> ptr(new TestItem());
  IntrusivePtr<TestItem> ptr_2(new TestItem());

  EXPECT_FALSE(ptr == ptr_2);
  EXPECT_FALSE(ptr_2 == ptr);
  EXPECT_TRUE(ptr != ptr_2);
  EXPECT_TRUE(ptr_2 != ptr);
}

TEST_F(IntrusivePtrTest, PtrsWithSameObjectsAreEqual) {
  IntrusivePtr<TestItem> ptr(new TestItem());
  auto ptr_2 = ptr;

  EXPECT_TRUE(ptr == ptr_2);
  EXPECT_TRUE(ptr_2 == ptr);
  EXPECT_FALSE(ptr != ptr_2);
  EXPECT_FALSE(ptr_2 != ptr);
}

TEST_F(IntrusivePtrTest, FilledPtrIsNotEqualToEmptyPtr) {
  IntrusivePtr<TestItem> ptr(new TestItem());
  IntrusivePtr<TestItem> empty;

  EXPECT_FALSE(ptr == empty);
  EXPECT_FALSE(empty == ptr);
  EXPECT_TRUE(ptr != empty);
  EXPECT_TRUE(empty != ptr);
}

TEST_F(IntrusivePtrTest, FilledPtrIsNotEqualToNullptr) {
  IntrusivePtr<TestItem> ptr(new TestItem());

  EXPECT_FALSE(ptr == nullptr);
  EXPECT_FALSE(nullptr == ptr);
  EXPECT_TRUE(ptr != nullptr);
  EXPECT_TRUE(nullptr != ptr);
}

TEST_F(IntrusivePtrTest, EmptyPtrIsEqualToNullptr) {
  IntrusivePtr<TestItem> empty;

  EXPECT_TRUE(empty == nullptr);
  EXPECT_TRUE(nullptr == empty);
  EXPECT_FALSE(empty != nullptr);
  EXPECT_FALSE(nullptr != empty);
}

TEST_F(IntrusivePtrTest, PtrsWithDifferentObjectsReturnDifferentPointers) {
  IntrusivePtr<TestItem> ptr(new TestItem());
  IntrusivePtr<TestItem> ptr_2(new TestItem());

  EXPECT_NE(ptr.get(), ptr_2.get());
}

TEST_F(IntrusivePtrTest, PtrsWithSameObjectsReturnSamePointer) {
  IntrusivePtr<TestItemDerived> ptr(new TestItemDerived());
  auto ptr_2 = ptr;
  IntrusivePtr<TestItem> ptr_3 = ptr;

  EXPECT_EQ(ptr.get(), ptr_2.get());
  EXPECT_EQ(static_cast<TestItem*>(ptr.get()), ptr_3.get());
}

TEST_F(IntrusivePtrTest, EmptyPtrReturnsNullptr) {
  IntrusivePtr<TestItem> empty;

  EXPECT_EQ(empty.get(), nullptr);
}

TEST_F(IntrusivePtrTest, ConstifyWorks) {
  IntrusivePtr<TestItem> ptr(new TestItem());
  IntrusivePtr<const TestItem> ptr_2 = ptr;

  EXPECT_EQ(TestItem::instance_counter, 1);
  EXPECT_EQ(ptr.get(), ptr_2.get());
}

TEST_F(IntrusivePtrTest, NonRefCountedObjectWorks) {
  // Compilation test only.
  IntrusivePtr<FreeTestItem> empty;
  IntrusivePtr<FreeTestItem> free(new FreeTestItem);
  IntrusivePtr<FreeTestItem> free_2(free);
  IntrusivePtr<FreeTestItem> free_3(std::move(free));
}

TEST_F(IntrusivePtrTest, MakeRefCounted) {
  auto ptr_1 = MakeRefCounted<TestItem>();
  EXPECT_EQ(TestItem::instance_counter, 1);
  EXPECT_EQ(ptr_1->first, 0);
  EXPECT_EQ(ptr_1->second, 1);

  auto ptr_2 = MakeRefCounted<TestItem>(int32_t(42));
  EXPECT_EQ(TestItem::instance_counter, 2);
  EXPECT_EQ(ptr_2->first, 42);
  EXPECT_EQ(ptr_2->second, 1);

  auto ptr_3 = MakeRefCounted<TestItem>(int64_t(2));
  EXPECT_EQ(TestItem::instance_counter, 3);
  EXPECT_EQ(ptr_3->first, 0);
  EXPECT_EQ(ptr_3->second, 2);

  auto ptr_4 = MakeRefCounted<TestItem>(42, 5);
  EXPECT_EQ(TestItem::instance_counter, 4);
  EXPECT_EQ(ptr_4->first, 42);
  EXPECT_EQ(ptr_4->second, 5);
}

TEST_F(IntrusivePtrTest, UseCount) {
  IntrusivePtr<TestItem> ptr(new TestItem());
  EXPECT_EQ(ptr.use_count(), 1);
  {
    IntrusivePtr<TestItem> ptr_copy = ptr;
    EXPECT_EQ(ptr.use_count(), 2);
  }
  EXPECT_EQ(ptr.use_count(), 1);
}

TEST_F(IntrusivePtrTest, UseCountForNullPtr) {
  IntrusivePtr<TestItem> ptr;
  EXPECT_EQ(ptr.use_count(), 0);

  ptr = IntrusivePtr(new TestItem);
  EXPECT_EQ(ptr.use_count(), 1);

  ptr = nullptr;
  EXPECT_EQ(ptr.use_count(), 0);
}

}  // namespace
}  // namespace pw
