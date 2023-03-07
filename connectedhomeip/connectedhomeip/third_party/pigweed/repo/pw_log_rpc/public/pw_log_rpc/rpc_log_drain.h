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

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <optional>
#include <string_view>

#include "pw_assert/assert.h"
#include "pw_bytes/span.h"
#include "pw_chrono/system_clock.h"
#include "pw_function/function.h"
#include "pw_log/proto/log.pwpb.h"
#include "pw_log_rpc/internal/config.h"
#include "pw_log_rpc/log_filter.h"
#include "pw_multisink/multisink.h"
#include "pw_protobuf/serialized_size.h"
#include "pw_result/result.h"
#include "pw_rpc/raw/server_reader_writer.h"
#include "pw_status/status.h"
#include "pw_sync/lock_annotations.h"
#include "pw_sync/mutex.h"

namespace pw::log_rpc {

// RpcLogDrain matches a MultiSink::Drain with with an RPC channel's writer. A
// RPC channel ID identifies this drain. The user must attach this drain
// to a MultiSink that returns a log::pwpb::LogEntry, and provide a buffer large
// enough to hold the largest log::pwpb::LogEntry transmittable. The user must
// call Flush(), which, on every call, packs as many log::pwpb::LogEntry items
// as possible into a log::pwpb::LogEntries message, writes the message to the
// provided writer, then repeats the process until there are no more entries in
// the MultiSink or the writer failed to write the outgoing package and
// error_handling is set to `kCloseStreamOnWriterError`. When error_handling is
// `kIgnoreWriterErrors` the drain will continue to retrieve log entries out of
// the MultiSink and attempt to send them out ignoring the writer errors without
// sending a drop count.
// Note: the error handling and drop count reporting might change in the future.
// Log filtering is done using the rules of the Filter provided if any.
class RpcLogDrain : public multisink::MultiSink::Drain {
 public:
  // Dictates how to handle server writer errors.
  enum class LogDrainErrorHandling {
    kIgnoreWriterErrors,
    kCloseStreamOnWriterError,
  };

  // The minimum buffer size, without the message payload or module sizes,
  // needed to retrieve a log::pwpb::LogEntry from the attached MultiSink. The
  // user must account for the max message size to avoid log entry drops. The
  // dropped field is not accounted since a dropped message has all other fields
  // unset.
  static constexpr size_t kMinEntrySizeWithoutPayload =
      protobuf::SizeOfFieldBytes(log::pwpb::LogEntry::Fields::MESSAGE, 0) +
      protobuf::SizeOfFieldUint32(log::pwpb::LogEntry::Fields::LINE_LEVEL) +
      protobuf::SizeOfFieldUint32(log::pwpb::LogEntry::Fields::FLAGS) +
      protobuf::SizeOfFieldInt64(log::pwpb::LogEntry::Fields::TIMESTAMP) +
      protobuf::SizeOfFieldBytes(log::pwpb::LogEntry::Fields::MODULE, 0) +
      protobuf::SizeOfFieldBytes(log::pwpb::LogEntry::Fields::FILE, 0) +
      protobuf::SizeOfFieldBytes(log::pwpb::LogEntry::Fields::THREAD, 0);

  // Error messages sent when logs are dropped.
  static constexpr std::string_view kIngressErrorMessage{
      PW_LOG_RPC_INGRESS_ERROR_MSG};
  static constexpr std::string_view kSlowDrainErrorMessage{
      PW_LOG_RPC_SLOW_DRAIN_MSG};
  static constexpr std::string_view kSmallOutboundBufferErrorMessage{
      PW_LOG_RPC_SMALL_OUTBOUND_BUFFER_MSG};
  static constexpr std::string_view kSmallStackBufferErrorMessage{
      PW_LOG_RPC_SMALL_STACK_BUFFER_MSG};
  static constexpr std::string_view kWriterErrorMessage{
      PW_LOG_RPC_WRITER_ERROR_MSG};
  // The smallest entry buffer must fit the largest error message, or a typical
  // token size (4B), whichever is largest.
  static constexpr size_t kLargestErrorMessageOrTokenSize =
      std::max({size_t(4),
                kIngressErrorMessage.size(),
                kSlowDrainErrorMessage.size(),
                kSmallOutboundBufferErrorMessage.size(),
                kSmallStackBufferErrorMessage.size(),
                kWriterErrorMessage.size()});
  static constexpr size_t kMinEntryBufferSize =
      kMinEntrySizeWithoutPayload + sizeof(kLargestErrorMessageOrTokenSize);

  // When encoding LogEntry in LogEntries, there are kLogEntriesEncodeFrameSize
  // bytes added to the encoded LogEntry. This constant and kMinEntryBufferSize
  // can be used to calculate the minimum RPC ChannelOutput buffer size.
  static constexpr size_t kLogEntriesEncodeFrameSize =
      protobuf::TagSizeBytes(log::pwpb::LogEntries::Fields::ENTRIES) +
      protobuf::kMaxSizeOfLength +
      protobuf::SizeOfFieldUint32(
          log::pwpb::LogEntries::Fields::FIRST_ENTRY_SEQUENCE_ID);

  // Creates a closed log stream with a writer that can be set at a later time.
  // The provided buffer must be large enough to hold the largest transmittable
  // log::pwpb::LogEntry or a drop count message at the very least. The user can
  // choose to provide a unique mutex for the drain, or share it to save RAM as
  // long as they are aware of contengency issues.
  RpcLogDrain(
      const uint32_t channel_id,
      ByteSpan log_entry_buffer,
      sync::Mutex& mutex,
      LogDrainErrorHandling error_handling,
      Filter* filter = nullptr,
      size_t max_bundles_per_trickle = std::numeric_limits<size_t>::max(),
      pw::chrono::SystemClock::duration trickle_delay =
          chrono::SystemClock::duration::zero())
      : channel_id_(channel_id),
        error_handling_(error_handling),
        server_writer_(),
        log_entry_buffer_(log_entry_buffer),
        drop_count_ingress_error_(0),
        drop_count_slow_drain_(0),
        drop_count_small_outbound_buffer_(0),
        drop_count_small_stack_buffer_(0),
        drop_count_writer_error_(0),
        mutex_(mutex),
        filter_(filter),
        sequence_id_(0),
        max_bundles_per_trickle_(max_bundles_per_trickle),
        trickle_delay_(trickle_delay),
        no_writes_until_(chrono::SystemClock::now()),
        on_open_callback_(nullptr) {
    PW_ASSERT(log_entry_buffer.size_bytes() >= kMinEntryBufferSize);
  }

  // Not copyable.
  RpcLogDrain(const RpcLogDrain&) = delete;
  RpcLogDrain& operator=(const RpcLogDrain&) = delete;

  // Configures the drain with a new open server writer if the current one is
  // not open.
  //
  // Return values:
  // OK - Successfully set the new open writer.
  // FAILED_PRECONDITION - The given writer is not open.
  // ALREADY_EXISTS - an open writer is already set.
  Status Open(rpc::RawServerWriter& writer) PW_LOCKS_EXCLUDED(mutex_);

  // Accesses log entries and sends them via the writer. Expected to be called
  // frequently to avoid log drops. If the writer fails to send a packet with
  // multiple log entries, the entries are dropped and a drop message with the
  // count is sent. When error_handling is kCloseStreamOnWriterError, the stream
  // will automatically be closed and Flush will return the writer error.
  //
  // Precondition: the drain must be attached to a MultiSink.
  //
  // Return values:
  // OK - all entries were consumed.
  // ABORTED - there was an error writing the packet, and error_handling equals
  // `kCloseStreamOnWriterError`.
  Status Flush(ByteSpan encoding_buffer) PW_LOCKS_EXCLUDED(mutex_);

  // Writes entries as dictated by this drain's rate limiting configuration.
  //
  // Returns:
  //   A minimum wait duration before Trickle() will be ready to write more logs
  // If no duration is returned, this drain is caught up.
  std::optional<pw::chrono::SystemClock::duration> Trickle(
      ByteSpan encoding_buffer) PW_LOCKS_EXCLUDED(mutex_);

  // Ends RPC log stream without flushing.
  //
  // Return values:
  // OK - successfully closed the server writer.
  // FAILED_PRECONDITION - The given writer is not open.
  // Errors from the underlying writer send packet.
  Status Close() PW_LOCKS_EXCLUDED(mutex_);

  uint32_t channel_id() const { return channel_id_; }

  size_t max_bundles_per_trickle() const { return max_bundles_per_trickle_; }
  void set_max_bundles_per_trickle(size_t max_num_entries) {
    max_bundles_per_trickle_ = max_num_entries;
  }

  chrono::SystemClock::duration trickle_delay() const { return trickle_delay_; }
  void set_trickle_delay(chrono::SystemClock::duration trickle_delay) {
    trickle_delay_ = trickle_delay;
  }

  // Stores a function that is called when Open() is successful. Pass nulltpr to
  // clear it. This is useful in cases where the owner of the drain needs to be
  // notified that the drain was opened.
  void set_on_open_callback(pw::Function<void()>&& callback) {
    on_open_callback_ = std::move(callback);
  }

 private:
  enum class LogDrainState {
    kCaughtUp,
    kMoreEntriesRemaining,
  };

  LogDrainState SendLogs(size_t max_num_bundles,
                         ByteSpan encoding_buffer,
                         Status& encoding_status) PW_LOCKS_EXCLUDED(mutex_);

  // Fills the outgoing buffer with as many entries as possible.
  LogDrainState EncodeOutgoingPacket(
      log::pwpb::LogEntries::MemoryEncoder& encoder,
      uint32_t& packed_entry_count_out) PW_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  const uint32_t channel_id_;
  const LogDrainErrorHandling error_handling_;
  rpc::RawServerWriter server_writer_ PW_GUARDED_BY(mutex_);
  const ByteSpan log_entry_buffer_ PW_GUARDED_BY(mutex_);
  uint32_t drop_count_ingress_error_ PW_GUARDED_BY(mutex_);
  uint32_t drop_count_slow_drain_ PW_GUARDED_BY(mutex_);
  uint32_t drop_count_small_outbound_buffer_ PW_GUARDED_BY(mutex_);
  uint32_t drop_count_small_stack_buffer_ PW_GUARDED_BY(mutex_);
  uint32_t drop_count_writer_error_ PW_GUARDED_BY(mutex_);
  sync::Mutex& mutex_;
  Filter* filter_;
  uint32_t sequence_id_;
  size_t max_bundles_per_trickle_;
  pw::chrono::SystemClock::duration trickle_delay_;
  pw::chrono::SystemClock::time_point no_writes_until_;
  pw::Function<void()> on_open_callback_;
};

}  // namespace pw::log_rpc
