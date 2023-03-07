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

#include "RTOS.h"
#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_log/log.h"
#include "pw_thread/sleep.h"
#include "pw_thread_embos/context.h"
#include "pw_thread_embos/options.h"

namespace pw::thread::test {
namespace {

std::array<embos::ContextWithStack<>, 2> thread_contexts;

}  // namespace

const Options& TestOptionsThread0() {
  static constexpr embos::Options thread_0_options =
      embos::Options()
          .set_name("pw::TestThread0")
          .set_context(thread_contexts[0]);
  return thread_0_options;
}

const Options& TestOptionsThread1() {
  static constexpr embos::Options thread_1_options =
      embos::Options()
          .set_name("pw::TestThread1")
          .set_context(thread_contexts[1]);
  return thread_1_options;
}

void WaitUntilDetachedThreadsCleanedUp() {
  // embOS does not permit us to invoke a callback after the TCB has been
  // unregistered from the kernel, however it does provide an API to query this
  // status.
  for (auto& context : thread_contexts) {
    while (OS_IsTask(&context.tcb())) {
      this_thread::sleep_for(
          chrono::SystemClock::for_at_least(std::chrono::milliseconds(1)));
    }
  }
}

}  // namespace pw::thread::test
