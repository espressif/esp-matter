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

#include <algorithm>
#include <thread>

#include "pw_chrono/system_clock.h"
#include "pw_thread/sleep.h"

namespace pw::this_thread {

inline void sleep_for(chrono::SystemClock::duration sleep_duration) {
  sleep_duration =
      std::max(sleep_duration, chrono::SystemClock::duration::zero());
  // Although many implementations do yield with sleep_for(0), it is not
  // required, ergo we explicitly add handling.
  if (sleep_duration == chrono::SystemClock::duration::zero()) {
    return std::this_thread::yield();
  }
#if defined(_WIN32)
  // For some reason MinGW's implementation for sleep_for doesn't work
  // correctly, however sleep_until does.
  return std::this_thread::sleep_until(
      chrono::SystemClock::TimePointAfterAtLeast(sleep_duration));
#else
  return std::this_thread::sleep_for(sleep_duration);
#endif  // defined(_WIN32)
}

inline void sleep_until(chrono::SystemClock::time_point wakeup_time) {
  // Although many implementations do yield with deadlines in the past until
  // the current time, it is not required, ergo we explicitly add handling.
  if (chrono::SystemClock::now() >= wakeup_time) {
    return std::this_thread::yield();
  }
  return std::this_thread::sleep_until(wakeup_time);
}

}  // namespace pw::this_thread
