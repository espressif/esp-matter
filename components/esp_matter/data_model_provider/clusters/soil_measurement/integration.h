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

#include <app/clusters/soil-measurement-server/SoilMeasurementCluster.h>
#include <clusters/SoilMeasurement/Attributes.h>

namespace chip::app::Clusters::SoilMeasurement {

CHIP_ERROR SetSoilMoistureMeasuredValue(
    EndpointId endpointId,
    const SoilMeasurement::Attributes::SoilMoistureMeasuredValue::TypeInfo::Type &soilMoistureMeasuredValue);

void SetSoilMoistureLimits(
    EndpointId endpointId,
    const SoilMeasurement::Attributes::SoilMoistureMeasurementLimits::TypeInfo::Type &soilMoistureLimits);

} // namespace chip::app::Clusters::SoilMeasurement
