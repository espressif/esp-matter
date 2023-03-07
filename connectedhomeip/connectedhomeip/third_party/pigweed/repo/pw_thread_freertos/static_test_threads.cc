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

#include "pw_chrono/system_clock.h"
#include "pw_thread/sleep.h"
#include "pw_thread/test_threads.h"
#include "pw_thread_freertos/context.h"
#include "pw_thread_freertos/options.h"

namespace pw::thread::test {
namespace {
freertos::StaticContextWithStack thread_0_context;
freertos::StaticContextWithStack thread_1_context;
}  // namespace

const Options& TestOptionsThread0() {
  static constexpr freertos::Options thread_0_options =
      freertos::Options()
          .set_name("pw::TestThread0")
          .set_static_context(thread_0_context);
  return thread_0_options;
}

const Options& TestOptionsThread1() {
  static constexpr freertos::Options thread_1_options =
      freertos::Options()
          .set_name("pw::TestThread1")
          .set_static_context(thread_1_context);
  return thread_1_options;
}

void WaitUntilDetachedThreadsCleanedUp() {
  // One may be tempted to use the context's task_handle to wait until it's a
  // nullptr. However, there's still a race condition that the task has not
  // finished the execution of vTaskDelete. In addition during this time the
  // the task_handle has been cleared meaning we cannot call vTaskDelete.
  this_thread::sleep_for(
      chrono::SystemClock::for_at_least(std::chrono::milliseconds(50)));
}

}  // namespace pw::thread::test
