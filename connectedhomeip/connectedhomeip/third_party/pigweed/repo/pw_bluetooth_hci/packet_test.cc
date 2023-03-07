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

#include "pw_bluetooth_hci/packet.h"

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_bytes/byte_builder.h"
#include "pw_containers/algorithm.h"
#include "pw_status/status.h"

namespace pw::bluetooth_hci {
namespace {

class PacketTest : public ::testing::Test {
 protected:
  constexpr static size_t kMaxHeaderSizeBytes = std::max({
      CommandPacket::kHeaderSizeBytes,
      AsyncDataPacket::kHeaderSizeBytes,
      SyncDataPacket::kHeaderSizeBytes,
      EventPacket::kHeaderSizeBytes,
  });
  // Arbitrarily add at most 2 bytes worth of payload (data or parameters).
  constexpr static size_t kArbitraryMaxPayloadSizeBytes = 2;
  constexpr static size_t kMaxPacketSizeBytes =
      kMaxHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes;
  std::array<std::byte, kMaxPacketSizeBytes> packet_buffer_;
};

TEST_F(PacketTest, CommandPacketHeaderUndersizedEncode) {
  const CommandPacket packet(0u, ConstByteSpan());
  EXPECT_EQ(0u, packet.parameters().size_bytes());
  const Result<ConstByteSpan> encode_result = packet.Encode(
      {packet_buffer_.data(), CommandPacket::kHeaderSizeBytes - 1});
  EXPECT_EQ(Status::ResourceExhausted(), encode_result.status());
}

TEST_F(PacketTest, CommandPacketHeaderUndersizedDecode) {
  EXPECT_FALSE(CommandPacket::Decode(
                   {packet_buffer_.data(), CommandPacket::kHeaderSizeBytes - 1})
                   .has_value());
}

TEST_F(PacketTest, CommandPacketHeaderOnlyEncodeAndDecode) {
  constexpr uint16_t kOpcodeCommandField = 0b00'0000'0000;
  constexpr uint8_t kOpcodeGroupField = 0b11'1111;

  constexpr uint16_t kOpcode = (kOpcodeGroupField << 10) | kOpcodeCommandField;

  const CommandPacket packet(kOpcode, ConstByteSpan());
  EXPECT_EQ(packet.type(), Packet::Type::kCommandPacket);
  EXPECT_EQ(packet.size_bytes(), CommandPacket::kHeaderSizeBytes);
  EXPECT_EQ(packet.opcode(), kOpcode);
  EXPECT_EQ(packet.opcode_command_field(), kOpcodeCommandField);
  EXPECT_EQ(packet.opcode_group_field(), kOpcodeGroupField);
  EXPECT_EQ(packet.parameters().size_bytes(), 0u);

  const Result<ConstByteSpan> encode_result = packet.Encode(packet_buffer_);
  ASSERT_EQ(OkStatus(), encode_result.status());

  constexpr std::array<const std::byte, CommandPacket::kHeaderSizeBytes>
      kExpectedEncodedPacket = bytes::MakeArray<const std::byte>(
          0b0000'0000, 0b1111'1100, 0b0000'0000);
  const ConstByteSpan& encoded_packet = encode_result.value();
  EXPECT_TRUE(pw::containers::Equal(kExpectedEncodedPacket, encoded_packet));

  // First, decode it from a perfectly sized span which we just encoded.
  std::optional<CommandPacket> possible_packet =
      CommandPacket::Decode(encoded_packet);
  ASSERT_TRUE(possible_packet.has_value());
  CommandPacket& decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kCommandPacket);
  EXPECT_EQ(decoded_packet.size_bytes(), CommandPacket::kHeaderSizeBytes);
  EXPECT_EQ(decoded_packet.opcode(), kOpcode);
  EXPECT_EQ(decoded_packet.opcode_command_field(), kOpcodeCommandField);
  EXPECT_EQ(decoded_packet.opcode_group_field(), kOpcodeGroupField);
  EXPECT_EQ(decoded_packet.parameters().size_bytes(), 0u);

  // Second, decode it from an oversized buffer.
  possible_packet = CommandPacket::Decode({packet_buffer_});
  ASSERT_TRUE(possible_packet.has_value());
  decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kCommandPacket);
  EXPECT_EQ(decoded_packet.size_bytes(), CommandPacket::kHeaderSizeBytes);
  EXPECT_EQ(decoded_packet.opcode(), kOpcode);
  EXPECT_EQ(decoded_packet.opcode_command_field(), kOpcodeCommandField);
  EXPECT_EQ(decoded_packet.opcode_group_field(), kOpcodeGroupField);
  EXPECT_EQ(decoded_packet.parameters().size_bytes(), 0u);
}

TEST_F(PacketTest, CommandPacketWithParametersEncodeAndDecode) {
  constexpr uint16_t kOpcodeCommandField = 0b10'1010'1010;
  constexpr uint8_t kOpcodeGroupField = 0b10'1010;

  constexpr uint16_t kOpcode = (kOpcodeGroupField << 10) | kOpcodeCommandField;

  constexpr std::array<const std::byte, kArbitraryMaxPayloadSizeBytes>
      kParameters = bytes::MakeArray<const std::byte>(1, 2);
  const CommandPacket packet(kOpcode, kParameters);
  EXPECT_EQ(packet.type(), Packet::Type::kCommandPacket);
  EXPECT_EQ(packet.size_bytes(),
            CommandPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(packet.opcode(), kOpcode);
  EXPECT_EQ(packet.opcode_command_field(), kOpcodeCommandField);
  EXPECT_EQ(packet.opcode_group_field(), kOpcodeGroupField);
  EXPECT_EQ(packet.parameters().size_bytes(), kArbitraryMaxPayloadSizeBytes);

  const Result<ConstByteSpan> encode_result = packet.Encode(packet_buffer_);
  ASSERT_EQ(OkStatus(), encode_result.status());

  constexpr std::array<const std::byte,
                       CommandPacket::kHeaderSizeBytes +
                           kArbitraryMaxPayloadSizeBytes>
      kExpectedEncodedPacket = bytes::MakeArray<const std::byte>(
          0b1010'1010, 0b1010'1010, 0b0000'0010, 1, 2);
  const ConstByteSpan& encoded_packet = encode_result.value();
  EXPECT_TRUE(pw::containers::Equal(kExpectedEncodedPacket, encoded_packet));

  // First, decode it from a perfectly sized span which we just encoded.
  std::optional<CommandPacket> possible_packet =
      CommandPacket::Decode(encoded_packet);
  ASSERT_TRUE(possible_packet.has_value());
  CommandPacket& decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kCommandPacket);
  EXPECT_EQ(decoded_packet.size_bytes(),
            CommandPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(decoded_packet.opcode(), kOpcode);
  EXPECT_EQ(decoded_packet.opcode_command_field(), kOpcodeCommandField);
  EXPECT_EQ(decoded_packet.opcode_group_field(), kOpcodeGroupField);
  EXPECT_EQ(decoded_packet.parameters().size_bytes(),
            kArbitraryMaxPayloadSizeBytes);

  // Second, decode it from an oversized buffer.
  possible_packet = CommandPacket::Decode({packet_buffer_});
  ASSERT_TRUE(possible_packet.has_value());
  decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kCommandPacket);
  EXPECT_EQ(decoded_packet.size_bytes(),
            CommandPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(decoded_packet.opcode(), kOpcode);
  EXPECT_EQ(decoded_packet.opcode_command_field(), kOpcodeCommandField);
  EXPECT_EQ(decoded_packet.opcode_group_field(), kOpcodeGroupField);
  EXPECT_EQ(decoded_packet.parameters().size_bytes(),
            kArbitraryMaxPayloadSizeBytes);
}

TEST_F(PacketTest, AsyncDataPacketHeaderUndersizedEncode) {
  const AsyncDataPacket packet(0u, ConstByteSpan());
  EXPECT_EQ(0u, packet.data().size_bytes());
  const Result<ConstByteSpan> encode_result = packet.Encode(
      {packet_buffer_.data(), AsyncDataPacket::kHeaderSizeBytes - 1});
  EXPECT_EQ(Status::ResourceExhausted(), encode_result.status());
}

TEST_F(PacketTest, AsyncDataPacketHeaderUndersizedDecode) {
  EXPECT_FALSE(AsyncDataPacket::Decode({packet_buffer_.data(),
                                        AsyncDataPacket::kHeaderSizeBytes - 1})
                   .has_value());
}

TEST_F(PacketTest, AsyncDataPacketHeaderOnlyEncodeAndDecode) {
  constexpr uint16_t kHandle = 0b00'0000'0000;
  constexpr uint8_t kPbFlag = 0b01;
  constexpr uint8_t kBcFlag = 0b10;

  constexpr uint16_t kHandleAndFragmentationBits =
      kHandle | (kPbFlag << 12) | (kBcFlag << 14);

  const AsyncDataPacket packet(kHandleAndFragmentationBits, ConstByteSpan());
  EXPECT_EQ(packet.type(), Packet::Type::kAsyncDataPacket);
  EXPECT_EQ(packet.size_bytes(), AsyncDataPacket::kHeaderSizeBytes);
  EXPECT_EQ(packet.handle_and_fragmentation_bits(),
            kHandleAndFragmentationBits);
  EXPECT_EQ(packet.handle(), kHandle);
  EXPECT_EQ(packet.pb_flag(), kPbFlag);
  EXPECT_EQ(packet.bc_flag(), kBcFlag);
  EXPECT_EQ(packet.data().size_bytes(), 0u);

  const Result<ConstByteSpan> encode_result = packet.Encode(packet_buffer_);
  ASSERT_EQ(OkStatus(), encode_result.status());

  constexpr std::array<const std::byte, AsyncDataPacket::kHeaderSizeBytes>
      kExpectedEncodedPacket = bytes::MakeArray<const std::byte>(
          0b0000'0000, 0b1001'0000, 0b0000'0000, 0b0000'0000);
  const ConstByteSpan& encoded_packet = encode_result.value();
  EXPECT_TRUE(pw::containers::Equal(kExpectedEncodedPacket, encoded_packet));

  // First, decode it from a perfectly sized span which we just encoded.
  std::optional<AsyncDataPacket> possible_packet =
      AsyncDataPacket::Decode(encoded_packet);
  ASSERT_TRUE(possible_packet.has_value());
  AsyncDataPacket& decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kAsyncDataPacket);
  EXPECT_EQ(decoded_packet.size_bytes(), AsyncDataPacket::kHeaderSizeBytes);
  EXPECT_EQ(decoded_packet.handle_and_fragmentation_bits(),
            kHandleAndFragmentationBits);
  EXPECT_EQ(decoded_packet.handle(), kHandle);
  EXPECT_EQ(decoded_packet.pb_flag(), kPbFlag);
  EXPECT_EQ(decoded_packet.bc_flag(), kBcFlag);
  EXPECT_EQ(decoded_packet.data().size_bytes(), 0u);

  // Second, decode it from an oversized buffer.
  possible_packet = AsyncDataPacket::Decode({packet_buffer_});
  ASSERT_TRUE(possible_packet.has_value());
  decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kAsyncDataPacket);
  EXPECT_EQ(decoded_packet.size_bytes(), AsyncDataPacket::kHeaderSizeBytes);
  EXPECT_EQ(decoded_packet.handle_and_fragmentation_bits(),
            kHandleAndFragmentationBits);
  EXPECT_EQ(decoded_packet.handle(), kHandle);
  EXPECT_EQ(decoded_packet.pb_flag(), kPbFlag);
  EXPECT_EQ(decoded_packet.bc_flag(), kBcFlag);
  EXPECT_EQ(decoded_packet.data().size_bytes(), 0u);
}

TEST_F(PacketTest, AsyncDataPacketWithDataEncodeAndDecode) {
  constexpr uint16_t kHandle = 0b00'0000'0000;
  constexpr uint8_t kPbFlag = 0b01;
  constexpr uint8_t kBcFlag = 0b10;

  constexpr uint16_t kHandleAndFragmentationBits =
      kHandle | (kPbFlag << 12) | (kBcFlag << 14);

  constexpr std::array<const std::byte, kArbitraryMaxPayloadSizeBytes> kData =
      bytes::MakeArray<const std::byte>(1, 2);
  const AsyncDataPacket packet(kHandleAndFragmentationBits, kData);
  EXPECT_EQ(packet.type(), Packet::Type::kAsyncDataPacket);
  EXPECT_EQ(packet.size_bytes(),
            AsyncDataPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(packet.handle_and_fragmentation_bits(),
            kHandleAndFragmentationBits);
  EXPECT_EQ(packet.handle(), kHandle);
  EXPECT_EQ(packet.pb_flag(), kPbFlag);
  EXPECT_EQ(packet.bc_flag(), kBcFlag);
  EXPECT_EQ(packet.data().size_bytes(), kArbitraryMaxPayloadSizeBytes);

  const Result<ConstByteSpan> encode_result = packet.Encode(packet_buffer_);
  ASSERT_EQ(OkStatus(), encode_result.status());

  constexpr std::array<const std::byte,
                       AsyncDataPacket::kHeaderSizeBytes +
                           kArbitraryMaxPayloadSizeBytes>
      kExpectedEncodedPacket = bytes::MakeArray<const std::byte>(
          0b0000'0000, 0b1001'0000, 0b0000'0010, 0b0000'0000, 1, 2);
  const ConstByteSpan& encoded_packet = encode_result.value();
  EXPECT_TRUE(pw::containers::Equal(kExpectedEncodedPacket, encoded_packet));

  // First, decode it from a perfectly sized span which we just encoded.
  std::optional<AsyncDataPacket> possible_packet =
      AsyncDataPacket::Decode(encoded_packet);
  ASSERT_TRUE(possible_packet.has_value());
  AsyncDataPacket& decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kAsyncDataPacket);
  EXPECT_EQ(decoded_packet.size_bytes(),
            AsyncDataPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(decoded_packet.handle_and_fragmentation_bits(),
            kHandleAndFragmentationBits);
  EXPECT_EQ(decoded_packet.handle(), kHandle);
  EXPECT_EQ(decoded_packet.pb_flag(), kPbFlag);
  EXPECT_EQ(decoded_packet.bc_flag(), kBcFlag);
  EXPECT_EQ(decoded_packet.data().size_bytes(), kArbitraryMaxPayloadSizeBytes);

  // Second, decode it from an oversized buffer.
  possible_packet = AsyncDataPacket::Decode({packet_buffer_});
  ASSERT_TRUE(possible_packet.has_value());
  decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kAsyncDataPacket);
  EXPECT_EQ(decoded_packet.size_bytes(),
            AsyncDataPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(decoded_packet.handle_and_fragmentation_bits(),
            kHandleAndFragmentationBits);
  EXPECT_EQ(decoded_packet.handle(), kHandle);
  EXPECT_EQ(decoded_packet.pb_flag(), kPbFlag);
  EXPECT_EQ(decoded_packet.bc_flag(), kBcFlag);
  EXPECT_EQ(decoded_packet.data().size_bytes(), kArbitraryMaxPayloadSizeBytes);
}

TEST_F(PacketTest, SyncDataPacketHeaderUndersizedEncode) {
  const SyncDataPacket packet(0u, ConstByteSpan());
  EXPECT_EQ(0u, packet.data().size_bytes());
  const Result<ConstByteSpan> encode_result = packet.Encode(
      {packet_buffer_.data(), SyncDataPacket::kHeaderSizeBytes - 1});
  EXPECT_EQ(Status::ResourceExhausted(), encode_result.status());
}

TEST_F(PacketTest, SyncDataPacketHeaderUndersizedDecode) {
  EXPECT_FALSE(SyncDataPacket::Decode({packet_buffer_.data(),
                                       SyncDataPacket::kHeaderSizeBytes - 1})
                   .has_value());
}

TEST_F(PacketTest, SyncDataPacketHeaderOnlyEncodeAndDecode) {
  constexpr uint16_t kHandle = 0b00'0000'0000;
  constexpr uint8_t kPacketStatusFlag = 0b11;
  constexpr uint8_t kReservedBits = 0;

  constexpr uint16_t kHandleAndStatusBits =
      kHandle | (kPacketStatusFlag << 12) | (kReservedBits << 14);

  const SyncDataPacket packet(kHandleAndStatusBits, ConstByteSpan());
  EXPECT_EQ(packet.type(), Packet::Type::kSyncDataPacket);
  EXPECT_EQ(packet.size_bytes(), SyncDataPacket::kHeaderSizeBytes);
  EXPECT_EQ(packet.handle_and_status_bits(), kHandleAndStatusBits);
  EXPECT_EQ(packet.handle(), kHandle);
  EXPECT_EQ(packet.packet_status_flag(), kPacketStatusFlag);
  EXPECT_EQ(packet.data().size_bytes(), 0u);

  const Result<ConstByteSpan> encode_result = packet.Encode(packet_buffer_);
  ASSERT_EQ(OkStatus(), encode_result.status());

  constexpr std::array<const std::byte, SyncDataPacket::kHeaderSizeBytes>
      kExpectedEncodedPacket = bytes::MakeArray<const std::byte>(
          0b0000'0000, 0b0011'0000, 0b0000'0000);
  const ConstByteSpan& encoded_packet = encode_result.value();
  EXPECT_TRUE(pw::containers::Equal(kExpectedEncodedPacket, encoded_packet));

  // First, decode it from a perfectly sized span which we just encoded.
  std::optional<SyncDataPacket> possible_packet =
      SyncDataPacket::Decode(encoded_packet);
  ASSERT_TRUE(possible_packet.has_value());
  SyncDataPacket& decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kSyncDataPacket);
  EXPECT_EQ(decoded_packet.size_bytes(), SyncDataPacket::kHeaderSizeBytes);
  EXPECT_EQ(packet.handle_and_status_bits(), kHandleAndStatusBits);
  EXPECT_EQ(decoded_packet.packet_status_flag(), kPacketStatusFlag);
  EXPECT_EQ(decoded_packet.handle(), kHandle);
  EXPECT_EQ(decoded_packet.data().size_bytes(), 0u);

  // Second, decode it from an oversized buffer.
  possible_packet = SyncDataPacket::Decode({packet_buffer_});
  ASSERT_TRUE(possible_packet.has_value());
  decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kSyncDataPacket);
  EXPECT_EQ(decoded_packet.size_bytes(), SyncDataPacket::kHeaderSizeBytes);
  EXPECT_EQ(packet.handle_and_status_bits(), kHandleAndStatusBits);
  EXPECT_EQ(decoded_packet.handle(), kHandle);
  EXPECT_EQ(decoded_packet.packet_status_flag(), kPacketStatusFlag);
  EXPECT_EQ(decoded_packet.data().size_bytes(), 0u);
}

TEST_F(PacketTest, SyncDataPacketWithDataEncodeAndDecode) {
  constexpr uint16_t kHandle = 0b00'0000'0000;
  constexpr uint8_t kPacketStatusFlag = 0b11;
  constexpr uint8_t kReservedBits = 0;

  constexpr uint16_t kHandleAndStatusBits =
      kHandle | (kPacketStatusFlag << 12) | (kReservedBits << 14);

  constexpr std::array<const std::byte, kArbitraryMaxPayloadSizeBytes> kData =
      bytes::MakeArray<const std::byte>(1, 2);
  const SyncDataPacket packet(kHandleAndStatusBits, kData);
  EXPECT_EQ(packet.type(), Packet::Type::kSyncDataPacket);
  EXPECT_EQ(packet.size_bytes(),
            SyncDataPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(packet.handle_and_status_bits(), kHandleAndStatusBits);
  EXPECT_EQ(packet.handle(), kHandle);
  EXPECT_EQ(packet.packet_status_flag(), kPacketStatusFlag);
  EXPECT_EQ(packet.data().size_bytes(), kArbitraryMaxPayloadSizeBytes);

  const Result<ConstByteSpan> encode_result = packet.Encode(packet_buffer_);
  ASSERT_EQ(OkStatus(), encode_result.status());

  constexpr std::array<const std::byte,
                       SyncDataPacket::kHeaderSizeBytes +
                           kArbitraryMaxPayloadSizeBytes>
      kExpectedEncodedPacket = bytes::MakeArray<const std::byte>(
          0b0000'0000, 0b0011'0000, 0b0000'0010, 1, 2);
  const ConstByteSpan& encoded_packet = encode_result.value();
  EXPECT_TRUE(pw::containers::Equal(kExpectedEncodedPacket, encoded_packet));

  // First, decode it from a perfectly sized span which we just encoded.
  std::optional<SyncDataPacket> possible_packet =
      SyncDataPacket::Decode(encoded_packet);
  ASSERT_TRUE(possible_packet.has_value());
  SyncDataPacket& decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kSyncDataPacket);
  EXPECT_EQ(decoded_packet.size_bytes(),
            SyncDataPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(packet.handle_and_status_bits(), kHandleAndStatusBits);
  EXPECT_EQ(packet.handle(), kHandle);
  EXPECT_EQ(packet.packet_status_flag(), kPacketStatusFlag);
  EXPECT_EQ(decoded_packet.data().size_bytes(), kArbitraryMaxPayloadSizeBytes);

  // Second, decode it from an oversized buffer.
  possible_packet = SyncDataPacket::Decode({packet_buffer_});
  ASSERT_TRUE(possible_packet.has_value());
  decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kSyncDataPacket);
  EXPECT_EQ(decoded_packet.size_bytes(),
            SyncDataPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(packet.handle_and_status_bits(), kHandleAndStatusBits);
  EXPECT_EQ(packet.handle(), kHandle);
  EXPECT_EQ(packet.packet_status_flag(), kPacketStatusFlag);
  EXPECT_EQ(decoded_packet.data().size_bytes(), kArbitraryMaxPayloadSizeBytes);
}

TEST_F(PacketTest, EventPacketHeaderUndersizedEncode) {
  const EventPacket packet(0u, ConstByteSpan());
  EXPECT_EQ(0u, packet.parameters().size_bytes());
  const Result<ConstByteSpan> encode_result =
      packet.Encode({packet_buffer_.data(), EventPacket::kHeaderSizeBytes - 1});
  EXPECT_EQ(Status::ResourceExhausted(), encode_result.status());
}

TEST_F(PacketTest, EventPacketHeaderUndersizedDecode) {
  EXPECT_FALSE(EventPacket::Decode(
                   {packet_buffer_.data(), EventPacket::kHeaderSizeBytes - 1})
                   .has_value());
}

TEST_F(PacketTest, EventPacketHeaderOnlyEncodeAndDecode) {
  constexpr uint8_t kEventCode = 0b1111'1111;

  const EventPacket packet(kEventCode, ConstByteSpan());
  EXPECT_EQ(packet.type(), Packet::Type::kEventPacket);
  EXPECT_EQ(packet.size_bytes(), EventPacket::kHeaderSizeBytes);
  EXPECT_EQ(packet.event_code(), kEventCode);
  EXPECT_EQ(packet.parameters().size_bytes(), 0u);

  const Result<ConstByteSpan> encode_result = packet.Encode(packet_buffer_);
  ASSERT_EQ(OkStatus(), encode_result.status());

  constexpr std::array<const std::byte, EventPacket::kHeaderSizeBytes>
      kExpectedEncodedPacket =
          bytes::MakeArray<const std::byte>(0b1111'11111, 0b0000'0000);
  const ConstByteSpan& encoded_packet = encode_result.value();
  EXPECT_TRUE(pw::containers::Equal(kExpectedEncodedPacket, encoded_packet));

  // First, decode it from a perfectly sized span which we just encoded.
  std::optional<EventPacket> possible_packet =
      EventPacket::Decode(encoded_packet);
  ASSERT_TRUE(possible_packet.has_value());
  EventPacket& decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kEventPacket);
  EXPECT_EQ(decoded_packet.size_bytes(), EventPacket::kHeaderSizeBytes);
  EXPECT_EQ(decoded_packet.event_code(), kEventCode);
  EXPECT_EQ(decoded_packet.parameters().size_bytes(), 0u);

  // Second, decode it from an oversized buffer.
  possible_packet = EventPacket::Decode({packet_buffer_});
  ASSERT_TRUE(possible_packet.has_value());
  decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kEventPacket);
  EXPECT_EQ(decoded_packet.size_bytes(), EventPacket::kHeaderSizeBytes);
  EXPECT_EQ(decoded_packet.event_code(), kEventCode);
  EXPECT_EQ(decoded_packet.parameters().size_bytes(), 0u);
}

TEST_F(PacketTest, EventPacketWithParametersEncodeAndDecode) {
  constexpr uint8_t kEventCode = 0b1111'0000;

  constexpr std::array<const std::byte, kArbitraryMaxPayloadSizeBytes>
      kParameters = bytes::MakeArray<const std::byte>(1, 2);
  const EventPacket packet(kEventCode, kParameters);
  EXPECT_EQ(packet.type(), Packet::Type::kEventPacket);
  EXPECT_EQ(packet.size_bytes(),
            EventPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(packet.event_code(), kEventCode);
  EXPECT_EQ(packet.parameters().size_bytes(), kArbitraryMaxPayloadSizeBytes);

  const Result<ConstByteSpan> encode_result = packet.Encode(packet_buffer_);
  ASSERT_EQ(OkStatus(), encode_result.status());

  constexpr std::array<const std::byte,
                       EventPacket::kHeaderSizeBytes +
                           kArbitraryMaxPayloadSizeBytes>
      kExpectedEncodedPacket =
          bytes::MakeArray<const std::byte>(0b1111'0000, 0b0000'0010, 1, 2);
  const ConstByteSpan& encoded_packet = encode_result.value();
  EXPECT_TRUE(pw::containers::Equal(kExpectedEncodedPacket, encoded_packet));

  // First, decode it from a perfectly sized span which we just encoded.
  std::optional<EventPacket> possible_packet =
      EventPacket::Decode(encoded_packet);
  ASSERT_TRUE(possible_packet.has_value());
  EventPacket& decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kEventPacket);
  EXPECT_EQ(decoded_packet.size_bytes(),
            EventPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(decoded_packet.event_code(), kEventCode);
  EXPECT_EQ(decoded_packet.parameters().size_bytes(),
            kArbitraryMaxPayloadSizeBytes);

  // Second, decode it from an oversized buffer.
  possible_packet = EventPacket::Decode({packet_buffer_});
  ASSERT_TRUE(possible_packet.has_value());
  decoded_packet = possible_packet.value();
  EXPECT_EQ(decoded_packet.type(), Packet::Type::kEventPacket);
  EXPECT_EQ(decoded_packet.size_bytes(),
            EventPacket::kHeaderSizeBytes + kArbitraryMaxPayloadSizeBytes);
  EXPECT_EQ(decoded_packet.event_code(), kEventCode);
  EXPECT_EQ(decoded_packet.parameters().size_bytes(),
            kArbitraryMaxPayloadSizeBytes);
}

}  // namespace
}  // namespace pw::bluetooth_hci
