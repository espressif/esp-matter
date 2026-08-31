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

#pragma once

#include <app/clusters/temperature-control-server/TemperatureControlCluster.h>
#include <app/clusters/temperature-control-server/supported-temperature-levels-manager.h>
#include <lib/core/DataModelTypes.h>

namespace chip {
namespace app {
namespace Clusters {

namespace TemperatureControl {

/// Returns the cluster instance registered on the given endpoint, nullptr otherwise
TemperatureControlCluster * FindClusterOnEndpoint(EndpointId endpointId);

/// Convenience helper to set the temperature setpoint (TN feature)
CHIP_ERROR SetTemperatureSetpoint(EndpointId endpointId, int16_t temperatureSetpoint);

/// Convenience helper to set the selected temperature level (TL feature)
CHIP_ERROR SetSelectedTemperatureLevel(EndpointId endpointId, uint8_t selectedTemperatureLevel);

SupportedTemperatureLevelsIteratorDelegate * GetDelegate();

void SetDelegate(SupportedTemperatureLevelsIteratorDelegate * delegate);

} // namespace TemperatureControl
} // namespace Clusters
} // namespace app
} // namespace chip
