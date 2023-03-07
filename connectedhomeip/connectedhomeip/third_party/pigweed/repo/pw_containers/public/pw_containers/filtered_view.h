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
#include <iterator>

#include "pw_assert/assert.h"
#include "pw_preprocessor/compiler.h"

namespace pw::containers {

// FilteredView supports iterating over only elements that match a filter in a
// container. FilteredView works with any container with an incrementable
// iterator. The back() function currently requires a bidirectional iterator.
//
// FilteredView is similar to C++20's std::filter_view.
template <typename Container, typename Filter>
class FilteredView {
 public:
  // Iterator that only moves to elements that match the provided filter.
  class iterator {
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename Container::value_type;
    using pointer = typename Container::pointer;
    using reference = typename Container::reference;
    using iterator_category = std::bidirectional_iterator_tag;

    constexpr iterator() : view_(nullptr), it_(0) {}

    iterator& operator++();

    iterator operator++(int) {
      iterator original = *this;
      operator++();
      return original;
    }

    iterator& operator--();

    iterator operator--(int) {
      iterator original = *this;
      operator--();
      return original;
    }

    const auto& operator*() const { return value(); }

    const auto* operator->() const { return &value(); }

    constexpr bool operator==(const iterator& other) const {
      return view_ == other.view_ && it_ == other.it_;
    }

    constexpr bool operator!=(const iterator& other) const {
      return !(*this == other);
    }

   private:
    friend class FilteredView;

    enum EndIterator { kEnd };

    explicit iterator(const FilteredView& view)
        : view_(&view), it_(view.container_.begin()) {
      FindMatch();
    }

    iterator(const FilteredView& view, EndIterator)
        : view_(&view), it_(view.container_.end()) {}

    // Accesses the value referred to by this iterator.
    const auto& value() const { return *it_; }

    // Iterates until a match is found, up to end().
    void FindMatch();

    bool MatchesItem(const value_type& value) const {
      return view_->filter_(value);
    }

    const FilteredView* view_;
    typename Container::const_iterator it_;
  };

  using const_iterator = iterator;

  template <typename... FilterArgs>
  constexpr FilteredView(const Container& container, Filter&& filter)
      : container_(container), filter_(std::move(filter)) {}

  constexpr FilteredView(const FilteredView&) = delete;
  constexpr FilteredView& operator=(const FilteredView&) = delete;

  const auto& operator[](size_t index) const {
    auto it = begin();
    std::advance(it, index);
    return *it;
  }

  // Accesses the first matching element. Invalid if empty().
  const auto& front() const { return *begin(); }

  // Accesses the last matching element. Invalid if empty().
  const auto& back() const { return *std::prev(end()); }

  // The number of elements in the container that match the filter.
  size_t size() const { return std::distance(begin(), end()); }

  bool empty() const { return begin() == end(); }

  iterator begin() const { return iterator(*this); }
  iterator end() const { return iterator(*this, iterator::kEnd); }

 private:
  const Container& container_;
  Filter filter_;
};

template <typename Container, typename Filter>
void FilteredView<Container, Filter>::iterator::FindMatch() {
  for (; it_ != view_->container_.end(); ++it_) {
    if (MatchesItem(*it_)) {
      break;
    }
  }
}

template <typename Container, typename Filter>
typename FilteredView<Container, Filter>::iterator&
FilteredView<Container, Filter>::iterator::operator++() {
  PW_ASSERT(it_ != view_->container_.end());

  ++it_;
  FindMatch();
  return *this;
}

template <typename Container, typename Filter>
typename FilteredView<Container, Filter>::iterator&
FilteredView<Container, Filter>::iterator::operator--() {
  decltype(it_) new_it = view_->container_.end();
  while (new_it != view_->container_.begin()) {
    --new_it;
    if (MatchesItem(*new_it)) {
      it_ = new_it;
      return *this;
    }
  }

  PW_ASSERT(false);
  PW_UNREACHABLE;
}

}  // namespace pw::containers
