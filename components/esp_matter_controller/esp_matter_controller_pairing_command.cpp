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

#include <esp_matter_commissioner.h>
#include <esp_matter_controller_pairing_command.h>

static const char *TAG = "pairing_command";

using namespace esp_matter::commissioner;
using namespace chip;
using namespace chip::Controller;

namespace esp_matter {
namespace controller {

pairing_command pairing_command::instance;

void pairing_command::OnStatusUpdate(DevicePairingDelegate::Status status)
{
    switch (status) {
    case DevicePairingDelegate::Status::SecurePairingSuccess:
        ESP_LOGI(TAG, "Secure Pairing Success");
        break;
    case DevicePairingDelegate::Status::SecurePairingFailed:
        ESP_LOGI(TAG, "Secure Pairing Failed");
        break;
    case DevicePairingDelegate::Status::SecurePairingDiscoveringMoreDevices:
        ESP_LOGI(TAG, "Secure Pairing Discovering More Device");
        break;
    }
}

void pairing_command::OnPairingComplete(CHIP_ERROR err)
{
    if (err == CHIP_NO_ERROR) {
        ESP_LOGI(TAG, "Pairing Success");
    } else {
        ESP_LOGI(TAG, "Pairing Failure: Matter-%s", ErrorStr(err));
    }
}

void pairing_command::OnPairingDeleted(CHIP_ERROR err)
{
    if (err == CHIP_NO_ERROR) {
        ESP_LOGI(TAG, "Pairing Deleted Success");
    } else {
        ESP_LOGI(TAG, "Pairing Deleted Failure: Matter-%s", ErrorStr(err));
    }
}

void pairing_command::OnCommissioningComplete(NodeId nodeId, CHIP_ERROR err)
{
    if (err == CHIP_NO_ERROR) {
        ESP_LOGI(TAG, "Device commissioning completed with success - getting OperationalDeviceProxy");
        esp_matter::commissioner::get_device_commissioner()->GetConnectedDevice(nodeId, &mOnDeviceConnectedCallback,
                                                                                &mOnDeviceConnectionFailureCallback);
    } else {
        ESP_LOGI(TAG, "Device commissioning Failure: Matter%s", ErrorStr(err));
        CommissionerDiscoveryController *cdc = esp_matter::commissioner::get_discovery_controller();
        if (cdc != nullptr) {
            cdc->CommissioningFailed(err);
        }
    }
}

void pairing_command::OnDeviceConnectedFn(void *context, ExchangeManager &exchangeMgr, SessionHandle &sessionHandle)
{
    ESP_LOGI(TAG, "OnDeviceConnectedFn");
    CommissionerDiscoveryController *cdc = esp_matter::commissioner::get_discovery_controller();

    if (cdc != nullptr) {
        uint16_t vendorId = get_auto_commissioner()->GetCommissioningParameters().GetRemoteVendorId().Value();
        uint16_t productId = get_auto_commissioner()->GetCommissioningParameters().GetRemoteProductId().Value();
        ESP_LOGI(TAG, " ----- AutoCommissioner -- Commissionee vendorId=0x%04X productId=0x%04X", vendorId, productId);

        cdc->CommissioningSucceeded(vendorId, productId, get_instance().m_remote_node_id, exchangeMgr, sessionHandle);
    }
}

void pairing_command::OnDeviceConnectionFailureFn(void *context, const ScopedNodeId &peerId, CHIP_ERROR err)
{
    ESP_LOGI(TAG, "OnDeviceConnectionFailureFn - attempt to get OperationalDeviceProxy failed");
    CommissionerDiscoveryController *cdc = esp_matter::commissioner::get_discovery_controller();
    if (cdc != nullptr) {
        cdc->CommissioningFailed(err);
    }
}

CommissioningParameters pairing_command::get_commissioning_params()
{
    switch (m_pairing_network_type) {
    case NETWORK_TYPE_ETHERNET:
    case NETWORK_TYPE_NONE:
        return CommissioningParameters();
        break;
    default:
        ESP_LOGE(TAG, "Unsuppoted pairing network type");
        break;
    }
    return CommissioningParameters();
}

void pairing_command::OnDiscoveredDevice(const chip::Dnssd::DiscoveredNodeData &nodeData)
{
    // Ignore nodes with closed comissioning window
    VerifyOrReturn(nodeData.commissionData.commissioningMode != 0);
    const uint16_t port = nodeData.resolutionData.port;
    char buf[chip::Inet::IPAddress::kMaxStringLength];
    nodeData.resolutionData.ipAddress[0].ToString(buf);
    ESP_LOGI(TAG, "Discovered Device: %s:%u", buf, port);

    // Stop Mdns discovery. TODO: Check whether it is a right method
    get_device_commissioner()->RegisterDeviceDiscoveryDelegate(nullptr);

    Inet::InterfaceId interfaceId = nodeData.resolutionData.ipAddress[0].IsIPv6LinkLocal()
        ? nodeData.resolutionData.interfaceId
        : Inet::InterfaceId::Null();
    PeerAddress peerAddress = PeerAddress::UDP(nodeData.resolutionData.ipAddress[0], port, interfaceId);
    RendezvousParameters params = RendezvousParameters().SetSetupPINCode(m_setup_pincode).SetPeerAddress(peerAddress);
    CommissioningParameters commissioning_params = get_commissioning_params();
    get_device_commissioner()->PairDevice(m_remote_node_id, params, commissioning_params);
}

esp_err_t pairing_on_network(NodeId node_id, uint32_t pincode)
{
    Dnssd::DiscoveryFilter filter(chip::Dnssd::DiscoveryFilterType::kNone);
    get_device_commissioner()->RegisterDeviceDiscoveryDelegate(&pairing_command::get_instance());
    pairing_command::get_instance().m_pairing_mode = PAIRING_MODE_ONNETWORK;
    pairing_command::get_instance().m_setup_pincode = pincode;
    pairing_command::get_instance().m_remote_node_id = node_id;
    pairing_command::get_instance().m_pairing_network_type = NETWORK_TYPE_NONE;
    if (CHIP_NO_ERROR != get_device_commissioner()->DiscoverCommissionableNodes(filter)) {
        ESP_LOGE(TAG, "Failed to discover commissionable nodes");
        return ESP_FAIL;
    }
    return ESP_OK;
}
} // namespace controller
} // namespace esp_matter
