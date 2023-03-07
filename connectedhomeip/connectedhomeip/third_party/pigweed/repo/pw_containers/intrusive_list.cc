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

#include "pw_assert/check.h"

namespace pw::intrusive_list_impl {

void List::Item::unlist(Item* prev) {
  if (prev == nullptr) {
    prev = previous();
  }
  // Skip over this.
  prev->next_ = next_;

  // Retain the invariant that unlisted items are self-cycles.
  next_ = this;
}

List::Item* List::Item::previous() {
  // Follow the cycle around to find the previous element; O(N).
  Item* prev = next_;
  while (prev->next_ != this) {
    prev = prev->next_;
  }
  return prev;
}

void List::insert_after(Item* pos, Item& item) {
  PW_CHECK(
      item.unlisted(),
      "Cannot add an item to a pw::IntrusiveList that is already in a list");
  item.next_ = pos->next_;
  pos->next_ = &item;
}

void List::erase_after(Item* pos) { pos->next_->unlist(pos); }

List::Item* List::before_end() noexcept { return before_begin()->previous(); }

void List::clear() {
  while (!empty()) {
    erase_after(before_begin());
  }
}

bool List::remove(const Item& item_to_remove) {
  for (Item* pos = before_begin(); pos->next_ != end(); pos = pos->next_) {
    if (pos->next_ == &item_to_remove) {
      erase_after(pos);
      return true;
    }
  }
  return false;
}

size_t List::size() const {
  size_t total = 0;
  Item* item = head_.next_;
  while (item != &head_) {
    item = item->next_;
    total++;
  }
  return total;
}

}  // namespace pw::intrusive_list_impl
