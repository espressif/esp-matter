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
#include "pw_multisink/multisink.h"

#include <cstring>

#include "pw_assert/check.h"
#include "pw_bytes/span.h"
#include "pw_function/function.h"
#include "pw_log/log.h"
#include "pw_result/result.h"
#include "pw_status/status.h"
#include "pw_status/try.h"
#include "pw_varint/varint.h"

namespace pw {
namespace multisink {

void MultiSink::HandleEntry(ConstByteSpan entry) {
  std::lock_guard lock(lock_);
  const Status push_back_status = ring_buffer_.PushBack(entry, sequence_id_++);
  PW_DCHECK_OK(push_back_status);
  NotifyListeners();
}

void MultiSink::HandleDropped(uint32_t drop_count) {
  std::lock_guard lock(lock_);
  // Updating the sequence ID helps identify where the ingress drop happend when
  // a drain peeks or pops.
  sequence_id_ += drop_count;
  total_ingress_drops_ += drop_count;
  NotifyListeners();
}

Status MultiSink::PopEntry(Drain& drain, const Drain::PeekedEntry& entry) {
  std::lock_guard lock(lock_);
  PW_DCHECK_PTR_EQ(drain.multisink_, this);

  // Ignore the call if the entry has been handled already.
  if (entry.sequence_id() == drain.last_handled_sequence_id_) {
    return OkStatus();
  }

  uint32_t next_entry_sequence_id;
  Status peek_status = drain.reader_.PeekFrontPreamble(next_entry_sequence_id);
  if (!peek_status.ok()) {
    // Ignore errors if the multisink is empty.
    if (peek_status.IsOutOfRange()) {
      return OkStatus();
    }
    return peek_status;
  }
  if (next_entry_sequence_id == entry.sequence_id()) {
    // A crash should not happen, since the peek was successful and `lock_` is
    // still held, there shouldn't be any modifications to the multisink in
    // between peeking and popping.
    PW_CHECK_OK(drain.reader_.PopFront());
  }
  // If the entry's sequence id is not the next one it means that the
  // multisink advanced since PeekEntry() was called. Advance the last handled
  // sequence id to the passed entry anyway to mark the fact that the dropped
  // messages reported on PeekEntry() are handled.
  drain.last_handled_sequence_id_ = entry.sequence_id();
  return OkStatus();
}

Result<ConstByteSpan> MultiSink::PeekOrPopEntry(
    Drain& drain,
    ByteSpan buffer,
    Request request,
    uint32_t& drain_drop_count_out,
    uint32_t& ingress_drop_count_out,
    uint32_t& entry_sequence_id_out) {
  size_t bytes_read = 0;
  entry_sequence_id_out = 0;
  drain_drop_count_out = 0;
  ingress_drop_count_out = 0;

  std::lock_guard lock(lock_);
  PW_DCHECK_PTR_EQ(drain.multisink_, this);

  const Status peek_status = drain.reader_.PeekFrontWithPreamble(
      buffer, entry_sequence_id_out, bytes_read);

  if (peek_status.IsOutOfRange()) {
    // If the drain has caught up, report the last handled sequence ID so that
    // it can still process any dropped entries.
    entry_sequence_id_out = sequence_id_ - 1;
  } else if (!peek_status.ok()) {
    // Discard the entry if the result isn't OK or OUT_OF_RANGE and exit, as the
    // entry_sequence_id_out cannot be used for computation. Later invocations
    // will calculate the drop count.
    PW_CHECK(drain.reader_.PopFront().ok());
    return peek_status;
  }

  // Compute the drop count delta by comparing this entry's sequence ID with the
  // last sequence ID this drain successfully read.
  //
  // The drop count calculation simply computes the difference between the
  // current and last sequence IDs. Consecutive successful reads will always
  // differ by one at least, so it is subtracted out. If the read was not
  // successful, the difference is not adjusted.
  drain_drop_count_out = entry_sequence_id_out -
                         drain.last_handled_sequence_id_ -
                         (peek_status.ok() ? 1 : 0);

  // Only report the ingress drop count when the drain catches up to where the
  // drop happened, accounting only for the drops found and no more, as
  // indicated by the gap in sequence IDs.
  if (drain_drop_count_out > 0) {
    ingress_drop_count_out =
        std::min(drain_drop_count_out,
                 total_ingress_drops_ - drain.last_handled_ingress_drop_count_);
    // Remove the ingress drop count duplicated in drain_drop_count_out.
    drain_drop_count_out -= ingress_drop_count_out;
    // Check if all the ingress drops were reported.
    drain.last_handled_ingress_drop_count_ =
        total_ingress_drops_ > ingress_drop_count_out
            ? total_ingress_drops_ - ingress_drop_count_out
            : total_ingress_drops_;
  }

  // The Peek above may have failed due to OutOfRange, now that we've set the
  // drop count see if we should return before attempting to pop.
  if (peek_status.IsOutOfRange()) {
    // No more entries, update the drain.
    drain.last_handled_sequence_id_ = entry_sequence_id_out;
    return peek_status;
  }
  if (request == Request::kPop) {
    PW_CHECK(drain.reader_.PopFront().ok());
    drain.last_handled_sequence_id_ = entry_sequence_id_out;
  }
  return as_bytes(buffer.first(bytes_read));
}

void MultiSink::AttachDrain(Drain& drain) {
  std::lock_guard lock(lock_);
  PW_DCHECK_PTR_EQ(drain.multisink_, nullptr);
  drain.multisink_ = this;

  PW_CHECK_OK(ring_buffer_.AttachReader(drain.reader_));
  if (&drain == &oldest_entry_drain_) {
    drain.last_handled_sequence_id_ = sequence_id_ - 1;
  } else {
    drain.last_handled_sequence_id_ =
        oldest_entry_drain_.last_handled_sequence_id_;
  }
  drain.last_peek_sequence_id_ = drain.last_handled_sequence_id_;
  drain.last_handled_ingress_drop_count_ = 0;
}

void MultiSink::DetachDrain(Drain& drain) {
  std::lock_guard lock(lock_);
  PW_DCHECK_PTR_EQ(drain.multisink_, this);
  drain.multisink_ = nullptr;
  PW_CHECK_OK(ring_buffer_.DetachReader(drain.reader_),
              "The drain wasn't already attached.");
}

void MultiSink::AttachListener(Listener& listener) {
  std::lock_guard lock(lock_);
  listeners_.push_back(listener);
  // Notify the newly added entry, in case there are items in the sink.
  listener.OnNewEntryAvailable();
}

void MultiSink::DetachListener(Listener& listener) {
  std::lock_guard lock(lock_);
  [[maybe_unused]] bool was_detached = listeners_.remove(listener);
  PW_DCHECK(was_detached, "The listener was already attached.");
}

void MultiSink::Clear() {
  std::lock_guard lock(lock_);
  ring_buffer_.Clear();
}

void MultiSink::NotifyListeners() {
  for (auto& listener : listeners_) {
    listener.OnNewEntryAvailable();
  }
}

Status MultiSink::UnsafeForEachEntry(
    const Function<void(ConstByteSpan)>& callback, size_t max_num_entries) {
  MultiSink::UnsafeIterationWrapper multisink_iteration = UnsafeIteration();

  // First count the number of entries.
  size_t num_entries = 0;
  for ([[maybe_unused]] ConstByteSpan entry : multisink_iteration) {
    num_entries++;
  }

  // Log up to the max number of logs to avoid overflowing the crash log
  // writer.
  const size_t first_logged_offset =
      max_num_entries > num_entries ? 0 : num_entries - max_num_entries;
  pw::multisink::MultiSink::iterator it = multisink_iteration.begin();
  for (size_t offset = 0; it != multisink_iteration.end(); ++it, ++offset) {
    if (offset < first_logged_offset) {
      continue;  // Skip this log.
    }
    callback(*it);
  }
  if (!it.status().ok()) {
    PW_LOG_WARN("Multisink corruption detected, some entries may be missing");
    return Status::DataLoss();
  }

  return OkStatus();
}

Status MultiSink::Drain::PopEntry(const PeekedEntry& entry) {
  PW_DCHECK_NOTNULL(multisink_);
  return multisink_->PopEntry(*this, entry);
}

Result<MultiSink::Drain::PeekedEntry> MultiSink::Drain::PeekEntry(
    ByteSpan buffer,
    uint32_t& drain_drop_count_out,
    uint32_t& ingress_drop_count_out) {
  PW_DCHECK_NOTNULL(multisink_);
  uint32_t entry_sequence_id_out;
  Result<ConstByteSpan> peek_result =
      multisink_->PeekOrPopEntry(*this,
                                 buffer,
                                 Request::kPeek,
                                 drain_drop_count_out,
                                 ingress_drop_count_out,
                                 entry_sequence_id_out);
  if (!peek_result.ok()) {
    return peek_result.status();
  }
  return PeekedEntry(peek_result.value(), entry_sequence_id_out);
}

Result<ConstByteSpan> MultiSink::Drain::PopEntry(
    ByteSpan buffer,
    uint32_t& drain_drop_count_out,
    uint32_t& ingress_drop_count_out) {
  PW_DCHECK_NOTNULL(multisink_);
  uint32_t entry_sequence_id_out;
  return multisink_->PeekOrPopEntry(*this,
                                    buffer,
                                    Request::kPop,
                                    drain_drop_count_out,
                                    ingress_drop_count_out,
                                    entry_sequence_id_out);
}

}  // namespace multisink
}  // namespace pw
