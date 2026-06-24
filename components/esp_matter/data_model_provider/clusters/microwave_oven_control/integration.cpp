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

// MicrowaveOvenControl code-driven cluster lifecycle is managed by
// MicrowaveOvenControlDelegateInitCB in esp_matter_delegate_callbacks.cpp.
// It constructs the MicrowaveOvenControlCluster with an IntegrationDelegate
// (bridging OperationalState + ModeBase) and the app-provided AppDelegate.
//
// This file provides weak ESPMatter callback stubs (linker fallback) and
// plugin init stubs for the ember layer.

#include <app/ClusterCallbacks.h>

using namespace chip;
using namespace chip::app;

void ESPMatterMicrowaveOvenControlClusterServerInitCallback(EndpointId endpointId)
{
    // No-op: cluster init handled by MicrowaveOvenControlDelegateInitCB
}

void ESPMatterMicrowaveOvenControlClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    // No-op: cluster shutdown handled by MicrowaveOvenControlShutdownCB
}

void MatterMicrowaveOvenControlPluginServerInitCallback() {}
void MatterMicrowaveOvenControlPluginServerShutdownCallback() {}
