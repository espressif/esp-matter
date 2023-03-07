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
#include "pw_thread_threadx/util.h"

#include "pw_function/function.h"
#include "pw_status/status.h"
#include "tx_api.h"
#include "tx_thread.h"

namespace pw::thread::threadx {

namespace internal {

// Iterates through all threads that haven't been deleted, calling the provided
// callback.
Status ForEachThread(const TX_THREAD& starting_thread,
                     const ThreadCallback& cb) {
  const TX_THREAD* thread = &starting_thread;
  do {
    if (!cb(*thread)) {
      // Early-terminate iteration if requested by the callback.
      return Status::Aborted();
    }
    thread = thread->tx_thread_created_next;
  } while (thread != &starting_thread);

  return OkStatus();
}

}  // namespace internal

Status ForEachThread(const ThreadCallback& cb) {
  return internal::ForEachThread(*_tx_thread_created_ptr, cb);
}

}  // namespace pw::thread::threadx
