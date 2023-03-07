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

namespace pw::thread::stl {

// Unfortunately std::thread:attributes was not accepted into the C++ standard.
// Instead, users are expected to start the thread and after dynamically adjust
// the thread's attributes using std::thread::native_handle based on the native
// threading APIs.
class Options : public thread::Options {
 public:
  constexpr Options() {}
};

}  // namespace pw::thread::stl
