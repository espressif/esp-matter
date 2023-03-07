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

#include "gtest/gtest.h"
#include "pw_thread/id.h"
#include "pw_thread/yield.h"

namespace pw::this_thread {
namespace {

extern "C" {

// Function defined in yield_facade_test_c.c which call the API from C.
void pw_this_thread_CallYield();

}  // extern "C"

TEST(Yield, CompilesAndRuns) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  // Unfortunately we have not thought of a useful way to test yield without
  // knowing the backend implementation as things like round robin scheduling
  // may be enabled meaning it may appear like yield is working when it isn't.
  // For now we just ensure it compiles and we can execute it without a crash.
  yield();
}

TEST(Yield, CompilesAndRunsInC) {
  // Ensure we are in a thread context, meaning we are permitted to sleep.
  ASSERT_NE(get_id(), thread::Id());

  // Unfortunately we have not thought of a useful way to test yield without
  // knowing the backend implementation as things like round robin scheduling
  // may be enabled meaning it may appear like yield is working when it isn't.
  // For now we just ensure it compiles and we can execute it without a crash.
  pw_this_thread_CallYield();
}

}  // namespace
}  // namespace pw::this_thread
