// Copyright 2022-2026 Espressif Systems (Shanghai) PTE LTD
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

#include <sdkconfig.h>

#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
#error "esp_matter_controller_client.h requires Matter server support to be disabled"
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

#include <esp_log.h>
#include <esp_matter_controller_credentials_issuer.h>

#include <app/icd/client/CheckInHandler.h>
#include <app/icd/client/DefaultCheckInDelegate.h>
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
#include <crypto/DefaultSessionKeystore.h>
#include <lib/core/CHIPError.h>
#include <lib/core/CHIPPersistentStorageDelegate.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/Span.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/KeyValueStoreManager.h>
#include <platform/PlatformManager.h>
#include <stdint.h>
#include <transport/TransportMgr.h>

#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
#include <controller/CommissionerDiscoveryController.h>
#endif // CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY

namespace esp_matter {
namespace controller {

typedef void (*remove_fabric_callback)(chip::NodeId remoteNodeId, CHIP_ERROR status);

class controller_check_in_delegate : public chip::app::DefaultCheckInDelegate {
public:
    using check_in_complete_callback = void (*)(const chip::app::ICDClientInfo &clientInfo);
    using key_refresh_done_callback = void (*)(const chip::app::RefreshKeySender *refreshKeySender, CHIP_ERROR error);
    CHIP_ERROR Init(chip::app::ICDClientStorage *storage, chip::app::InteractionModelEngine *engine)
    {
        return chip::app::DefaultCheckInDelegate::Init(storage, engine);
    }

    void SetICDDelegateCallback(check_in_complete_callback check_in_complete_cb,
                                key_refresh_done_callback key_refresh_done_cb)
    {
        m_check_in_complete_cb = check_in_complete_cb;
        m_key_refresh_done_cb = key_refresh_done_cb;
    }

    void OnCheckInComplete(const chip::app::ICDClientInfo &clientInfo) override
    {
        if (m_check_in_complete_cb) {
            m_check_in_complete_cb(clientInfo);
        }
    }

    void OnKeyRefreshDone(chip::app::RefreshKeySender *refreshKeySender, CHIP_ERROR error) override
    {
        if (m_key_refresh_done_cb) {
            m_key_refresh_done_cb(refreshKeySender, error);
        }
        chip::app::DefaultCheckInDelegate::OnKeyRefreshDone(refreshKeySender, error);
    }

private:
    check_in_complete_callback m_check_in_complete_cb = nullptr;
    key_refresh_done_callback m_key_refresh_done_cb = nullptr;
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
    chip::app::DefaultICDClientStorage &get_icd_client_storage()
    {
        return m_icd_client_storage;
    }
    void set_icd_client_callback(controller_check_in_delegate::check_in_complete_callback check_in_complete_cb,
                                 controller_check_in_delegate::key_refresh_done_callback key_refresh_done_cb)
    {
        m_icd_check_in_delegate.SetICDDelegateCallback(check_in_complete_cb, key_refresh_done_cb);
    }

#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    esp_err_t setup_commissioner();
    MatterDeviceCommissioner *get_commissioner()
    {
        return &m_device_commissioner;
    }
    // Successful removal clears this peer's ICD registration before notifying the caller.
    esp_err_t unpair(NodeId remote_node, remove_fabric_callback callback = nullptr);
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
    CommissionerDiscoveryController *get_discovery_controller()
    {
        return &m_commissioner_discovery_controller;
    }
#endif // CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
#else // !CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    esp_err_t setup_controller(chip::MutableByteSpan &ipk, chip::FabricIndex stored_fabric_index = chip::kUndefinedFabricIndex);
    MatterDeviceController *get_controller()
    {
        return &m_device_controller;
    }
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    chip::FabricIndex get_fabric_index()
    {
#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
        return m_device_commissioner.GetFabricIndex();
#else // !CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
        return m_device_controller.GetFabricIndex();
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    }

private:
    class GroupDataProviderListener final : public chip::Credentials::GroupDataProvider::GroupListener {
    public:
        GroupDataProviderListener() : mSystemState(nullptr) {}

        CHIP_ERROR Init(const chip::Controller::DeviceControllerSystemState *systemState)
        {
            VerifyOrReturnError(systemState != nullptr, CHIP_ERROR_INVALID_ARGUMENT);

            mSystemState = systemState;
            return CHIP_NO_ERROR;
        };

        void OnGroupAdded(chip::FabricIndex fabric_index,
                          const chip::Credentials::GroupDataProvider::GroupInfo &new_group) override
        {
            VerifyOrReturn(mSystemState);
            auto *fabricTable = mSystemState->Fabrics();
            if (!fabricTable) {
                return;
            }
            const chip::FabricInfo *fabric = fabricTable->FindFabricWithIndex(fabric_index);
            if (fabric == nullptr) {
                ChipLogError(AppServer, "Group added to nonexistent fabric?");
                return;
            }

            if (mSystemState->TransportMgr()->MulticastGroupJoinLeave(
                        chip::Transport::PeerAddress::BuildMatterPerGroupMulticastAddress(fabric->GetFabricId(), new_group.group_id), true) !=
                    CHIP_NO_ERROR) {
                ChipLogError(AppServer, "Unable to listen to group");
            }
        };

        void OnGroupRemoved(chip::FabricIndex fabric_index,
                            const chip::Credentials::GroupDataProvider::GroupInfo &old_group) override
        {
            VerifyOrReturn(mSystemState);
            auto *fabricTable = mSystemState->Fabrics();
            if (!fabricTable) {
                return;
            }
            const chip::FabricInfo *fabric = fabricTable->FindFabricWithIndex(fabric_index);
            if (fabric == nullptr) {
                ChipLogError(AppServer, "Group removed from nonexistent fabric?");
                return;
            }

            LogErrorOnFailure(mSystemState->TransportMgr()->MulticastGroupJoinLeave(
                                  chip::Transport::PeerAddress::BuildMatterPerGroupMulticastAddress(fabric->GetFabricId(), old_group.group_id), false));
        };

    private:
        const chip::Controller::DeviceControllerSystemState *mSystemState;
    };

    matter_controller_client() {}

    bool m_operational_advertising = true;
    controller_storage_delegate m_default_storage;
    chip::PersistentStorageOperationalKeystore m_operational_keystore;
    chip::Credentials::PersistentStorageOpCertStore m_operational_cert_store;
    chip::Crypto::DefaultSessionKeystore m_session_key_store;
    chip::Credentials::GroupDataProviderImpl m_group_data_provider{k_max_groups_per_fabric,
             k_max_group_keys_per_fabric};
    GroupDataProviderListener m_group_data_provider_listener;
    credentials_issuer *m_credentials_issuer;
    NodeId m_controller_node_id;
    FabricId m_controller_fabric_id;
    chip::app::DefaultICDClientStorage m_icd_client_storage;
    controller_check_in_delegate m_icd_check_in_delegate;
    chip::app::CheckInHandler m_check_in_handler;

#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    chip::Controller::AutoCommissioner m_auto_commissioner;
    MatterDeviceCommissioner m_device_commissioner;
#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
    CommissionerDiscoveryController m_commissioner_discovery_controller;
#endif // CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
#else // !CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    MatterDeviceController m_device_controller;
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
};

} // namespace controller
} // namespace esp_matter
