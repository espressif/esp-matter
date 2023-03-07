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

#include "pw_chrono/system_clock.h"
#include "pw_sync/mutex.h"
#include "pw_sync_backend/condition_variable_native.h"

namespace pw::sync {

// ConditionVariable represents a condition variable using an API very similar
// to std::condition_variable. Implementations of this class should share the
// same semantics as std::condition_variable.
class ConditionVariable {
 public:
  using native_handle_type = backend::NativeConditionVariableHandle;

  ConditionVariable() = default;

  ConditionVariable(const ConditionVariable&) = delete;

  ~ConditionVariable() = default;

  ConditionVariable& operator=(const ConditionVariable&) = delete;

  // Wake up one thread waiting on a condition.
  //
  // The thread will re-evaluate the condition via its predicate. Threads where
  // the predicate evaluates false will go back to waiting. The new order of
  // waiting threads is undefined.
  void notify_one();

  // Wake up all threads waiting on the condition variable.
  //
  // Woken threads will re-evaluate the condition via their predicate. Threads
  // where the predicate evaluates false will go back to waiting. The new order
  // of waiting threads is undefined.
  void notify_all();

  // Block the current thread until predicate() == true.
  //
  // Precondition: the provided lock must be locked.
  template <typename Predicate>
  void wait(std::unique_lock<Mutex>& lock, Predicate predicate);

  // Block the current thread for a duration up to the given timeout or
  // until predicate() == true whichever comes first.
  //
  // Returns: true if predicate() == true.
  //          false if timeout expired.
  //
  // Precondition: the provided lock must be locked.
  template <typename Predicate>
  bool wait_for(std::unique_lock<Mutex>& lock,
                pw::chrono::SystemClock::duration timeout,
                Predicate predicate);

  // Block the current thread until given point in time or until predicate() ==
  // true whichever comes first.
  //
  // Returns: true if predicate() == true.
  //          false if the deadline was reached.
  //
  // Precondition: the provided lock must be locked.
  template <typename Predicate>
  bool wait_until(std::unique_lock<Mutex>& lock,
                  pw::chrono::SystemClock::time_point deadline,
                  Predicate predicate);

  native_handle_type native_handle();

 private:
  backend::NativeConditionVariable native_type_;
};

}  // namespace pw::sync

#include "pw_sync_backend/condition_variable_inline.h"
