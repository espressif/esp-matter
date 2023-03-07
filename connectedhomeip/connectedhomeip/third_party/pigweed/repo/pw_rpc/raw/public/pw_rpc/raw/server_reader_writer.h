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

// This file defines the ServerReaderWriter, ServerReader, and ServerWriter
// classes for the raw RPC interface. These classes are used for bidirectional,
// client, and server streaming RPCs.
#pragma once

#include "pw_bytes/span.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/internal/method_info.h"
#include "pw_rpc/internal/method_lookup.h"
#include "pw_rpc/internal/server_call.h"
#include "pw_rpc/server.h"
#include "pw_rpc/writer.h"

namespace pw::rpc {
namespace internal {

// Forward declarations for internal classes needed in friend statements.
class RawMethod;

namespace test {

template <typename, typename, uint32_t>
class InvocationContext;

}  // namespace test
}  // namespace internal

class RawServerReader;
class RawServerWriter;

// The RawServerReaderWriter is used to send and receive messages in a raw
// bidirectional streaming RPC.
class RawServerReaderWriter : private internal::ServerCall {
 public:
  constexpr RawServerReaderWriter() = default;

  RawServerReaderWriter(RawServerReaderWriter&&) = default;
  RawServerReaderWriter& operator=(RawServerReaderWriter&&) = default;

  // Creates a RawServerReaderWriter that is ready to send responses for a
  // particular RPC. This can be used for testing or to send responses to an RPC
  // that has not been started by a client.
  template <auto kMethod, typename ServiceImpl>
  [[nodiscard]] static RawServerReaderWriter Open(Server& server,
                                                  uint32_t channel_id,
                                                  ServiceImpl& service) {
    internal::LockGuard lock(internal::rpc_lock());
    return {server
                .OpenContext<kMethod, MethodType::kBidirectionalStreaming>(
                    channel_id,
                    service,
                    internal::MethodLookup::GetRawMethod<
                        ServiceImpl,
                        internal::MethodInfo<kMethod>::kMethodId>())
                .ClaimLocked()};
  }

  using internal::Call::active;
  using internal::Call::channel_id;

  // Functions for setting the callbacks.
  using internal::Call::set_on_error;
  using internal::Call::set_on_next;
  using internal::ServerCall::set_on_client_stream_end;

  // Sends a response packet with the given raw payload.
  using internal::Call::Write;

  Status Finish(Status status = OkStatus()) {
    return CloseAndSendResponse(status);
  }

  // Allow use as a generic RPC Writer.
  using internal::Call::operator Writer&;
  using internal::Call::operator const Writer&;

 protected:
  RawServerReaderWriter(const internal::LockedCallContext& context,
                        MethodType type = MethodType::kBidirectionalStreaming)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : internal::ServerCall(context, type) {}

  using internal::Call::CloseAndSendResponse;

 private:
  friend class internal::RawMethod;  // Needed to construct

  template <typename, typename, uint32_t>
  friend class internal::test::InvocationContext;
};

// The RawServerReader is used to receive messages and send a response in a
// raw client streaming RPC.
class RawServerReader : private RawServerReaderWriter {
 public:
  // Creates a RawServerReader that is ready to send a response to a particular
  // RPC. This can be used for testing or to finish an RPC that has not been
  // started by the client.
  template <auto kMethod, typename ServiceImpl>
  [[nodiscard]] static RawServerReader Open(Server& server,
                                            uint32_t channel_id,
                                            ServiceImpl& service) {
    internal::LockGuard lock(internal::rpc_lock());
    return {server
                .OpenContext<kMethod, MethodType::kClientStreaming>(
                    channel_id,
                    service,
                    internal::MethodLookup::GetRawMethod<
                        ServiceImpl,
                        internal::MethodInfo<kMethod>::kMethodId>())
                .ClaimLocked()};
  }

  constexpr RawServerReader() = default;

  RawServerReader(RawServerReader&&) = default;
  RawServerReader& operator=(RawServerReader&&) = default;

  using RawServerReaderWriter::active;
  using RawServerReaderWriter::channel_id;

  using RawServerReaderWriter::set_on_client_stream_end;
  using RawServerReaderWriter::set_on_error;
  using RawServerReaderWriter::set_on_next;

  Status Finish(ConstByteSpan response, Status status = OkStatus()) {
    return CloseAndSendResponse(response, status);
  }

 private:
  friend class internal::RawMethod;  // Needed for conversions from ReaderWriter

  template <typename, typename, uint32_t>
  friend class internal::test::InvocationContext;

  RawServerReader(const internal::LockedCallContext& context)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : RawServerReaderWriter(context, MethodType::kClientStreaming) {}
};

// The RawServerWriter is used to send responses in a raw server streaming RPC.
class RawServerWriter : private RawServerReaderWriter {
 public:
  // Creates a RawServerWriter that is ready to send responses for a particular
  // RPC. This can be used for testing or to send responses to an RPC that has
  // not been started by a client.
  template <auto kMethod, typename ServiceImpl>
  [[nodiscard]] static RawServerWriter Open(Server& server,
                                            uint32_t channel_id,
                                            ServiceImpl& service) {
    internal::LockGuard lock(internal::rpc_lock());
    return {server
                .OpenContext<kMethod, MethodType::kServerStreaming>(
                    channel_id,
                    service,
                    internal::MethodLookup::GetRawMethod<
                        ServiceImpl,
                        internal::MethodInfo<kMethod>::kMethodId>())
                .ClaimLocked()};
  }

  constexpr RawServerWriter() = default;

  RawServerWriter(RawServerWriter&&) = default;
  RawServerWriter& operator=(RawServerWriter&&) = default;

  using RawServerReaderWriter::active;
  using RawServerReaderWriter::channel_id;

  using RawServerReaderWriter::set_on_error;

  using RawServerReaderWriter::Finish;
  using RawServerReaderWriter::Write;

  // Allow use as a generic RPC Writer.
  using internal::Call::operator Writer&;
  using internal::Call::operator const Writer&;

 private:
  template <typename, typename, uint32_t>
  friend class internal::test::InvocationContext;

  friend class internal::RawMethod;

  RawServerWriter(const internal::LockedCallContext& context)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : RawServerReaderWriter(context, MethodType::kServerStreaming) {}
};

// The RawUnaryResponder is used to send a response in a raw unary RPC.
class RawUnaryResponder : private RawServerReaderWriter {
 public:
  // Creates a RawUnaryResponder that is ready to send responses for a
  // particular RPC. This can be used for testing or to send responses to an RPC
  // that has not been started by a client.
  template <auto kMethod, typename ServiceImpl>
  [[nodiscard]] static RawUnaryResponder Open(Server& server,
                                              uint32_t channel_id,
                                              ServiceImpl& service) {
    internal::LockGuard lock(internal::rpc_lock());
    return {server
                .OpenContext<kMethod, MethodType::kUnary>(
                    channel_id,
                    service,
                    internal::MethodLookup::GetRawMethod<
                        ServiceImpl,
                        internal::MethodInfo<kMethod>::kMethodId>())
                .ClaimLocked()};
  }

  constexpr RawUnaryResponder() = default;

  RawUnaryResponder(RawUnaryResponder&&) = default;
  RawUnaryResponder& operator=(RawUnaryResponder&&) = default;

  using RawServerReaderWriter::active;
  using RawServerReaderWriter::channel_id;

  using RawServerReaderWriter::set_on_error;

  Status Finish(ConstByteSpan response, Status status = OkStatus()) {
    return CloseAndSendResponse(response, status);
  }

 private:
  template <typename, typename, uint32_t>
  friend class internal::test::InvocationContext;

  friend class internal::RawMethod;

  RawUnaryResponder(const internal::LockedCallContext& context)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : RawServerReaderWriter(context, MethodType::kUnary) {}
};

}  // namespace pw::rpc
