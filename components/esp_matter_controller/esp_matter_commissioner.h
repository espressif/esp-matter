// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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

#include <controller/CHIPDeviceController.h>
#include <controller/CommissionerDiscoveryController.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <lib/core/CHIPError.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/PlatformManager.h>
#include <transport/TransportMgr.h>

#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
using chip::NodeId;
using chip::Controller::AutoCommissioner;
using chip::Controller::DeviceCommissioner;
using chip::Controller::DevicePairingDelegate;
using chip::Transport::PeerAddress;

namespace esp_matter {
namespace commissioner {

esp_err_t init(uint16_t commissioner_port);
void shutdown();

DeviceCommissioner *get_device_commissioner();
CommissionerDiscoveryController *get_discovery_controller();
AutoCommissioner *get_auto_commissioner();
} // namespace commissioner
} // namespace esp_matter
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
