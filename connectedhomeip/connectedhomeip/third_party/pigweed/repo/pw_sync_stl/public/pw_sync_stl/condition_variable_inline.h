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

#include <mutex>

#include "pw_sync/condition_variable.h"

namespace pw::sync {

inline void ConditionVariable::notify_one() { native_type_.notify_one(); }

inline void ConditionVariable::notify_all() { native_type_.notify_all(); }

template <typename Predicate>
void ConditionVariable::wait(std::unique_lock<Mutex>& lock,
                             Predicate predicate) {
  native_type_.wait(lock, std::move(predicate));
}

template <typename Predicate>
bool ConditionVariable::wait_for(std::unique_lock<Mutex>& lock,
                                 pw::chrono::SystemClock::duration timeout,
                                 Predicate predicate) {
  return native_type_.wait_for(lock, timeout, std::move(predicate));
}

template <typename Predicate>
bool ConditionVariable::wait_until(std::unique_lock<Mutex>& lock,
                                   pw::chrono::SystemClock::time_point deadline,
                                   Predicate predicate) {
  return native_type_.wait_until(lock, deadline, std::move(predicate));
}

inline auto ConditionVariable::native_handle() -> native_handle_type {
  return native_type_;
}

}  // namespace pw::sync
