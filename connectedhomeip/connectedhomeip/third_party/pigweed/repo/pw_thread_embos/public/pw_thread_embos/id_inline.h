// Copyright 2021 The Pigweed Authors
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

#include "RTOS.h"
#include "pw_assert/assert.h"
#include "pw_interrupt/context.h"
#include "pw_thread/id.h"

namespace pw::this_thread {

inline thread::Id get_id() noexcept {
  // Ensure this is not being called by an interrupt.
  PW_DASSERT(!interrupt::InInterruptContext());

  // Ensure the kernel is running.
  PW_DASSERT(OS_IsRunning() != 0);

  return thread::Id(OS_GetTaskID());
}

}  // namespace pw::this_thread
