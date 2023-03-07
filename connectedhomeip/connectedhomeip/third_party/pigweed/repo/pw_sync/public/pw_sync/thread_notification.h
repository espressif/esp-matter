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

#include "pw_sync_backend/thread_notification_native.h"

namespace pw::sync {

// The ThreadNotification is a synchronization primitive that can be used to
// permit a SINGLE thread to block and consume a latching, saturating
// notification from multiple notifiers.
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
// The ThreadNotification is initialized to being empty (latch is not set).
class ThreadNotification {
 public:
  using native_handle_type = backend::NativeThreadNotificationHandle;

  ThreadNotification();
  ~ThreadNotification();
  ThreadNotification(const ThreadNotification&) = delete;
  ThreadNotification(ThreadNotification&&) = delete;
  ThreadNotification& operator=(const ThreadNotification&) = delete;
  ThreadNotification& operator=(ThreadNotification&&) = delete;

  // Blocks indefinitely until the thread is notified, i.e. until the
  // notification latch can be cleared because it was set.
  //
  // Clears the notification latch.
  //
  // IMPORTANT: This should only be used by a single consumer thread.
  void acquire();

  // Returns whether the thread has been notified, i.e. whether the notificion
  // latch was set and resets the latch regardless.
  //
  // Clears the notification latch.
  //
  // Returns true if the thread was notified, meaning the the internal latch was
  // reset successfully.
  //
  // IMPORTANT: This should only be used by a single consumer thread.
  bool try_acquire();

  // Notifies the thread in a saturating manner, setting the notification latch.
  //
  // Raising the notification multiple time without it being acquired by the
  // consuming thread is equivalent to raising the notification once to the
  // thread. The notification is latched in case the thread was not waiting at
  // the time.
  //
  // This is IRQ and thread safe.
  void release();

  native_handle_type native_handle();

 private:
  // This may be a wrapper around a native type with additional members.
  backend::NativeThreadNotification native_type_;
};

}  // namespace pw::sync

#include "pw_sync_backend/thread_notification_inline.h"
