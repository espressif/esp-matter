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

#include "pw_assert/assert.h"
#include "pw_thread/id.h"
#include "tx_api.h"
// Prior to ThreadX 6.1, this contained TX_THREAD_GET_SYSTEM_STATE().
#include "tx_thread.h"

namespace pw::this_thread {

inline thread::Id get_id() noexcept {
  // When this value is 0, a thread is executing or the system is idle.
  // Other values indicate that interrupt or initialization processing is
  // active.
  PW_DASSERT(TX_THREAD_GET_SYSTEM_STATE() == 0);

  return thread::Id(tx_thread_identify());
}

}  // namespace pw::this_thread
