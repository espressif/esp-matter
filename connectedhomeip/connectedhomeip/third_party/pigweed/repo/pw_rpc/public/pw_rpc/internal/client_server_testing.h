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

#include <cinttypes>

#include "pw_rpc/channel.h"
#include "pw_rpc/client_server.h"
#include "pw_rpc/internal/fake_channel_output.h"
#include "pw_rpc/internal/lock.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::rpc {
namespace internal {

// Expands on a Fake Channel Output implementation to allow for forwarding of
// packets.
template <typename FakeChannelOutputImpl,
          size_t kOutputSize,
          size_t kMaxPackets,
          size_t kPayloadsBufferSizeBytes>
class ForwardingChannelOutput : public ChannelOutput {
 public:
  size_t MaximumTransmissionUnit() override {
    return output_.MaximumTransmissionUnit();
  }

  Status Send(span<const std::byte> buffer) override {
    return output_.Send(buffer);
  }

  // Returns true if new packets were available to forward
  bool ForwardNextPacket(ClientServer& client_server) {
    std::array<std::byte, kOutputSize> packet_buffer;
    Result<ConstByteSpan> result = EncodeNextUnsentPacket(packet_buffer);
    if (!result.ok()) {
      return false;
    }
    ++sent_packets_;
    const auto process_result = client_server.ProcessPacket(*result);
    PW_ASSERT(process_result.ok());
    return true;
  }

 protected:
  constexpr ForwardingChannelOutput()
      : ChannelOutput("testing::FakeChannelOutput") {}

  FakeChannelOutputImpl output_;

  // Functions are virtual to allow for their override in threaded version, so
  // threading protection can be added.
  virtual size_t PacketCount() const { return output_.total_packets(); }

  virtual Result<ConstByteSpan> EncodeNextUnsentPacket(
      std::array<std::byte, kPayloadsBufferSizeBytes>& packet_buffer) {
    pw::rpc::internal::LockGuard lock(output_.mutex_);
    const auto& packets = output_.packets();
    if (packets.size() <= sent_packets_) {
      return Status::NotFound();
    }
    return packets[sent_packets_].Encode(packet_buffer);
  }

  uint16_t sent_packets_ = 0;
};

// Provides a testing context with a real client and server
template <typename ForwardingChannelOutputImpl,
          size_t kOutputSize = 128,
          size_t kMaxPackets = 16,
          size_t kPayloadsBufferSizeBytes = 128>
class ClientServerTestContext {
 public:
  const pw::rpc::Channel& channel() { return channel_; }
  Client& client() { return client_server_.client(); }
  Server& server() { return client_server_.server(); }

  // Should be called after each rpc call to synchronously forward all queued
  // messages. Otherwise this function can be ignored.
  void ForwardNewPackets() {
    while (channel_output_.ForwardNextPacket(client_server_)) {
    }
  }

 protected:
  explicit ClientServerTestContext()
      : channel_(Channel::Create<1>(&channel_output_)),
        client_server_({&channel_, 1}) {}

  ~ClientServerTestContext() = default;

  ForwardingChannelOutputImpl channel_output_;

 private:
  pw::rpc::Channel channel_;
  ClientServer client_server_;
};

}  // namespace internal
}  // namespace pw::rpc
