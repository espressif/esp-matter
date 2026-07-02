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
namespace operational_state {

inline constexpr uint32_t Id = 0x0060;

namespace attribute {
namespace PhaseList {
inline constexpr uint32_t Id = 0x0000;
} /* PhaseList */
namespace CurrentPhase {
inline constexpr uint32_t Id = 0x0001;
} /* CurrentPhase */
namespace CountdownTime {
inline constexpr uint32_t Id = 0x0002;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 259200;
} /* CountdownTime */
namespace OperationalStateList {
inline constexpr uint32_t Id = 0x0003;
} /* OperationalStateList */
namespace OperationalState {
inline constexpr uint32_t Id = 0x0004;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 3;
} /* OperationalState */
namespace OperationalError {
inline constexpr uint32_t Id = 0x0005;
} /* OperationalError */
} /* attribute */

namespace command {
namespace Pause {
inline constexpr uint32_t Id = 0x00;
} /* Pause */
namespace Stop {
inline constexpr uint32_t Id = 0x01;
} /* Stop */
namespace Start {
inline constexpr uint32_t Id = 0x02;
} /* Start */
namespace Resume {
inline constexpr uint32_t Id = 0x03;
} /* Resume */
namespace OperationalCommandResponse {
inline constexpr uint32_t Id = 0x04;
} /* OperationalCommandResponse */
} /* command */

namespace event {
namespace OperationalError {
inline constexpr uint32_t Id = 0x00;
} /* OperationalError */
namespace OperationCompletion {
inline constexpr uint32_t Id = 0x01;
} /* OperationCompletion */
} /* event */

} /* operational_state */
} /* cluster */
} /* esp_matter */
