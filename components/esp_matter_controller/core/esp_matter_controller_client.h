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

#include <esp_log.h>
#include <esp_matter_controller_credentials_issuer.h>

#include <app/icd/client/DefaultICDClientStorage.h>
#include <controller/AutoCommissioner.h>
#include <controller/CHIPDeviceController.h>
#include <controller/CommissionerDiscoveryController.h>
#include <controller/CurrentFabricRemover.h>
#include <controller/ExampleOperationalCredentialsIssuer.h>
#include <controller/OperationalCredentialsDelegate.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/GroupDataProviderImpl.h>
#include <credentials/PersistentStorageOpCertStore.h>
#include <credentials/attestation_verifier/DeviceAttestationVerifier.h>
#include <crypto/PersistentStorageOperationalKeystore.h>
#include <crypto/RawKeySessionKeystore.h>
#include <lib/core/CHIPError.h>
#include <lib/core/CHIPPersistentStorageDelegate.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/Span.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/KeyValueStoreManager.h>
#include <platform/PlatformManager.h>
#include <stdint.h>
#include <transport/TransportMgr.h>

#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
#include <controller/CommissionerDiscoveryController.h>
#endif
namespace esp_matter {
namespace controller {

#ifndef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
typedef void (*remove_fabric_callback)(chip::NodeId remoteNodeId, CHIP_ERROR status);

class auto_fabric_remover : private chip::Controller::CurrentFabricRemover
{
public:
    static esp_err_t remove_fabric(chip::Controller::DeviceController *controller, chip::NodeId remote_node,
                                   remove_fabric_callback callback)
    {
        auto * remover = chip::Platform::New<auto_fabric_remover>(controller, callback);
        if (remover == nullptr) {
            return ESP_ERR_NO_MEM;
        }
        CHIP_ERROR err = remover->CurrentFabricRemover::RemoveCurrentFabric(remote_node, &remover->m_matter_callback);
        if (err != CHIP_NO_ERROR)
        {
            // If failing to call RemoveCurrentFabric(), delete the remover here. Otherwise the remover will be deleted
            // in on_remove_current_fabric().
            chip::Platform::Delete(remover);
        }
        return err == CHIP_NO_ERROR ? ESP_OK : ESP_FAIL;
    }

    auto_fabric_remover(chip::Controller::DeviceController *controller, remove_fabric_callback callback) :
        chip::Controller::CurrentFabricRemover(controller), m_matter_callback(on_remove_current_fabric, this),
        m_remove_fabric_callback(callback) {}
private:
    static void on_remove_current_fabric(void * context, chip::NodeId remote_node, CHIP_ERROR status)
    {
        auto *self = static_cast<auto_fabric_remover *>(context);
        if (self && self->m_remove_fabric_callback) {
            self->m_remove_fabric_callback(remote_node, status);
        }
        chip::Platform::Delete(self);
    }
    chip::Callback::Callback<chip::Controller::OnCurrentFabricRemove> m_matter_callback;
    remove_fabric_callback m_remove_fabric_callback;
};

class matter_controller_client {
public:
    class controller_storage_delegate : public chip::PersistentStorageDelegate {
        CHIP_ERROR SyncGetKeyValue(const char *key, void *buffer, uint16_t &size) override
        {
            ESP_LOGD("MatterController", "Retrieving value from controller storage.");
            size_t bytesRead = 0;
            CHIP_ERROR err = chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Get(key, buffer, size, &bytesRead);

            if (err == CHIP_NO_ERROR) {
                ESP_LOGD("MatterController", "Retrieved value from controller storage.");
            }
            size = static_cast<uint16_t>(bytesRead);
            return err;
        }

        CHIP_ERROR SyncSetKeyValue(const char *key, const void *value, uint16_t size) override
        {
            ESP_LOGD("MatterController", "Stored value in controller storage");
            return chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Put(key, value, size);
        }

        CHIP_ERROR SyncDeleteKeyValue(const char *key) override
        {
            ESP_LOGD("MatterController", "Delete value in controller storage");
            return chip::DeviceLayer::PersistedStorage::KeyValueStoreMgr().Delete(key);
        }
    };

    using NodeId = ::chip::NodeId;
    using FabricId = ::chip::FabricId;
    using MatterDeviceCommissioner = ::chip::Controller::DeviceCommissioner;
    using MatterDeviceController = ::chip::Controller::DeviceController;

    static constexpr uint16_t k_max_groups_per_fabric = 50;
    static constexpr uint16_t k_max_group_keys_per_fabric = 25;

    static matter_controller_client &get_instance()
    {
        static matter_controller_client s_instance;
        return s_instance;
    }

    esp_err_t init(NodeId node_id, FabricId fabric_id, uint16_t listen_port);

#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    esp_err_t setup_commissioner();
    MatterDeviceCommissioner *get_commissioner() { return &m_device_commissioner; }
    esp_err_t unpair(NodeId remote_node, remove_fabric_callback callback = nullptr)
    {
        return auto_fabric_remover::remove_fabric(&m_device_commissioner, remote_node, callback);
    }
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
    CommissionerDiscoveryController *get_discovery_controller() { return &m_commissioner_discovery_controller; }
#endif
#else
    esp_err_t setup_controller(chip::MutableByteSpan &ipk);
    MatterDeviceController *get_controller() { return &m_device_controller; }
#endif
    chip::FabricIndex get_fabric_index()
    {
#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
        return m_device_commissioner.GetFabricIndex();
#else
        return m_device_controller.GetFabricIndex();
#endif
    }

private:
    matter_controller_client() {}

    bool m_operational_advertising = true;
    controller_storage_delegate m_default_storage;
    chip::PersistentStorageOperationalKeystore m_operational_keystore;
    chip::Credentials::PersistentStorageOpCertStore m_operational_cert_store;
    chip::Crypto::RawKeySessionKeystore m_session_key_store;
    chip::Credentials::GroupDataProviderImpl m_group_data_provider{k_max_groups_per_fabric,
                                                                   k_max_group_keys_per_fabric};
    credentials_issuer *m_credentials_issuer;
    NodeId m_controller_node_id;
    FabricId m_controller_fabric_id;
    // TODO: Enable ICD client from ESP32 platform
    //  chip::app::DefaultICDClientStorage s_icd_client_storage;

#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    chip::Controller::AutoCommissioner m_auto_commissioner;
    MatterDeviceCommissioner m_device_commissioner;
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
    CommissionerDiscoveryController m_commissioner_discovery_controller;
#endif
#else
    MatterDeviceController m_device_controller;
#endif
};
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
class ESPCommissionerCallback : public CommissionerCallback {
    void ReadyForCommissioning(uint32_t pincode, uint16_t longDiscriminator, PeerAddress peerAddress) override
    {
        NodeId gRemoteId = chip::kTestDeviceNodeId;
        chip::RendezvousParameters params = chip::RendezvousParameters()
            .SetSetupPINCode(pincode).SetDiscriminator(longDiscriminator).SetPeerAddress(peerAddress);
        do {
            chip::Crypto::DRBG_get_bytes(reinterpret_cast<uint8_t *>(&gRemoteId), sizeof(gRemoteId));
        } while (!chip::IsOperationalNodeId(gRemoteId));
        matter_controller_client::get_instance().get_commissioner()->PairDevice(gRemoteId, params);
    }
};
#endif

} // namespace controller
} // namespace esp_matter
