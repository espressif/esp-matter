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

#include "pw_thread/test_threads.h"

#include "pw_thread_stl/options.h"

namespace pw::thread::test {

// The STL doesn't offer any options so the default constructed options are used
// directly.

const Options& TestOptionsThread0() {
  static constexpr stl::Options thread_0_options;
  return thread_0_options;
}

const Options& TestOptionsThread1() {
  static constexpr stl::Options thread_1_options;
  return thread_1_options;
}

// Thanks to the dynamic allocation, there's no need to wait here as there's
// no re-use of contexts. In addition we have a very large heap so we expect
// the risk to be minimal to non-existent for heap exhaustion to occur if this
// test is run back to back.
void WaitUntilDetachedThreadsCleanedUp() {}

}  // namespace pw::thread::test
