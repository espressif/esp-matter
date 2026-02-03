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

#include <app/clusters/electrical-energy-measurement-server/ElectricalEnergyMeasurementCluster.h>

namespace chip::app::Clusters::ElectricalEnergyMeasurement {

bool NotifyCumulativeEnergyMeasured(EndpointId endpointId, const Optional<Structs::EnergyMeasurementStruct::Type> & energyImported,
                                    const Optional<Structs::EnergyMeasurementStruct::Type> & energyExported);

bool NotifyPeriodicEnergyMeasured(EndpointId endpointId, const Optional<Structs::EnergyMeasurementStruct::Type> & energyImported,
                                  const Optional<Structs::EnergyMeasurementStruct::Type> & energyExported);

CHIP_ERROR SetMeasurementAccuracy(EndpointId endpointId, const Structs::MeasurementAccuracyStruct::Type & accuracy);

CHIP_ERROR SetCumulativeReset(EndpointId endpointId, const Optional<Structs::CumulativeEnergyResetStruct::Type> & cumulativeReset);

const ElectricalEnergyMeasurement::MeasurementData * MeasurementDataForEndpoint(EndpointId endpointId);

ElectricalEnergyMeasurementCluster *GetClusterInstance(EndpointId endpointId);
}
