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

#include "pw_sync/thread_notification.h"

#include <chrono>

#include "FreeRTOS.h"
#include "gtest/gtest.h"
#include "pw_chrono/system_clock.h"
#include "pw_thread/sleep.h"
#include "pw_thread/test_threads.h"
#include "pw_thread/thread.h"
#include "pw_thread/thread_core.h"
#include "task.h"

namespace pw::sync::freertos {
namespace {

using pw::chrono::SystemClock;
using pw::thread::Thread;

}  // namespace

// These tests are targeted specifically to verify interactions between suspend
// and being blocked on direct task notifications and how they impact usage of
// the FreeRTOS optimized ThreadNotification backend.
#if INCLUDE_vTaskSuspend == 1

class NotificationAcquirer : public thread::ThreadCore {
 public:
  void WaitUntilRunning() { started_notification_.acquire(); }
  void Release() { unblock_notification_.release(); }
  void WaitUntilFinished() { finished_notification_.acquire(); }
  std::optional<SystemClock::time_point> notified_time() const {
    return notified_time_;
  }
  TaskHandle_t task_handle() const { return task_handle_; }

 private:
  void Run() final {
    task_handle_ = xTaskGetCurrentTaskHandle();
    started_notification_.release();
    unblock_notification_.acquire();
    notified_time_ = SystemClock::now();
    finished_notification_.release();
  }

  TaskHandle_t task_handle_;
  ThreadNotification started_notification_;
  ThreadNotification unblock_notification_;
  ThreadNotification finished_notification_;
  std::optional<SystemClock::time_point> notified_time_;
};

TEST(ThreadNotification, AcquireWithoutSuspend) {
  NotificationAcquirer notification_acquirer;
  Thread thread =
      Thread(thread::test::TestOptionsThread0(), notification_acquirer);

  notification_acquirer.WaitUntilRunning();
  // At this point the thread is blocked and waiting on the notification.
  const SystemClock::time_point release_time = SystemClock::now();
  notification_acquirer.Release();
  notification_acquirer.WaitUntilFinished();
  ASSERT_TRUE(notification_acquirer.notified_time().has_value());
  EXPECT_GE(notification_acquirer.notified_time().value(), release_time);

  // Clean up the test thread context.
#if PW_THREAD_JOINING_ENABLED
  thread.join();
#else
  thread.detach();
  thread::test::WaitUntilDetachedThreadsCleanedUp();
#endif  // PW_THREAD_JOINING_ENABLED
}

TEST(ThreadNotification, AcquireWithSuspend) {
  NotificationAcquirer notification_acquirer;
  Thread thread =
      Thread(thread::test::TestOptionsThread0(), notification_acquirer);

  notification_acquirer.WaitUntilRunning();

  // Suspend and resume the task before notifying it, which should cause the
  // internal xTaskNotifyWait to stop blocking and return pdFALSE upon resume.
  vTaskSuspend(notification_acquirer.task_handle());
  vTaskResume(notification_acquirer.task_handle());

  // Sleep for at least one tick to ensure the time moved forward to let us
  // observe the unblock time is in fact after resumed it.
  this_thread::sleep_for(SystemClock::duration(1));

  // At this point the thread is blocked and waiting on the notification.
  const SystemClock::time_point release_time = SystemClock::now();
  notification_acquirer.Release();
  notification_acquirer.WaitUntilFinished();
  ASSERT_TRUE(notification_acquirer.notified_time().has_value());
  EXPECT_GE(notification_acquirer.notified_time().value(), release_time);

  // Clean up the test thread context.
#if PW_THREAD_JOINING_ENABLED
  thread.join();
#else
  thread.detach();
  thread::test::WaitUntilDetachedThreadsCleanedUp();
#endif  // PW_THREAD_JOINING_ENABLED
}

#endif  // INCLUDE_vTaskSuspend == 1

}  // namespace pw::sync::freertos
