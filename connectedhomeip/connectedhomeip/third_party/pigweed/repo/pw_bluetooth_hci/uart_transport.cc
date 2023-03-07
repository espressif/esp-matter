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

namespace pw::bluetooth_hci {

StatusWithSize DecodeHciUartData(ConstByteSpan data,
                                 const DecodedPacketCallback& packet_callback) {
  size_t bytes_consumed = 0;
  while (data.size_bytes() > 0) {
    const std::byte packet_indicator = data[0];
    data = data.subspan(1);  // Pop off the packet indicator byte.
    // Note that we do not yet claim the byte consumed until we know what it is,
    // as it may be a partial HCI packet which cannot be consumed until later.

    size_t packet_size_bytes = 0;
    switch (packet_indicator) {
      case kUartCommandPacketIndicator: {
        const std::optional<CommandPacket> maybe_packet =
            CommandPacket::Decode(data, endian::little);
        if (!maybe_packet.has_value()) {
          return StatusWithSize(
              bytes_consumed);  // Not enough data to complete this packet.
        }

        const CommandPacket& packet = maybe_packet.value();
        packet_callback(packet);
        packet_size_bytes = packet.size_bytes();
        break;
      }

      case kUartAsyncDataPacketIndicator: {
        const std::optional<AsyncDataPacket> maybe_packet =
            AsyncDataPacket::Decode(data, endian::little);
        if (!maybe_packet.has_value()) {
          return StatusWithSize(
              bytes_consumed);  // Not enough data to complete this packet.
        }

        const AsyncDataPacket& packet = maybe_packet.value();
        packet_callback(packet);
        packet_size_bytes = packet.size_bytes();
        break;
      }

      case kUartSyncDataPacketIndicator: {
        const std::optional<SyncDataPacket> maybe_packet =
            SyncDataPacket::Decode(data, endian::little);
        if (!maybe_packet.has_value()) {
          return StatusWithSize(
              bytes_consumed);  // Not enough data to complete this packet.
        }

        const SyncDataPacket& packet = maybe_packet.value();
        packet_callback(packet);
        packet_size_bytes = packet.size_bytes();
        break;
      }

      case kUartEventPacketIndicator: {
        const std::optional<EventPacket> maybe_packet =
            EventPacket::Decode(data);
        if (!maybe_packet.has_value()) {
          return StatusWithSize(
              bytes_consumed);  // Not enough data to complete this packet.
        }

        const EventPacket& packet = maybe_packet.value();
        packet_callback(packet);
        packet_size_bytes = packet.size_bytes();
        break;
      }

      default:
        // Unrecognized PacketIndicator type, we've lost synchronization!
        ++bytes_consumed;  // Consume the invalid packet indicator.
        return StatusWithSize::DataLoss(bytes_consumed);
    }

    data = data.subspan(packet_size_bytes);  // Pop off the HCI packet.
    // Consume the packet indicator and the packet.
    bytes_consumed += 1 + packet_size_bytes;
  }
  return StatusWithSize(bytes_consumed);
}

}  // namespace pw::bluetooth_hci
