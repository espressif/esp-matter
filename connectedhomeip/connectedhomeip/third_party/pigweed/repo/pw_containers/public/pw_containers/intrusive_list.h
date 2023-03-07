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
#pragma once

#include <cstddef>
#include <initializer_list>
#include <type_traits>

#include "pw_containers/internal/intrusive_list_impl.h"

namespace pw {

// IntrusiveList provides singly-linked list functionality for derived
// class items. IntrusiveList<T> is a handle to access and manipulate the
// list, and IntrusiveList<T>::Item is a base class items must inherit
// from. An instantiation of the derived class becomes a list item when inserted
// into a IntrusiveList.
//
// This has two important ramifications:
//
// - An instantiated IntrusiveList::Item must remain in scope for the
//   lifetime of the IntrusiveList it has been added to.
// - A linked list item CANNOT be included in two lists, as it is part of a
//   preexisting list and adding it to another implicitly breaks correctness of
//   the first list.
//
// Usage:
//
//   class TestItem
//      : public IntrusiveList<TestItem>::Item {}
//
//   IntrusiveList<TestItem> test_items;
//
//   auto item = TestItem();
//   test_items.push_back(item);
//
//   for (auto& test_item : test_items) {
//     // Do a thing.
//   }
//
template <typename T>
class IntrusiveList {
 public:
  class Item : public intrusive_list_impl::List::Item {
   public:
    Item(const Item&) = delete;
    Item& operator=(const Item&) = delete;

   protected:
    constexpr Item() = default;

   private:
    // GetListElementTypeFromItem is used to find the element type from an item.
    // It is used to ensure list items inherit from the correct Item type.
    template <typename, bool>
    friend struct intrusive_list_impl::GetListElementTypeFromItem;

    using PwIntrusiveListElementType = T;
  };

  using element_type = T;
  using value_type = std::remove_cv_t<T>;
  using pointer = T*;
  using reference = T&;
  using iterator = intrusive_list_impl::Iterator<T, Item>;
  using const_iterator =
      intrusive_list_impl::Iterator<std::add_const_t<T>, const Item>;

  constexpr IntrusiveList() { CheckItemType(); }

  // Constructs an IntrusiveList from an iterator over Items. The iterator may
  // dereference as either Item& (e.g. from std::array<Item>) or Item* (e.g.
  // from std::initializer_list<Item*>).
  template <typename Iterator>
  IntrusiveList(Iterator first, Iterator last) : list_(first, last) {
    CheckItemType();
  }

  // Constructs an IntrusiveList from a std::initializer_list of pointers to
  // items.
  IntrusiveList(std::initializer_list<Item*> items)
      : IntrusiveList(items.begin(), items.end()) {}

  template <typename Iterator>
  void assign(Iterator first, Iterator last) {
    list_.assign(first, last);
  }

  void assign(std::initializer_list<Item*> items) {
    list_.assign(items.begin(), items.end());
  }

  [[nodiscard]] bool empty() const noexcept { return list_.empty(); }

  void push_front(T& item) { list_.insert_after(list_.before_begin(), item); }

  void push_back(T& item) { list_.insert_after(list_.before_end(), item); }

  iterator insert_after(iterator pos, T& item) {
    list_.insert_after(pos.item_, item);
    return iterator(&item);
  }

  // Removes the first item in the list. The list must not be empty.
  void pop_front() { list_.erase_after(list_.before_begin()); }

  // Removes the item following pos from the list. The item is not destructed.
  iterator erase_after(iterator pos) {
    list_.erase_after(pos.item_);
    return ++pos;
  }

  // Removes all items from the list. The items themselves are not destructed.
  void clear() { list_.clear(); }

  // Removes this specific item from the list, if it is present. Finds the item
  // by identity (address comparison) rather than value equality. Returns true
  // if the item was removed; false if it was not present.
  bool remove(const T& item) { return list_.remove(item); }

  // Reference to the first element in the list. Undefined behavior if empty().
  T& front() { return *static_cast<T*>(list_.begin()); }

  // Reference to the last element in the list. Undefined behavior if empty().
  T& back() { return *static_cast<T*>(list_.before_end()); }

  // As in std::forward_list, returns the iterator before the begin() iterator.
  iterator before_begin() noexcept {
    return iterator(static_cast<Item*>(list_.before_begin()));
  }
  const_iterator before_begin() const noexcept {
    return const_iterator(static_cast<const Item*>(list_.before_begin()));
  }
  const_iterator cbefore_begin() const noexcept { return before_begin(); }

  iterator begin() noexcept {
    return iterator(static_cast<Item*>(list_.begin()));
  }
  const_iterator begin() const noexcept {
    return const_iterator(static_cast<const Item*>(list_.begin()));
  }
  const_iterator cbegin() const noexcept { return begin(); }

  iterator end() noexcept { return iterator(static_cast<Item*>(list_.end())); }
  const_iterator end() const noexcept {
    return const_iterator(static_cast<const Item*>(list_.end()));
  }
  const_iterator cend() const noexcept { return end(); }

  // Operation is O(size).
  size_t size() const { return list_.size(); }

 private:
  // Check that T is an Item in a function, since the class T will not be fully
  // defined when the IntrusiveList<T> class is instantiated.
  static constexpr void CheckItemType() {
    static_assert(
        std::is_base_of<intrusive_list_impl::ElementTypeFromItem<T>, T>(),
        "IntrusiveList items must be derived from IntrusiveList<T>::Item, "
        "where T is the item or one of its bases.");
  }

  intrusive_list_impl::List list_;
};

}  // namespace pw
