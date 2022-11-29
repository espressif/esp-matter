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
#include <esp_matter.h>

using chip::NodeId;
using chip::ScopedNodeId;
using chip::SessionHandle;
using chip::Controller::CommissioningParameters;
using chip::Messaging::ExchangeManager;

namespace esp_matter {
namespace controller {

typedef enum pairing_network_type {
    NETWORK_TYPE_NONE,
    NETWORK_TYPE_WIFI,
    NETWORK_TYPE_THREAD,
    NETWORK_TYPE_ETHERNET,
} pairing_network_type_t;

typedef enum pairing_mode {
    PAIRING_MODE_NONE,
    PAIRING_MODE_CODE,
    PAIRING_MODE_BLE,
    PAIRING_MODE_SOFTAP,
    PAIRING_MODE_ETHERNET,
    PAIRING_MODE_ONNETWORK,
} pairing_mode_t;

class pairing_command : public chip::Controller::DevicePairingDelegate,
                        public chip::Controller::DeviceDiscoveryDelegate {
public:
    pairing_command()
        : mOnDeviceConnectedCallback(OnDeviceConnectedFn, this)
        , mOnDeviceConnectionFailureCallback(OnDeviceConnectionFailureFn, this)
    {
    }

    /****************** DevicePairingDelegate Interface *****************/
    void OnStatusUpdate(DevicePairingDelegate::Status status) override;
    void OnPairingComplete(CHIP_ERROR error) override;
    void OnPairingDeleted(CHIP_ERROR error) override;
    void OnCommissioningComplete(NodeId deviceId, CHIP_ERROR error) override;

    /****************** DeviceDiscoveryDelegate Interface ***************/
    void OnDiscoveredDevice(const chip::Dnssd::DiscoveredNodeData &nodeData) override;

    static pairing_command &get_instance() { return instance; }

    NodeId m_remote_node_id;
    uint32_t m_setup_pincode;
    uint16_t m_discriminator;
    pairing_network_type_t m_pairing_network_type;
    pairing_mode_t m_pairing_mode;

private:
    static pairing_command instance;
    static void OnDeviceConnectedFn(void *context, ExchangeManager &exchangeMgr, SessionHandle &sessionHandle);
    static void OnDeviceConnectionFailureFn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error);

    CommissioningParameters get_commissioning_params();

    chip::Callback::Callback<chip::OnDeviceConnected> mOnDeviceConnectedCallback;
    chip::Callback::Callback<chip::OnDeviceConnectionFailure> mOnDeviceConnectionFailureCallback;
};

esp_err_t pairing_on_network(NodeId node_id, uint32_t pincode);

} // namespace controller
} // namespace esp_matter
