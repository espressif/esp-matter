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

#include "pw_bluetooth_hci/uart_transport.h"

#include "gtest/gtest.h"
#include "pw_bluetooth_hci/packet.h"
#include "pw_bytes/byte_builder.h"
#include "pw_status/status.h"

namespace pw::bluetooth_hci {
namespace {

class UartTransportTest : public ::testing::Test {
 protected:
  constexpr static std::byte kInvalidPacketIndicator = std::byte{0x0};
  static_assert(kInvalidPacketIndicator != kUartCommandPacketIndicator);
  static_assert(kInvalidPacketIndicator != kUartAsyncDataPacketIndicator);
  static_assert(kInvalidPacketIndicator != kUartSyncDataPacketIndicator);
  static_assert(kInvalidPacketIndicator != kUartEventPacketIndicator);

  constexpr static size_t kUartBufferSizeBytes = 256;
  ByteBuffer<kUartBufferSizeBytes> uart_buffer_;
};

TEST_F(UartTransportTest, EmptyBuffer) {
  const StatusWithSize status_with_size =
      DecodeHciUartData(ConstByteSpan(), [](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), 0u);
}

TEST_F(UartTransportTest, InvalidPacketIndicator) {
  uart_buffer_.push_back(kInvalidPacketIndicator);
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), Status::DataLoss());
  EXPECT_EQ(status_with_size.size(), 1u);
}

TEST_F(UartTransportTest, CommandPacketIndicatorOnly) {
  uart_buffer_.push_back(kUartCommandPacketIndicator);
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), 0u);
}

TEST_F(UartTransportTest, CommandPacketPartialPacket) {
  uart_buffer_.push_back(kUartCommandPacketIndicator);

  std::array<std::byte, CommandPacket::kHeaderSizeBytes> packet_buffer;
  CommandPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());

  uart_buffer_.append(result.value().first(result.value().size_bytes() - 1));
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), 0u);
}

TEST_F(UartTransportTest, CommandPacket) {
  uart_buffer_.push_back(kUartCommandPacketIndicator);

  std::array<std::byte, CommandPacket::kHeaderSizeBytes> packet_buffer;
  CommandPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());

  uart_buffer_.append(result.value());
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  size_t command_packet_count = 0;
  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [&](const Packet& decoded_packet) {
        ASSERT_EQ(decoded_packet.type(), Packet::Type::kCommandPacket);
        ++command_packet_count;
      });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), uart_buffer_.size());
  EXPECT_EQ(command_packet_count, 1u);
}

TEST_F(UartTransportTest, AsyncDataPacketIndicatorOnly) {
  uart_buffer_.push_back(kUartAsyncDataPacketIndicator);
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), 0u);
}

TEST_F(UartTransportTest, AsyncDataPacketPartialPacket) {
  uart_buffer_.push_back(kUartAsyncDataPacketIndicator);

  std::array<std::byte, AsyncDataPacket::kHeaderSizeBytes> packet_buffer;
  AsyncDataPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());

  uart_buffer_.append(result.value().first(result.value().size_bytes() - 1));
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), 0u);
}

TEST_F(UartTransportTest, AsyncDataPacket) {
  uart_buffer_.push_back(kUartAsyncDataPacketIndicator);

  std::array<std::byte, AsyncDataPacket::kHeaderSizeBytes> packet_buffer;
  AsyncDataPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());

  uart_buffer_.append(result.value());
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  size_t async_data_packet_count = 0;
  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [&](const Packet& decoded_packet) {
        ASSERT_EQ(decoded_packet.type(), Packet::Type::kAsyncDataPacket);
        ++async_data_packet_count;
      });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), uart_buffer_.size());
  EXPECT_EQ(async_data_packet_count, 1u);
}

TEST_F(UartTransportTest, SyncDataPacketIndicatorOnly) {
  uart_buffer_.push_back(kUartSyncDataPacketIndicator);
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), 0u);
}

TEST_F(UartTransportTest, SyncDataPacketPartialPacket) {
  uart_buffer_.push_back(kUartSyncDataPacketIndicator);

  std::array<std::byte, SyncDataPacket::kHeaderSizeBytes> packet_buffer;
  SyncDataPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());

  uart_buffer_.append(result.value().first(result.value().size_bytes() - 1));
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), 0u);
}

TEST_F(UartTransportTest, SyncDataPacket) {
  uart_buffer_.push_back(kUartSyncDataPacketIndicator);

  std::array<std::byte, SyncDataPacket::kHeaderSizeBytes> packet_buffer;
  SyncDataPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());

  uart_buffer_.append(result.value());
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  size_t sync_data_packet_count = 0;
  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [&](const Packet& decoded_packet) {
        ASSERT_EQ(decoded_packet.type(), Packet::Type::kSyncDataPacket);
        ++sync_data_packet_count;
      });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), uart_buffer_.size());
  EXPECT_EQ(sync_data_packet_count, 1u);
}

TEST_F(UartTransportTest, EventPacketIndicatorOnly) {
  uart_buffer_.push_back(kUartEventPacketIndicator);
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), 0u);
}

TEST_F(UartTransportTest, EventPacketPartialPacket) {
  uart_buffer_.push_back(kUartEventPacketIndicator);

  std::array<std::byte, EventPacket::kHeaderSizeBytes> packet_buffer;
  EventPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());

  uart_buffer_.append(result.value().first(result.value().size_bytes() - 1));
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), 0u);
}

TEST_F(UartTransportTest, EventPacket) {
  uart_buffer_.push_back(kUartEventPacketIndicator);

  std::array<std::byte, EventPacket::kHeaderSizeBytes> packet_buffer;
  EventPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());

  uart_buffer_.append(result.value());
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  size_t event_packet_count = 0;
  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [&](const Packet& decoded_packet) {
        ASSERT_EQ(decoded_packet.type(), Packet::Type::kEventPacket);
        ++event_packet_count;
      });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), uart_buffer_.size());
  EXPECT_EQ(event_packet_count, 1u);
}

TEST_F(UartTransportTest, BadIndicatorThenPacketSequence) {
  // Invalid packet indicator.
  uart_buffer_.push_back(kInvalidPacketIndicator);

  // Valid EventPacket w/ packet indicator.
  uart_buffer_.push_back(kUartEventPacketIndicator);
  std::array<std::byte, EventPacket::kHeaderSizeBytes> packet_buffer;
  EventPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());
  uart_buffer_.append(result.value());
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  // First decode should fail after consuming the invalid packet indicator.
  StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [&](const Packet&) { FAIL(); });
  EXPECT_EQ(status_with_size.status(), Status::DataLoss());
  EXPECT_EQ(status_with_size.size(), 1u);

  const ConstByteSpan remaining_data =
      ConstByteSpan(uart_buffer_).subspan(status_with_size.size());

  // Second decode should work now that the invalid byte is gone.
  size_t event_packet_count = 0;
  status_with_size =
      DecodeHciUartData(remaining_data, [&](const Packet& decoded_packet) {
        ASSERT_EQ(decoded_packet.type(), Packet::Type::kEventPacket);
        ++event_packet_count;
      });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), remaining_data.size_bytes());
  EXPECT_EQ(event_packet_count, 1u);
}

TEST_F(UartTransportTest, PacketThenBadIndicatorSequence) {
  // Valid EventPacket w/ packet indicator.
  uart_buffer_.push_back(kUartEventPacketIndicator);
  std::array<std::byte, EventPacket::kHeaderSizeBytes> packet_buffer;
  EventPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());
  uart_buffer_.append(result.value());
  ASSERT_EQ(uart_buffer_.status(), OkStatus());

  // Invalid packet indicator.
  uart_buffer_.push_back(kInvalidPacketIndicator);

  // First decode should fail, consuming all data.
  size_t event_packet_count = 0;
  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [&](const Packet& decoded_packet) {
        ASSERT_EQ(decoded_packet.type(), Packet::Type::kEventPacket);
        ++event_packet_count;
      });
  EXPECT_EQ(status_with_size.status(), Status::DataLoss());
  EXPECT_EQ(status_with_size.size(), uart_buffer_.size());
  EXPECT_EQ(event_packet_count, 1u);
}

TEST_F(UartTransportTest, AllPacketTypes) {
  // Valid CommandPacket w/ packet indicator.
  {
    uart_buffer_.push_back(kUartCommandPacketIndicator);
    std::array<std::byte, CommandPacket::kHeaderSizeBytes> packet_buffer;
    CommandPacket packet(0u, ConstByteSpan());
    const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
    ASSERT_EQ(result.status(), OkStatus());
    uart_buffer_.append(result.value());
    ASSERT_EQ(uart_buffer_.status(), OkStatus());
  }

  // Valid AsyncDataPacket w/ packet indicator.
  {
    uart_buffer_.push_back(kUartAsyncDataPacketIndicator);
    std::array<std::byte, AsyncDataPacket::kHeaderSizeBytes> packet_buffer;
    AsyncDataPacket packet(0u, ConstByteSpan());
    const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
    ASSERT_EQ(result.status(), OkStatus());
    uart_buffer_.append(result.value());
    ASSERT_EQ(uart_buffer_.status(), OkStatus());
  }

  // Valid SyncDataPacket w/ packet indicator.
  {
    uart_buffer_.push_back(kUartSyncDataPacketIndicator);
    std::array<std::byte, SyncDataPacket::kHeaderSizeBytes> packet_buffer;
    SyncDataPacket packet(0u, ConstByteSpan());
    const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
    ASSERT_EQ(result.status(), OkStatus());
    uart_buffer_.append(result.value());
    ASSERT_EQ(uart_buffer_.status(), OkStatus());
  }

  // Valid EventPacket w/ packet indicator.
  {
    uart_buffer_.push_back(kUartEventPacketIndicator);
    std::array<std::byte, EventPacket::kHeaderSizeBytes> packet_buffer;
    EventPacket packet(0u, ConstByteSpan());
    const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
    ASSERT_EQ(result.status(), OkStatus());
    uart_buffer_.append(result.value());
    ASSERT_EQ(uart_buffer_.status(), OkStatus());
  }

  // First decode should succeed, consuming all data.
  size_t packet_count = 0u;
  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [&](const Packet& decoded_packet) {
        ++packet_count;
        switch (packet_count) {
          case 1u:
            EXPECT_EQ(decoded_packet.type(), Packet::Type::kCommandPacket);
            break;

          case 2u:
            EXPECT_EQ(decoded_packet.type(), Packet::Type::kAsyncDataPacket);
            break;

          case 3u:
            EXPECT_EQ(decoded_packet.type(), Packet::Type::kSyncDataPacket);
            break;

          case 4u:
            EXPECT_EQ(decoded_packet.type(), Packet::Type::kEventPacket);
            break;
        }
      });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), uart_buffer_.size());
  EXPECT_EQ(packet_count, 4u);
}

TEST_F(UartTransportTest, LotsOfEventPackets) {
  std::array<std::byte, EventPacket::kHeaderSizeBytes> packet_buffer;
  EventPacket packet(0u, ConstByteSpan());
  const Result<ConstByteSpan> result = packet.Encode(packet_buffer);
  ASSERT_EQ(result.status(), OkStatus());

  size_t expected_packet_count = 0;
  while ((uart_buffer_.max_size() - uart_buffer_.size()) >
         packet.size_bytes()) {
    ++expected_packet_count;
    uart_buffer_.push_back(kUartEventPacketIndicator);
    uart_buffer_.append(result.value());
    ASSERT_EQ(uart_buffer_.status(), OkStatus());
  }

  size_t event_packet_count = 0;
  const StatusWithSize status_with_size =
      DecodeHciUartData(uart_buffer_, [&](const Packet& decoded_packet) {
        ASSERT_EQ(decoded_packet.type(), Packet::Type::kEventPacket);
        ++event_packet_count;
      });
  EXPECT_EQ(status_with_size.status(), OkStatus());
  EXPECT_EQ(status_with_size.size(), uart_buffer_.size());
  EXPECT_EQ(event_packet_count, expected_packet_count);
}

}  // namespace
}  // namespace pw::bluetooth_hci
