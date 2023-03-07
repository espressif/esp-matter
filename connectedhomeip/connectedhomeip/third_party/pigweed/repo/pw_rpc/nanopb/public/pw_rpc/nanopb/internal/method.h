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
#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "pw_function/function.h"
#include "pw_rpc/internal/config.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/internal/method.h"
#include "pw_rpc/method_type.h"
#include "pw_rpc/nanopb/internal/common.h"
#include "pw_rpc/nanopb/server_reader_writer.h"
#include "pw_span/span.h"
#include "pw_status/status.h"
#include "pw_status/status_with_size.h"

namespace pw::rpc::internal {

class NanopbMethod;
class Packet;

// Expected function signatures for user-implemented RPC functions.
template <typename Request, typename Response>
using NanopbSynchronousUnary = Status(const Request&, Response&);

template <typename Request, typename Response>
using NanopbAsynchronousUnary = void(const Request&,
                                     NanopbUnaryResponder<Response>&);

template <typename Request, typename Response>
using NanopbServerStreaming = void(const Request&,
                                   NanopbServerWriter<Response>&);

template <typename Request, typename Response>
using NanopbClientStreaming = void(NanopbServerReader<Request, Response>&);

template <typename Request, typename Response>
using NanopbBidirectionalStreaming =
    void(NanopbServerReaderWriter<Request, Response>&);

// MethodTraits specialization for a static synchronous unary method.
template <typename Req, typename Resp>
struct MethodTraits<NanopbSynchronousUnary<Req, Resp>*> {
  using Implementation = NanopbMethod;
  using Request = Req;
  using Response = Resp;

  static constexpr MethodType kType = MethodType::kUnary;
  static constexpr bool kSynchronous = true;

  static constexpr bool kServerStreaming = false;
  static constexpr bool kClientStreaming = false;
};

// MethodTraits specialization for a synchronous unary method.
template <typename T, typename Req, typename Resp>
struct MethodTraits<NanopbSynchronousUnary<Req, Resp>(T::*)>
    : MethodTraits<NanopbSynchronousUnary<Req, Resp>*> {
  using Service = T;
};

// MethodTraits specialization for a static asynchronous unary method.
template <typename Req, typename Resp>
struct MethodTraits<NanopbAsynchronousUnary<Req, Resp>*>
    : MethodTraits<NanopbSynchronousUnary<Req, Resp>*> {
  static constexpr bool kSynchronous = false;
};

// MethodTraits specialization for an asynchronous unary method.
template <typename T, typename Req, typename Resp>
struct MethodTraits<NanopbAsynchronousUnary<Req, Resp>(T::*)>
    : MethodTraits<NanopbSynchronousUnary<Req, Resp>(T::*)> {
  static constexpr bool kSynchronous = false;
};

// MethodTraits specialization for a static server streaming method.
template <typename Req, typename Resp>
struct MethodTraits<NanopbServerStreaming<Req, Resp>*> {
  using Implementation = NanopbMethod;
  using Request = Req;
  using Response = Resp;

  static constexpr MethodType kType = MethodType::kServerStreaming;
  static constexpr bool kServerStreaming = true;
  static constexpr bool kClientStreaming = false;
};

// MethodTraits specialization for a server streaming method.
template <typename T, typename Req, typename Resp>
struct MethodTraits<NanopbServerStreaming<Req, Resp>(T::*)>
    : MethodTraits<NanopbServerStreaming<Req, Resp>*> {
  using Service = T;
};

// MethodTraits specialization for a static server streaming method.
template <typename Req, typename Resp>
struct MethodTraits<NanopbClientStreaming<Req, Resp>*> {
  using Implementation = NanopbMethod;
  using Request = Req;
  using Response = Resp;

  static constexpr MethodType kType = MethodType::kClientStreaming;
  static constexpr bool kServerStreaming = false;
  static constexpr bool kClientStreaming = true;
};

// MethodTraits specialization for a server streaming method.
template <typename T, typename Req, typename Resp>
struct MethodTraits<NanopbClientStreaming<Req, Resp>(T::*)>
    : MethodTraits<NanopbClientStreaming<Req, Resp>*> {
  using Service = T;
};

// MethodTraits specialization for a static server streaming method.
template <typename Req, typename Resp>
struct MethodTraits<NanopbBidirectionalStreaming<Req, Resp>*> {
  using Implementation = NanopbMethod;
  using Request = Req;
  using Response = Resp;

  static constexpr MethodType kType = MethodType::kBidirectionalStreaming;
  static constexpr bool kServerStreaming = true;
  static constexpr bool kClientStreaming = true;
};

// MethodTraits specialization for a server streaming method.
template <typename T, typename Req, typename Resp>
struct MethodTraits<NanopbBidirectionalStreaming<Req, Resp>(T::*)>
    : MethodTraits<NanopbBidirectionalStreaming<Req, Resp>*> {
  using Service = T;
};

// The NanopbMethod class invokes user-defined service methods. When a
// pw::rpc::Server receives an RPC request packet, it looks up the matching
// NanopbMethod instance and calls its Invoke method, which eventually calls
// into the user-defined RPC function.
//
// A NanopbMethod instance is created for each user-defined RPC in the pw_rpc
// generated code. The NanopbMethod stores a pointer to the RPC function, a
// pointer to an "invoker" function that calls that function, and pointers to
// the Nanopb descriptors used to encode and decode request and response
// structs.
class NanopbMethod : public Method {
 public:
  template <auto kMethod, typename RequestType, typename ResponseType>
  static constexpr bool matches() {
    return std::is_same_v<MethodImplementation<kMethod>, NanopbMethod> &&
           std::is_same_v<RequestType, Request<kMethod>> &&
           std::is_same_v<ResponseType, Response<kMethod>>;
  }

  // Creates a NanopbMethod for a synchronous unary RPC.
  template <auto kMethod>
  static constexpr NanopbMethod SynchronousUnary(
      uint32_t id, const NanopbMethodSerde& serde) {
    // Define a wrapper around the user-defined function that takes the
    // request and response protobuf structs as void*. This wrapper is stored
    // generically in the Function union, defined below.
    //
    // In optimized builds, the compiler inlines the user-defined function into
    // this wrapper, elminating any overhead.
    constexpr SynchronousUnaryFunction wrapper =
        [](Service& service, const void* req, void* resp) {
          return CallMethodImplFunction<kMethod>(
              service,
              *static_cast<const Request<kMethod>*>(req),
              *static_cast<Response<kMethod>*>(resp));
        };
    return NanopbMethod(
        id,
        SynchronousUnaryInvoker<AllocateSpaceFor<Request<kMethod>>(),
                                AllocateSpaceFor<Response<kMethod>>()>,
        Function{.synchronous_unary = wrapper},
        serde);
  }

  // Creates a NanopbMethod for an asynchronous unary RPC.
  template <auto kMethod>
  static constexpr NanopbMethod AsynchronousUnary(
      uint32_t id, const NanopbMethodSerde& serde) {
    // Define a wrapper around the user-defined function that takes the
    // request and response protobuf structs as void*. This wrapper is stored
    // generically in the Function union, defined below.
    //
    // In optimized builds, the compiler inlines the user-defined function into
    // this wrapper, elminating any overhead.
    constexpr UnaryRequestFunction wrapper =
        [](Service& service, const void* req, NanopbServerCall& resp) {
          return CallMethodImplFunction<kMethod>(
              service,
              *static_cast<const Request<kMethod>*>(req),
              static_cast<NanopbUnaryResponder<Response<kMethod>>&>(resp));
        };
    return NanopbMethod(
        id,
        AsynchronousUnaryInvoker<AllocateSpaceFor<Request<kMethod>>()>,
        Function{.unary_request = wrapper},
        serde);
  }

  // Creates a NanopbMethod for a server-streaming RPC.
  template <auto kMethod>
  static constexpr NanopbMethod ServerStreaming(
      uint32_t id, const NanopbMethodSerde& serde) {
    // Define a wrapper around the user-defined function that takes the request
    // struct as void* and a NanopbServerCall instead of the
    // templated NanopbServerWriter class. This wrapper is stored generically in
    // the Function union, defined below.
    constexpr UnaryRequestFunction wrapper =
        [](Service& service, const void* req, NanopbServerCall& writer) {
          return CallMethodImplFunction<kMethod>(
              service,
              *static_cast<const Request<kMethod>*>(req),
              static_cast<NanopbServerWriter<Response<kMethod>>&>(writer));
        };
    return NanopbMethod(
        id,
        ServerStreamingInvoker<AllocateSpaceFor<Request<kMethod>>()>,
        Function{.unary_request = wrapper},
        serde);
  }

  // Creates a NanopbMethod for a client-streaming RPC.
  template <auto kMethod>
  static constexpr NanopbMethod ClientStreaming(
      uint32_t id, const NanopbMethodSerde& serde) {
    constexpr StreamRequestFunction wrapper = [](Service& service,
                                                 NanopbServerCall& reader) {
      return CallMethodImplFunction<kMethod>(
          service,
          static_cast<NanopbServerReader<Request<kMethod>, Response<kMethod>>&>(
              reader));
    };
    return NanopbMethod(id,
                        ClientStreamingInvoker<Request<kMethod>>,
                        Function{.stream_request = wrapper},
                        serde);
  }

  // Creates a NanopbMethod for a bidirectional-streaming RPC.
  template <auto kMethod>
  static constexpr NanopbMethod BidirectionalStreaming(
      uint32_t id, const NanopbMethodSerde& serde) {
    constexpr StreamRequestFunction wrapper =
        [](Service& service, NanopbServerCall& reader_writer) {
          return CallMethodImplFunction<kMethod>(
              service,
              static_cast<NanopbServerReaderWriter<Request<kMethod>,
                                                   Response<kMethod>>&>(
                  reader_writer));
        };
    return NanopbMethod(id,
                        BidirectionalStreamingInvoker<Request<kMethod>>,
                        Function{.stream_request = wrapper},
                        serde);
  }

  // Represents an invalid method. Used to reduce error message verbosity.
  static constexpr NanopbMethod Invalid() {
    return {0, InvalidInvoker, {}, NanopbMethodSerde(nullptr, nullptr)};
  }

  // Give access to the serializer/deserializer object for converting requests
  // and responses between the wire format and Nanopb structs.
  const NanopbMethodSerde& serde() const { return serde_; }

 private:
  // Generic function signature for synchronous unary RPCs.
  using SynchronousUnaryFunction = Status (*)(Service&,
                                              const void* request,
                                              void* response);

  // Generic function signature for asynchronous unary and server streaming
  // RPCs.
  using UnaryRequestFunction = void (*)(Service&,
                                        const void* request,
                                        NanopbServerCall& writer);

  // Generic function signature for client and bidirectional streaming RPCs.
  using StreamRequestFunction = void (*)(Service&,
                                         NanopbServerCall& reader_writer);

  // The Function union stores a pointer to a generic version of the
  // user-defined RPC function. Using a union instead of void* avoids
  // reinterpret_cast, which keeps this class fully constexpr.
  union Function {
    SynchronousUnaryFunction synchronous_unary;
    UnaryRequestFunction unary_request;
    StreamRequestFunction stream_request;
  };

  // Allocates space for a struct. Rounds up to a reasonable minimum size to
  // avoid generating unnecessary copies of the invoker functions.
  template <typename T>
  static constexpr size_t AllocateSpaceFor() {
    return std::max(sizeof(T), cfg::kNanopbStructMinBufferSize);
  }

  constexpr NanopbMethod(uint32_t id,
                         Invoker invoker,
                         Function function,
                         const NanopbMethodSerde& serde)
      : Method(id, invoker), function_(function), serde_(serde) {}

  void CallSynchronousUnary(const CallContext& context,
                            const Packet& request,
                            void* request_struct,
                            void* response_struct) const
      PW_UNLOCK_FUNCTION(rpc_lock());

  void CallUnaryRequest(const CallContext& context,
                        MethodType type,
                        const Packet& request,
                        void* request_struct) const
      PW_UNLOCK_FUNCTION(rpc_lock());

  // Invoker function for synchronous unary RPCs. Allocates request and response
  // structs by size, with maximum alignment, to avoid generating unnecessary
  // copies of this function for each request/response type.
  template <size_t kRequestSize, size_t kResponseSize>
  static void SynchronousUnaryInvoker(const CallContext& context,
                                      const Packet& request)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    _PW_RPC_NANOPB_STRUCT_STORAGE_CLASS
    std::aligned_storage_t<kRequestSize, alignof(std::max_align_t)>
        request_struct{};
    _PW_RPC_NANOPB_STRUCT_STORAGE_CLASS
    std::aligned_storage_t<kResponseSize, alignof(std::max_align_t)>
        response_struct{};

    static_cast<const NanopbMethod&>(context.method())
        .CallSynchronousUnary(
            context, request, &request_struct, &response_struct);
  }

  // Invoker function for asynchronous unary RPCs. Allocates space for a request
  // struct. Ignores the payload buffer since resposnes are sent through the
  // NanopbUnaryResponder.
  template <size_t kRequestSize>
  static void AsynchronousUnaryInvoker(const CallContext& context,
                                       const Packet& request)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    _PW_RPC_NANOPB_STRUCT_STORAGE_CLASS
    std::aligned_storage_t<kRequestSize, alignof(std::max_align_t)>
        request_struct{};

    static_cast<const NanopbMethod&>(context.method())
        .CallUnaryRequest(
            context, MethodType::kUnary, request, &request_struct);
  }

  // Invoker function for server streaming RPCs. Allocates space for a request
  // struct. Ignores the payload buffer since resposnes are sent through the
  // NanopbServerWriter.
  template <size_t kRequestSize>
  static void ServerStreamingInvoker(const CallContext& context,
                                     const Packet& request)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    _PW_RPC_NANOPB_STRUCT_STORAGE_CLASS
    std::aligned_storage_t<kRequestSize, alignof(std::max_align_t)>
        request_struct{};

    static_cast<const NanopbMethod&>(context.method())
        .CallUnaryRequest(
            context, MethodType::kServerStreaming, request, &request_struct);
  }

  // Invoker function for client streaming RPCs.
  template <typename Request>
  static void ClientStreamingInvoker(const CallContext& context, const Packet&)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    BaseNanopbServerReader<Request> reader(context.ClaimLocked(),
                                           MethodType::kClientStreaming);
    rpc_lock().unlock();
    static_cast<const NanopbMethod&>(context.method())
        .function_.stream_request(context.service(), reader);
  }

  // Invoker function for bidirectional streaming RPCs.
  template <typename Request>
  static void BidirectionalStreamingInvoker(const CallContext& context,
                                            const Packet&)
      PW_UNLOCK_FUNCTION(rpc_lock()) {
    BaseNanopbServerReader<Request> reader_writer(
        context.ClaimLocked(), MethodType::kBidirectionalStreaming);
    rpc_lock().unlock();
    static_cast<const NanopbMethod&>(context.method())
        .function_.stream_request(context.service(), reader_writer);
  }

  // Decodes a request protobuf with Nanopb to the provided buffer. Sends an
  // error packet if the request failed to decode.
  bool DecodeRequest(const CallContext& context,
                     const Packet& request,
                     void* proto_struct) const
      PW_EXCLUSIVE_LOCKS_REQUIRED(rpc_lock());

  // Stores the user-defined RPC in a generic wrapper.
  Function function_;

  // Serde used to encode and decode Nanopb structs.
  const NanopbMethodSerde& serde_;
};

}  // namespace pw::rpc::internal
