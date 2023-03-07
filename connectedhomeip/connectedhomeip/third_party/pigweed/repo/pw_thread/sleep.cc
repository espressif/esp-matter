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

#include "pw_thread/sleep.h"

using pw::chrono::SystemClock;

extern "C" void pw_this_thread_SleepFor(
    pw_chrono_SystemClock_Duration sleep_duration) {
  pw::this_thread::sleep_for(SystemClock::duration(sleep_duration.ticks));
}

extern "C" void pw_this_thread_SleepUntil(
    pw_chrono_SystemClock_TimePoint wakeup_time) {
  pw::this_thread::sleep_until(SystemClock::time_point(
      SystemClock::duration(wakeup_time.duration_since_epoch.ticks)));
}
