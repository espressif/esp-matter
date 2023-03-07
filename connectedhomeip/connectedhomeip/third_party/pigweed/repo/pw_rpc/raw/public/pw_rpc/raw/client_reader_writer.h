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

// This file defines the ServerReaderWriter, ServerReader, and ServerWriter
// classes for the raw RPC interface. These classes are used for bidirectional,
// client, and server streaming RPCs.
#pragma once

#include "pw_bytes/span.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/internal/client_call.h"
#include "pw_rpc/writer.h"

namespace pw::rpc {

// Sends requests and handles responses for a bidirectional streaming RPC.
//
// These classes use private inheritance to hide the internal::Call API while
// allow direct use of its public and protected functions.
class RawClientReaderWriter : private internal::StreamResponseClientCall {
 public:
  constexpr RawClientReaderWriter() = default;

  RawClientReaderWriter(RawClientReaderWriter&&) = default;
  RawClientReaderWriter& operator=(RawClientReaderWriter&&) = default;

  using internal::Call::active;
  using internal::Call::channel_id;

  using internal::ClientCall::id;

  // Functions for setting the callbacks.
  using internal::StreamResponseClientCall::set_on_completed;
  using internal::StreamResponseClientCall::set_on_error;
  using internal::StreamResponseClientCall::set_on_next;

  // Sends a stream request packet with the given raw payload.
  using internal::Call::Write;

  // Notifies the server that no further client stream messages will be sent.
  using internal::Call::CloseClientStream;

  // Cancels this RPC.
  using internal::Call::Cancel;

  // Allow use as a generic RPC Writer.
  using internal::Call::operator Writer&;
  using internal::Call::operator const Writer&;

 protected:
  friend class internal::StreamResponseClientCall;

  RawClientReaderWriter(internal::LockedEndpoint& client,
                        uint32_t channel_id,
                        uint32_t service_id,
                        uint32_t method_id,
                        MethodType type = MethodType::kBidirectionalStreaming)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : StreamResponseClientCall(
            client, channel_id, service_id, method_id, type) {}
};

// Handles responses for a server streaming RPC.
class RawClientReader : private internal::StreamResponseClientCall {
 public:
  constexpr RawClientReader() = default;

  RawClientReader(RawClientReader&&) = default;
  RawClientReader& operator=(RawClientReader&&) = default;

  using internal::StreamResponseClientCall::active;
  using internal::StreamResponseClientCall::channel_id;

  using internal::StreamResponseClientCall::set_on_completed;
  using internal::StreamResponseClientCall::set_on_error;
  using internal::StreamResponseClientCall::set_on_next;

  using internal::Call::Cancel;

 private:
  friend class internal::StreamResponseClientCall;

  RawClientReader(internal::LockedEndpoint& client,
                  uint32_t channel_id,
                  uint32_t service_id,
                  uint32_t method_id)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : StreamResponseClientCall(client,
                                 channel_id,
                                 service_id,
                                 method_id,
                                 MethodType::kServerStreaming) {}
};

// Sends requests and handles the response for a client streaming RPC.
class RawClientWriter : private internal::UnaryResponseClientCall {
 public:
  constexpr RawClientWriter() = default;

  RawClientWriter(RawClientWriter&&) = default;
  RawClientWriter& operator=(RawClientWriter&&) = default;

  using internal::UnaryResponseClientCall::active;
  using internal::UnaryResponseClientCall::channel_id;

  using internal::UnaryResponseClientCall::set_on_completed;
  using internal::UnaryResponseClientCall::set_on_error;

  using internal::Call::Cancel;
  using internal::Call::CloseClientStream;
  using internal::Call::Write;

  // Allow use as a generic RPC Writer.
  using internal::Call::operator Writer&;
  using internal::Call::operator const Writer&;

 private:
  friend class internal::UnaryResponseClientCall;

  RawClientWriter(internal::LockedEndpoint& client,
                  uint32_t channel_id,
                  uint32_t service_id,
                  uint32_t method_id)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : UnaryResponseClientCall(client,
                                channel_id,
                                service_id,
                                method_id,
                                MethodType::kClientStreaming) {}
};

// Handles the response for to unary RPC.
class RawUnaryReceiver : private internal::UnaryResponseClientCall {
 public:
  constexpr RawUnaryReceiver() = default;

  RawUnaryReceiver(RawUnaryReceiver&&) = default;
  RawUnaryReceiver& operator=(RawUnaryReceiver&&) = default;

  using internal::UnaryResponseClientCall::active;
  using internal::UnaryResponseClientCall::channel_id;

  using internal::UnaryResponseClientCall::set_on_completed;
  using internal::UnaryResponseClientCall::set_on_error;

  using internal::UnaryResponseClientCall::Cancel;

 private:
  friend class internal::UnaryResponseClientCall;

  RawUnaryReceiver(internal::LockedEndpoint& client,
                   uint32_t channel_id,
                   uint32_t service_id,
                   uint32_t method_id)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : UnaryResponseClientCall(
            client, channel_id, service_id, method_id, MethodType::kUnary) {}
};

}  // namespace pw::rpc
