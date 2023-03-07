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
#pragma once

#include "pw_thread/thread.h"

namespace pw::thread::test {

// Two test threads are used to verify the thread facade.
const Options& TestOptionsThread0();
const Options& TestOptionsThread1();

// Unfortunately the thread facade test's job is also to test detached threads
// which may be backed by static contexts or dynamic context heap allocations.
// In literally every other case you would use join for this, however that is
// not an option here as detached thread functionality is being tested.
// For this reason a backend specific cleanup API is provided which shall block
// until all the test threads above have finished executions and are ready for
// potential re-use and/or freed any dynamic allocations.
//
// Precondition: The threads must have started to execute before calling this
// if cleanup is expected.
void WaitUntilDetachedThreadsCleanedUp();

}  // namespace pw::thread::test
