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

#include "pw_chrono/system_clock.h"
#include "pw_sync/thread_notification.h"

namespace pw::sync {

// The TimedThreadNotification is a synchronization primitive that can be used
// to permit a SINGLE thread to block and consume a latching, saturating
// notification from  multiple notifiers.
//
// IMPORTANT: This is a single consumer/waiter, multiple producer/notifier API!
// The acquire APIs must only be invoked by a single consuming thread. As a
// result, having multiple threads receiving notifications via the acquire API
// is unsupported.
//
// This is effectively a subset of a binary semaphore API, except that only a
// single thread can be notified and block at a time.
//
// The single consumer aspect of the API permits the use of a smaller and/or
// faster native APIs such as direct thread signaling.
//
// The TimedThreadNotification is initialized to being empty (latch is not set).
class TimedThreadNotification : public ThreadNotification {
 public:
  TimedThreadNotification() = default;
  ~TimedThreadNotification() = default;
  TimedThreadNotification(const TimedThreadNotification&) = delete;
  TimedThreadNotification(TimedThreadNotification&&) = delete;
  TimedThreadNotification& operator=(const TimedThreadNotification&) = delete;
  TimedThreadNotification& operator=(TimedThreadNotification&&) = delete;

  // Blocks until the specified timeout duration has elapsed or the thread
  // has been notified (i.e. notification latch can be cleared because it was
  // set), whichever comes first.
  //
  // Clears the notification latch.
  //
  // Returns true if the thread was notified, meaning the the internal latch was
  // reset successfully.
  //
  // IMPORTANT: This should only be used by a single consumer thread.
  bool try_acquire_for(chrono::SystemClock::duration timeout);

  // Blocks until the specified deadline time has been reached the thread has
  // been notified (i.e. notification latch can be cleared because it was set),
  // whichever comes first.
  //
  // Clears the notification latch.
  //
  // Returns true if the thread was notified, meaning the the internal latch was
  // reset successfully.
  //
  // IMPORTANT: This should only be used by a single consumer thread.
  bool try_acquire_until(chrono::SystemClock::time_point deadline);
};

}  // namespace pw::sync

#include "pw_sync_backend/timed_thread_notification_inline.h"
