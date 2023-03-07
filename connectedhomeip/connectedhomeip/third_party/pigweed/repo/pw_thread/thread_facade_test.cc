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

#include "gtest/gtest.h"
#include "pw_sync/binary_semaphore.h"
#include "pw_thread/id.h"
#include "pw_thread/test_threads.h"
#include "pw_thread/thread.h"

using pw::thread::test::TestOptionsThread0;
using pw::thread::test::TestOptionsThread1;
using pw::thread::test::WaitUntilDetachedThreadsCleanedUp;

namespace pw::thread {
namespace {

TEST(Thread, DefaultIds) {
  Thread not_executing_thread;
  EXPECT_EQ(not_executing_thread.get_id(), Id());
}

static void ReleaseBinarySemaphore(void* arg) {
  static_cast<sync::BinarySemaphore*>(arg)->release();
}

#if PW_THREAD_JOINING_ENABLED
TEST(Thread, Join) {
  Thread thread;
  EXPECT_FALSE(thread.joinable());
  sync::BinarySemaphore thread_ran_sem;
  thread =
      Thread(TestOptionsThread0(), ReleaseBinarySemaphore, &thread_ran_sem);
  EXPECT_TRUE(thread.joinable());
  thread.join();
  EXPECT_EQ(thread.get_id(), Id());
  EXPECT_TRUE(thread_ran_sem.try_acquire());
}
#endif  // PW_THREAD_JOINING_ENABLED

TEST(Thread, Detach) {
  Thread thread;
  sync::BinarySemaphore thread_ran_sem;
  thread =
      Thread(TestOptionsThread0(), ReleaseBinarySemaphore, &thread_ran_sem);
  EXPECT_NE(thread.get_id(), Id());
  EXPECT_TRUE(thread.joinable());
  thread.detach();
  EXPECT_EQ(thread.get_id(), Id());
  EXPECT_FALSE(thread.joinable());
  thread_ran_sem.acquire();

  WaitUntilDetachedThreadsCleanedUp();
}

TEST(Thread, SwapWithoutExecution) {
  Thread thread_0;
  Thread thread_1;

  // Make sure we can swap threads which are not associated with any execution.
  thread_0.swap(thread_1);
}

TEST(Thread, SwapWithOneExecuting) {
  Thread thread_0;
  EXPECT_EQ(thread_0.get_id(), Id());

  sync::BinarySemaphore thread_ran_sem;
  Thread thread_1(
      TestOptionsThread1(), ReleaseBinarySemaphore, &thread_ran_sem);

  EXPECT_NE(thread_1.get_id(), Id());

  thread_0.swap(thread_1);
  EXPECT_NE(thread_0.get_id(), Id());
  EXPECT_EQ(thread_1.get_id(), Id());

  thread_0.detach();
  EXPECT_EQ(thread_0.get_id(), Id());

  thread_ran_sem.acquire();
  WaitUntilDetachedThreadsCleanedUp();
}

TEST(Thread, SwapWithTwoExecuting) {
  sync::BinarySemaphore thread_a_ran_sem;
  Thread thread_0(
      TestOptionsThread0(), ReleaseBinarySemaphore, &thread_a_ran_sem);
  sync::BinarySemaphore thread_b_ran_sem;
  Thread thread_1(
      TestOptionsThread1(), ReleaseBinarySemaphore, &thread_b_ran_sem);
  const Id thread_a_id = thread_0.get_id();
  EXPECT_NE(thread_a_id, Id());
  const Id thread_b_id = thread_1.get_id();
  EXPECT_NE(thread_b_id, Id());
  EXPECT_NE(thread_a_id, thread_b_id);

  thread_0.swap(thread_1);
  EXPECT_EQ(thread_1.get_id(), thread_a_id);
  EXPECT_EQ(thread_0.get_id(), thread_b_id);

  thread_0.detach();
  EXPECT_EQ(thread_0.get_id(), Id());
  thread_1.detach();
  EXPECT_EQ(thread_1.get_id(), Id());

  thread_a_ran_sem.acquire();
  thread_b_ran_sem.acquire();
  WaitUntilDetachedThreadsCleanedUp();
}

TEST(Thread, MoveOperator) {
  Thread thread_0;
  EXPECT_EQ(thread_0.get_id(), Id());

  sync::BinarySemaphore thread_ran_sem;
  Thread thread_1(
      TestOptionsThread1(), ReleaseBinarySemaphore, &thread_ran_sem);
  EXPECT_NE(thread_1.get_id(), Id());

  thread_0 = std::move(thread_1);
  EXPECT_NE(thread_0.get_id(), Id());
#ifndef __clang_analyzer__
  EXPECT_EQ(thread_1.get_id(), Id());
#endif  // ignore use-after-move

  thread_0.detach();
  EXPECT_EQ(thread_0.get_id(), Id());

  thread_ran_sem.acquire();
  WaitUntilDetachedThreadsCleanedUp();
}

class SemaphoreReleaser : public ThreadCore {
 public:
  pw::sync::BinarySemaphore& semaphore() { return semaphore_; }

 private:
  void Run() override { semaphore_.release(); }

  sync::BinarySemaphore semaphore_;
};

TEST(Thread, ThreadCore) {
  SemaphoreReleaser semaphore_releaser;
  Thread thread(TestOptionsThread0(), semaphore_releaser);
  EXPECT_NE(thread.get_id(), Id());
  EXPECT_TRUE(thread.joinable());
  thread.detach();
  EXPECT_EQ(thread.get_id(), Id());
  EXPECT_FALSE(thread.joinable());
  semaphore_releaser.semaphore().acquire();

  WaitUntilDetachedThreadsCleanedUp();
}
}  // namespace
}  // namespace pw::thread
