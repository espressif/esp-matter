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

#include <cstdint>

#include "pw_assert/assert.h"
#include "pw_chrono/system_clock.h"
#include "pw_function/function.h"
#include "pw_preprocessor/compiler.h"
#include "pw_rpc/raw/client_reader_writer.h"
#include "pw_rpc/raw/server_reader_writer.h"
#include "pw_span/span.h"
#include "pw_sync/binary_semaphore.h"
#include "pw_sync/timed_thread_notification.h"
#include "pw_thread/thread_core.h"
#include "pw_transfer/handler.h"
#include "pw_transfer/internal/client_context.h"
#include "pw_transfer/internal/context.h"
#include "pw_transfer/internal/event.h"
#include "pw_transfer/internal/server_context.h"

namespace pw::transfer {
namespace internal {

class TransferThread : public thread::ThreadCore {
 public:
  TransferThread(span<ClientContext> client_transfers,
                 span<ServerContext> server_transfers,
                 ByteSpan chunk_buffer,
                 ByteSpan encode_buffer)
      : client_transfers_(client_transfers),
        server_transfers_(server_transfers),
        chunk_buffer_(chunk_buffer),
        encode_buffer_(encode_buffer) {}

  void StartClientTransfer(TransferType type,
                           ProtocolVersion version,
                           uint32_t resource_id,
                           stream::Stream* stream,
                           const TransferParameters& max_parameters,
                           Function<void(Status)>&& on_completion,
                           chrono::SystemClock::duration timeout,
                           uint8_t max_retries,
                           uint32_t max_lifetime_retries) {
    uint32_t session_id = version == ProtocolVersion::kLegacy
                              ? resource_id
                              : Context::kUnassignedSessionId;
    StartTransfer(type,
                  version,
                  session_id,
                  resource_id,
                  /*raw_chunk=*/{},
                  stream,
                  max_parameters,
                  std::move(on_completion),
                  timeout,
                  max_retries,
                  max_lifetime_retries);
  }

  void StartServerTransfer(TransferType type,
                           ProtocolVersion version,
                           uint32_t session_id,
                           uint32_t resource_id,
                           ConstByteSpan raw_chunk,
                           const TransferParameters& max_parameters,
                           chrono::SystemClock::duration timeout,
                           uint8_t max_retries,
                           uint32_t max_lifetime_retries) {
    StartTransfer(type,
                  version,
                  session_id,
                  resource_id,
                  raw_chunk,
                  /*stream=*/nullptr,
                  max_parameters,
                  /*on_completion=*/nullptr,
                  timeout,
                  max_retries,
                  max_lifetime_retries);
  }

  void ProcessClientChunk(ConstByteSpan chunk) {
    ProcessChunk(EventType::kClientChunk, chunk);
  }

  void ProcessServerChunk(ConstByteSpan chunk) {
    ProcessChunk(EventType::kServerChunk, chunk);
  }

  void EndClientTransfer(uint32_t session_id,
                         Status status,
                         bool send_status_chunk = false) {
    EndTransfer(
        EventType::kClientEndTransfer, session_id, status, send_status_chunk);
  }

  void EndServerTransfer(uint32_t session_id,
                         Status status,
                         bool send_status_chunk = false) {
    EndTransfer(
        EventType::kServerEndTransfer, session_id, status, send_status_chunk);
  }

  void SetClientReadStream(rpc::RawClientReaderWriter& read_stream) {
    SetClientStream(TransferStream::kClientRead, read_stream);
  }

  void SetClientWriteStream(rpc::RawClientReaderWriter& write_stream) {
    SetClientStream(TransferStream::kClientWrite, write_stream);
  }

  void SetServerReadStream(rpc::RawServerReaderWriter& read_stream) {
    SetServerStream(TransferStream::kServerRead, read_stream);
  }

  void SetServerWriteStream(rpc::RawServerReaderWriter& write_stream) {
    SetServerStream(TransferStream::kServerWrite, write_stream);
  }

  void AddTransferHandler(Handler& handler) {
    TransferHandlerEvent(EventType::kAddTransferHandler, handler);
  }

  void RemoveTransferHandler(Handler& handler) {
    TransferHandlerEvent(EventType::kRemoveTransferHandler, handler);
    // Ensure this function blocks until the transfer handler is fully cleaned
    // up.
    WaitUntilEventIsProcessed();
  }

  size_t max_chunk_size() const { return chunk_buffer_.size(); }

  // For testing only: terminates the transfer thread with a kTerminate event.
  void Terminate();

  // For testing only: blocks until the next event can be acquired, which means
  // a previously enqueued event has been processed.
  void WaitUntilEventIsProcessed() {
    next_event_ownership_.acquire();
    next_event_ownership_.release();
  }

  // For testing only: simulates a timeout event for a client transfer.
  void SimulateClientTimeout(uint32_t session_id) {
    SimulateTimeout(EventType::kClientTimeout, session_id);
  }

  // For testing only: simulates a timeout event for a server transfer.
  void SimulateServerTimeout(uint32_t session_id) {
    SimulateTimeout(EventType::kServerTimeout, session_id);
  }

 private:
  friend class Context;

  // Maximum amount of time between transfer thread runs.
  static constexpr chrono::SystemClock::duration kMaxTimeout =
      std::chrono::seconds(2);

  // Finds an active server or client transfer, matching against its legacy ID.
  template <typename T>
  static Context* FindActiveTransferByLegacyId(const span<T>& transfers,
                                               uint32_t session_id) {
    auto transfer =
        std::find_if(transfers.begin(), transfers.end(), [session_id](auto& c) {
          return c.initialized() && c.id() == session_id;
        });
    return transfer != transfers.end() ? &*transfer : nullptr;
  }

  // Finds an active server or client transfer, matching against resource ID.
  template <typename T>
  static Context* FindActiveTransferByResourceId(const span<T>& transfers,
                                                 uint32_t resource_id) {
    auto transfer = std::find_if(
        transfers.begin(), transfers.end(), [resource_id](auto& c) {
          return c.initialized() && c.resource_id() == resource_id;
        });
    return transfer != transfers.end() ? &*transfer : nullptr;
  }

  void SimulateTimeout(EventType type, uint32_t session_id);

  // Finds an new server or client transfer.
  template <typename T>
  static Context* FindNewTransfer(const span<T>& transfers,
                                  uint32_t session_id) {
    Context* new_transfer = nullptr;

    for (Context& context : transfers) {
      if (context.active()) {
        if (context.session_id() == session_id) {
          // Restart an already active transfer.
          return &context;
        }
      } else {
        // Store the inactive context as an option, but keep checking for the
        // restart case.
        new_transfer = &context;
      }
    }

    return new_transfer;
  }

  const ByteSpan& encode_buffer() const { return encode_buffer_; }

  void Run() final;

  void HandleTimeouts();

  rpc::Writer& stream_for(TransferStream stream) {
    switch (stream) {
      case TransferStream::kClientRead:
        return client_read_stream_;
      case TransferStream::kClientWrite:
        return client_write_stream_;
      case TransferStream::kServerRead:
        return server_read_stream_;
      case TransferStream::kServerWrite:
        return server_write_stream_;
    }
    // An unknown TransferStream value was passed, which means this function
    // was passed an invalid enum value.
    PW_ASSERT(false);
  }

  // Returns the earliest timeout among all active transfers, up to kMaxTimeout.
  chrono::SystemClock::time_point GetNextTransferTimeout() const;

  void StartTransfer(TransferType type,
                     ProtocolVersion version,
                     uint32_t session_id,
                     uint32_t resource_id,
                     ConstByteSpan raw_chunk,
                     stream::Stream* stream,
                     const TransferParameters& max_parameters,
                     Function<void(Status)>&& on_completion,
                     chrono::SystemClock::duration timeout,
                     uint8_t max_retries,
                     uint32_t max_lifetime_retries);

  void ProcessChunk(EventType type, ConstByteSpan chunk);

  void EndTransfer(EventType type,
                   uint32_t session_id,
                   Status status,
                   bool send_status_chunk);

  void SetClientStream(TransferStream type, rpc::RawClientReaderWriter& stream);
  void SetServerStream(TransferStream type, rpc::RawServerReaderWriter& stream);

  void TransferHandlerEvent(EventType type, Handler& handler);

  void HandleEvent(const Event& event);
  Context* FindContextForEvent(const Event& event) const;

  void SendStatusChunk(const SendStatusChunkEvent& event);

  sync::TimedThreadNotification event_notification_;
  sync::BinarySemaphore next_event_ownership_;

  Event next_event_;
  Function<void(Status)> staged_on_completion_;
  rpc::RawClientReaderWriter staged_client_stream_;
  rpc::RawServerReaderWriter staged_server_stream_;

  rpc::RawClientReaderWriter client_read_stream_;
  rpc::RawClientReaderWriter client_write_stream_;
  rpc::RawServerReaderWriter server_read_stream_;
  rpc::RawServerReaderWriter server_write_stream_;

  span<ClientContext> client_transfers_;
  span<ServerContext> server_transfers_;

  // All registered transfer handlers.
  IntrusiveList<Handler> handlers_;

  // Buffer in which chunk data is staged for CHUNK events.
  ByteSpan chunk_buffer_;

  // Buffer into which responses are encoded. Only ever used from within the
  // transfer thread, so no locking is required.
  ByteSpan encode_buffer_;
};

}  // namespace internal

using TransferThread = internal::TransferThread;

template <size_t kMaxConcurrentClientTransfers,
          size_t kMaxConcurrentServerTransfers>
class Thread final : public internal::TransferThread {
 public:
  Thread(ByteSpan chunk_buffer, ByteSpan encode_buffer)
      : internal::TransferThread(
            client_contexts_, server_contexts_, chunk_buffer, encode_buffer) {}

 private:
  std::array<internal::ClientContext, kMaxConcurrentClientTransfers>
      client_contexts_;
  std::array<internal::ServerContext, kMaxConcurrentServerTransfers>
      server_contexts_;
};

}  // namespace pw::transfer
