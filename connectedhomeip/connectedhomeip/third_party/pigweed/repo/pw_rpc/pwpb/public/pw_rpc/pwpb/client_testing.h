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

#include "pw_bytes/span.h"
#include "pw_rpc/client.h"
#include "pw_rpc/internal/method_info.h"
#include "pw_rpc/pwpb/fake_channel_output.h"
#include "pw_rpc/raw/client_testing.h"

namespace pw::rpc {

// TODO(b/234878467): Document the client testing APIs.

// Sends packets to an RPC client as if it were a pw_rpc server. Accepts
// payloads as pw_protobuf message structs.
class PwpbFakeServer : public FakeServer {
 private:
  template <auto kMethod>
  using Response = typename internal::MethodInfo<kMethod>::Response;

 public:
  using FakeServer::FakeServer;

  // Sends a response packet for a server or bidirectional streaming RPC to the
  // client.
  template <auto kMethod>
  void SendResponse(Status status) const {
    FakeServer::SendResponse<kMethod>(status);
  }

  // Sends a response packet for a unary or client streaming streaming RPC to
  // the client.
  template <auto kMethod,
            size_t kEncodeBufferSizeBytes = 2 * sizeof(Response<kMethod>)>
  void SendResponse(const Response<kMethod>& payload, Status status) const {
    std::byte buffer[kEncodeBufferSizeBytes] = {};
    FakeServer::SendResponse<kMethod>(EncodeResponse<kMethod>(payload, buffer),
                                      status);
  }

  // Sends a stream packet for a server or bidirectional streaming RPC to the
  // client.
  template <auto kMethod,
            size_t kEncodeBufferSizeBytes = 2 * sizeof(Response<kMethod>)>
  void SendServerStream(const Response<kMethod>& payload) const {
    std::byte buffer[kEncodeBufferSizeBytes] = {};
    FakeServer::SendServerStream<kMethod>(
        EncodeResponse<kMethod>(payload, buffer));
  }

 private:
  template <auto kMethod>
  static ConstByteSpan EncodeResponse(const Response<kMethod>& payload,
                                      ByteSpan buffer) {
    const StatusWithSize result =
        internal::MethodInfo<kMethod>::serde().EncodeResponse(payload, buffer);
    PW_ASSERT(result.ok());
    return span(buffer).first(result.size());
  }
};

// Instantiates a PwpbFakeServer, Client, Channel, and PwpbFakeChannelOutput
// for testing RPC client calls. These components may be used individually, but
// are instantiated together for convenience.
template <size_t kMaxPackets = 10,
          size_t kPacketEncodeBufferSizeBytes = 128,
          size_t kPayloadsBufferSizeBytes = 256>
class PwpbClientTestContext {
 public:
  constexpr PwpbClientTestContext()
      : channel_(Channel::Create<kDefaultChannelId>(&channel_output_)),
        client_(span(&channel_, 1)),
        packet_buffer_{},
        fake_server_(
            channel_output_, client_, kDefaultChannelId, packet_buffer_) {}

  const Channel& channel() const { return channel_; }
  Channel& channel() { return channel_; }

  const PwpbFakeServer& server() const { return fake_server_; }
  PwpbFakeServer& server() { return fake_server_; }

  const Client& client() const { return client_; }
  Client& client() { return client_; }

  const auto& output() const { return channel_output_; }
  auto& output() { return channel_output_; }

 private:
  static constexpr uint32_t kDefaultChannelId = 1;

  PwpbFakeChannelOutput<kMaxPackets, kPayloadsBufferSizeBytes> channel_output_;
  Channel channel_;
  Client client_;
  std::byte packet_buffer_[kPacketEncodeBufferSizeBytes];
  PwpbFakeServer fake_server_;
};

}  // namespace pw::rpc
