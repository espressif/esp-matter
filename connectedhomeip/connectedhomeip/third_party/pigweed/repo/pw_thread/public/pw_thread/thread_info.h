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
#pragma once

#include <bitset>
#include <optional>

#include "pw_span/span.h"

namespace pw::thread {

// The class ThreadInfo provides a summary of specific thread information and is
// used by thread iteration to dump thread info generically.
//
// Captures the following fields:
//     stack_start_pointer
//     stack_end_pointer
//     stack_est_peak_pointer
//     thread_name
class ThreadInfo {
 public:
  ThreadInfo() = default;

  constexpr std::optional<uintptr_t> stack_low_addr() const {
    return get_stack_info_ptr(kStackLowAddress);
  }

  void set_stack_low_addr(uintptr_t val) {
    set_stack_info_ptr(kStackLowAddress, val);
  }

  void clear_stack_low_addr() { clear_stack_info_ptr(kStackLowAddress); }

  constexpr std::optional<uintptr_t> stack_high_addr() const {
    return get_stack_info_ptr(kStackHighAddress);
  }

  void set_stack_high_addr(uintptr_t val) {
    set_stack_info_ptr(kStackHighAddress, val);
  }

  void clear_stack_high_addr() { clear_stack_info_ptr(kStackHighAddress); }

  constexpr std::optional<uintptr_t> stack_pointer() const {
    return get_stack_info_ptr(kStackPointer);
  }

  void set_stack_pointer(uintptr_t val) {
    set_stack_info_ptr(kStackPointer, val);
  }

  void clear_stack_pointer() { clear_stack_info_ptr(kStackPointer); }

  constexpr std::optional<uintptr_t> stack_peak_addr() const {
    return get_stack_info_ptr(kStackPeakAddress);
  }

  void set_stack_peak_addr(uintptr_t val) {
    set_stack_info_ptr(kStackPeakAddress, val);
  }

  void clear_stack_peak_addr() { clear_stack_info_ptr(kStackPeakAddress); }

  constexpr std::optional<span<const std::byte>> thread_name() const {
    return has_value_[kThreadName] ? std::make_optional(thread_name_)
                                   : std::nullopt;
  }

  void set_thread_name(span<const std::byte> val) {
    thread_name_ = val;
    has_value_.set(kThreadName, true);
  }

  void clear_thread_name() { clear_stack_info_ptr(kThreadName); }

 private:
  enum ThreadInfoIndex {
    kStackLowAddress,
    kStackHighAddress,
    kStackPointer,
    kStackPeakAddress,
    kThreadName,
    kMaxNumMembersDoNotUse,
  };

  constexpr std::optional<uintptr_t> get_stack_info_ptr(
      ThreadInfoIndex index) const {
    return has_value_[index] ? std::make_optional(stack_info_ptrs_[index])
                             : std::nullopt;
  }

  void set_stack_info_ptr(ThreadInfoIndex index, uintptr_t val) {
    stack_info_ptrs_[index] = val;
    has_value_.set(index, true);
  }

  void clear_stack_info_ptr(ThreadInfoIndex index) {
    has_value_.set(index, false);
  }

  std::bitset<ThreadInfoIndex::kMaxNumMembersDoNotUse> has_value_;
  uintptr_t stack_info_ptrs_[ThreadInfoIndex::kMaxNumMembersDoNotUse];
  span<const std::byte> thread_name_;
};

}  // namespace pw::thread
