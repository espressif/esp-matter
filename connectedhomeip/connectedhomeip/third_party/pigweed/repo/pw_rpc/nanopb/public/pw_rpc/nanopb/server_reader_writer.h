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
// classes for the Nanopb RPC interface. These classes are used for
// bidirectional, client, and server streaming RPCs.
#pragma once

#include "pw_bytes/span.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/internal/method_info.h"
#include "pw_rpc/internal/method_lookup.h"
#include "pw_rpc/internal/server_call.h"
#include "pw_rpc/nanopb/internal/common.h"
#include "pw_rpc/server.h"

namespace pw::rpc {
namespace internal {

// Forward declarations for internal classes needed in friend statements.
class NanopbMethod;

namespace test {

template <typename, typename, uint32_t>
class InvocationContext;

}  // namespace test

class NanopbServerCall : public internal::ServerCall {
 public:
  constexpr NanopbServerCall() : serde_(nullptr) {}

  NanopbServerCall(const LockedCallContext& context, MethodType type)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  Status SendUnaryResponse(const void* payload, Status status)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    return SendFinalResponse(*this, payload, status);
  }

  const NanopbMethodSerde& serde() const { return *serde_; }

 protected:
  NanopbServerCall(NanopbServerCall&& other) PW_LOCKS_EXCLUDED(rpc_lock()) {
    *this = std::move(other);
  }

  NanopbServerCall& operator=(NanopbServerCall&& other)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    internal::LockGuard lock(internal::rpc_lock());
    MoveNanopbServerCallFrom(other);
    return *this;
  }

  void MoveNanopbServerCallFrom(NanopbServerCall& other)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    MoveServerCallFrom(other);
    serde_ = other.serde_;
  }

  Status SendServerStream(const void* payload) PW_LOCKS_EXCLUDED(rpc_lock());

  bool DecodeRequest(ConstByteSpan payload, void* request_struct) const {
    return serde_->DecodeRequest(payload, request_struct);
  }

 private:
  const NanopbMethodSerde* serde_;
};

// The BaseNanopbServerReader serves as the base for the ServerReader and
// ServerReaderWriter classes. It adds a callback templated on the request
// struct type. It is templated on the Request type only.
template <typename Request>
class BaseNanopbServerReader : public NanopbServerCall {
 public:
  BaseNanopbServerReader(const internal::LockedCallContext& context,
                         MethodType type)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : NanopbServerCall(context, type) {}

 protected:
  constexpr BaseNanopbServerReader() = default;

  BaseNanopbServerReader(BaseNanopbServerReader&& other)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    *this = std::move(other);
  }

  BaseNanopbServerReader& operator=(BaseNanopbServerReader&& other)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    internal::LockGuard lock(internal::rpc_lock());
    MoveNanopbServerCallFrom(other);
    set_on_next_locked(std::move(other.nanopb_on_next_));
    return *this;
  }

  void set_on_next(Function<void(const Request& request)>&& on_next)
      PW_LOCKS_EXCLUDED(rpc_lock()) {
    internal::LockGuard lock(internal::rpc_lock());
    set_on_next_locked(std::move(on_next));
  }

 private:
  void set_on_next_locked(Function<void(const Request& request)>&& on_next)
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    nanopb_on_next_ = std::move(on_next);

    internal::Call::set_on_next_locked([this](ConstByteSpan payload) {
      if (nanopb_on_next_) {
        Request request_struct{};
        if (DecodeRequest(payload, &request_struct)) {
          nanopb_on_next_(request_struct);
        }
      }
    });
  }

  Function<void(const Request&)> nanopb_on_next_;
};

}  // namespace internal

// The NanopbServerReaderWriter is used to send and receive messages in a Nanopb
// bidirectional streaming RPC.
//
// These classes use private inheritance to hide the internal::Call API while
// allow direct use of its public and protected functions.
template <typename Request, typename Response>
class NanopbServerReaderWriter
    : private internal::BaseNanopbServerReader<Request> {
 public:
  // Creates a NanopbServerReaderWriter that is ready to send responses for a
  // particular RPC. This can be used for testing or to send responses to an RPC
  // that has not been started by a client.
  template <auto kMethod, typename ServiceImpl>
  [[nodiscard]] static NanopbServerReaderWriter Open(Server& server,
                                                     uint32_t channel_id,
                                                     ServiceImpl& service) {
    using Info = internal::MethodInfo<kMethod>;
    static_assert(std::is_same_v<Request, typename Info::Request>,
                  "The request type of a NanopbServerReaderWriter must match "
                  "the method.");
    static_assert(std::is_same_v<Response, typename Info::Response>,
                  "The response type of a NanopbServerReaderWriter must match "
                  "the method.");
    internal::LockGuard lock(internal::rpc_lock());
    return {server
                .OpenContext<kMethod, MethodType::kBidirectionalStreaming>(
                    channel_id,
                    service,
                    internal::MethodLookup::GetNanopbMethod<ServiceImpl,
                                                            Info::kMethodId>())
                .ClaimLocked()};
  }

  constexpr NanopbServerReaderWriter() = default;

  NanopbServerReaderWriter(NanopbServerReaderWriter&&) = default;
  NanopbServerReaderWriter& operator=(NanopbServerReaderWriter&&) = default;

  using internal::Call::active;
  using internal::Call::channel_id;

  // Writes a response struct. Returns the following Status codes:
  //
  //   OK - the response was successfully sent
  //   FAILED_PRECONDITION - the writer is closed
  //   INTERNAL - pw_rpc was unable to encode the Nanopb protobuf
  //   other errors - the ChannelOutput failed to send the packet; the error
  //       codes are determined by the ChannelOutput implementation
  //
  Status Write(const Response& response) {
    return internal::NanopbServerCall::SendServerStream(&response);
  }

  Status Finish(Status status = OkStatus()) {
    return internal::Call::CloseAndSendResponse(status);
  }

  // Functions for setting RPC event callbacks.
  using internal::Call::set_on_error;
  using internal::ServerCall::set_on_client_stream_end;
  using internal::BaseNanopbServerReader<Request>::set_on_next;

 private:
  friend class internal::NanopbMethod;

  template <typename, typename, uint32_t>
  friend class internal::test::InvocationContext;

  NanopbServerReaderWriter(const internal::LockedCallContext& context)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : internal::BaseNanopbServerReader<Request>(
            context, MethodType::kBidirectionalStreaming) {}
};

// The NanopbServerReader is used to receive messages and send a response in a
// Nanopb client streaming RPC.
template <typename Request, typename Response>
class NanopbServerReader : private internal::BaseNanopbServerReader<Request> {
 public:
  // Creates a NanopbServerReader that is ready to send a response to a
  // particular RPC. This can be used for testing or to finish an RPC that has
  // not been started by the client.
  template <auto kMethod, typename ServiceImpl>
  [[nodiscard]] static NanopbServerReader Open(Server& server,
                                               uint32_t channel_id,
                                               ServiceImpl& service) {
    using Info = internal::MethodInfo<kMethod>;
    static_assert(
        std::is_same_v<Request, typename Info::Request>,
        "The request type of a NanopbServerReader must match the method.");
    static_assert(
        std::is_same_v<Response, typename Info::Response>,
        "The response type of a NanopbServerReader must match the method.");
    internal::LockGuard lock(internal::rpc_lock());
    return {server
                .OpenContext<kMethod, MethodType::kClientStreaming>(
                    channel_id,
                    service,
                    internal::MethodLookup::GetNanopbMethod<ServiceImpl,
                                                            Info::kMethodId>())
                .ClaimLocked()};
  }

  // Allow default construction so that users can declare a variable into which
  // to move NanopbServerReaders from RPC calls.
  constexpr NanopbServerReader() = default;

  NanopbServerReader(NanopbServerReader&&) = default;
  NanopbServerReader& operator=(NanopbServerReader&&) = default;

  using internal::Call::active;
  using internal::Call::channel_id;

  // Functions for setting RPC event callbacks.
  using internal::Call::set_on_error;
  using internal::ServerCall::set_on_client_stream_end;
  using internal::BaseNanopbServerReader<Request>::set_on_next;

  Status Finish(const Response& response, Status status = OkStatus()) {
    return internal::NanopbServerCall::SendUnaryResponse(&response, status);
  }

 private:
  friend class internal::NanopbMethod;

  template <typename, typename, uint32_t>
  friend class internal::test::InvocationContext;

  NanopbServerReader(const internal::LockedCallContext& context)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : internal::BaseNanopbServerReader<Request>(
            context, MethodType::kClientStreaming) {}
};

// The NanopbServerWriter is used to send responses in a Nanopb server streaming
// RPC.
template <typename Response>
class NanopbServerWriter : private internal::NanopbServerCall {
 public:
  // Creates a NanopbServerWriter that is ready to send responses for a
  // particular RPC. This can be used for testing or to send responses to an RPC
  // that has not been started by a client.
  template <auto kMethod, typename ServiceImpl>
  [[nodiscard]] static NanopbServerWriter Open(Server& server,
                                               uint32_t channel_id,
                                               ServiceImpl& service) {
    using Info = internal::MethodInfo<kMethod>;
    static_assert(
        std::is_same_v<Response, typename Info::Response>,
        "The response type of a NanopbServerWriter must match the method.");
    internal::LockGuard lock(internal::rpc_lock());
    return {server
                .OpenContext<kMethod, MethodType::kServerStreaming>(
                    channel_id,
                    service,
                    internal::MethodLookup::GetNanopbMethod<ServiceImpl,
                                                            Info::kMethodId>())
                .ClaimLocked()};
  }

  // Allow default construction so that users can declare a variable into which
  // to move ServerWriters from RPC calls.
  constexpr NanopbServerWriter() = default;

  NanopbServerWriter(NanopbServerWriter&&) = default;
  NanopbServerWriter& operator=(NanopbServerWriter&&) = default;

  using internal::Call::active;
  using internal::Call::channel_id;

  // Writes a response struct. Returns the following Status codes:
  //
  //   OK - the response was successfully sent
  //   FAILED_PRECONDITION - the writer is closed
  //   INTERNAL - pw_rpc was unable to encode the Nanopb protobuf
  //   other errors - the ChannelOutput failed to send the packet; the error
  //       codes are determined by the ChannelOutput implementation
  //
  Status Write(const Response& response) {
    return internal::NanopbServerCall::SendServerStream(&response);
  }

  Status Finish(Status status = OkStatus()) {
    return internal::Call::CloseAndSendResponse(status);
  }

  using internal::Call::set_on_error;
  using internal::ServerCall::set_on_client_stream_end;

 private:
  friend class internal::NanopbMethod;

  template <typename, typename, uint32_t>
  friend class internal::test::InvocationContext;

  NanopbServerWriter(const internal::LockedCallContext& context)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : internal::NanopbServerCall(context, MethodType::kServerStreaming) {}
};

template <typename Response>
class NanopbUnaryResponder : private internal::NanopbServerCall {
 public:
  // Creates a NanopbUnaryResponder that is ready to send a response for a
  // particular RPC. This can be used for testing or to send responses to an RPC
  // that has not been started by a client.
  template <auto kMethod, typename ServiceImpl>
  [[nodiscard]] static NanopbUnaryResponder Open(Server& server,
                                                 uint32_t channel_id,
                                                 ServiceImpl& service) {
    using Info = internal::MethodInfo<kMethod>;
    static_assert(
        std::is_same_v<Response, typename Info::Response>,
        "The response type of a NanopbUnaryResponder must match the method.");
    internal::LockGuard lock(internal::rpc_lock());
    return {server
                .OpenContext<kMethod, MethodType::kUnary>(
                    channel_id,
                    service,
                    internal::MethodLookup::GetNanopbMethod<ServiceImpl,
                                                            Info::kMethodId>())
                .ClaimLocked()};
  }

  // Allow default construction so that users can declare a variable into which
  // to move ServerWriters from RPC calls.
  constexpr NanopbUnaryResponder() = default;

  NanopbUnaryResponder(NanopbUnaryResponder&&) = default;
  NanopbUnaryResponder& operator=(NanopbUnaryResponder&&) = default;

  using internal::Call::active;
  using internal::Call::channel_id;

  // Sends the response. Returns the following Status codes:
  //
  //   OK - the response was successfully sent
  //   FAILED_PRECONDITION - the writer is closed
  //   INTERNAL - pw_rpc was unable to encode the Nanopb protobuf
  //   other errors - the ChannelOutput failed to send the packet; the error
  //       codes are determined by the ChannelOutput implementation
  //
  Status Finish(const Response& response, Status status = OkStatus()) {
    return internal::NanopbServerCall::SendUnaryResponse(&response, status);
  }

  using internal::Call::set_on_error;
  using internal::ServerCall::set_on_client_stream_end;

 private:
  friend class internal::NanopbMethod;

  template <typename, typename, uint32_t>
  friend class internal::test::InvocationContext;

  NanopbUnaryResponder(const internal::LockedCallContext& context)
      PW_EXCLUSIVE_LOCKS_REQUIRED(internal::rpc_lock())
      : internal::NanopbServerCall(context, MethodType::kUnary) {}
};

}  // namespace pw::rpc
