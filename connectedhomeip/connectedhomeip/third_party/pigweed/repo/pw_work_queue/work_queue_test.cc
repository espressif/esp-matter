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

#include "pw_work_queue/work_queue.h"

#include "gtest/gtest.h"
#include "pw_function/function.h"
#include "pw_log/log.h"
#include "pw_sync/thread_notification.h"
#include "pw_thread/thread.h"
#include "pw_work_queue/test_thread.h"

namespace pw::work_queue {
namespace {

TEST(WorkQueue, PingPongOneRequestType) {
  struct {
    int counter = 0;
    sync::ThreadNotification worker_ping;
  } context;

  WorkQueueWithBuffer<10> work_queue;

  // Start the worker thread.
  thread::Thread work_thread(test::WorkQueueThreadOptions(), work_queue);

  // Pick a number bigger than the circular buffer to ensure we loop around.
  const int kPingPongs = 300;

  for (int i = 0; i < kPingPongs; ++i) {
    // Ping: throw work at the queue that will increment our counter.
    EXPECT_EQ(OkStatus(), work_queue.PushWork([&context] {
      context.counter++;
      PW_LOG_INFO("Send pong...");
      context.worker_ping.release();
    }));

    // Throw a distraction in the queue.
    EXPECT_EQ(OkStatus(), work_queue.PushWork([] {
      PW_LOG_INFO("I'm a random task in the work queue; nothing to see here!");
    }));

    // Pong: wait for the callback to notify us from the worker thread.
    context.worker_ping.acquire();
  }

  // Wait for the worker thread to terminate.
  work_queue.RequestStop();
  work_thread.join();

  EXPECT_EQ(context.counter, kPingPongs);
}

TEST(WorkQueue, PingPongTwoRequestTypesWithExtraRequests) {
  struct {
    int counter = 0;
    sync::ThreadNotification worker_ping;
  } context_a, context_b;

  WorkQueueWithBuffer<10> work_queue;

  // Start the worker thread.
  thread::Thread work_thread(test::WorkQueueThreadOptions(), work_queue);

  // Pick a number bigger than the circular buffer to ensure we loop around.
  const int kPingPongs = 300;

  // Run a bunch of work items in the queue.
  for (int i = 0; i < kPingPongs; ++i) {
    // Other requests...
    EXPECT_EQ(OkStatus(),
              work_queue.PushWork([] { PW_LOG_INFO("Chopping onions"); }));

    // Ping A: throw work at the queue that will increment our counter.
    EXPECT_EQ(OkStatus(), work_queue.PushWork([&context_a] {
      context_a.counter++;
      context_a.worker_ping.release();
    }));

    // Other requests...
    EXPECT_EQ(OkStatus(),
              work_queue.PushWork([] { PW_LOG_INFO("Dicing carrots"); }));
    EXPECT_EQ(OkStatus(),
              work_queue.PushWork([] { PW_LOG_INFO("Blanching spinach"); }));

    // Ping B: throw work at the queue that will increment our counter.
    EXPECT_EQ(OkStatus(), work_queue.PushWork([&context_b] {
      context_b.counter++;
      context_b.worker_ping.release();
    }));

    // Other requests...
    EXPECT_EQ(OkStatus(),
              work_queue.PushWork([] { PW_LOG_INFO("Peeling potatoes"); }));

    // Pong A & B: wait for the callbacks to notify us from the worker thread.
    context_a.worker_ping.acquire();
    context_b.worker_ping.acquire();
  }

  // Wait for the worker thread to terminate.
  work_queue.RequestStop();
  work_thread.join();

  EXPECT_EQ(context_a.counter, kPingPongs);
  EXPECT_EQ(context_b.counter, kPingPongs);
}

// TODO(ewout): Add unit tests for the metrics once they have been restructured.

}  // namespace
}  // namespace pw::work_queue
