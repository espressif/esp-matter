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
namespace ethernet_network_diagnostics {

inline constexpr uint32_t Id = 0x0037;

namespace feature {
namespace PacketCounts {
inline constexpr uint32_t Id = 0x1;
} /* PacketCounts */
namespace ErrorCounts {
inline constexpr uint32_t Id = 0x2;
} /* ErrorCounts */
} /* feature */

namespace attribute {
namespace PHYRate {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 9;
} /* PHYRate */
namespace FullDuplex {
inline constexpr uint32_t Id = 0x0001;
} /* FullDuplex */
namespace PacketRxCount {
inline constexpr uint32_t Id = 0x0002;
} /* PacketRxCount */
namespace PacketTxCount {
inline constexpr uint32_t Id = 0x0003;
} /* PacketTxCount */
namespace TxErrCount {
inline constexpr uint32_t Id = 0x0004;
} /* TxErrCount */
namespace CollisionCount {
inline constexpr uint32_t Id = 0x0005;
} /* CollisionCount */
namespace OverrunCount {
inline constexpr uint32_t Id = 0x0006;
} /* OverrunCount */
namespace CarrierDetect {
inline constexpr uint32_t Id = 0x0007;
} /* CarrierDetect */
namespace TimeSinceReset {
inline constexpr uint32_t Id = 0x0008;
} /* TimeSinceReset */
} /* attribute */

namespace command {
namespace ResetCounts {
inline constexpr uint32_t Id = 0x00;
} /* ResetCounts */
} /* command */

} /* ethernet_network_diagnostics */
} /* cluster */
} /* esp_matter */
