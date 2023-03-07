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

#include <cstdint>
#include <optional>

#include "pw_assert/assert.h"
#include "pw_span/span.h"

namespace pw::work_queue::internal {

// TODO(hepler): Replace this with a std::deque like container.
template <typename T>
class CircularBuffer {
 public:
  explicit constexpr CircularBuffer(span<T> buffer)
      : buffer_(buffer), head_(0), tail_(0), count_(0) {}

  bool empty() const { return count_ == 0; }
  bool full() const { return count_ == buffer_.size(); }
  size_t size() const { return count_; }
  size_t capacity() const { return buffer_.size(); }

  template <typename Ty>
  bool Push(Ty&& value) {
    PW_DASSERT(tail_ < buffer_.size());

    if (full()) {
      return false;
    }

    buffer_[tail_] = std::forward<Ty>(value);
    IncrementWithWrap(tail_);
    ++count_;
    return true;
  }

  std::optional<T> Pop() {
    PW_DASSERT(head_ < buffer_.size());

    if (empty()) {
      return std::nullopt;
    }

    T entry = std::move(buffer_[head_]);
    IncrementWithWrap(head_);
    --count_;
    return entry;
  }

 private:
  void IncrementWithWrap(size_t& index) const {
    index++;
    // Note: branch is faster than mod (%) on common embedded architectures.
    if (index == buffer_.size()) {
      index = 0;
    }
  }

  span<T> buffer_;

  size_t head_;
  size_t tail_;
  size_t count_;
};

}  // namespace pw::work_queue::internal
