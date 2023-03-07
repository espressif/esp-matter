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

#include "pw_thread/test_threads.h"

#include <chrono>

#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_log/log.h"
#include "pw_thread/sleep.h"
#include "pw_thread_threadx/context.h"
#include "pw_thread_threadx/options.h"

namespace pw::thread::test {
namespace {

std::array<threadx::ContextWithStack<>, 2> thread_contexts;

}  // namespace

const Options& TestOptionsThread0() {
  static constexpr threadx::Options thread_0_options =
      threadx::Options()
          .set_name("pw::TestThread0")
          .set_context(thread_contexts[0]);
  return thread_0_options;
}

const Options& TestOptionsThread1() {
  static constexpr threadx::Options thread_1_options =
      threadx::Options()
          .set_name("pw::TestThread1")
          .set_context(thread_contexts[1]);
  return thread_1_options;
}

void WaitUntilDetachedThreadsCleanedUp() {
  // ThreadX does not permit the running thread to delete itself, which means
  // we have to do this to re-use a TCB as otherwise we will be leaking stale
  // references in the kernel.
  for (auto& context : thread_contexts) {
    if (context.tcb().tx_thread_id != TX_THREAD_ID) {
      // The TCB was either not used or was already deleted. Note that
      // tx_thread_terminate does NOT clear this state by design.
      continue;
    }

    // If the thread was created but has not been deleted, it means that the
    // thread was detached before it finished. Wait until it is completed.
    while (context.tcb().tx_thread_state != TX_COMPLETED) {
      pw::this_thread::sleep_for(
          chrono::SystemClock::for_at_least(std::chrono::milliseconds(1)));
    }

    const UINT result = tx_thread_delete(&context.tcb());
    PW_CHECK_UINT_EQ(TX_SUCCESS, result, "Failed to delete thread");
  }
}

}  // namespace pw::thread::test
