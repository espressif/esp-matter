// Copyright 2022 The Pigweed Authors
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

#include <cstddef>
#include <cstdint>
#include <limits>

#include "pw_bytes/span.h"
#include "pw_transfer/handler.h"
#include "pw_transfer/internal/config.h"
#include "pw_transfer/internal/server_context.h"
#include "pw_transfer/transfer.raw_rpc.pb.h"
#include "pw_transfer/transfer_thread.h"

namespace pw::transfer {
namespace internal {

class Chunk;

}  // namespace internal

class TransferService : public pw_rpc::raw::Transfer::Service<TransferService> {
 public:
  // Initializes a TransferService that can be registered with an RPC server.
  //
  // The transfer service requires a work queue to perform deferred tasks, such
  // as handling transfer timeouts and retries. This work queue does not need to
  // be unique to the transfer service; it may be shared with other parts of the
  // system.
  //
  // The provided buffer is used to stage data from transfer chunks before it is
  // written out to the writer. The size of this buffer is the largest amount of
  // data that can be sent in a single transfer chunk, excluding any transport
  // layer overhead.
  //
  // max_pending_bytes is the maximum amount of data to ask for at a
  // time during a write transfer, unless told a more restrictive amount by a
  // transfer handler. This size can span multiple chunks. A larger value
  // generally increases the efficiency of write transfers when sent over a
  // reliable transport. However, if the underlying transport is unreliable,
  // larger values could slow down a transfer in the event of repeated packet
  // loss.
  TransferService(
      TransferThread& transfer_thread,
      uint32_t max_pending_bytes,
      chrono::SystemClock::duration chunk_timeout = cfg::kDefaultChunkTimeout,
      uint8_t max_retries = cfg::kDefaultMaxRetries,
      uint32_t extend_window_divisor = cfg::kDefaultExtendWindowDivisor,
      uint32_t max_lifetime_retries = cfg::kDefaultMaxLifetimeRetries)
      : max_parameters_(max_pending_bytes,
                        transfer_thread.max_chunk_size(),
                        extend_window_divisor),
        thread_(transfer_thread),
        chunk_timeout_(chunk_timeout),
        max_retries_(max_retries),
        max_lifetime_retries_(max_lifetime_retries),
        next_session_id_(1) {}

  TransferService(const TransferService&) = delete;
  TransferService(TransferService&&) = delete;

  TransferService& operator=(const TransferService&) = delete;
  TransferService& operator=(TransferService&&) = delete;

  void Read(RawServerReaderWriter& reader_writer) {
    reader_writer.set_on_next([this](ConstByteSpan message) {
      HandleChunk(message, internal::TransferType::kTransmit);
    });
    thread_.SetServerReadStream(reader_writer);
  }

  void Write(RawServerReaderWriter& reader_writer) {
    reader_writer.set_on_next([this](ConstByteSpan message) {
      HandleChunk(message, internal::TransferType::kReceive);
    });
    thread_.SetServerWriteStream(reader_writer);
  }

  void RegisterHandler(Handler& handler) {
    thread_.AddTransferHandler(handler);
  }

  void UnregisterHandler(Handler& handler) {
    thread_.RemoveTransferHandler(handler);
  }

  void set_max_pending_bytes(uint32_t max_pending_bytes) {
    max_parameters_.set_pending_bytes(max_pending_bytes);
  }

  // Sets the maximum size for the data in a pw_transfer chunk. Note that the
  // max chunk size must always fit within the transfer thread's chunk buffer.
  void set_max_chunk_size_bytes(uint32_t max_chunk_size_bytes) {
    max_parameters_.set_max_chunk_size_bytes(max_chunk_size_bytes);
  }

  void set_chunk_timeout(chrono::SystemClock::duration chunk_timeout) {
    chunk_timeout_ = chunk_timeout;
  }

  void set_max_retries(uint8_t max_retries) { max_retries_ = max_retries; }

  Status set_extend_window_divisor(uint32_t extend_window_divisor) {
    if (extend_window_divisor <= 1) {
      return Status::InvalidArgument();
    }

    max_parameters_.set_extend_window_divisor(extend_window_divisor);
    return OkStatus();
  }

 private:
  void HandleChunk(ConstByteSpan message, internal::TransferType type);

  // TODO(frolv): This could be more sophisticated and less predictable.
  uint32_t GenerateNewSessionId() { return next_session_id_++; }

  internal::TransferParameters max_parameters_;
  TransferThread& thread_;

  chrono::SystemClock::duration chunk_timeout_;
  uint8_t max_retries_;
  uint32_t max_lifetime_retries_;

  uint32_t next_session_id_;
};

}  // namespace pw::transfer
