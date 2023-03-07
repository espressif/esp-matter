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
#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace pw {

template <typename>
class IntrusiveList;

namespace intrusive_list_impl {

template <typename T, typename I>
class Iterator {
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = std::remove_cv_t<T>;
  using pointer = T*;
  using reference = T&;
  using iterator_category = std::forward_iterator_tag;

  constexpr explicit Iterator() : item_(nullptr) {}

  constexpr Iterator& operator++() {
    item_ = static_cast<I*>(item_->next_);
    return *this;
  }

  constexpr Iterator operator++(int) {
    Iterator previous_value(item_);
    operator++();
    return previous_value;
  }

  constexpr const T& operator*() const { return *static_cast<T*>(item_); }
  constexpr T& operator*() { return *static_cast<T*>(item_); }

  constexpr const T* operator->() const { return static_cast<T*>(item_); }
  constexpr T* operator->() { return static_cast<T*>(item_); }

  template <typename U, typename J>
  constexpr bool operator==(const Iterator<U, J>& rhs) const {
    return item_ == rhs.item_;
  }

  template <typename U, typename J>
  constexpr bool operator!=(const Iterator<U, J>& rhs) const {
    return item_ != rhs.item_;
  }

 private:
  template <typename, typename>
  friend class Iterator;

  template <typename>
  friend class ::pw::IntrusiveList;

  // Only allow IntrusiveList to create iterators that point to something.
  constexpr explicit Iterator(I* item) : item_{item} {}

  I* item_;
};

class List {
 public:
  class Item {
   protected:
    constexpr Item() : Item(this) {}

    ~Item() { unlist(); }

   private:
    friend class List;

    template <typename T, typename I>
    friend class Iterator;

    constexpr Item(Item* next) : next_(next) {}

    bool unlisted() const { return this == next_; }

    // Unlink this from the list it is apart of, if any. Specifying prev saves
    // calling previous(), which requires looping around the cycle.
    void unlist(Item* prev = nullptr);

    Item* previous();  // Note: O(n) since it loops around the cycle.

    // The next pointer. Unlisted items must be self-cycles (next_ == this).
    Item* next_;
  };

  constexpr List() : head_(end()) {}

  template <typename Iterator>
  List(Iterator first, Iterator last) : List() {
    AssignFromIterator(first, last);
  }

  // Intrusive lists cannot be copied, since each Item can only be in one list.
  List(const List&) = delete;
  List& operator=(const List&) = delete;

  template <typename Iterator>
  void assign(Iterator first, Iterator last) {
    clear();
    AssignFromIterator(first, last);
  }

  bool empty() const noexcept { return begin() == end(); }

  static void insert_after(Item* pos, Item& item);

  static void erase_after(Item* pos);

  void clear();

  bool remove(const Item& item_to_remove);

  constexpr Item* before_begin() noexcept { return &head_; }
  constexpr const Item* before_begin() const noexcept { return &head_; }

  constexpr Item* begin() noexcept { return head_.next_; }
  constexpr const Item* begin() const noexcept { return head_.next_; }

  Item* before_end() noexcept;

  constexpr Item* end() noexcept { return &head_; }
  constexpr const Item* end() const noexcept { return &head_; }

  size_t size() const;

 private:
  template <typename Iterator>
  void AssignFromIterator(Iterator first, Iterator last);

  // Use an Item for the head pointer. This gives simpler logic for inserting
  // elements compared to using an Item*. It also makes it possible to use
  // &head_ for end(), rather than nullptr. This makes end() unique for each
  // List and ensures that items already in a list cannot be added to another.
  Item head_;
};

template <typename Iterator>
void List::AssignFromIterator(Iterator first, Iterator last) {
  Item* current = &head_;

  for (Iterator it = first; it != last; ++it) {
    if constexpr (std::is_pointer<std::remove_reference_t<decltype(*it)>>()) {
      insert_after(current, **it);
      current = *it;
    } else {
      insert_after(current, *it);
      current = &(*it);
    }
  }
}

// Gets the element type from an Item. This is used to check that an
// IntrusiveList element class inherits from Item, either directly or through
// another class.
template <typename T, bool kIsItem = std::is_base_of<List::Item, T>()>
struct GetListElementTypeFromItem {
  using Type = void;
};

template <typename T>
struct GetListElementTypeFromItem<T, true> {
  using Type = typename T::PwIntrusiveListElementType;
};

template <typename T>
using ElementTypeFromItem = typename GetListElementTypeFromItem<T>::Type;

}  // namespace intrusive_list_impl
}  // namespace pw
