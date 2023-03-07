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

// These tests call the pw_chrono module system_clock API from C. The return
// values are checked in the main C++ tests.

#include "pw_chrono/system_clock.h"

pw_chrono_SystemClock_TimePoint pw_chrono_SystemClock_CallNow(void) {
  return pw_chrono_SystemClock_Now();
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_CallTimeElapsed(
    pw_chrono_SystemClock_TimePoint last_time,
    pw_chrono_SystemClock_TimePoint current_time) {
  return pw_chrono_SystemClock_TimeElapsed(last_time, current_time);
}

pw_chrono_SystemClock_Nanoseconds pw_chrono_SystemClock_CallDurationToNsFloor(
    pw_chrono_SystemClock_Duration ticks) {
  return pw_chrono_SystemClock_DurationToNsFloor(ticks);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_100ms(void) {
  return PW_SYSTEM_CLOCK_MS(100);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_10s(void) {
  return PW_SYSTEM_CLOCK_S(10);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_1min(void) {
  return PW_SYSTEM_CLOCK_MIN(1);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_2h(void) {
  return PW_SYSTEM_CLOCK_H(2);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_100msCeil(void) {
  return PW_SYSTEM_CLOCK_MS_CEIL(100);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_10sCeil(void) {
  return PW_SYSTEM_CLOCK_S_CEIL(10);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_1minCeil(void) {
  return PW_SYSTEM_CLOCK_MIN_CEIL(1);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_2hCeil(void) {
  return PW_SYSTEM_CLOCK_H_CEIL(2);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_100msFloor(void) {
  return PW_SYSTEM_CLOCK_MS_FLOOR(100);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_10sFloor(void) {
  return PW_SYSTEM_CLOCK_S_FLOOR(10);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_1minFloor(void) {
  return PW_SYSTEM_CLOCK_MIN_FLOOR(1);
}

pw_chrono_SystemClock_Duration pw_chrono_SystemClock_Macros_2hFloor(void) {
  return PW_SYSTEM_CLOCK_H_FLOOR(2);
}
