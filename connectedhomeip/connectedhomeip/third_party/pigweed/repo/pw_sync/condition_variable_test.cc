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

#include "pw_sync/condition_variable.h"

#include <chrono>
#include <functional>

#include "gtest/gtest.h"
#include "pw_containers/vector.h"
#include "pw_sync/mutex.h"
#include "pw_sync/timed_thread_notification.h"
#include "pw_thread/sleep.h"
#include "pw_thread/test_threads.h"
#include "pw_thread/thread.h"

namespace pw::sync {
namespace {

using namespace std::chrono_literals;

// A timeout for tests where successful behaviour involves waiting.
constexpr auto kRequiredTimeout = 100ms;

// Maximum extra wait time allowed for test that ensure something waits for
// `kRequiredTimeout`.
const auto kAllowedSlack = kRequiredTimeout * 1.5;

// A timeout that should only be hit if something goes wrong.
constexpr auto kFailureTimeout = 5s;

using StateLock = std::unique_lock<Mutex>;

struct ThreadInfo {
  explicit ThreadInfo(int id) : thread_id(id) {}

  // waiting_notifier is signalled in predicates to indicate that the predicate
  // has been evaluated. This guarantees (via insider information) that the
  // thread will acquire the internal ThreadNotification.
  TimedThreadNotification waiting_notifier;

  // Signals when the worker thread is done.
  TimedThreadNotification done_notifier;

  // The result of the predicate the worker thread uses with wait*(). Set from
  // the main test thread and read by the worker thread.
  bool predicate_result = false;

  // Stores the result of ConditionVariable::wait_for() or ::wait_until() for
  // use in test asserts.
  bool wait_result = false;

  // For use in recording the order in which threads block on a condition.
  const int thread_id;

  // Returns a function which will return the current value of
  //`predicate_result` and release `waiting_notifier`.
  std::function<bool()> Predicate() {
    return [this]() {
      bool result = this->predicate_result;
      this->waiting_notifier.release();
      return result;
    };
  }
};

// A `ThreadCore` implementation that delegates to an `std::function`.
class LambdaThreadCore : public pw::thread::ThreadCore {
 public:
  explicit LambdaThreadCore(std::function<void()> work)
      : work_(std::move(work)) {}

 private:
  void Run() override { work_(); }

  std::function<void()> work_;
};

class LambdaThread {
 public:
  // Starts a new thread which runs `work`, joining the thread on destruction.
  explicit LambdaThread(
      std::function<void()> work,
      pw::thread::Options options = pw::thread::test::TestOptionsThread0())
      : thread_core_(std::move(work)), thread_(options, thread_core_) {}
  ~LambdaThread() { thread_.join(); }
  LambdaThread(const LambdaThread&) = delete;
  LambdaThread(LambdaThread&&) = delete;
  LambdaThread& operator=(const LambdaThread&) = delete;
  LambdaThread&& operator=(LambdaThread&&) = delete;

 private:
  LambdaThreadCore thread_core_;
  pw::thread::Thread thread_;
};

TEST(Wait, PredicateTrueNoWait) {
  Mutex mutex;
  ConditionVariable condvar;
  ThreadInfo thread_info(0);

  LambdaThread thread([&mutex, &condvar, &info = thread_info] {
    StateLock l{mutex};
    condvar.wait(l, [] { return true; });

    info.done_notifier.release();
  });
  EXPECT_TRUE(thread_info.done_notifier.try_acquire_for(kFailureTimeout));
}

TEST(NotifyOne, BlocksUntilSignaled) {
  Mutex mutex;
  ConditionVariable condvar;
  ThreadInfo thread_info(0);

  LambdaThread thread([&mutex, &condvar, &info = thread_info] {
    StateLock l{mutex};
    condvar.wait(l, info.Predicate());
    info.done_notifier.release();
  });
  ASSERT_TRUE(thread_info.waiting_notifier.try_acquire_for(kFailureTimeout));
  {
    StateLock l{mutex};
    thread_info.predicate_result = true;
  }
  condvar.notify_one();
  ASSERT_TRUE(thread_info.done_notifier.try_acquire_for(kFailureTimeout));
}

TEST(NotifyOne, UnblocksOne) {
  Mutex mutex;
  ConditionVariable condvar;
  std::array<ThreadInfo, 2> thread_info = {ThreadInfo(0), ThreadInfo(1)};
  pw::Vector<int, 2> wait_order;

  LambdaThread thread_1(
      [&mutex, &condvar, &info = thread_info[0], &wait_order] {
        StateLock l{mutex};
        auto predicate = [&info, &wait_order] {
          wait_order.push_back(info.thread_id);
          auto result = info.predicate_result;
          info.waiting_notifier.release();
          return result;
        };
        condvar.wait(l, predicate);
        info.done_notifier.release();
      },
      pw::thread::test::TestOptionsThread0());
  LambdaThread thread_2(
      [&mutex, &condvar, &info = thread_info[1], &wait_order] {
        StateLock l{mutex};
        auto predicate = [&info, &wait_order] {
          wait_order.push_back(info.thread_id);
          auto result = info.predicate_result;
          info.waiting_notifier.release();
          return result;
        };
        condvar.wait(l, predicate);
        info.done_notifier.release();
      },
      pw::thread::test::TestOptionsThread1());

  ASSERT_TRUE(thread_info[0].waiting_notifier.try_acquire_for(kFailureTimeout));
  ASSERT_TRUE(thread_info[1].waiting_notifier.try_acquire_for(kFailureTimeout));

  {
    StateLock l{mutex};
    thread_info[1].predicate_result = true;
    thread_info[0].predicate_result = true;
  }
  condvar.notify_one();
  ASSERT_TRUE(thread_info[wait_order[0]].done_notifier.try_acquire_for(
      kFailureTimeout));
  ASSERT_FALSE(thread_info[wait_order[0]].done_notifier.try_acquire());
  condvar.notify_one();
  ASSERT_TRUE(thread_info[wait_order[1]].done_notifier.try_acquire_for(
      kFailureTimeout));
}

TEST(NotifyAll, UnblocksMultiple) {
  Mutex mutex;
  ConditionVariable condvar;
  std::array<ThreadInfo, 2> thread_info = {ThreadInfo(0), ThreadInfo(1)};

  LambdaThread thread_1(
      [&mutex, &condvar, &info = thread_info[0]] {
        StateLock l{mutex};
        condvar.wait(l, info.Predicate());
        info.done_notifier.release();
      },
      pw::thread::test::TestOptionsThread0());
  LambdaThread thread_2(
      [&mutex, &condvar, &info = thread_info[1]] {
        StateLock l{mutex};
        condvar.wait(l, info.Predicate());
        info.done_notifier.release();
      },
      pw::thread::test::TestOptionsThread1());

  ASSERT_TRUE(thread_info[0].waiting_notifier.try_acquire_for(kFailureTimeout));
  ASSERT_TRUE(thread_info[1].waiting_notifier.try_acquire_for(kFailureTimeout));
  {
    StateLock l{mutex};
    thread_info[0].predicate_result = true;
    thread_info[1].predicate_result = true;
  }
  condvar.notify_all();
  ASSERT_TRUE(thread_info[0].done_notifier.try_acquire_for(kFailureTimeout));
  ASSERT_TRUE(thread_info[1].done_notifier.try_acquire_for(kFailureTimeout));
}

TEST(WaitFor, ReturnsTrueIfSignalled) {
  Mutex mutex;
  ConditionVariable condvar;
  ThreadInfo thread_info(0);

  LambdaThread thread([&mutex, &condvar, &info = thread_info] {
    StateLock l{mutex};
    info.wait_result = condvar.wait_for(l, kFailureTimeout, info.Predicate());
    info.done_notifier.release();
  });

  ASSERT_TRUE(thread_info.waiting_notifier.try_acquire_for(kFailureTimeout));
  {
    StateLock l{mutex};
    thread_info.predicate_result = true;
  }
  condvar.notify_one();
  ASSERT_TRUE(thread_info.done_notifier.try_acquire_for(kFailureTimeout));
  ASSERT_TRUE(thread_info.wait_result);
}

TEST(WaitFor, ReturnsFalseIfTimesOut) {
  Mutex mutex;
  ConditionVariable condvar;
  ThreadInfo thread_info(0);

  LambdaThread thread([&mutex, &condvar, &info = thread_info] {
    StateLock l{mutex};
    info.wait_result = condvar.wait_for(l, 0ms, info.Predicate());
    info.done_notifier.release();
  });

  ASSERT_TRUE(thread_info.waiting_notifier.try_acquire_for(kFailureTimeout));
  ASSERT_TRUE(thread_info.done_notifier.try_acquire_for(kFailureTimeout));
  ASSERT_FALSE(thread_info.wait_result);
}

// NOTE: This test waits even in successful circumstances.
TEST(WaitFor, TimeoutApproximatelyCorrect) {
  Mutex mutex;
  ConditionVariable condvar;
  ThreadInfo thread_info(0);
  pw::chrono::SystemClock::duration wait_duration{};

  LambdaThread thread([&mutex, &condvar, &info = thread_info, &wait_duration] {
    StateLock l{mutex};
    auto start = pw::chrono::SystemClock::now();
    info.wait_result = condvar.wait_for(l, kRequiredTimeout, info.Predicate());
    wait_duration = pw::chrono::SystemClock::now() - start;
    info.done_notifier.release();
  });

  ASSERT_TRUE(thread_info.waiting_notifier.try_acquire_for(kFailureTimeout));
  // Wake up thread multiple times. Make sure the timeout is observed.
  for (int i = 0; i < 5; ++i) {
    condvar.notify_one();
    pw::this_thread::sleep_for(kRequiredTimeout / 6);
  }
  ASSERT_TRUE(thread_info.done_notifier.try_acquire_for(kFailureTimeout));
  EXPECT_FALSE(thread_info.wait_result);
  EXPECT_GE(wait_duration, kRequiredTimeout);
  EXPECT_LT(wait_duration, (kRequiredTimeout + kAllowedSlack));
}

TEST(WaitUntil, ReturnsTrueIfSignalled) {
  Mutex mutex;
  ConditionVariable condvar;
  ThreadInfo thread_info(0);

  LambdaThread thread([&mutex, &condvar, &info = thread_info] {
    StateLock l{mutex};
    info.wait_result = condvar.wait_until(
        l, pw::chrono::SystemClock::now() + kRequiredTimeout, info.Predicate());
    info.done_notifier.release();
  });

  ASSERT_TRUE(thread_info.waiting_notifier.try_acquire_for(kFailureTimeout));
  {
    StateLock l{mutex};
    thread_info.predicate_result = true;
  }
  condvar.notify_one();
  ASSERT_TRUE(thread_info.done_notifier.try_acquire_for(kFailureTimeout));
  ASSERT_TRUE(thread_info.wait_result);
}

// NOTE: This test waits even in successful circumstances.
TEST(WaitUntil, ReturnsFalseIfTimesOut) {
  Mutex mutex;
  ConditionVariable condvar;
  ThreadInfo thread_info(0);

  LambdaThread thread([&mutex, &condvar, &info = thread_info] {
    StateLock l{mutex};
    info.wait_result = condvar.wait_until(
        l, pw::chrono::SystemClock::now() + kRequiredTimeout, info.Predicate());
    info.done_notifier.release();
  });

  ASSERT_TRUE(thread_info.waiting_notifier.try_acquire_for(kFailureTimeout));
  ASSERT_TRUE(thread_info.done_notifier.try_acquire_for(kFailureTimeout));
  ASSERT_FALSE(thread_info.wait_result);
}

// NOTE: This test waits even in successful circumstances.
TEST(WaitUntil, TimeoutApproximatelyCorrect) {
  Mutex mutex;
  ConditionVariable condvar;
  ThreadInfo thread_info(0);
  pw::chrono::SystemClock::duration wait_duration{};

  LambdaThread thread([&mutex, &condvar, &info = thread_info, &wait_duration] {
    StateLock l{mutex};
    auto start = pw::chrono::SystemClock::now();
    info.wait_result = condvar.wait_until(
        l, pw::chrono::SystemClock::now() + kRequiredTimeout, info.Predicate());
    wait_duration = pw::chrono::SystemClock::now() - start;
    info.done_notifier.release();
  });

  ASSERT_TRUE(thread_info.waiting_notifier.try_acquire_for(kFailureTimeout));
  // Wake up thread multiple times. Make sure the timeout is observed.
  for (int i = 0; i < 5; ++i) {
    condvar.notify_one();
    pw::this_thread::sleep_for(kRequiredTimeout / 6);
  }
  ASSERT_TRUE(thread_info.done_notifier.try_acquire_for(kFailureTimeout));
  ASSERT_FALSE(thread_info.wait_result);
  ASSERT_GE(wait_duration, kRequiredTimeout);
  ASSERT_LE(wait_duration, kRequiredTimeout + kAllowedSlack);
}

}  // namespace
}  // namespace pw::sync
