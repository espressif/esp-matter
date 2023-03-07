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

#include <chrono>

#include "gtest/gtest.h"
#include "pw_chrono/system_clock.h"
#include "pw_chrono/system_timer.h"
#include "pw_sync/thread_notification.h"

using namespace std::chrono_literals;

namespace pw::chrono {
namespace {

// We can't control the SystemClock's period configuration, so just in case
// duration cannot be accurately expressed in integer ticks, round the
// duration up.
constexpr SystemClock::duration kRoundedArbitraryShortDuration =
    SystemClock::for_at_least(42ms);
constexpr SystemClock::duration kRoundedArbitraryLongDuration =
    SystemClock::for_at_least(1s);

void ShouldNotBeInvoked(SystemClock::time_point) { FAIL(); }

TEST(SystemTimer, CancelInactive) {
  SystemTimer timer(ShouldNotBeInvoked);
  timer.Cancel();
}

TEST(SystemTimer, CancelExplicitly) {
  SystemTimer timer(ShouldNotBeInvoked);
  timer.InvokeAfter(kRoundedArbitraryLongDuration);
  timer.Cancel();
}

TEST(SystemTimer, CancelThroughDestruction) {
  SystemTimer timer(ShouldNotBeInvoked);
  timer.InvokeAfter(kRoundedArbitraryLongDuration);
}

TEST(SystemTimer, CancelThroughRescheduling) {
  SystemTimer timer(ShouldNotBeInvoked);
  timer.InvokeAfter(kRoundedArbitraryLongDuration);
  // Cancel the first with this rescheduling.
  timer.InvokeAfter(kRoundedArbitraryLongDuration);
  timer.Cancel();
}

// Helper class to let test cases easily instantiate a timer with a handler
// and its own context.
class TimerWithHandler {
 public:
  TimerWithHandler()
      : timer_([this](SystemClock::time_point expired_deadline) {
          this->OnExpiryCallback(expired_deadline);
        }) {}
  virtual ~TimerWithHandler() = default;

  // To be implemented by the test case.
  virtual void OnExpiryCallback(SystemClock::time_point expired_deadline) = 0;

  SystemTimer& timer() { return timer_; }

 private:
  SystemTimer timer_;
};

TEST(SystemTimer, StaticInvokeAt) {
  class TimerWithContext : public TimerWithHandler {
   public:
    void OnExpiryCallback(SystemClock::time_point expired_deadline) override {
      EXPECT_GE(SystemClock::now(), expired_deadline);
      EXPECT_EQ(expired_deadline, expected_deadline);
      callback_ran_notification.release();
    }

    SystemClock::time_point expected_deadline;
    sync::ThreadNotification callback_ran_notification;
  };
  static TimerWithContext uut;

  uut.expected_deadline = SystemClock::now() + kRoundedArbitraryShortDuration;
  uut.timer().InvokeAt(uut.expected_deadline);
  uut.callback_ran_notification.acquire();

  // Ensure you can re-use the timer.
  uut.expected_deadline = SystemClock::now() + kRoundedArbitraryShortDuration;
  uut.timer().InvokeAt(uut.expected_deadline);
  uut.callback_ran_notification.acquire();
}

TEST(SystemTimer, InvokeAt) {
  class TimerWithContext : public TimerWithHandler {
   public:
    void OnExpiryCallback(SystemClock::time_point expired_deadline) override {
      EXPECT_GE(SystemClock::now(), expired_deadline);
      EXPECT_EQ(expired_deadline, expected_deadline);
      callback_ran_notification.release();
    }

    SystemClock::time_point expected_deadline;
    sync::ThreadNotification callback_ran_notification;
  };
  TimerWithContext uut;

  uut.expected_deadline = SystemClock::now() + kRoundedArbitraryShortDuration;
  uut.timer().InvokeAt(uut.expected_deadline);
  uut.callback_ran_notification.acquire();

  // Ensure you can re-use the timer.
  uut.expected_deadline = SystemClock::now() + kRoundedArbitraryShortDuration;
  uut.timer().InvokeAt(uut.expected_deadline);
  uut.callback_ran_notification.acquire();

  // Ensure scheduling it in the past causes it to execute immediately.
  uut.expected_deadline = SystemClock::now() - SystemClock::duration(1);
  uut.timer().InvokeAt(uut.expected_deadline);
  uut.callback_ran_notification.acquire();
}

TEST(SystemTimer, InvokeAfter) {
  class TimerWithContext : public TimerWithHandler {
   public:
    void OnExpiryCallback(SystemClock::time_point expired_deadline) override {
      EXPECT_GE(SystemClock::now(), expired_deadline);
      EXPECT_GE(expired_deadline, expected_min_deadline);
      callback_ran_notification.release();
    }

    SystemClock::time_point expected_min_deadline;
    sync::ThreadNotification callback_ran_notification;
  };
  TimerWithContext uut;

  uut.expected_min_deadline =
      SystemClock::TimePointAfterAtLeast(kRoundedArbitraryShortDuration);
  uut.timer().InvokeAfter(kRoundedArbitraryShortDuration);
  uut.callback_ran_notification.acquire();

  // Ensure you can re-use the timer.
  uut.expected_min_deadline =
      SystemClock::TimePointAfterAtLeast(kRoundedArbitraryShortDuration);
  uut.timer().InvokeAfter(kRoundedArbitraryShortDuration);
  uut.callback_ran_notification.acquire();

  // Ensure scheduling it immediately works.
  uut.expected_min_deadline = SystemClock::now();
  uut.timer().InvokeAfter(SystemClock::duration(0));
  uut.callback_ran_notification.acquire();
}

TEST(SystemTimer, CancelFromCallback) {
  class TimerWithContext : public TimerWithHandler {
   public:
    void OnExpiryCallback(SystemClock::time_point) override {
      timer().Cancel();
      callback_ran_notification.release();
    }

    sync::ThreadNotification callback_ran_notification;
  };
  TimerWithContext uut;

  uut.timer().InvokeAfter(kRoundedArbitraryShortDuration);
  uut.callback_ran_notification.acquire();
}

TEST(SystemTimer, RescheduleAndCancelFromCallback) {
  class TimerWithContext : public TimerWithHandler {
   public:
    void OnExpiryCallback(SystemClock::time_point) override {
      timer().InvokeAfter(kRoundedArbitraryShortDuration);
      timer().Cancel();
      callback_ran_notification.release();
    }

    sync::ThreadNotification callback_ran_notification;
  };
  TimerWithContext uut;

  uut.timer().InvokeAfter(kRoundedArbitraryShortDuration);
  uut.callback_ran_notification.acquire();
}

TEST(SystemTimer, RescheduleFromCallback) {
  class TimerWithContext : public TimerWithHandler {
   public:
    void OnExpiryCallback(SystemClock::time_point expired_deadline) override {
      EXPECT_GE(SystemClock::now(), expired_deadline);

      EXPECT_EQ(expired_deadline, expected_deadline);
      invocation_count++;
      ASSERT_LE(invocation_count, kRequiredInvocations);
      if (invocation_count < kRequiredInvocations) {
        expected_deadline = expired_deadline + kPeriod;
        timer().InvokeAt(expected_deadline);
      } else {
        callbacks_done_notification.release();
      }
    }

    const uint8_t kRequiredInvocations = 5;
    const SystemClock::duration kPeriod = kRoundedArbitraryShortDuration;
    uint8_t invocation_count = 0;
    SystemClock::time_point expected_deadline;
    sync::ThreadNotification callbacks_done_notification;
  };
  TimerWithContext uut;

  uut.expected_deadline = SystemClock::now() + kRoundedArbitraryShortDuration;
  uut.timer().InvokeAt(uut.expected_deadline);
  uut.callbacks_done_notification.acquire();
}

TEST(SystemTimer, DoubleRescheduleFromCallback) {
  class TimerWithContext : public TimerWithHandler {
   public:
    void OnExpiryCallback(SystemClock::time_point expired_deadline) override {
      EXPECT_GE(SystemClock::now(), expired_deadline);

      EXPECT_EQ(expired_deadline, expected_deadline);
      invocation_count++;
      ASSERT_LE(invocation_count, kExpectedInvocations);
      if (invocation_count == 1) {
        expected_deadline = expired_deadline + kPeriod;
        timer().InvokeAt(expected_deadline);
        timer().InvokeAt(expected_deadline);
      } else {
        callbacks_done_notification.release();
      }
    }

    const uint8_t kExpectedInvocations = 2;
    const SystemClock::duration kPeriod = kRoundedArbitraryShortDuration;
    uint8_t invocation_count = 0;
    SystemClock::time_point expected_deadline;
    sync::ThreadNotification callbacks_done_notification;
  };
  TimerWithContext uut;

  uut.expected_deadline = SystemClock::now() + kRoundedArbitraryShortDuration;
  uut.timer().InvokeAt(uut.expected_deadline);
  uut.callbacks_done_notification.acquire();
}

}  // namespace
}  // namespace pw::chrono
