// Copyright 2020 The Pigweed Authors
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

#include <type_traits>

#include "pw_assert/assert.h"
#include "pw_containers/vector.h"
#include "pw_preprocessor/arguments.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/internal/hash.h"
#include "pw_rpc/internal/method_lookup.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/internal/test_method_context.h"
#include "pw_rpc/raw/fake_channel_output.h"
#include "pw_rpc/raw/internal/method.h"

namespace pw::rpc {

// Declares a context object that may be used to invoke an RPC. The context is
// declared with the name of the implemented service and the method to invoke.
// The RPC can then be invoked with the call method.
//
// For a unary RPC, context.call(request) returns the status, and the response
// struct can be accessed via context.response().
//
//   PW_RAW_TEST_METHOD_CONTEXT(my::CoolService, TheMethod) context;
//   EXPECT_EQ(OkStatus(), context.call(encoded_request).status());
//   EXPECT_EQ(0,
//             std::memcmp(encoded_response,
//                         context.response().data(),
//                         sizeof(encoded_response)));
//
// For a server streaming RPC, context.call(request) invokes the method. As in a
// normal RPC, the method completes when the ServerWriter's Finish method is
// called (or it goes out of scope).
//
//   PW_RAW_TEST_METHOD_CONTEXT(my::CoolService, TheStreamingMethod) context;
//   context.call(encoded_response);
//
//   EXPECT_TRUE(context.done());  // Check that the RPC completed
//   EXPECT_EQ(OkStatus(), context.status());  // Check the status
//
//   EXPECT_EQ(3u, context.responses().size());
//   ByteSpan& response = context.responses()[0];  // check individual responses
//
//   for (ByteSpan& response : context.responses()) {
//     // iterate over the responses
//   }
//
// PW_RAW_TEST_METHOD_CONTEXT forwards its constructor arguments to the
// underlying service. For example:
//
//   PW_RAW_TEST_METHOD_CONTEXT(MyService, Go) context(service, args);
//
// PW_RAW_TEST_METHOD_CONTEXT takes one optional arguments:
//
//   size_t kMaxPackets: maximum packets to store
//
// Example:
//
//   PW_RAW_TEST_METHOD_CONTEXT(MyService, BestMethod, 3, 256) context;
//   ASSERT_EQ(3u, context.responses().max_size());
//
#define PW_RAW_TEST_METHOD_CONTEXT(service, method, ...)             \
  ::pw::rpc::RawTestMethodContext<service,                           \
                                  &service::method,                  \
                                  ::pw::rpc::internal::Hash(#method) \
                                      PW_COMMA_ARGS(__VA_ARGS__)>
template <typename Service,
          auto kMethod,
          uint32_t kMethodId,
          size_t kMaxPackets = 6>
class RawTestMethodContext;

// Internal classes that implement RawTestMethodContext.
namespace internal::test::raw {

inline constexpr size_t kPayloadsBufferSizeBytes = 256;

// Collects everything needed to invoke a particular RPC.
template <typename Service,
          auto kMethod,
          uint32_t kMethodId,
          size_t kMaxPackets>
class RawInvocationContext
    : public InvocationContext<
          RawFakeChannelOutput<kMaxPackets, kPayloadsBufferSizeBytes>,
          Service,
          kMethodId> {
 public:
  // Gives access to the RPC's most recent response.
  const ConstByteSpan& response() const { return Base::responses().back(); }

 protected:
  template <typename... Args>
  RawInvocationContext(Args&&... args)
      : Base(MethodLookup::GetRawMethod<Service, kMethodId>(),
             MethodTraits<decltype(kMethod)>::kType,
             std::forward<Args>(args)...) {}

 private:
  using Base = InvocationContext<
      RawFakeChannelOutput<kMaxPackets, kPayloadsBufferSizeBytes>,
      Service,
      kMethodId>;
};

// Method invocation context for a unary RPC. Returns the status in call() and
// provides the response through the response() method.
template <typename Service, auto kMethod, uint32_t kMethodId>
class UnaryContext
    : public RawInvocationContext<Service, kMethod, kMethodId, 1> {
  using Base = RawInvocationContext<Service, kMethod, kMethodId, 1>;

 public:
  template <typename... Args>
  UnaryContext(Args&&... args) : Base(std::forward<Args>(args)...) {}

  // Invokes the RPC with the provided request. Returns RPC's StatusWithSize.
  template <size_t kSynchronousResponseBufferSizeBytes = 64>
  auto call(ConstByteSpan request) {
    if constexpr (MethodTraits<decltype(kMethod)>::kSynchronous) {
      Base::output().clear();

      auto responder = Base::template GetResponder<RawUnaryResponder>();
      std::byte response[kSynchronousResponseBufferSizeBytes] = {};
      auto sws = CallMethodImplFunction<kMethod>(
          Base::service(), request, span(response));
      PW_ASSERT(responder.Finish(span(response).first(sws.size()), sws.status())
                    .ok());
      return sws;
    } else {
      Base::template call<kMethod, RawUnaryResponder>(request);
    }
  }
};

// Method invocation context for a server streaming RPC.
template <typename Service,
          auto kMethod,
          uint32_t kMethodId,
          size_t kMaxPackets>
class ServerStreamingContext
    : public RawInvocationContext<Service, kMethod, kMethodId, kMaxPackets> {
  using Base = RawInvocationContext<Service, kMethod, kMethodId, kMaxPackets>;

 public:
  template <typename... Args>
  ServerStreamingContext(Args&&... args) : Base(std::forward<Args>(args)...) {}

  // Invokes the RPC with the provided request.
  void call(ConstByteSpan request) {
    Base::template call<kMethod, RawServerWriter>(request);
  }

  // Returns a server writer which writes responses into the context's buffer.
  // This should not be called alongside call(); use one or the other.
  RawServerWriter writer() {
    return Base::template GetResponder<RawServerWriter>();
  }
};

// Method invocation context for a client streaming RPC.
template <typename Service,
          auto kMethod,
          uint32_t kMethodId,
          size_t kMaxPackets>
class ClientStreamingContext
    : public RawInvocationContext<Service, kMethod, kMethodId, kMaxPackets> {
  using Base = RawInvocationContext<Service, kMethod, kMethodId, kMaxPackets>;

 public:
  template <typename... Args>
  ClientStreamingContext(Args&&... args) : Base(std::forward<Args>(args)...) {}

  // Invokes the RPC.
  void call() { Base::template call<kMethod, RawServerReader>(); }

  // Returns a reader/writer which writes responses into the context's buffer.
  // This should not be called alongside call(); use one or the other.
  RawServerReader reader() {
    return Base::template GetResponder<RawServerReader>();
  }

  // Allow sending client streaming packets.
  using Base::SendClientStream;
  using Base::SendClientStreamEnd;
};

// Method invocation context for a bidirectional streaming RPC.
template <typename Service,
          auto kMethod,
          uint32_t kMethodId,
          size_t kMaxPackets>
class BidirectionalStreamingContext
    : public RawInvocationContext<Service, kMethod, kMethodId, kMaxPackets> {
  using Base = RawInvocationContext<Service, kMethod, kMethodId, kMaxPackets>;

 public:
  template <typename... Args>
  BidirectionalStreamingContext(Args&&... args)
      : Base(std::forward<Args>(args)...) {}

  // Invokes the RPC.
  void call() { Base::template call<kMethod, RawServerReaderWriter>(); }

  // Returns a reader/writer which writes responses into the context's buffer.
  // This should not be called alongside call(); use one or the other.
  RawServerReaderWriter reader_writer() {
    return Base::template GetResponder<RawServerReaderWriter>();
  }

  // Allow sending client streaming packets.
  using Base::SendClientStream;
  using Base::SendClientStreamEnd;
};

// Alias to select the type of the context object to use based on which type of
// RPC it is for.
template <typename Service,
          auto kMethod,
          uint32_t kMethodId,
          size_t kMaxPackets>
using Context = std::tuple_element_t<
    static_cast<size_t>(MethodTraits<decltype(kMethod)>::kType),
    std::tuple<UnaryContext<Service, kMethod, kMethodId>,
               ServerStreamingContext<Service, kMethod, kMethodId, kMaxPackets>,
               ClientStreamingContext<Service, kMethod, kMethodId, kMaxPackets>,
               BidirectionalStreamingContext<Service,
                                             kMethod,
                                             kMethodId,
                                             kMaxPackets>>>;

}  // namespace internal::test::raw

template <typename Service,
          auto kMethod,
          uint32_t kMethodId,
          size_t kMaxPackets>
class RawTestMethodContext
    : public internal::test::raw::
          Context<Service, kMethod, kMethodId, kMaxPackets> {
 public:
  // Forwards constructor arguments to the service class.
  template <typename... ServiceArgs>
  RawTestMethodContext(ServiceArgs&&... service_args)
      : internal::test::raw::Context<Service, kMethod, kMethodId, kMaxPackets>(
            std::forward<ServiceArgs>(service_args)...) {}
};

}  // namespace pw::rpc
