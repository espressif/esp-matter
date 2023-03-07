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

#include "pw_preprocessor/util.h"

#ifdef __cplusplus

namespace pw::this_thread {

// Provides a hint to the implementation to reschedule the execution of threads,
// allowing other threads to run.
//
// The exact behavior of this function depends on the implementation, in
// particular on the mechanics of the OS scheduler in use and the state of the
// system.
//
// Precondition: This can only be called from a thread, meaning the scheduler
// must be running.
void yield() noexcept;

}  // namespace pw::this_thread

// The backend can opt to include an inline implementation.
#if __has_include("pw_thread_backend/yield_inline.h")
#include "pw_thread_backend/yield_inline.h"
#endif  // __has_include("pw_thread_backend/yield_inline.h")

#endif  // __cplusplus

PW_EXTERN_C_START

void pw_this_thread_Yield(void);

PW_EXTERN_C_END
