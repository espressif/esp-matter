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

namespace pw::containers {

// Wraps an iterator with another iterator. This is helpful for creating an
// iterator that yields items derived from the original iterator's type. For
// example, the derived iterator might return a member of or a value calculated
// from the original iterator's value.
//
// Classes inherit from this and provide operator* and operator-> as
// appropriate.
template <typename Impl, typename Iterator, typename ValueType>
class WrappedIterator {
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = ValueType;
  using pointer = ValueType*;
  using reference = ValueType&;
  using iterator_category = std::bidirectional_iterator_tag;

  constexpr WrappedIterator(const WrappedIterator&) = default;
  constexpr WrappedIterator& operator=(const WrappedIterator&) = default;

  Impl& operator++() {
    ++iterator_;
    return static_cast<Impl&>(*this);
  }

  Impl operator++(int) {
    Impl original = static_cast<const Impl&>(*this);
    ++iterator_;
    return original;
  }

  Impl& operator--() {
    --iterator_;
    return static_cast<Impl&>(*this);
  }

  Impl operator--(int) {
    Impl original = static_cast<const Impl&>(*this);
    --iterator_;
    return original;
  }

  constexpr bool operator==(const WrappedIterator& other) const {
    return iterator_ == other.iterator_;
  }

  constexpr bool operator!=(const WrappedIterator& other) const {
    return !(*this == other);
  }

 protected:
  constexpr WrappedIterator() = default;

  constexpr WrappedIterator(const Iterator& it) : iterator_(it) {}

  const auto& value() const { return *iterator_; }
  const auto* ptr() const { return iterator_.operator->(); }

 private:
  Iterator iterator_;
};

}  // namespace pw::containers
