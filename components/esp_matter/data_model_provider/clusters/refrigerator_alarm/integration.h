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

#include <app/clusters/alarm-base-server/Delegate.h>
#include <app/clusters/refrigerator-alarm-server/RefrigeratorAlarmCluster.h>

namespace chip::app::Clusters::RefrigeratorAlarm {

/** Returns the cluster instance registered on the given endpoint, or nullptr. */
RefrigeratorAlarmCluster * FindClusterOnEndpoint(EndpointId endpointId);

/** Sets the application delegate consulted for ModifyEnabledAlarms/Reset commands. */
void SetDefaultDelegate(EndpointId endpointId, AlarmBase::Delegate *delegate);

} // namespace chip::app::Clusters::RefrigeratorAlarm
