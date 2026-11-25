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

#include <esp_matter_client_target.h>

#include "esp_matter_client_operation.h"

#include <credentials/FabricTable.h>
#include <lib/support/CodeUtils.h>

namespace esp_matter {
namespace client {

CHIP_ERROR resolve_target(const chip::ScopedNodeId &node, execution_t &execution)
{
    VerifyOrReturnError(node.IsOperational(), CHIP_ERROR_INVALID_ARGUMENT);
    chip::FabricTable *fabric_table = operation::GetFabricTable();
    VerifyOrReturnError(fabric_table != nullptr, CHIP_ERROR_INCORRECT_STATE);
    const chip::FabricInfo *fabric = fabric_table->FindFabricWithIndex(node.GetFabricIndex());
    VerifyOrReturnError(fabric != nullptr, CHIP_ERROR_INVALID_FABRIC_INDEX);
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    execution = fabric->GetNodeId() == node.GetNodeId() ? execution_t::local : execution_t::remote;
#else // !CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    execution = execution_t::remote;
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    return CHIP_NO_ERROR;
}

} // namespace client
} // namespace esp_matter
