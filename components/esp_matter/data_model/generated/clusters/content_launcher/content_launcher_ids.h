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
namespace content_launcher {

inline constexpr uint32_t Id = 0x050A;

namespace feature {
namespace ContentSearch {
inline constexpr uint32_t Id = 0x1;
} /* ContentSearch */
namespace URLPlayback {
inline constexpr uint32_t Id = 0x2;
} /* URLPlayback */
namespace AdvancedSeek {
inline constexpr uint32_t Id = 0x4;
} /* AdvancedSeek */
namespace TextTracks {
inline constexpr uint32_t Id = 0x8;
} /* TextTracks */
namespace AudioTracks {
inline constexpr uint32_t Id = 0x10;
} /* AudioTracks */
} /* feature */

namespace attribute {
namespace AcceptHeader {
inline constexpr uint32_t Id = 0x0000;
} /* AcceptHeader */
namespace SupportedStreamingProtocols {
inline constexpr uint32_t Id = 0x0001;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* SupportedStreamingProtocols */
} /* attribute */

namespace command {
namespace LaunchContent {
inline constexpr uint32_t Id = 0x00;
} /* LaunchContent */
namespace LaunchURL {
inline constexpr uint32_t Id = 0x01;
} /* LaunchURL */
namespace LauncherResponse {
inline constexpr uint32_t Id = 0x02;
} /* LauncherResponse */
} /* command */

} /* content_launcher */
} /* cluster */
} /* esp_matter */
