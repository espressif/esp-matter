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
namespace ota_software_update_requestor {

inline constexpr uint32_t Id = 0x002A;

namespace attribute {
namespace DefaultOTAProviders {
inline constexpr uint32_t Id = 0x0000;
} /* DefaultOTAProviders */
namespace UpdatePossible {
inline constexpr uint32_t Id = 0x0001;
} /* UpdatePossible */
namespace UpdateState {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 8;
} /* UpdateState */
namespace UpdateStateProgress {
inline constexpr uint32_t Id = 0x0003;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 100;
} /* UpdateStateProgress */
} /* attribute */

namespace command {
namespace AnnounceOTAProvider {
inline constexpr uint32_t Id = 0x00;
} /* AnnounceOTAProvider */
} /* command */

namespace event {
namespace StateTransition {
inline constexpr uint32_t Id = 0x00;
} /* StateTransition */
namespace VersionApplied {
inline constexpr uint32_t Id = 0x01;
} /* VersionApplied */
namespace DownloadError {
inline constexpr uint32_t Id = 0x02;
} /* DownloadError */
} /* event */

} /* ota_software_update_requestor */
} /* cluster */
} /* esp_matter */
