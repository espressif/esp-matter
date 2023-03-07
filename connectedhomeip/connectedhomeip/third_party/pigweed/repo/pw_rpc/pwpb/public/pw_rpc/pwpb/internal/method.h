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
#include <type_traits>

#include "pw_bytes/span.h"
#include "pw_rpc/internal/call_context.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/internal/method.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/method_type.h"
#include "pw_rpc/pwpb/internal/common.h"
#include "pw_rpc/pwpb/server_reader_writer.h"
#include "pw_rpc/service.h"
#include "pw_span/span.h"
#include "pw_status/status_with_size.h"

namespace pw::rpc::internal {

// Expected function signatures for user-implemented RPC functions.
template <typename Request, typename Response>
using PwpbSynchronousUnary = Status(const Request&, Response&);

template <typename Request, typename Response>
using PwpbAsynchronousUnary = void(const Request&,
                                   PwpbUnaryResponder<Response>&);

template <typename Request, typename Response>
using PwpbServerStreaming = void(const Request&, PwpbServerWriter<Response>&);

template <typename Request, typename Response>
using PwpbClientStreaming = void(PwpbServerReader<Request, Response>&);

template <typename Request, typename Response>
using PwpbBidirectionalStreaming =
    void(PwpbServerReaderWriter<Request, Response>&);

// The PwpbMethod class invokes user-defined service methods. When a
// pw::rpc::Server receives an RPC request packet, it looks up the matching
// PwpbMethod instance and calls its Invoke method, which eventually calls into
// the user-defined RPC function.
//
// A PwpbMethod instance is created for each user-defined RPC in the pw_rpc
// generated code. The PwpbMethod stores a pointer to the RPC function,
// a pointer to an "invoker" function that calls that function, and a
// reference to a serializer/deserializer initiiated with the message struct
// tables used to encode and decode request and response message structs.
class PwpbMethod : public Method {
 public:
  template <auto kMethod, typename RequestType, typename ResponseType>
  static constexpr bool matches() {
    return std::conjunction_v<
        std::is_same<MethodImplementation<kMethod>, PwpbMethod>,
        std::is_same<RequestType, Request<kMethod>>,
        std::is_same<ResponseType, Response<kMethod>>>;
  }

  // Creates a PwpbMethod for a synchronous unary RPC.
  // TODO(b/234874001): Find a way to reduce the number of monomorphized copies
  // of this method.
  template <auto kMethod>
  static constexpr PwpbMethod SynchronousUnary(uint32_t id,
                                               const PwpbMethodSerde& serde) {
    // Define a wrapper around the user-defined function that takes the
    // request and response protobuf structs as byte spans, and calls the
    // implementation with the correct type.
    //
    // This wrapper is stored generically in the Function union, defined below.
    // In optimized builds, the compiler inlines the user-defined function into
    // this wrapper, elminating any overhead.
    constexpr SynchronousUnaryFunction wrapper =
        [](Service& service, const void* request, void* response) {
          return CallMethodImplFunction<kMethod>(
              service,
              *reinterpret_cast<const Request<kMethod>*>(request),
              *reinterpret_cast<Response<kMethod>*>(response));
        };
    return PwpbMethod(
        id,
        SynchronousUnaryInvoker<Request<kMethod>, Response<kMethod>>,
        Function{.synchronous_unary = wrapper},
        serde);
  }

  // Creates a PwpbMethod for an asynchronous unary RPC.
  // TODO(b/234874001): Find a way to reduce the number of monomorphized copies
  // of this method.
  template <auto kMethod>
  static constexpr PwpbMethod AsynchronousUnary(uint32_t id,
                                                const PwpbMethodSerde& serde) {
    // Define a wrapper around the user-defined function that takes the
    // request struct as a byte span, the response as a server call, and calls
    // the implementation with the correct types.
    //
    // This wrapper is stored generically in the Function union, defined below.
    // In optimized builds, the compiler inlines the user-defined function into
    // this wrapper, elminating any overhead.
    constexpr UnaryRequestFunction wrapper =
        [](Service& service,
           const void* request,
           internal::PwpbServerCall& writer) {
          return CallMethodImplFunction<kMethod>(
              service,
              *reinterpret_cast<const Request<kMethod>*>(request),
              static_cast<PwpbUnaryResponder<Response<kMethod>>&>(writer));
        };
    return PwpbMethod(id,
                      AsynchronousUnaryInvoker<Request<kMethod>>,
                      Function{.unary_request = wrapper},
                      serde);
  }

  // Creates a PwpbMethod for a server-streaming RPC.
  template <auto kMethod>
  static constexpr PwpbMethod ServerStreaming(uint32_t id,
                                              const PwpbMethodSerde& serde) {
    // Define a wrapper around the user-defined function that takes the
    // request struct as a byte span, the response as a server call, and calls
    // the implementation with the correct types.
    //
    // This wrapper is stored generically in the Function union, defined below.
    // In optimized builds, the compiler inlines the user-defined function into
    // this wrapper, elminating any overhead.
    constexpr UnaryRequestFunction wrapper =
        [](Service& service,
           const void* request,
           internal::PwpbServerCall& writer) {
          return CallMethodImplFunction<kMethod>(
              service,
              *reinterpret_cast<const Request<kMethod>*>(request),
              static_cast<PwpbServerWriter<Response<kMethod>>&>(writer));
        };
    return PwpbMethod(id,
                      ServerStreamingInvoker<Request<kMethod>>,
                      Function{.unary_request = wrapper},
                      serde);
  }

  // Creates a PwpbMethod for a client-streaming RPC.
  template <auto kMethod>
  static constexpr PwpbMethod ClientStreaming(uint32_t id,
                                              const PwpbMethodSerde& serde) {
    // Define a wrapper around the user-defined function that takes the
    // request as a server call, and calls the implementation with the correct
    // types.
    //
    // This wrapper is stored generically in the Function union, defined below.
    // In optimized builds, the compiler inlines the user-defined function into
    // this wrapper, elminating any overhead.
    constexpr StreamRequestFunction wrapper = [](Service& service,
                                                 internal::PwpbServerCall&
                                                     reader) {
      return CallMethodImplFunction<kMethod>(
          service,
          static_cast<PwpbServerReader<Request<kMethod>, Response<kMethod>>&>(
              reader));
    };
    return PwpbMethod(id,
                      ClientStreamingInvoker<Request<kMethod>>,
                      Function{.stream_request = wrapper},
                      serde);
  }

  // Creates a PwpbMethod for a bidirectional-streaming RPC.
  template <auto kMethod>
  static constexpr PwpbMethod BidirectionalStreaming(
      uint32_t id, const PwpbMethodSerde& serde) {
    // Define a wrapper around the user-defined function that takes the
    // request and response as a server call, and calls the implementation with
    // the correct types.
    //
    // This wrapper is stored generically in the Function union, defined below.
    // In optimized builds, the compiler inlines the user-defined function into
    // this wrapper, elminating any overhead.
    constexpr StreamRequestFunction wrapper =
        [](Service& service, internal::PwpbServerCall& reader_writer) {
          return CallMethodImplFunction<kMethod>(
              service,
              static_cast<
                  PwpbServerReaderWriter<Request<kMethod>, Response<kMethod>>&>(
                  reader_writer));
        };
    return PwpbMethod(id,
                      BidirectionalStreamingInvoker<Request<kMethod>>,
                      Function{.stream_request = wrapper},
                      serde);
  }

  // Represents an invalid method. Used to reduce error message verbosity.
  static constexpr PwpbMethod Invalid() {
    return {0, InvalidInvoker, {}, PwpbMethodSerde(nullptr, nullptr)};
  }

  // Give access to the serializer/deserializer object for converting requests
  // and responses between the wire format and pw_protobuf structs.
  const PwpbMethodSerde& serde() const { return serde_; }

 private:
  // Generic function signature for synchronous unary RPCs.
  using SynchronousUnaryFunction = Status (*)(Service&,
                                              const void* request,
                                              void* response);

  // Generic function signature for asynchronous unary and server streaming
  // RPCs.
  using UnaryRequestFunction = void (*)(Service&,
                                        const void* request,
                                        internal::PwpbServerCall& writer);

  // Generic function signature for client and bidirectional streaming RPCs.
  using StreamRequestFunction =
      void (*)(Service&, internal::PwpbServerCall& reader_writer);

  // The Function union stores a pointer to a generic version of the
  // user-defined RPC function. Using a union instead of void* avoids
  // reinterpret_cast, which keeps this class fully constexpr.
  union Function {
    SynchronousUnaryFunction synchronous_unary;
    UnaryRequestFunction unary_request;
    StreamRequestFunction stream_request;
  };

  constexpr PwpbMethod(uint32_t id,
                       Invoker invoker,
                       Function function,
                       const PwpbMethodSerde& serde)
      : Method(id, invoker), function_(function), serde_(serde) {}

  template <typename Request, typename Response>
  void CallSynchronousUnary(const CallContext& context,
                            const Packet& request,
                            Request& request_struct,
                            Response& response_struct) const
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    if (!DecodeRequest(context, request, request_struct).ok()) {
      rpc_lock().unlock();
      return;
    }

    internal::PwpbServerCall responder(context.ClaimLocked(),
                                       MethodType::kUnary);
    rpc_lock().unlock();
    const Status status = function_.synchronous_unary(
        context.service(), &request_struct, &response_struct);
    responder.SendUnaryResponse(response_struct, status).IgnoreError();
  }

  template <typename Request>
  void CallUnaryRequest(const CallContext& context,
                        MethodType method_type,
                        const Packet& request,
                        Request& request_struct) const
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    if (!DecodeRequest(context, request, request_struct).ok()) {
      rpc_lock().unlock();
      return;
    }

    internal::PwpbServerCall server_writer(context.ClaimLocked(), method_type);
    rpc_lock().unlock();
    function_.unary_request(context.service(), &request_struct, server_writer);
  }

  // Decodes a request protobuf into the provided buffer. Sends an error packet
  // if the request failed to decode.
  template <typename Request>
  Status DecodeRequest(const CallContext& context,
                       const Packet& request,
                       Request& request_struct) const
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock()) {
    const auto status = serde_.DecodeRequest(request.payload(), request_struct);
    if (status.ok()) {
      return status;
    }

    // The channel is known to exist. It was found when the request was
    // processed and the lock has been held since, so GetInternalChannel cannot
    // fail.
    context.server()
        .GetInternalChannel(context.channel_id())
        ->Send(Packet::ServerError(request, Status::DataLoss()))
        .IgnoreError();
    return status;
  }

  // Invoker function for synchronous unary RPCs.
  template <typename Request, typename Response>
  static void SynchronousUnaryInvoker(const CallContext& context,
                                      const Packet& request)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    Request request_struct{};
    Response response_struct{};

    static_cast<const PwpbMethod&>(context.method())
        .CallSynchronousUnary(
            context, request, request_struct, response_struct);
  }

  // Invoker function for asynchronous unary RPCs.
  template <typename Request>
  static void AsynchronousUnaryInvoker(const CallContext& context,
                                       const Packet& request)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    Request request_struct{};

    static_cast<const PwpbMethod&>(context.method())
        .CallUnaryRequest(context, MethodType::kUnary, request, request_struct);
  }

  // Invoker function for server streaming RPCs.
  template <typename Request>
  static void ServerStreamingInvoker(const CallContext& context,
                                     const Packet& request)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    Request request_struct{};

    static_cast<const PwpbMethod&>(context.method())
        .CallUnaryRequest(
            context, MethodType::kServerStreaming, request, request_struct);
  }

  // Invoker function for client streaming RPCs.
  template <typename Request>
  static void ClientStreamingInvoker(const CallContext& context, const Packet&)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    internal::BasePwpbServerReader<Request> reader(
        context.ClaimLocked(), MethodType::kClientStreaming);
    rpc_lock().unlock();
    static_cast<const PwpbMethod&>(context.method())
        .function_.stream_request(context.service(), reader);
  }

  // Invoker function for bidirectional streaming RPCs.
  template <typename Request>
  static void BidirectionalStreamingInvoker(const CallContext& context,
                                            const Packet&)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    internal::BasePwpbServerReader<Request> reader_writer(
        context.ClaimLocked(), MethodType::kBidirectionalStreaming);
    rpc_lock().unlock();
    static_cast<const PwpbMethod&>(context.method())
        .function_.stream_request(context.service(), reader_writer);
  }

  // Stores the user-defined RPC in a generic wrapper.
  Function function_;

  // Serde used to encode and decode pw_protobuf structs.
  const PwpbMethodSerde& serde_;
};

// MethodTraits specialization for a static synchronous unary method.
// TODO(b/234874320): Further qualify this (and nanopb) definition so that they
// can co-exist in the same project.
template <typename Req, typename Res>
struct MethodTraits<PwpbSynchronousUnary<Req, Res>*> {
  using Implementation = PwpbMethod;
  using Request = Req;
  using Response = Res;

  static constexpr MethodType kType = MethodType::kUnary;
  static constexpr bool kSynchronous = true;

  static constexpr bool kServerStreaming = false;
  static constexpr bool kClientStreaming = false;
};

// MethodTraits specialization for a synchronous raw unary method.
template <typename T, typename Req, typename Res>
struct MethodTraits<PwpbSynchronousUnary<Req, Res>(T::*)>
    : MethodTraits<PwpbSynchronousUnary<Req, Res>*> {
  using Service = T;
};

// MethodTraits specialization for a static asynchronous unary method.
template <typename Req, typename Resp>
struct MethodTraits<PwpbAsynchronousUnary<Req, Resp>*>
    : MethodTraits<PwpbSynchronousUnary<Req, Resp>*> {
  static constexpr bool kSynchronous = false;
};

// MethodTraits specialization for an asynchronous unary method.
template <typename T, typename Req, typename Resp>
struct MethodTraits<PwpbAsynchronousUnary<Req, Resp>(T::*)>
    : MethodTraits<PwpbSynchronousUnary<Req, Resp>(T::*)> {
  static constexpr bool kSynchronous = false;
};

// MethodTraits specialization for a static server streaming method.
template <typename Req, typename Resp>
struct MethodTraits<PwpbServerStreaming<Req, Resp>*> {
  using Implementation = PwpbMethod;
  using Request = Req;
  using Response = Resp;

  static constexpr MethodType kType = MethodType::kServerStreaming;
  static constexpr bool kServerStreaming = true;
  static constexpr bool kClientStreaming = false;
};

// MethodTraits specialization for a server streaming method.
template <typename T, typename Req, typename Resp>
struct MethodTraits<PwpbServerStreaming<Req, Resp>(T::*)>
    : MethodTraits<PwpbServerStreaming<Req, Resp>*> {
  using Service = T;
};

// MethodTraits specialization for a static server streaming method.
template <typename Req, typename Resp>
struct MethodTraits<PwpbClientStreaming<Req, Resp>*> {
  using Implementation = PwpbMethod;
  using Request = Req;
  using Response = Resp;

  static constexpr MethodType kType = MethodType::kClientStreaming;
  static constexpr bool kServerStreaming = false;
  static constexpr bool kClientStreaming = true;
};

// MethodTraits specialization for a server streaming method.
template <typename T, typename Req, typename Resp>
struct MethodTraits<PwpbClientStreaming<Req, Resp>(T::*)>
    : MethodTraits<PwpbClientStreaming<Req, Resp>*> {
  using Service = T;
};

// MethodTraits specialization for a static server streaming method.
template <typename Req, typename Resp>
struct MethodTraits<PwpbBidirectionalStreaming<Req, Resp>*> {
  using Implementation = PwpbMethod;
  using Request = Req;
  using Response = Resp;

  static constexpr MethodType kType = MethodType::kBidirectionalStreaming;
  static constexpr bool kServerStreaming = true;
  static constexpr bool kClientStreaming = true;
};

// MethodTraits specialization for a server streaming method.
template <typename T, typename Req, typename Resp>
struct MethodTraits<PwpbBidirectionalStreaming<Req, Resp>(T::*)>
    : MethodTraits<PwpbBidirectionalStreaming<Req, Resp>*> {
  using Service = T;
};

}  // namespace pw::rpc::internal
