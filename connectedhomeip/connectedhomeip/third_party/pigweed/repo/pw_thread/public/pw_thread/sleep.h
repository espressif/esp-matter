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

#include "pw_chrono/system_clock.h"
#include "pw_preprocessor/util.h"

#ifdef __cplusplus

namespace pw::this_thread {

// Blocks the execution of the current thread for at least the specified
// duration. This function may block for longer due to scheduling or resource
// contention delays.
//
// A sleep duration of 0 will at minimum yield, meaning it will provide a hint
// to the implementation to reschedule the execution of threads, allowing other
// threads to run.
//
// Precondition: This can only be called from a thread, meaning the scheduler
// is running.
void sleep_for(chrono::SystemClock::duration sleep_duration);

// Blocks the execution of the current thread until at least the specified
// time has been reached. This function may block for longer due to scheduling
// or resource contention delays.
//
// A sleep deadline in the past up to the current time will at minimum yield
// meaning it will provide a hint to the implementation to reschedule the
// execution of threads, allowing other threads to run.
//
// Precondition: This can only be called from a thread, meaning the scheduler
// is running.
void sleep_until(chrono::SystemClock::time_point wakeup_time);

}  // namespace pw::this_thread

// The backend can opt to include inlined implementations.
#if __has_include("pw_thread_backend/sleep_inline.h")
#include "pw_thread_backend/sleep_inline.h"
#endif  // __has_include("pw_thread_backend/sleep_inline.h")

#endif  // __cplusplus

PW_EXTERN_C_START

void pw_this_thread_SleepFor(pw_chrono_SystemClock_Duration sleep_duration);
void pw_this_thread_SleepUntil(pw_chrono_SystemClock_TimePoint wakeup_time);

PW_EXTERN_C_END
