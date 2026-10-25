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
namespace av_analysis {

inline constexpr uint32_t Id = 0x0557;

namespace feature {
namespace LocalContextDetection {
inline constexpr uint32_t Id = 0x1;
} /* LocalContextDetection */
namespace RemoteContextDetection {
inline constexpr uint32_t Id = 0x2;
} /* RemoteContextDetection */
namespace PerZoneContextDetection {
inline constexpr uint32_t Id = 0x4;
} /* PerZoneContextDetection */
} /* feature */

namespace attribute {
namespace SupportedAmbientContexts {
inline constexpr uint32_t Id = 0x0000;
} /* SupportedAmbientContexts */
namespace ActiveAmbientContextTriggers {
inline constexpr uint32_t Id = 0x0001;
} /* ActiveAmbientContextTriggers */
namespace MaxAnalysisStreamCount {
inline constexpr uint32_t Id = 0x0002;
} /* MaxAnalysisStreamCount */
namespace CurrentAnalysisStreamCount {
inline constexpr uint32_t Id = 0x0003;
} /* CurrentAnalysisStreamCount */
namespace AnalysisStreams {
inline constexpr uint32_t Id = 0x0004;
} /* AnalysisStreams */
namespace TrackingEnabled {
inline constexpr uint32_t Id = 0x0005;
} /* TrackingEnabled */
} /* attribute */

namespace command {
namespace EnableContextTriggers {
inline constexpr uint32_t Id = 0x00;
} /* EnableContextTriggers */
namespace DisableContextTriggers {
inline constexpr uint32_t Id = 0x01;
} /* DisableContextTriggers */
namespace EstablishAnalysisStream {
inline constexpr uint32_t Id = 0x02;
} /* EstablishAnalysisStream */
namespace EstablishAnalysisStreamResponse {
inline constexpr uint32_t Id = 0x03;
} /* EstablishAnalysisStreamResponse */
namespace ActivateAnalysisStream {
inline constexpr uint32_t Id = 0x04;
} /* ActivateAnalysisStream */
namespace DeactivateAnalysisStream {
inline constexpr uint32_t Id = 0x05;
} /* DeactivateAnalysisStream */
namespace RemoveAnalysisStream {
inline constexpr uint32_t Id = 0x06;
} /* RemoveAnalysisStream */
} /* command */

} /* av_analysis */
} /* cluster */
} /* esp_matter */
