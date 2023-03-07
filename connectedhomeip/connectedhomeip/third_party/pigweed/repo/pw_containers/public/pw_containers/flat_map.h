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

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace pw::containers {

// Define and use a custom Pair object. This is because std::pair does not
// support constexpr assignment until C++20. The assignment is needed since
// the array of pairs will be sorted in the constructor (if not already).
template <typename First, typename Second>
struct Pair {
  First first;
  Second second;
};

template <typename T1, typename T2>
Pair(T1, T2) -> Pair<T1, T2>;

// A simple, fixed-size associative array with lookup by key or value.
//
// FlatMaps are initialized with a std::array of Pair<K, V> objects:
//   FlatMap<int, int> map({{{1, 2}, {3, 4}}});
//
// The keys do not need to be sorted as the constructor will sort the items
// if need be.
template <typename Key, typename Value, size_t kArraySize>
class FlatMap {
 public:
  using key_type = Key;
  using mapped_type = Value;
  using value_type = Pair<key_type, mapped_type>;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using container_type = typename std::array<value_type, kArraySize>;
  using iterator = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;

  constexpr FlatMap(const std::array<value_type, kArraySize>& items)
      : items_(items) {
    ConstexprSort(items_.data(), kArraySize);
  }

  FlatMap(FlatMap&) = delete;
  FlatMap& operator=(FlatMap&) = delete;

  // Capacity.
  constexpr size_type size() const { return kArraySize; }
  constexpr size_type empty() const { return size() == 0; }
  constexpr size_type max_size() const { return kArraySize; }

  // Lookup.
  constexpr bool contains(const key_type& key) const {
    return find(key) != end();
  }

  constexpr const_iterator find(const key_type& key) const {
    if (end() == begin()) {
      return end();
    }

    const_iterator it = lower_bound(key);
    if (it == end() || it->first != key) {
      return end();
    }
    return it;
  }

  constexpr const_iterator lower_bound(const key_type& key) const {
    return std::lower_bound(
        begin(), end(), key, [](const value_type& item, key_type lkey) {
          return item.first < lkey;
        });
  }

  constexpr const_iterator upper_bound(const key_type& key) const {
    return std::upper_bound(
        begin(), end(), key, [](key_type lkey, const value_type& item) {
          return item.first > lkey;
        });
  }

  constexpr std::pair<const_iterator, const_iterator> equal_range(
      const key_type& key) const {
    if (end() == begin()) {
      return std::make_pair(end(), end());
    }

    return std::make_pair(lower_bound(key), upper_bound(key));
  }

  // Iterators.
  constexpr const_iterator begin() const { return cbegin(); }
  constexpr const_iterator cbegin() const { return items_.cbegin(); }
  constexpr const_iterator end() const { return cend(); }
  constexpr const_iterator cend() const { return items_.cend(); }

 private:
  // Simple stable insertion sort function for constexpr support.
  // std::stable_sort is not constexpr. Should not be a problem with performance
  // in regards to the sizes that are typically dealt with.
  static constexpr void ConstexprSort(iterator data, size_type size) {
    if (size < 2) {
      return;
    }

    for (iterator it = data + 1, end = data + size; it < end; ++it) {
      if (it->first < it[-1].first) {
        // Rotate the value into place.
        value_type temp = std::move(*it);
        iterator it2 = it - 1;
        while (true) {
          *(it2 + 1) = std::move(*it2);
          if (it2 == data || !(temp.first < it2[-1].first)) {
            break;
          }
          --it2;
        }
        *it2 = std::move(temp);
      }
    }
  }

  std::array<value_type, kArraySize> items_;
};

}  // namespace pw::containers
