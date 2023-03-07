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

#include "pw_system/work_queue.h"

#include "pw_system/config.h"
#include "pw_work_queue/work_queue.h"

namespace pw::system {

// TODO(b/234876895): Consider switching this to a "NoDestroy" wrapped type to
// allow the static destructor to be optimized out.
work_queue::WorkQueue& GetWorkQueue() {
  static constexpr size_t kMaxWorkQueueEntries =
      PW_SYSTEM_WORK_QUEUE_MAX_ENTRIES;
  static pw::work_queue::WorkQueueWithBuffer<kMaxWorkQueueEntries> work_queue;
  return work_queue;
}

}  // namespace pw::system
