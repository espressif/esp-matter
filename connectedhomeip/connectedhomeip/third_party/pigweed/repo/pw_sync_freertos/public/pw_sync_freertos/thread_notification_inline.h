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

#include <mutex>

#include "FreeRTOS.h"
#include "pw_assert/assert.h"
#include "pw_interrupt/context.h"
#include "pw_sync/thread_notification.h"
#include "task.h"

namespace pw::sync {
namespace backend {

static_assert(configUSE_TASK_NOTIFICATIONS != 0,
              "Task Notifications aren't enabled.");

}  // namespace backend

inline ThreadNotification::ThreadNotification()
    : native_type_{
          .blocked_thread = nullptr,
          .notified = false,
      } {}

inline ThreadNotification::~ThreadNotification() = default;

inline bool ThreadNotification::try_acquire() {
  // Enforce the pw::sync::ThreadNotification IRQ contract.
  PW_DASSERT(!interrupt::InInterruptContext());
  std::lock_guard lock(native_type_.shared_spin_lock);
  const bool notified = native_type_.notified;
  native_type_.notified = false;
  return notified;
}

inline ThreadNotification::native_handle_type
ThreadNotification::native_handle() {
  return native_type_;
}

}  // namespace pw::sync
