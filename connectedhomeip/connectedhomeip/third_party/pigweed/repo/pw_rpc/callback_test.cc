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

#include <atomic>

#include "gtest/gtest.h"
#include "pw_rpc/raw/client_testing.h"
#include "pw_rpc_test_protos/test.raw_rpc.pb.h"
#include "pw_sync/binary_semaphore.h"
#include "pw_thread/sleep.h"
#include "pw_thread/test_threads.h"
#include "pw_thread/thread.h"

namespace pw::rpc {
namespace {

using namespace std::chrono_literals;

using test::pw_rpc::raw::TestService;

TEST(Callbacks, DISABLED_DanglingReference) {
  struct Context {
    RawClientTestContext<> test;
    sync::BinarySemaphore callback_thread;
    sync::BinarySemaphore main_thread;
    std::atomic<bool> call_is_in_scope = true;
    std::atomic<bool> callback_executed = false;
  } ctx;

  thread::Thread callback_thread(
      thread::test::TestOptionsThread0(),
      [](void* arg) {
        Context& context = *static_cast<Context*>(arg);
        // Once the main thread tells this thread to go, invoke the callback.
        context.callback_thread.acquire();
        context.test.server().SendResponse<TestService::TestUnaryRpc>(
            {}, Status::InvalidArgument());
      },
      &ctx);

  {
    RawUnaryReceiver call = TestService::TestUnaryRpc(
        ctx.test.client(), ctx.test.channel().id(), {});

    ctx.call_is_in_scope = true;

    call.set_on_completed([&ctx](ConstByteSpan, Status) {
      ctx.main_thread.release();

      // Wait for a while so the main thread tries to destroy the call.
      // Yes, this is a race, but this code is testing a race condition. This
      // sleep should be plenty long enough for the main thread to run.
      this_thread::sleep_for(200ms);

      // Now, make sure the call is still in scope. The main thread should
      // block in the call's destructor until this callback completes.
      EXPECT_TRUE(ctx.call_is_in_scope);

      ctx.callback_executed = true;
    });

    // Start the callback thread so it can invoke the callback.
    ctx.callback_thread.release();

    // Wait until the callback thread starts.
    ctx.main_thread.acquire();
  }

  // The callback thread will sleep for a bit. Meanwhile, mark the call as out
  // of scope, then let it go out of scope.
  ctx.call_is_in_scope = false;

  // Wait for the callback thread to finish.
  callback_thread.join();

  EXPECT_TRUE(ctx.callback_executed.load());
}

}  // namespace
}  // namespace pw::rpc
