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

#include "pw_log_rpc/rpc_log_drain.h"

#include <limits>
#include <mutex>
#include <optional>
#include <string_view>

#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_log/proto/log.pwpb.h"
#include "pw_result/result.h"
#include "pw_rpc/raw/server_reader_writer.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/try.h"

namespace pw::log_rpc {
namespace {

// Creates an encoded drop message on the provided buffer and adds it to the
// bulk log entries. Resets the drop count when successfull.
void TryEncodeDropMessage(
    ByteSpan encoded_drop_message_buffer,
    std::string_view reason,
    uint32_t& drop_count,
    log::pwpb::LogEntries::MemoryEncoder& entries_encoder) {
  // Encode drop count and reason, if any, in log proto.
  log::pwpb::LogEntry::MemoryEncoder encoder(encoded_drop_message_buffer);
  if (!reason.empty()) {
    encoder.WriteMessage(as_bytes(span<const char>(reason))).IgnoreError();
  }
  encoder.WriteDropped(drop_count).IgnoreError();
  if (!encoder.status().ok()) {
    return;
  }
  // Add encoded drop messsage if fits in buffer.
  ConstByteSpan drop_message(encoder);
  if (drop_message.size() + RpcLogDrain::kLogEntriesEncodeFrameSize <
      entries_encoder.ConservativeWriteLimit()) {
    PW_CHECK_OK(entries_encoder.WriteBytes(
        static_cast<uint32_t>(log::pwpb::LogEntries::Fields::ENTRIES),
        drop_message));
    drop_count = 0;
  }
}

}  // namespace

Status RpcLogDrain::Open(rpc::RawServerWriter& writer) {
  if (!writer.active()) {
    return Status::FailedPrecondition();
  }
  std::lock_guard lock(mutex_);
  if (server_writer_.active()) {
    return Status::AlreadyExists();
  }
  server_writer_ = std::move(writer);
  if (on_open_callback_ != nullptr) {
    on_open_callback_();
  }
  return OkStatus();
}

Status RpcLogDrain::Flush(ByteSpan encoding_buffer) {
  Status status;
  SendLogs(std::numeric_limits<size_t>::max(), encoding_buffer, status);
  return status;
}

std::optional<chrono::SystemClock::duration> RpcLogDrain::Trickle(
    ByteSpan encoding_buffer) {
  chrono::SystemClock::time_point now = chrono::SystemClock::now();
  // Called before drain is ready to send more logs. Ignore this request and
  // remind the caller how much longer they'll need to wait.
  if (no_writes_until_ > now) {
    return no_writes_until_ - now;
  }

  Status encoding_status;
  if (SendLogs(max_bundles_per_trickle_, encoding_buffer, encoding_status) ==
      LogDrainState::kCaughtUp) {
    return std::nullopt;
  }

  no_writes_until_ = chrono::SystemClock::TimePointAfterAtLeast(trickle_delay_);
  return trickle_delay_;
}

RpcLogDrain::LogDrainState RpcLogDrain::SendLogs(size_t max_num_bundles,
                                                 ByteSpan encoding_buffer,
                                                 Status& encoding_status_out) {
  PW_CHECK_NOTNULL(multisink_);

  LogDrainState log_sink_state = LogDrainState::kMoreEntriesRemaining;
  std::lock_guard lock(mutex_);
  size_t sent_bundle_count = 0;
  while (sent_bundle_count < max_num_bundles &&
         log_sink_state != LogDrainState::kCaughtUp) {
    if (!server_writer_.active()) {
      encoding_status_out = Status::Unavailable();
      // No reason to keep polling this drain until the writer is opened.
      return LogDrainState::kCaughtUp;
    }
    log::pwpb::LogEntries::MemoryEncoder encoder(encoding_buffer);
    uint32_t packed_entry_count = 0;
    log_sink_state = EncodeOutgoingPacket(encoder, packed_entry_count);

    // Avoid sending empty packets.
    if (encoder.size() == 0) {
      continue;
    }

    encoder.WriteFirstEntrySequenceId(sequence_id_)
        .IgnoreError();  // TODO(b/242598609): Handle Status properly
    sequence_id_ += packed_entry_count;
    const Status status = server_writer_.Write(encoder);
    sent_bundle_count++;

    if (!status.ok() &&
        error_handling_ == LogDrainErrorHandling::kCloseStreamOnWriterError) {
      // Only update this drop count when writer errors are not ignored.
      drop_count_writer_error_ += packed_entry_count;
      server_writer_.Finish().IgnoreError();
      encoding_status_out = Status::Aborted();
      return log_sink_state;
    }
  }
  return log_sink_state;
}

RpcLogDrain::LogDrainState RpcLogDrain::EncodeOutgoingPacket(
    log::pwpb::LogEntries::MemoryEncoder& encoder,
    uint32_t& packed_entry_count_out) {
  const size_t total_buffer_size = encoder.ConservativeWriteLimit();
  do {
    // Peek entry and get drop count from multisink.
    uint32_t drop_count = 0;
    uint32_t ingress_drop_count = 0;
    Result<multisink::MultiSink::Drain::PeekedEntry> possible_entry =
        PeekEntry(log_entry_buffer_, drop_count, ingress_drop_count);
    drop_count_ingress_error_ += ingress_drop_count;

    // Check if the entry fits in the entry buffer.
    if (possible_entry.status().IsResourceExhausted()) {
      ++drop_count_small_stack_buffer_;
      continue;
    }

    // Check if there are any entries left.
    if (possible_entry.status().IsOutOfRange()) {
      // Stash multisink's reported drop count that will be reported later with
      // any other drop counts.
      drop_count_slow_drain_ += drop_count;
      return LogDrainState::kCaughtUp;  // There are no more entries.
    }

    // At this point all expected errors have been handled.
    PW_CHECK_OK(possible_entry.status());

    // Check if the entry passes any set filter rules.
    if (filter_ != nullptr &&
        filter_->ShouldDropLog(possible_entry.value().entry())) {
      // Add the drop count from the multisink peek, stored in `drop_count`, to
      // the total drop count. Then drop the entry without counting it towards
      // the total drop count. Drops will be reported later all together.
      drop_count_slow_drain_ += drop_count;
      PW_CHECK_OK(PopEntry(possible_entry.value()));
      continue;
    }

    // Check if the entry fits in the encoder buffer by itself.
    const size_t encoded_entry_size =
        possible_entry.value().entry().size() + kLogEntriesEncodeFrameSize;
    if (encoded_entry_size + kLogEntriesEncodeFrameSize > total_buffer_size) {
      // Entry is larger than the entire available buffer.
      ++drop_count_small_outbound_buffer_;
      PW_CHECK_OK(PopEntry(possible_entry.value()));
      continue;
    }

    // At this point, we have a valid entry that may fit in the encode buffer.
    // Report any drop counts combined reusing the log_entry_buffer_ to encode a
    // drop message.
    drop_count_slow_drain_ += drop_count;
    // Account for dropped entries too large for stack buffer, which PeekEntry()
    // also reports.
    drop_count_slow_drain_ -= drop_count_small_stack_buffer_;
    bool log_entry_buffer_has_valid_entry = possible_entry.ok();
    if (drop_count_slow_drain_ > 0) {
      TryEncodeDropMessage(log_entry_buffer_,
                           std::string_view(kSlowDrainErrorMessage),
                           drop_count_slow_drain_,
                           encoder);
      log_entry_buffer_has_valid_entry = false;
    }
    if (drop_count_ingress_error_ > 0) {
      TryEncodeDropMessage(log_entry_buffer_,
                           std::string_view(kIngressErrorMessage),
                           drop_count_ingress_error_,
                           encoder);
      log_entry_buffer_has_valid_entry = false;
    }
    if (drop_count_small_stack_buffer_ > 0) {
      TryEncodeDropMessage(log_entry_buffer_,
                           std::string_view(kSmallStackBufferErrorMessage),
                           drop_count_small_stack_buffer_,
                           encoder);
      log_entry_buffer_has_valid_entry = false;
    }
    if (drop_count_small_outbound_buffer_ > 0) {
      TryEncodeDropMessage(log_entry_buffer_,
                           std::string_view(kSmallOutboundBufferErrorMessage),
                           drop_count_small_outbound_buffer_,
                           encoder);
      log_entry_buffer_has_valid_entry = false;
    }
    if (drop_count_writer_error_ > 0) {
      TryEncodeDropMessage(log_entry_buffer_,
                           std::string_view(kWriterErrorMessage),
                           drop_count_writer_error_,
                           encoder);
      log_entry_buffer_has_valid_entry = false;
    }
    if (possible_entry.ok() && !log_entry_buffer_has_valid_entry) {
      PW_CHECK_OK(PeekEntry(log_entry_buffer_, drop_count, ingress_drop_count)
                      .status());
    }

    // Check if the entry fits in the partially filled encoder buffer.
    if (encoded_entry_size > encoder.ConservativeWriteLimit()) {
      // Notify the caller there are more entries to send.
      return LogDrainState::kMoreEntriesRemaining;
    }

    // Encode the entry and remove it from multisink.
    PW_CHECK_OK(encoder.WriteBytes(
        static_cast<uint32_t>(log::pwpb::LogEntries::Fields::ENTRIES),
        possible_entry.value().entry()));
    PW_CHECK_OK(PopEntry(possible_entry.value()));
    ++packed_entry_count_out;
  } while (true);
}

Status RpcLogDrain::Close() {
  std::lock_guard lock(mutex_);
  return server_writer_.Finish();
}

}  // namespace pw::log_rpc
