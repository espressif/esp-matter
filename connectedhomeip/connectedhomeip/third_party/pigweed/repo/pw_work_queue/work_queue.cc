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

#include "pw_work_queue/work_queue.h"

#include <mutex>

#include "pw_assert/check.h"

namespace pw::work_queue {

void WorkQueue::RequestStop() {
  std::lock_guard lock(lock_);
  stop_requested_ = true;
  work_notification_.release();
}

void WorkQueue::Run() {
  while (true) {
    work_notification_.acquire();

    // Drain the work queue.
    bool stop_requested;
    bool work_remaining;
    do {
      std::optional<WorkItem> possible_work_item;
      {
        std::lock_guard lock(lock_);
        possible_work_item = circular_buffer_.Pop();
        work_remaining = !circular_buffer_.empty();
        stop_requested = stop_requested_;
      }
      if (!possible_work_item.has_value()) {
        continue;  // No work item to process.
      }
      WorkItem& work_item = possible_work_item.value();
      PW_CHECK(work_item != nullptr);
      work_item();
    } while (work_remaining);

    // Queue was drained, return if we've been requested to stop.
    if (stop_requested) {
      return;
    }
  }
}

void WorkQueue::CheckPushWork(WorkItem&& work_item) {
  PW_CHECK_OK(InternalPushWork(std::move(work_item)),
              "Failed to push work item into the work queue");
}

Status WorkQueue::InternalPushWork(WorkItem&& work_item) {
  std::lock_guard lock(lock_);

  if (stop_requested_) {
    // Entries are not permitted to be enqueued once stop has been requested.
    return Status::FailedPrecondition();
  }

  if (circular_buffer_.full()) {
    return Status::ResourceExhausted();
  }

  circular_buffer_.Push(std::move(work_item));

  // Update the watermarks for the queue.
  const uint32_t queue_entries = circular_buffer_.size();
  if (queue_entries > max_queue_used_.value()) {
    max_queue_used_.Set(queue_entries);
  }
  const uint32_t queue_remaining = circular_buffer_.capacity() - queue_entries;
  if (queue_remaining < min_queue_remaining_.value()) {
    min_queue_remaining_.Set(queue_entries);
  }

  work_notification_.release();
  return OkStatus();
}

}  // namespace pw::work_queue
