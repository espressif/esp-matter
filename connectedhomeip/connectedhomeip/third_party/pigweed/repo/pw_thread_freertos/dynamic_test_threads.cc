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

const Options& TestOptionsThread0() {
  static constexpr freertos::Options thread_0_options =
      freertos::Options().set_name("pw::TestThread0");
  return thread_0_options;
}

const Options& TestOptionsThread1() {
  static constexpr freertos::Options thread_1_options =
      freertos::Options().set_name("pw::TestThread0");
  return thread_1_options;
}

// Although there's no risk of dynamic context re-use, there is a risk
// running out of heap. The way the FreeRTOS kernel works is that dynamic thread
// allocations are cleaned up during idle. There is no clean way to cleanly
// sleep until idle, ergo we simply sleep for a long period in the hope that
// the application will on average not be able to starve the heap if they
// execute this test over and over again.
void WaitUntilDetachedThreadsCleanedUp() {
  this_thread::sleep_for(
      chrono::SystemClock::for_at_least(std::chrono::milliseconds(50)));
}

}  // namespace pw::thread::test
