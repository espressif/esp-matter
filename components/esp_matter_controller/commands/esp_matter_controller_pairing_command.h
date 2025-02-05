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
#include "dnssd/Types.h"

using chip::NodeId;
using chip::ScopedNodeId;
using chip::SessionHandle;
using chip::Controller::CommissioningParameters;
using chip::Messaging::ExchangeManager;

namespace esp_matter {
namespace controller {

typedef struct {
    // Callback for the success or failure of PASE session establishment. err will be CHIP_NO_ERROR when the commissioner
    // establishes PASE session with peer node. Otherwise the commissioner fails to establish PASE session.
    void (*pase_callback)(CHIP_ERROR err);
    // Callback for the sussess of commisioning
    void (*commissioning_success_callback)(ScopedNodeId peer_id);
    // Callback for the failure of commissioning
    void (*commissioning_failure_callback)(
        ScopedNodeId peer_id, CHIP_ERROR error, chip::Controller::CommissioningStage stage,
        std::optional<chip::Credentials::AttestationVerificationResult> addtional_err_info);
} pairing_command_callbacks_t;

/** Pairing command class to finish commissioning with Matter end-devices **/
class pairing_command : public chip::Controller::DevicePairingDelegate,
                        public chip::Controller::DeviceDiscoveryDelegate {
public:
    /****************** DevicePairingDelegate Interface *****************/
    // This function will be called when the PASE session is established or the commisioner fails to establish
    // PASE session.
    void OnPairingComplete(CHIP_ERROR error) override;
    // The two functions are invoked upon the completion of the commissioning process, either successfully or
    // failed.
    void OnCommissioningSuccess(chip::PeerId peerId) override;
    void OnCommissioningFailure(
        chip::PeerId peerId, CHIP_ERROR error, chip::Controller::CommissioningStage stageFailed,
        chip::Optional<chip::Credentials::AttestationVerificationResult> additionalErrorInfo) override;

    /****************** DeviceDiscoveryDelegate Interface ***************/
    void OnDiscoveredDevice(const chip::Dnssd::CommissionNodeData &nodeData) override;

    static pairing_command &get_instance()
    {
        static pairing_command s_instance;
        return s_instance;
    }

    void set_callbacks(pairing_command_callbacks_t callbacks) { m_callbacks = callbacks; }

    NodeId m_remote_node_id;
    uint32_t m_setup_pincode;
    uint16_t m_discriminator;
    pairing_command_callbacks_t m_callbacks;

private:
    pairing_command()
        : m_remote_node_id(0), m_setup_pincode(0), m_discriminator(0), m_callbacks{nullptr, nullptr, nullptr} {}
};

/**
 * Pairing a Matter end-device on the same IP network
 *
 * @param[in] node_id NodeId assigned to the Matter end-device.
 * @param[in] pincode Setup PIN code of the Matter end-device.
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t pairing_on_network(NodeId node_id, uint32_t pincode);
#if CONFIG_ENABLE_ESP32_BLE_CONTROLLER
/**
 * Pairing a Matter over Wi-Fi end-device with BLE
 *
 * @param[in] node_id NodeId assigned to the Matter end-device.
 * @param[in] pincode Setup PIN code of the Matter end-device.
 * @param[in] disc Discriminator of the Matter end-device.
 * @param[in] ssid SSID of the Wi-Fi AP.
 * @param[in] pwd Password of the Wi-Fi AP.
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t pairing_ble_wifi(NodeId node_id, uint32_t pincode, uint16_t disc, const char *ssid, const char *pwd);

/**
 * Pairing a Matter over Thread end-device with BLE
 *
 * @param[in] node_id NodeId assigned to the Matter end-device.
 * @param[in] pincode Setup PIN code of the Matter end-device.
 * @param[in] disc Discriminator of the Matter end-device.
 * @param[in] dataset_tlvs Dataset TLV string of the Thread network.
 * @param[in] dataset_len Length of the dataset TLV string.
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t pairing_ble_thread(NodeId node_id, uint32_t pincode, uint16_t disc, uint8_t *dataset_tlvs,
                             uint8_t dataset_len);
#endif // CONFIG_ENABLE_ESP32_BLE_CONTROLLER

/**
 * Pair a on-network Matter end-device with a pairing code
 *
 * @param[in] node_id NodeId assigned to the Matter end-device.
 * @param[in] payload Pairing code
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t pairing_code(NodeId node_id, const char *payload);

/**
 * Pair a thread Matter end-device with a pairing code
 *
 * @param[in] node_id     NodeId assigned to the Matter end-device.
 * @param[in] payload     Pairing code
 * @param[in] dataset_buf Buffer containing the Thread network dataset
 * @param[in] dataset_len Length of the dataset buffer
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t pairing_code_thread(NodeId node_id, const char *payload, uint8_t *dataset_buf, uint8_t dataset_len);

/**
 * Pair a Wi-Fi Matter end-device with a pairing code
 *
 * @param[in] node_id  NodeId assigned to the Matter end-device.
 * @param[in] ssid     SSID of the Wi-Fi AP.
 * @param[in] password Password of the Wi-Fi AP.
 * @param[in] payload  Pairing code
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t pairing_code_wifi(NodeId node_id, const char *ssid, const char *password, const char *payload);

/**
 * Pair a Matter end-device which supports both Wi-Fi as well as Thread with a pairing code
 *
 * @param[in] node_id     NodeId that will be assigned to the Matter end-device.
 * @param[in] ssid        SSID of the Wi-Fi AP.
 * @param[in] password    Password of the Wi-Fi AP.
 * @param[in] payload     Pairing code
 * @param[in] dataset_buf Buffer containing the Thread network dataset
 * @param[in] dataset_len Length of the dataset buffer
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t pairing_code_wifi_thread(NodeId node_id, const char *ssid, const char *password, const char *payload,
                                   uint8_t *dataset_buf, uint8_t dataset_len);

/**
 * Unpair a Matter end-device which will remove the fabric from the remote device
 *
 * @param[in] node_id  NodeId of the Matter end-device to be unpaired.
 *
 * @return ESP_OK on success
 * @return error in case of failure
 */
esp_err_t unpair_device(NodeId node_id);

} // namespace controller
} // namespace esp_matter
