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

#include "pw_rpc/internal/packet.h"

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_protobuf/wire_format.h"

namespace pw::rpc::internal {
namespace {

using protobuf::FieldKey;
using ::pw::rpc::internal::pwpb::PacketType;
using std::byte;

constexpr auto kPayload = bytes::Array<0x82, 0x02, 0xff, 0xff>();

constexpr auto kEncoded = bytes::Array<
    // Payload
    uint32_t(FieldKey(5, protobuf::WireType::kDelimited)),
    0x04,
    0x82,
    0x02,
    0xff,
    0xff,

    // Packet type
    uint32_t(FieldKey(1, protobuf::WireType::kVarint)),
    1,  // RESPONSE

    // Channel ID
    uint32_t(FieldKey(2, protobuf::WireType::kVarint)),
    1,

    // Service ID
    uint32_t(FieldKey(3, protobuf::WireType::kFixed32)),
    42,
    0,
    0,
    0,

    // Method ID
    uint32_t(FieldKey(4, protobuf::WireType::kFixed32)),
    100,
    0,
    0,
    0,

    // Status (not encoded if it is zero)
    // FieldKey(6, protobuf::WireType::kVarint),
    // 0x00

    // Call ID
    uint32_t(FieldKey(7, protobuf::WireType::kVarint)),
    7>();

// Test that a default-constructed packet sets its members to the default
// protobuf values.
static_assert(Packet().type() == PacketType{});
static_assert(Packet().channel_id() == 0);
static_assert(Packet().service_id() == 0);
static_assert(Packet().method_id() == 0);
static_assert(Packet().status() == static_cast<Status::Code>(0));
static_assert(Packet().payload().empty());

TEST(Packet, Encode) {
  byte buffer[64];

  Packet packet(PacketType::RESPONSE, 1, 42, 100, 7, kPayload);

  auto result = packet.Encode(buffer);
  ASSERT_EQ(OkStatus(), result.status());
  ASSERT_EQ(kEncoded.size(), result.value().size());
  EXPECT_EQ(std::memcmp(kEncoded.data(), buffer, kEncoded.size()), 0);
}

TEST(Packet, Encode_BufferTooSmall) {
  byte buffer[2];

  Packet packet(PacketType::RESPONSE, 1, 42, 100, 0, kPayload);

  auto result = packet.Encode(buffer);
  EXPECT_EQ(Status::ResourceExhausted(), result.status());
}

TEST(Packet, Decode_ValidPacket) {
  auto result = Packet::FromBuffer(kEncoded);
  ASSERT_TRUE(result.ok());

  auto& packet = result.value();
  EXPECT_EQ(PacketType::RESPONSE, packet.type());
  EXPECT_EQ(1u, packet.channel_id());
  EXPECT_EQ(42u, packet.service_id());
  EXPECT_EQ(100u, packet.method_id());
  EXPECT_EQ(7u, packet.call_id());
  ASSERT_EQ(sizeof(kPayload), packet.payload().size());
  EXPECT_EQ(
      0,
      std::memcmp(packet.payload().data(), kPayload.data(), kPayload.size()));
}

TEST(Packet, Decode_InvalidPacket) {
  byte bad_data[] = {byte{0xFF}, byte{0x00}, byte{0x00}, byte{0xFF}};
  EXPECT_EQ(Status::DataLoss(), Packet::FromBuffer(bad_data).status());
}

TEST(Packet, EncodeDecode) {
  constexpr byte payload[]{byte(0x00), byte(0x01), byte(0x02), byte(0x03)};

  Packet packet;
  packet.set_channel_id(12);
  packet.set_service_id(0xdeadbeef);
  packet.set_method_id(0x03a82921);
  packet.set_call_id(33);
  packet.set_payload(payload);
  packet.set_status(Status::Unavailable());

  byte buffer[128];
  Result result = packet.Encode(buffer);
  ASSERT_EQ(result.status(), OkStatus());

  span<byte> packet_data(buffer, result.value().size());
  auto decode_result = Packet::FromBuffer(packet_data);
  ASSERT_TRUE(decode_result.ok());

  auto& decoded = decode_result.value();
  EXPECT_EQ(decoded.type(), packet.type());
  EXPECT_EQ(decoded.channel_id(), packet.channel_id());
  EXPECT_EQ(decoded.service_id(), packet.service_id());
  EXPECT_EQ(decoded.method_id(), packet.method_id());
  EXPECT_EQ(decoded.call_id(), packet.call_id());
  ASSERT_EQ(decoded.payload().size(), packet.payload().size());
  EXPECT_EQ(std::memcmp(decoded.payload().data(),
                        packet.payload().data(),
                        packet.payload().size()),
            0);
  EXPECT_EQ(decoded.status(), Status::Unavailable());
}

constexpr size_t kReservedSize = 2 /* type */ + 2 /* channel */ +
                                 5 /* service */ + 5 /* method */ +
                                 2 /* payload key */ + 2 /* status */;

TEST(Packet, PayloadUsableSpace_ExactFit) {
  EXPECT_EQ(kReservedSize,
            Packet(PacketType::RESPONSE, 1, 42, 100).MinEncodedSizeBytes());
}

TEST(Packet, PayloadUsableSpace_LargerVarints) {
  EXPECT_EQ(
      kReservedSize + 2 /* channel */,  // service and method are Fixed32
      Packet(PacketType::RESPONSE, 17000, 200, 200).MinEncodedSizeBytes());
}

}  // namespace
}  // namespace pw::rpc::internal
