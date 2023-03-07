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
#include <cstdint>
#include <type_traits>

#include "pw_bytes/span.h"
#include "pw_rpc/client.h"
#include "pw_rpc/internal/method_info.h"
#include "pw_rpc/internal/packet.h"
#include "pw_rpc/method_type.h"
#include "pw_rpc/raw/fake_channel_output.h"

namespace pw::rpc {

// TODO(b/234878467): Document the client testing APIs.

// Sends packets to an RPC client as if it were a pw_rpc server.
class FakeServer {
 public:
  constexpr FakeServer(internal::test::FakeChannelOutput& output,
                       Client& client,
                       uint32_t channel_id,
                       ByteSpan packet_buffer)
      : output_(output),
        client_(client),
        channel_id_(channel_id),
        packet_buffer_(packet_buffer) {}

  // Sends a response packet for a server or bidirectional streaming RPC to the
  // client.
  template <auto kMethod,
            typename = std::enable_if_t<
                HasServerStream(internal::MethodInfo<kMethod>::kType)>>
  void SendResponse(Status status,
                    std::optional<uint32_t> call_id = std::nullopt) const {
    SendPacket<kMethod>(
        internal::pwpb::PacketType::RESPONSE, {}, status, call_id);
  }

  // Sends a response packet for a unary or client streaming streaming RPC to
  // the client.
  template <auto kMethod,
            typename = std::enable_if_t<
                !HasServerStream(internal::MethodInfo<kMethod>::kType)>>
  void SendResponse(ConstByteSpan payload,
                    Status status,
                    std::optional<uint32_t> call_id = std::nullopt) const {
    SendPacket<kMethod>(
        internal::pwpb::PacketType::RESPONSE, payload, status, call_id);
  }

  // Sends a stream packet for a server or bidirectional streaming RPC to the
  // client.
  template <auto kMethod>
  void SendServerStream(ConstByteSpan payload,
                        std::optional<uint32_t> call_id = std::nullopt) const {
    static_assert(HasServerStream(internal::MethodInfo<kMethod>::kType),
                  "Only server and bidirectional streaming methods can receive "
                  "server stream packets");
    SendPacket<kMethod>(internal::pwpb::PacketType::SERVER_STREAM,
                        payload,
                        OkStatus(),
                        call_id);
  }

  // Sends a server error packet to the client.
  template <auto kMethod>
  void SendServerError(Status error,
                       std::optional<uint32_t> call_id = std::nullopt) const {
    SendPacket<kMethod>(
        internal::pwpb::PacketType::SERVER_ERROR, {}, error, call_id);
  }

 private:
  template <auto kMethod>
  void SendPacket(internal::pwpb::PacketType type,
                  ConstByteSpan payload,
                  Status status,
                  std::optional<uint32_t> call_id) const {
    using Info = internal::MethodInfo<kMethod>;
    CheckProcessPacket(
        type, Info::kServiceId, Info::kMethodId, call_id, payload, status);
  }

  void CheckProcessPacket(internal::pwpb::PacketType type,
                          uint32_t service_id,
                          uint32_t method_id,
                          std::optional<uint32_t> call_id,
                          ConstByteSpan payload,
                          Status status) const;

  Status ProcessPacket(internal::pwpb::PacketType type,
                       uint32_t service_id,
                       uint32_t method_id,
                       std::optional<uint32_t> call_id,
                       ConstByteSpan payload,
                       Status status) const;

  internal::test::FakeChannelOutput& output_;
  Client& client_;
  const uint32_t channel_id_;
  ByteSpan packet_buffer_;  // For encoding packets sent by the server
};

// Instantiates a FakeServer, Client, Channel, and RawFakeChannelOutput for
// testing RPC client calls. These components may be used individually, but are
// instantiated together for convenience.
template <size_t kMaxPackets = 10,
          size_t kPacketEncodeBufferSizeBytes = 128,
          size_t kPayloadsBufferSizeBytes = 256>
class RawClientTestContext {
 public:
  static constexpr uint32_t kDefaultChannelId = 1;

  constexpr RawClientTestContext()
      : channel_(Channel::Create<kDefaultChannelId>(&channel_output_)),
        client_(span(&channel_, 1)),
        packet_buffer_{},
        fake_server_(
            channel_output_, client_, kDefaultChannelId, packet_buffer_) {}

  const Channel& channel() const { return channel_; }
  Channel& channel() { return channel_; }

  const FakeServer& server() const { return fake_server_; }
  FakeServer& server() { return fake_server_; }

  const Client& client() const { return client_; }
  Client& client() { return client_; }

  const auto& output() const { return channel_output_; }
  auto& output() { return channel_output_; }

 private:
  RawFakeChannelOutput<kMaxPackets, kPayloadsBufferSizeBytes> channel_output_;
  Channel channel_;
  Client client_;
  std::byte packet_buffer_[kPacketEncodeBufferSizeBytes];
  FakeServer fake_server_;
};

}  // namespace pw::rpc
