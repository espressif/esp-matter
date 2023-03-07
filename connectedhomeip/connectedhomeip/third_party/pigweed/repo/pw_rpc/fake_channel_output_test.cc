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

#include "pw_rpc/internal/fake_channel_output.h"

#include <array>
#include <cstddef>
#include <memory>

#include "gtest/gtest.h"
#include "pw_rpc/internal/channel.h"
#include "pw_rpc/internal/lock.h"
#include "pw_rpc/internal/packet.h"

namespace pw::rpc::internal::test {
namespace {

constexpr uint32_t kChannelId = 1;
constexpr uint32_t kServiceId = 1;
constexpr uint32_t kMethodId = 1;
constexpr uint32_t kCallId = 0;
constexpr std::array<std::byte, 3> kPayload = {
    std::byte(1), std::byte(2), std::byte(3)};

class TestFakeChannelOutput final : public FakeChannelOutputBuffer<9, 128> {
 public:
  TestFakeChannelOutput() = default;

  const ConstByteSpan& last_response(MethodType type) {
    return payloads(type, kChannelId, kServiceId, kMethodId).back();
  }

  size_t total_payloads(MethodType type) {
    return payloads(type, kChannelId, kServiceId, kMethodId).size();
  }
};

TEST(FakeChannelOutput, SendAndClear) {
  constexpr MethodType type = MethodType::kServerStreaming;
  TestFakeChannelOutput output;
  Channel channel(kChannelId, &output);
  const internal::Packet server_stream_packet(pwpb::PacketType::SERVER_STREAM,
                                              kChannelId,
                                              kServiceId,
                                              kMethodId,
                                              kCallId,
                                              kPayload);
  LockGuard lock(rpc_lock());
  ASSERT_EQ(channel.Send(server_stream_packet), OkStatus());
  ASSERT_EQ(output.last_response(type).size(), kPayload.size());
  EXPECT_EQ(
      std::memcmp(
          output.last_response(type).data(), kPayload.data(), kPayload.size()),
      0);
  EXPECT_EQ(output.total_payloads(type), 1u);
  EXPECT_EQ(output.total_packets(), 1u);
  EXPECT_FALSE(output.done());

  output.clear();
  EXPECT_EQ(output.total_payloads(type), 0u);
  EXPECT_EQ(output.total_packets(), 0u);
  EXPECT_FALSE(output.done());
}

TEST(FakeChannelOutput, SendAndFakeFutureResults) {
  constexpr MethodType type = MethodType::kUnary;
  TestFakeChannelOutput output;
  Channel channel(kChannelId, &output);
  const internal::Packet response_packet(pwpb::PacketType::RESPONSE,
                                         kChannelId,
                                         kServiceId,
                                         kMethodId,
                                         kCallId,
                                         kPayload);
  LockGuard lock(rpc_lock());
  EXPECT_EQ(channel.Send(response_packet), OkStatus());
  EXPECT_EQ(output.total_payloads(type), 1u);
  EXPECT_EQ(output.total_packets(), 1u);
  EXPECT_TRUE(output.done());

  // Multiple calls will return the same error status.
  output.set_send_status(Status::Unknown());
  EXPECT_EQ(channel.Send(response_packet), Status::Unknown());
  EXPECT_EQ(channel.Send(response_packet), Status::Unknown());
  EXPECT_EQ(channel.Send(response_packet), Status::Unknown());
  EXPECT_EQ(output.total_payloads(type), 1u);
  EXPECT_EQ(output.total_packets(), 1u);

  // Turn off error status behavior.
  output.set_send_status(OkStatus());
  EXPECT_EQ(channel.Send(response_packet), OkStatus());
  EXPECT_EQ(output.total_payloads(type), 2u);
  EXPECT_EQ(output.total_packets(), 2u);

  const internal::Packet server_stream_packet(pwpb::PacketType::SERVER_STREAM,
                                              kChannelId,
                                              kServiceId,
                                              kMethodId,
                                              kCallId,
                                              kPayload);
  EXPECT_EQ(channel.Send(server_stream_packet), OkStatus());
  ASSERT_EQ(output.last_response(type).size(), kPayload.size());
  EXPECT_EQ(
      std::memcmp(
          output.last_response(type).data(), kPayload.data(), kPayload.size()),
      0);
  EXPECT_EQ(output.total_payloads(type), 2u);
  EXPECT_EQ(output.total_packets(), 3u);
  EXPECT_TRUE(output.done());
}

TEST(FakeChannelOutput, SendAndFakeSingleResult) {
  constexpr MethodType type = MethodType::kUnary;
  TestFakeChannelOutput output;
  Channel channel(kChannelId, &output);
  const internal::Packet response_packet(pwpb::PacketType::RESPONSE,
                                         kChannelId,
                                         kServiceId,
                                         kMethodId,
                                         kCallId,
                                         kPayload);
  // Multiple calls will return the same error status.
  const int packet_count_fail = 4;
  output.set_send_status(Status::Unknown(), packet_count_fail);
  LockGuard lock(rpc_lock());

  for (int i = 0; i < packet_count_fail; ++i) {
    EXPECT_EQ(channel.Send(response_packet), OkStatus());
  }
  EXPECT_EQ(channel.Send(response_packet), Status::Unknown());
  for (int i = 0; i < packet_count_fail; ++i) {
    EXPECT_EQ(channel.Send(response_packet), OkStatus());
  }

  const size_t total_response_packets =
      static_cast<size_t>(2 * packet_count_fail);
  EXPECT_EQ(output.total_payloads(type), total_response_packets);
  EXPECT_EQ(output.total_packets(), total_response_packets);

  // Turn off error status behavior.
  output.set_send_status(OkStatus());
  EXPECT_EQ(channel.Send(response_packet), OkStatus());
  EXPECT_EQ(output.total_payloads(type), total_response_packets + 1);
  EXPECT_EQ(output.total_packets(), total_response_packets + 1);
}

TEST(FakeChannelOutput, SendResponseUpdated) {
  TestFakeChannelOutput output;
  Channel channel(kChannelId, &output);
  const internal::Packet response_packet(pwpb::PacketType::RESPONSE,
                                         kChannelId,
                                         kServiceId,
                                         kMethodId,
                                         kCallId,
                                         kPayload);
  LockGuard lock(rpc_lock());
  ASSERT_EQ(channel.Send(response_packet), OkStatus());
  ASSERT_EQ(output.last_response(MethodType::kUnary).size(), kPayload.size());
  EXPECT_EQ(std::memcmp(output.last_response(MethodType::kUnary).data(),
                        kPayload.data(),
                        kPayload.size()),
            0);
  EXPECT_EQ(output.total_payloads(MethodType::kUnary), 1u);
  EXPECT_EQ(output.total_packets(), 1u);
  EXPECT_TRUE(output.done());

  output.clear();
  const internal::Packet packet_empty_payload(pwpb::PacketType::RESPONSE,
                                              kChannelId,
                                              kServiceId,
                                              kMethodId,
                                              kCallId,
                                              {});
  EXPECT_EQ(channel.Send(packet_empty_payload), OkStatus());
  EXPECT_EQ(output.last_response(MethodType::kUnary).size(), 0u);
  EXPECT_EQ(output.total_payloads(MethodType::kUnary), 1u);
  EXPECT_EQ(output.total_packets(), 1u);
  EXPECT_TRUE(output.done());

  const internal::Packet server_stream_packet(pwpb::PacketType::SERVER_STREAM,
                                              kChannelId,
                                              kServiceId,
                                              kMethodId,
                                              kCallId,
                                              kPayload);
  ASSERT_EQ(channel.Send(server_stream_packet), OkStatus());
  ASSERT_EQ(output.total_payloads(MethodType::kServerStreaming), 1u);
  ASSERT_EQ(output.last_response(MethodType::kServerStreaming).size(),
            kPayload.size());
  EXPECT_EQ(
      std::memcmp(output.last_response(MethodType::kServerStreaming).data(),
                  kPayload.data(),
                  kPayload.size()),
      0);
  EXPECT_EQ(output.total_payloads(MethodType::kServerStreaming), 1u);
  EXPECT_EQ(output.total_packets(), 2u);
  EXPECT_TRUE(output.done());
}

}  // namespace
}  // namespace pw::rpc::internal::test
