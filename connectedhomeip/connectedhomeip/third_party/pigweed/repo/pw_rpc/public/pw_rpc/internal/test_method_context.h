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

#include <cstddef>

#include "pw_assert/assert.h"
#include "pw_rpc/channel.h"
#include "pw_rpc/internal/call.h"
#include "pw_rpc/internal/fake_channel_output.h"
#include "pw_rpc/internal/method.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/server.h"

namespace pw::rpc::internal::test {

// Collects everything needed to invoke a particular RPC.
template <typename Output, typename Service, uint32_t kMethodId>
class InvocationContext {
 public:
  InvocationContext(const InvocationContext&) = delete;
  InvocationContext(InvocationContext&&) = delete;

  InvocationContext& operator=(const InvocationContext&) = delete;
  InvocationContext& operator=(InvocationContext&&) = delete;

  Service& service() { return service_; }
  const Service& service() const { return service_; }

  // Sets the channel ID, which defaults to an arbitrary value.
  void set_channel_id(uint32_t channel_id) {
    PW_ASSERT(channel_id != Channel::kUnassignedChannelId);

    // If using dynamic allocation, the channel objects are owned by the
    // endpoint. The external channel is only used to initialize the endpoint's
    // channels vector. To update that channel, remove and re-add the channel.
    PW_ASSERT(server_.CloseChannel(context_.channel_id()).ok());
    PW_ASSERT(server_.OpenChannel(channel_id, output_).ok());

    channel_ = Channel(channel_id, &output_);
    context_.set_channel_id(channel_id);
  }

  size_t total_responses() const { return responses().size(); }

  size_t max_packets() const { return output_.max_packets(); }

  // Returns the responses that have been recorded. The maximum number of
  // responses is responses().max_size(). responses().back() is always the most
  // recent response, even if total_responses() > responses().max_size().
  auto responses() const {
    return output().payloads(method_type_,
                             channel_.id(),
                             UnwrapServiceId(service().service_id()),
                             kMethodId);
  }

  // True if the RPC has completed.
  bool done() const { return output_.done(); }

  // The status of the stream. Only valid if done() is true.
  Status status() const {
    PW_ASSERT(done());
    return output_.last_status();
  }

  void SendClientError(Status error) {
    using PacketType = ::pw::rpc::internal::pwpb::PacketType;

    std::byte packet[kNoPayloadPacketSizeBytes];
    PW_ASSERT(server_
                  .ProcessPacket(Packet(PacketType::CLIENT_ERROR,
                                        channel_.id(),
                                        UnwrapServiceId(service_.service_id()),
                                        kMethodId,
                                        0,
                                        {},
                                        error)
                                     .Encode(packet)
                                     .value())
                  .ok());
  }

  const Output& output() const { return output_; }
  Output& output() { return output_; }

 protected:
  // Constructs the invocation context. The args for the ChannelOutput type are
  // passed in a std::tuple. The args for the Service are forwarded directly
  // from the callsite.
  template <typename... ServiceArgs>
  InvocationContext(const Method& method,
                    MethodType method_type,
                    ServiceArgs&&... service_args)
      : method_type_(method_type),
        channel_(123, &output_),
        server_(span(static_cast<rpc::Channel*>(&channel_), 1)),
        service_(std::forward<ServiceArgs>(service_args)...),
        context_(server_, channel_.id(), service_, method, 0) {
    server_.RegisterService(service_);
  }

  uint32_t channel_id() const { return channel_.id(); }

  template <size_t kMaxPayloadSize = 32>
  void SendClientStream(ConstByteSpan payload) {
    using PacketType = ::pw::rpc::internal::pwpb::PacketType;

    std::byte packet[kNoPayloadPacketSizeBytes + 3 + kMaxPayloadSize];
    PW_ASSERT(server_
                  .ProcessPacket(Packet(PacketType::CLIENT_STREAM,
                                        channel_.id(),
                                        UnwrapServiceId(service_.service_id()),
                                        kMethodId,
                                        0,
                                        payload)
                                     .Encode(packet)
                                     .value())
                  .ok());
  }

  void SendClientStreamEnd() {
    using PacketType = ::pw::rpc::internal::pwpb::PacketType;

    std::byte packet[kNoPayloadPacketSizeBytes];
    PW_ASSERT(server_
                  .ProcessPacket(Packet(PacketType::CLIENT_STREAM_END,
                                        channel_.id(),
                                        UnwrapServiceId(service_.service_id()),
                                        kMethodId)
                                     .Encode(packet)
                                     .value())
                  .ok());
  }

  // Invokes the RPC, optionally with a request argument.
  template <auto kMethod, typename T, typename... RequestArg>
  void call(RequestArg&&... request) PW_LOCKS_EXCLUDED(rpc_lock()) {
    static_assert(sizeof...(request) <= 1);
    output_.clear();
    T responder = GetResponder<T>();
    CallMethodImplFunction<kMethod>(
        service(), std::forward<RequestArg>(request)..., responder);
  }

  template <typename T>
  T GetResponder() PW_LOCKS_EXCLUDED(rpc_lock()) {
    LockGuard lock(rpc_lock());
    return T(call_context().ClaimLocked());
  }

  const internal::CallContext& call_context() const { return context_; }

 private:
  static constexpr size_t kNoPayloadPacketSizeBytes =
      2 /* type */ + 2 /* channel */ + 5 /* service */ + 5 /* method */ +
      2 /* status */;

  const MethodType method_type_;
  Output output_;
  Channel channel_;
  rpc::Server server_;
  Service service_;
  internal::CallContext context_;
};

}  // namespace pw::rpc::internal::test
