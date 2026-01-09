// Copyright 2023 Espressif Systems (Shanghai) PTE LTD
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

#include <app/clusters/mode-select-server/supported-modes-manager.h>
#include <app/ConcreteAttributePath.h>
#include <app-common/zap-generated/callback.h>
#include "esp_log.h"

static const char *TAG = "weak_functions";

namespace chip {
namespace app {
namespace Clusters {
namespace ModeSelect {
// Definition of weak getSupportedModesManager() function to avoid compilation error.
__attribute__((weak)) const SupportedModesManager * getSupportedModesManager(void)
{
    return NULL;
}

} /* namespace chip */
} /* namespace app */
} /* namespace Clusters */
} /* namespace ModeSelect */


// Provide weak defaults for attribute changed callbacks so apps
// Strong definitions in an app will override these.

__attribute__((weak)) void MatterClosureControlClusterServerAttributeChangedCallback(
    const chip::app::ConcreteAttributePath & attributePath)
{
    ESP_LOGI(TAG, "Attribute Changed Callback: Endpoint: %d, Cluster: %ld, Attribute: %ld",
             attributePath.mEndpointId, attributePath.mClusterId, attributePath.mAttributeId);
}

__attribute__((weak)) void MatterClosureDimensionClusterServerAttributeChangedCallback(
    const chip::app::ConcreteAttributePath & attributePath)
{
    ESP_LOGI(TAG, "Attribute Changed Callback: Endpoint: %d, Cluster: %ld, Attribute: %ld",
             attributePath.mEndpointId, attributePath.mClusterId, attributePath.mAttributeId);
}

