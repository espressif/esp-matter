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

#include "tx_api.h"

namespace pw::thread::backend {

// Instead of using a pw::thread::threadx specific identifier, the ThreadX
// thread pointer is used as this means pw::this_thread::id works correctly on
// threads started with the native ThreadX APIs as well as those started
// using the pw::thread APIs.
class NativeId {
 public:
  constexpr NativeId(TX_THREAD* thread_ptr = nullptr)
      : thread_ptr_(thread_ptr) {}

  constexpr bool operator==(NativeId other) const {
    return thread_ptr_ == other.thread_ptr_;
  }
  constexpr bool operator!=(NativeId other) const {
    return thread_ptr_ != other.thread_ptr_;
  }
  constexpr bool operator<(NativeId other) const {
    return thread_ptr_ < other.thread_ptr_;
  }
  constexpr bool operator<=(NativeId other) const {
    return thread_ptr_ <= other.thread_ptr_;
  }
  constexpr bool operator>(NativeId other) const {
    return thread_ptr_ > other.thread_ptr_;
  }
  constexpr bool operator>=(NativeId other) const {
    return thread_ptr_ >= other.thread_ptr_;
  }

 private:
  TX_THREAD* thread_ptr_;
};

}  // namespace pw::thread::backend
