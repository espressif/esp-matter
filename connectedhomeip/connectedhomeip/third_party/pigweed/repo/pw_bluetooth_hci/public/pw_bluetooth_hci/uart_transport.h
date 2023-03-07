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

#include "pw_bluetooth_hci/packet.h"
#include "pw_bytes/bit.h"
#include "pw_bytes/span.h"
#include "pw_function/function.h"
#include "pw_status/status_with_size.h"

namespace pw::bluetooth_hci {

// The HCI UART Transport Layer supports uses the following packet indicator
// bytes to decode the HCI packet type as defined by the Bluetooth Core
// Specification version 5.3 "Host Controller Interface Transport Layer" volume
// 4, part A:
constexpr inline std::byte kUartCommandPacketIndicator = std::byte{0x01};
constexpr inline std::byte kUartAsyncDataPacketIndicator = std::byte{0x02};
constexpr inline std::byte kUartSyncDataPacketIndicator = std::byte{0x03};
constexpr inline std::byte kUartEventPacketIndicator = std::byte{0x04};

// The HCI UART Transport Layer may be invoked with the following packet types,
// as defined by Bluetooth Core Specification version 5.3 "Host Controller
// Interface Transport Layer" volume 4, part A:
//   PacketType::kCommandPacket
//   PacketType::kAsyncDataPacket
//   PacketType::kSyncDataPacket
//   PacketType::kEventPacket
using DecodedPacketCallback = Function<void(const Packet& packet)>;

// Parses the HCI Packets out of a HCI UART Transport Layer buffer.
//
// Parses as many complete HCI packets out of the provided buffer based on the
// HCI UART Transport Layer as defined by Bluetooth Core Specification version
// 5.3 "Host Controller Interface Transport Layer" volume 4, part A.
//
// The HciPacketCallback is invoked for each full HCI packet.
//
// Returns the number of bytes processed and a status based on:
// OK - No invalid packet indicator found.
// DATA_LOSS - An invalid packet indicator was detected between packets.
//             Synchronization has been lost. The caller is responsible for
//             regaining synchronization
//
// Note: The caller is responsible for detecting the lack of progress due to
// an undersized data buffer and/or an invalid length field in case a full
// buffer is passed and no bytes are processed.
StatusWithSize DecodeHciUartData(ConstByteSpan data,
                                 const DecodedPacketCallback& packet_callback);

}  // namespace pw::bluetooth_hci
