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

#include <algorithm>

#include "pw_assert/assert.h"
#include "pw_thread/id.h"
#include "pw_thread_threadx/config.h"
#include "pw_thread_threadx/options.h"

namespace pw::thread {

inline Thread::Thread() : native_type_(nullptr) {}

inline Thread& Thread::operator=(Thread&& other) {
  native_type_ = other.native_type_;
  other.native_type_ = nullptr;
  return *this;
}

inline Thread::~Thread() { PW_DASSERT(native_type_ == nullptr); }

inline Id Thread::get_id() const {
  if (native_type_ == nullptr) {
    return Id(nullptr);
  }
  return Id(&native_type_->tcb());
}

inline void Thread::swap(Thread& other) {
  std::swap(native_type_, other.native_type_);
}

inline Thread::native_handle_type Thread::native_handle() {
  return native_type_;
}

}  // namespace pw::thread
