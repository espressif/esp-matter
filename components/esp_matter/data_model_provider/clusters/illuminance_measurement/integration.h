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

#include <app/clusters/illuminance-measurement-server/IlluminanceMeasurementCluster.h>

namespace chip::app::Clusters::IlluminanceMeasurement {

/// Returns the cluster instance registered on the given endpoint, nullptr otherwise
IlluminanceMeasurementCluster * FindClusterOnEndpoint(EndpointId endpointId);

/// Convenience helper to set new measured value
CHIP_ERROR SetMeasuredValue(EndpointId endpointId, DataModel::Nullable<uint16_t> measuredValue);

/// Convenience helper to set the min/max measured value range
CHIP_ERROR SetMeasuredValueRange(EndpointId endpointId, DataModel::Nullable<uint16_t> minMeasuredValue,
                                 DataModel::Nullable<uint16_t> maxMeasuredValue);

} // namespace chip::app::Clusters::IlluminanceMeasurement
