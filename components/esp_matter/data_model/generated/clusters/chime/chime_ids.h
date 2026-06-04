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
namespace chime {

inline constexpr uint32_t Id = 0x0556;

namespace attribute {
namespace InstalledChimeSounds {
inline constexpr uint32_t Id = 0x0000;
} /* InstalledChimeSounds */
namespace SelectedChime {
inline constexpr uint32_t Id = 0x0001;
} /* SelectedChime */
namespace Enabled {
inline constexpr uint32_t Id = 0x0002;
} /* Enabled */
} /* attribute */

namespace command {
namespace PlayChimeSound {
inline constexpr uint32_t Id = 0x00;
} /* PlayChimeSound */
} /* command */

namespace event {
namespace ChimeStartedPlaying {
inline constexpr uint32_t Id = 0x00;
} /* ChimeStartedPlaying */
} /* event */

} /* chime */
} /* cluster */
} /* esp_matter */
