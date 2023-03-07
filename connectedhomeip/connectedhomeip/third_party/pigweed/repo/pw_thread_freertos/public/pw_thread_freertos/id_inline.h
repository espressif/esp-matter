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

#include "FreeRTOS.h"
#include "pw_assert/assert.h"
#include "pw_interrupt/context.h"
#include "pw_thread/id.h"
#include "task.h"

namespace pw::this_thread {

inline thread::Id get_id() noexcept {
  // Ensure this is not being called by an interrupt.
  PW_DASSERT(!interrupt::InInterruptContext());

#if INCLUDE_xTaskGetSchedulerState == 1 or configUSE_TIMERS == 1
  // Ensure the kernel is running.
  PW_DASSERT(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED);
#endif  // xTaskGetSchedulerState available.

  return thread::Id(xTaskGetCurrentTaskHandle());
}

}  // namespace pw::this_thread
