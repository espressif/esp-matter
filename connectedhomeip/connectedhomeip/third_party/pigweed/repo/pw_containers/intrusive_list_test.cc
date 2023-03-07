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

#include "pw_containers/intrusive_list.h"

#include <array>
#include <cstddef>
#include <cstdint>

#include "gtest/gtest.h"
#include "pw_compilation_testing/negative_compilation.h"
#include "pw_preprocessor/util.h"

namespace pw {
namespace {

class TestItem : public IntrusiveList<TestItem>::Item {
 public:
  TestItem() : number_(0) {}
  TestItem(int number) : number_(number) {}

  int GetNumber() const { return number_; }
  void SetNumber(int num) { number_ = num; }

  // Add equality comparison to ensure comparisons are done by identity rather
  // than equality for the remove function.
  bool operator==(const TestItem& other) const {
    return number_ == other.number_;
  }

 private:
  int number_;
};

TEST(IntrusiveList, Construct_InitializerList_Empty) {
  IntrusiveList<TestItem> list({});
  EXPECT_TRUE(list.empty());
}

TEST(IntrusiveList, Construct_InitializerList_One) {
  TestItem one(1);
  IntrusiveList<TestItem> list({&one});

  EXPECT_EQ(&one, &list.front());
}

TEST(IntrusiveList, Construct_InitializerList_Multiple) {
  TestItem one(1);
  TestItem two(2);
  TestItem thr(3);

  IntrusiveList<TestItem> list({&one, &two, &thr});
  auto it = list.begin();
  EXPECT_EQ(&one, &(*it++));
  EXPECT_EQ(&two, &(*it++));
  EXPECT_EQ(&thr, &(*it++));
  EXPECT_EQ(list.end(), it);
}

TEST(IntrusiveList, Construct_ObjectIterator_Empty) {
  std::array<TestItem, 0> array;
  IntrusiveList<TestItem> list(array.begin(), array.end());

  EXPECT_TRUE(list.empty());
}

TEST(IntrusiveList, Construct_ObjectIterator_One) {
  std::array<TestItem, 1> array{{{1}}};
  IntrusiveList<TestItem> list(array.begin(), array.end());

  EXPECT_EQ(&array.front(), &list.front());
}

TEST(IntrusiveList, Construct_ObjectIterator_Multiple) {
  std::array<TestItem, 3> array{{{1}, {2}, {3}}};

  IntrusiveList<TestItem> list(array.begin(), array.end());
  auto it = list.begin();
  EXPECT_EQ(&array[0], &(*it++));
  EXPECT_EQ(&array[1], &(*it++));
  EXPECT_EQ(&array[2], &(*it++));
  EXPECT_EQ(list.end(), it);
}

TEST(IntrusiveList, Construct_PointerIterator_Empty) {
  std::array<TestItem*, 0> array;
  IntrusiveList<TestItem> list(array.begin(), array.end());

  EXPECT_TRUE(list.empty());
}

TEST(IntrusiveList, Construct_PointerIterator_One) {
  std::array<TestItem, 1> array{{{1}}};
  std::array<TestItem*, 1> ptrs{{&array[0]}};

  IntrusiveList<TestItem> list(ptrs.begin(), ptrs.end());

  EXPECT_EQ(ptrs[0], &list.front());
}

TEST(IntrusiveList, Construct_PointerIterator_Multiple) {
  std::array<TestItem, 3> array{{{1}, {2}, {3}}};
  std::array<TestItem*, 3> ptrs{{&array[0], &array[1], &array[2]}};

  IntrusiveList<TestItem> list(ptrs.begin(), ptrs.end());
  auto it = list.begin();
  EXPECT_EQ(ptrs[0], &(*it++));
  EXPECT_EQ(ptrs[1], &(*it++));
  EXPECT_EQ(ptrs[2], &(*it++));
  EXPECT_EQ(list.end(), it);
}

TEST(IntrusiveList, Assign_ReplacesPriorContents) {
  std::array<TestItem, 3> array{{{0}, {100}, {200}}};
  IntrusiveList<TestItem> list(array.begin(), array.end());

  list.assign(array.begin() + 1, array.begin() + 2);

  auto it = list.begin();
  EXPECT_EQ(&array[1], &(*it++));
  EXPECT_EQ(list.end(), it);
}

TEST(IntrusiveList, Assign_EmptyRange) {
  std::array<TestItem, 3> array{{{0}, {100}, {200}}};
  IntrusiveList<TestItem> list(array.begin(), array.end());

  list.assign(array.begin() + 1, array.begin() + 1);

  EXPECT_TRUE(list.empty());
}

TEST(IntrusiveList, PushOne) {
  constexpr int kMagicValue = 31;
  TestItem item1(kMagicValue);
  IntrusiveList<TestItem> list;
  list.push_back(item1);
  EXPECT_FALSE(list.empty());
  EXPECT_EQ(list.front().GetNumber(), kMagicValue);
}

TEST(IntrusiveList, PushThree) {
  TestItem item1(1);
  TestItem item2(2);
  TestItem item3(3);

  IntrusiveList<TestItem> list;
  list.push_back(item1);
  list.push_back(item2);
  list.push_back(item3);

  int loop_count = 0;
  for (auto& test_item : list) {
    loop_count++;
    EXPECT_EQ(loop_count, test_item.GetNumber());
  }
  EXPECT_EQ(loop_count, 3);
}

TEST(IntrusiveList, IsEmpty) {
  TestItem item1(1);

  IntrusiveList<TestItem> list;
  EXPECT_TRUE(list.empty());

  list.push_back(item1);
  EXPECT_FALSE(list.empty());
}

TEST(IntrusiveList, InsertAfter) {
  // Create a test item to insert midway through the list.
  constexpr int kMagicValue = 42;
  TestItem inserted_item(kMagicValue);

  // Create initial values to fill in the start/end.
  TestItem item_array[20];

  IntrusiveList<TestItem> list;
  // Fill the list with TestItem objects that have a value of zero.
  for (size_t i = 0; i < PW_ARRAY_SIZE(item_array); ++i) {
    item_array[i].SetNumber(0);
    list.push_back(item_array[i]);
  }

  // Move an iterator to the middle of the list, and then insert the magic item.
  auto it = list.begin();
  size_t expected_index = 1;  // Expected index is iterator index + 1.
  for (size_t i = 0; i < PW_ARRAY_SIZE(item_array) / 2; ++i) {
    it++;
    expected_index++;
  }
  it = list.insert_after(it, inserted_item);

  // Ensure the returned iterator from insert_after is the newly inserted
  // element.
  EXPECT_EQ(it->GetNumber(), kMagicValue);

  // Ensure the value is in the expected location (index of the iterator + 1).
  size_t i = 0;
  for (TestItem& item : list) {
    if (item.GetNumber() == kMagicValue) {
      EXPECT_EQ(i, expected_index);
    } else {
      EXPECT_EQ(item.GetNumber(), 0);
    }
    i++;
  }

  // Ensure the list didn't break and change sizes.
  EXPECT_EQ(i, PW_ARRAY_SIZE(item_array) + 1);
}

TEST(IntrusiveList, InsertAfterBeforeBegin) {
  // Create a test item to insert at the beginning of the list.
  constexpr int kMagicValue = 42;
  TestItem inserted_item(kMagicValue);

  // Create initial values to fill in the start/end.
  TestItem item_array[20];

  IntrusiveList<TestItem> list;
  // Fill the list with TestItem objects that have a value of zero.
  for (size_t i = 0; i < PW_ARRAY_SIZE(item_array); ++i) {
    item_array[i].SetNumber(0);
    list.push_back(item_array[i]);
  }

  auto it = list.insert_after(list.before_begin(), inserted_item);

  // Ensure the returned iterator from insert_after is the newly inserted
  // element.
  EXPECT_EQ(it->GetNumber(), kMagicValue);

  // Ensure the value is at the beginning of the list.
  size_t i = 0;
  for (TestItem& item : list) {
    if (item.GetNumber() == kMagicValue) {
      EXPECT_EQ(i, static_cast<size_t>(0));
    } else {
      EXPECT_EQ(item.GetNumber(), 0);
    }
    i++;
  }
}

TEST(IntrusiveList, PushFront) {
  constexpr int kMagicValue = 42;
  TestItem pushed_item(kMagicValue);

  TestItem item_array[20];
  IntrusiveList<TestItem> list;
  // Fill the list with TestItem objects that have a value of zero.
  for (size_t i = 0; i < PW_ARRAY_SIZE(item_array); ++i) {
    item_array[i].SetNumber(0);
    list.push_back(item_array[i]);
  }

  // Create a test item to push to the front of the list.
  list.push_front(pushed_item);
  EXPECT_EQ(list.front().GetNumber(), kMagicValue);
}

TEST(IntrusiveList, Clear_Empty) {
  IntrusiveList<TestItem> list;
  EXPECT_TRUE(list.empty());
  list.clear();
  EXPECT_TRUE(list.empty());
}

TEST(IntrusiveList, Clear_OneItem) {
  TestItem item(42);
  IntrusiveList<TestItem> list;
  list.push_back(item);
  EXPECT_FALSE(list.empty());
  list.clear();
  EXPECT_TRUE(list.empty());
}

TEST(IntrusiveList, Clear_TwoItems) {
  TestItem item1(42);
  TestItem item2(42);
  IntrusiveList<TestItem> list;
  list.push_back(item1);
  list.push_back(item2);
  EXPECT_FALSE(list.empty());
  list.clear();
  EXPECT_TRUE(list.empty());
}

TEST(IntrusiveList, Clear_ReinsertClearedItems) {
  std::array<TestItem, 20> item_array;
  IntrusiveList<TestItem> list;
  EXPECT_TRUE(list.empty());
  list.clear();
  EXPECT_TRUE(list.empty());

  // Fill the list with TestItem objects.
  for (size_t i = 0; i < item_array.size(); ++i) {
    item_array[i].SetNumber(0);
    list.push_back(item_array[i]);
  }

  // Remove everything.
  list.clear();
  EXPECT_TRUE(list.empty());

  // Ensure all the removed elements can still be added back to a list.
  for (size_t i = 0; i < item_array.size(); ++i) {
    item_array[i].SetNumber(0);
    list.push_back(item_array[i]);
  }
}

TEST(IntrusiveList, PopFront) {
  constexpr int kValue1 = 32;
  constexpr int kValue2 = 4083;

  TestItem item1(kValue1);
  TestItem item2(kValue2);

  IntrusiveList<TestItem> list;
  EXPECT_TRUE(list.empty());

  list.push_front(item2);
  list.push_front(item1);
  list.pop_front();
  EXPECT_EQ(list.front().GetNumber(), kValue2);
  EXPECT_FALSE(list.empty());
  list.pop_front();
  EXPECT_TRUE(list.empty());
}

TEST(IntrusiveList, PopFrontAndReinsert) {
  constexpr int kValue1 = 32;
  constexpr int kValue2 = 4083;

  TestItem item1(kValue1);
  TestItem item2(kValue2);

  IntrusiveList<TestItem> list;
  EXPECT_TRUE(list.empty());

  list.push_front(item2);
  list.push_front(item1);
  list.pop_front();
  list.push_front(item1);
  EXPECT_EQ(list.front().GetNumber(), kValue1);
}

TEST(IntrusiveList, ListFront) {
  TestItem item1(1);
  TestItem item2(0);
  TestItem item3(0xffff);

  IntrusiveList<TestItem> list;
  list.push_back(item1);
  list.push_back(item2);
  list.push_back(item3);

  EXPECT_EQ(&item1, &list.front());
  EXPECT_EQ(&item1, &(*list.begin()));
}

TEST(IntrusiveList, IteratorIncrement) {
  TestItem item_array[20];
  IntrusiveList<TestItem> list;
  for (size_t i = 0; i < PW_ARRAY_SIZE(item_array); ++i) {
    item_array[i].SetNumber(i);
    list.push_back(item_array[i]);
  }

  auto it = list.begin();
  int i = 0;
  while (it != list.end()) {
    if (i == 0) {
      // Test pre-incrementing on the first element.
      EXPECT_EQ((++it)->GetNumber(), item_array[++i].GetNumber());
    } else {
      EXPECT_EQ((it++)->GetNumber(), item_array[i++].GetNumber());
    }
  }
}

TEST(IntrusiveList, ConstIteratorRead) {
  // For this test, items are checked to be non-zero.
  TestItem item1(1);
  TestItem item2(99);
  IntrusiveList<TestItem> list;

  const IntrusiveList<TestItem>* const_list = &list;

  list.push_back(item1);
  list.push_back(item2);

  auto it = const_list->begin();
  while (it != const_list->end()) {
    EXPECT_NE(it->GetNumber(), 0);
    it++;
  }
}

TEST(IntrusiveList, CompareConstAndNonConstIterator) {
  IntrusiveList<TestItem> list;
  EXPECT_EQ(list.end(), list.cend());
}

#if PW_NC_TEST(IncompatibleIteratorTypes)
PW_NC_EXPECT("comparison (of|between) distinct pointer types");

struct OtherItem : public IntrusiveList<OtherItem>::Item {};

TEST(IntrusiveList, CompareConstAndNonConstIterator_CompilationFails) {
  IntrusiveList<TestItem> list;
  IntrusiveList<OtherItem> list2;
  static_cast<void>(list.end() == list2.end());
}

#endif  // PW_NC_TEST

#if PW_NC_TEST(CannotModifyThroughConstIterator)
PW_NC_EXPECT("function is not marked const|discards qualifiers");

TEST(IntrusiveList, ConstIteratorModify) {
  TestItem item1(1);
  TestItem item2(99);
  IntrusiveList<TestItem> list;

  const IntrusiveList<TestItem>* const_list = &list;

  list.push_back(item1);
  list.push_back(item2);

  auto it = const_list->begin();
  while (it != const_list->end()) {
    it->SetNumber(0);
    it++;
  }
}
#endif  // PW_NC_TEST

// TODO(b/235289499): These tests should trigger a CHECK failure. This requires
// using a testing version of pw_assert.
#define TESTING_CHECK_FAILURES_IS_SUPPORTED 0
#if TESTING_CHECK_FAILURES_IS_SUPPORTED
TEST(IntrusiveList, Construct_DuplicateItems) {
  TestItem item(1);
  IntrusiveList<TestItem> list({&item, &item});
}

TEST(IntrusiveList, InsertAfter_SameItem) {
  TestItem item(1);
  IntrusiveList<TestItem> list({&item});

  list.insert_after(list.begin(), item);
}

TEST(IntrusiveList, InsertAfter_SameItemAfterEnd) {
  TestItem item(1);
  IntrusiveList<TestItem> list({&item});

  list.insert_after(list.end(), item);
}

TEST(IntrusiveList, PushBack_SameItem) {
  TestItem item(1);
  IntrusiveList<TestItem> list({&item});

  list.push_back(item);
}

TEST(IntrusiveList, PushFront_SameItem) {
  TestItem item(1);
  IntrusiveList<TestItem> list({&item});

  list.push_front(item);
}
#endif  // TESTING_CHECK_FAILURES_IS_SUPPORTED

TEST(IntrusiveList, EraseAfter_FirstItem) {
  std::array<TestItem, 3> items{{{0}, {1}, {2}}};
  IntrusiveList<TestItem> list(items.begin(), items.end());

  auto it = list.erase_after(list.before_begin());
  EXPECT_EQ(list.begin(), it);
  EXPECT_EQ(&items[1], &list.front());
}

TEST(IntrusiveList, EraseAfter_LastItem) {
  std::array<TestItem, 3> items{{{0}, {1}, {2}}};
  IntrusiveList<TestItem> list(items.begin(), items.end());

  auto it = list.begin();
  ++it;

  it = list.erase_after(it);
  EXPECT_EQ(list.end(), it);

  it = list.begin();
  ++it;

  EXPECT_EQ(&items[1], &(*it));
}

TEST(IntrusiveList, EraseAfter_AllItems) {
  std::array<TestItem, 3> items{{{0}, {1}, {2}}};
  IntrusiveList<TestItem> list(items.begin(), items.end());

  list.erase_after(list.begin());
  list.erase_after(list.begin());
  auto it = list.erase_after(list.before_begin());

  EXPECT_EQ(list.end(), it);
  EXPECT_TRUE(list.empty());
}

TEST(IntrusiveList, Remove_EmptyList) {
  std::array<TestItem, 1> items{{{3}}};
  IntrusiveList<TestItem> list(items.begin(), items.begin());  // Add nothing!

  EXPECT_TRUE(list.empty());
  EXPECT_FALSE(list.remove(items[0]));
}

TEST(IntrusiveList, Remove_SingleItem_NotPresent) {
  std::array<TestItem, 1> items{{{1}}};
  IntrusiveList<TestItem> list(items.begin(), items.end());

  EXPECT_FALSE(list.remove(TestItem(1)));
  EXPECT_EQ(&items.front(), &list.front());
}

TEST(IntrusiveList, Remove_SingleItem_Removed) {
  std::array<TestItem, 1> items{{{1}}};
  IntrusiveList<TestItem> list(items.begin(), items.end());

  EXPECT_TRUE(list.remove(items[0]));
  EXPECT_TRUE(list.empty());
}

TEST(IntrusiveList, Remove_MultipleItems_NotPresent) {
  std::array<TestItem, 5> items{{{1}, {1}, {2}, {3}, {4}}};
  IntrusiveList<TestItem> list(items.begin(), items.end());

  EXPECT_FALSE(list.remove(TestItem(1)));
}

TEST(IntrusiveList, Remove_MultipleItems_RemoveAndPushBack) {
  std::array<TestItem, 5> items{{{1}, {1}, {2}, {3}, {4}}};
  IntrusiveList<TestItem> list(items.begin(), items.end());

  EXPECT_TRUE(list.remove(items[0]));
  EXPECT_TRUE(list.remove(items[3]));
  list.push_back(items[0]);  // Make sure can add the item after removing it.

  auto it = list.begin();
  EXPECT_EQ(&items[1], &(*it++));
  EXPECT_EQ(&items[2], &(*it++));
  EXPECT_EQ(&items[4], &(*it++));
  EXPECT_EQ(&items[0], &(*it++));
  EXPECT_EQ(list.end(), it);
}

TEST(IntrusiveList, ItemsRemoveThemselvesFromListsWhenDestructed) {
  // Create a list with some items it.
  TestItem a, b, c, d;
  IntrusiveList<TestItem> list;
  list.push_back(a);
  list.push_back(b);
  list.push_back(c);
  list.push_back(d);

  // Insert items that will be destructed before the list.
  {
    TestItem x, y, z, w;
    list.push_back(x);
    list.push_back(z);
    list.push_front(y);
    list.push_front(w);

    auto it = list.begin();
    EXPECT_EQ(&w, &(*it++));
    EXPECT_EQ(&y, &(*it++));
    EXPECT_EQ(&a, &(*it++));
    EXPECT_EQ(&b, &(*it++));
    EXPECT_EQ(&c, &(*it++));
    EXPECT_EQ(&d, &(*it++));
    EXPECT_EQ(&x, &(*it++));
    EXPECT_EQ(&z, &(*it++));
    EXPECT_EQ(list.end(), it);

    // Here, x, y, z, w are removed from the list for the destructor.
  }

  // Ensure we get back our original list.
  auto it = list.begin();
  EXPECT_EQ(&a, &(*it++));
  EXPECT_EQ(&b, &(*it++));
  EXPECT_EQ(&c, &(*it++));
  EXPECT_EQ(&d, &(*it++));
  EXPECT_EQ(list.end(), it);
}

TEST(IntrusiveList, SizeBasic) {
  IntrusiveList<TestItem> list;
  EXPECT_EQ(list.size(), 0u);

  TestItem one(55);
  list.push_front(one);
  EXPECT_EQ(list.size(), static_cast<size_t>(1));

  TestItem two(66);
  list.push_back(two);
  EXPECT_EQ(list.size(), static_cast<size_t>(2));

  TestItem thr(77);
  list.push_back(thr);
  EXPECT_EQ(list.size(), static_cast<size_t>(3));
}

TEST(IntrusiveList, SizeScoped) {
  IntrusiveList<TestItem> list;
  EXPECT_EQ(list.size(), 0u);

  // Add elements in new scopes; verify size on the way in and on the way out.
  {
    TestItem one(55);
    list.push_back(one);
    EXPECT_EQ(list.size(), static_cast<size_t>(1));

    {
      TestItem two(66);
      list.push_back(two);
      EXPECT_EQ(list.size(), static_cast<size_t>(2));
      {
        TestItem thr(77);
        list.push_back(thr);
        EXPECT_EQ(list.size(), static_cast<size_t>(3));
      }
      EXPECT_EQ(list.size(), static_cast<size_t>(2));
    }
    EXPECT_EQ(list.size(), static_cast<size_t>(1));
  }
  EXPECT_EQ(list.size(), static_cast<size_t>(0));
}

// Test that a list of items derived from a different Item class can be created.
class DerivedTestItem : public TestItem {};

TEST(IntrusiveList, AddItemsOfDerivedClassToList) {
  IntrusiveList<TestItem> list;

  DerivedTestItem item1;
  list.push_front(item1);

  TestItem item2;
  list.push_front(item2);

  EXPECT_EQ(2u, list.size());
}

TEST(IntrusiveList, ListOfDerivedClassItems) {
  IntrusiveList<DerivedTestItem> derived_from_compatible_item_type;

  DerivedTestItem item1;
  derived_from_compatible_item_type.push_front(item1);

  EXPECT_EQ(1u, derived_from_compatible_item_type.size());

#if PW_NC_TEST(CannotAddBaseClassToDerivedClassList)
  PW_NC_EXPECT_CLANG("cannot bind to a value of unrelated type");
  PW_NC_EXPECT_GCC("cannot convert");

  TestItem item2;
  derived_from_compatible_item_type.push_front(item2);
#endif
}

#if PW_NC_TEST(IncompatibileItemType)
PW_NC_EXPECT("IntrusiveList items must be derived from IntrusiveList<T>::Item");

struct Foo {};

class BadItem : public IntrusiveList<Foo>::Item {};

[[maybe_unused]] IntrusiveList<BadItem> derived_from_incompatible_item_type;

#elif PW_NC_TEST(DoesNotInheritFromItem)
PW_NC_EXPECT("IntrusiveList items must be derived from IntrusiveList<T>::Item");

struct NotAnItem {};

[[maybe_unused]] IntrusiveList<NotAnItem> list;

#endif  // PW_NC_TEST

}  // namespace
}  // namespace pw
