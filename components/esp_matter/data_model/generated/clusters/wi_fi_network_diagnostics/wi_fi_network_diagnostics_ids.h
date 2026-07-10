// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#pragma once
#include <esp_matter_data_model.h>

namespace esp_matter {
namespace cluster {
namespace wi_fi_network_diagnostics {

inline constexpr uint32_t Id = 0x0036;

namespace feature {
namespace PacketCounts {
inline constexpr uint32_t Id = 0x1;
} /* PacketCounts */
namespace ErrorCounts {
inline constexpr uint32_t Id = 0x2;
} /* ErrorCounts */
} /* feature */

namespace attribute {
namespace BSSID {
inline constexpr uint32_t Id = 0x0000;
} /* BSSID */
namespace SecurityType {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 5;
} /* SecurityType */
namespace WiFiVersion {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 6;
} /* WiFiVersion */
namespace ChannelNumber {
inline constexpr uint32_t Id = 0x0003;
} /* ChannelNumber */
namespace RSSI {
inline constexpr uint32_t Id = 0x0004;
inline constexpr int8_t Min = -120;
inline constexpr int8_t Max = 0;
} /* RSSI */
namespace BeaconLostCount {
inline constexpr uint32_t Id = 0x0005;
} /* BeaconLostCount */
namespace BeaconRxCount {
inline constexpr uint32_t Id = 0x0006;
} /* BeaconRxCount */
namespace PacketMulticastRxCount {
inline constexpr uint32_t Id = 0x0007;
} /* PacketMulticastRxCount */
namespace PacketMulticastTxCount {
inline constexpr uint32_t Id = 0x0008;
} /* PacketMulticastTxCount */
namespace PacketUnicastRxCount {
inline constexpr uint32_t Id = 0x0009;
} /* PacketUnicastRxCount */
namespace PacketUnicastTxCount {
inline constexpr uint32_t Id = 0x000A;
} /* PacketUnicastTxCount */
namespace CurrentMaxRate {
inline constexpr uint32_t Id = 0x000B;
} /* CurrentMaxRate */
namespace OverrunCount {
inline constexpr uint32_t Id = 0x000C;
} /* OverrunCount */
} /* attribute */

namespace command {
namespace ResetCounts {
inline constexpr uint32_t Id = 0x00;
} /* ResetCounts */
} /* command */

namespace event {
namespace Disconnection {
inline constexpr uint32_t Id = 0x00;
} /* Disconnection */
namespace AssociationFailure {
inline constexpr uint32_t Id = 0x01;
} /* AssociationFailure */
namespace ConnectionStatus {
inline constexpr uint32_t Id = 0x02;
} /* ConnectionStatus */
} /* event */

} /* wi_fi_network_diagnostics */
} /* cluster */
} /* esp_matter */
