// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_command.h>
#include <esp_matter_core.h>

#include <app-common/zap-generated/callback.h>
#include <app/InteractionModelEngine.h>
#include <app/util/af.h>
#include <app/util/ember-compatibility-functions.h>

using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;

#if (FIXED_ENDPOINT_COUNT == 0)

static const char *TAG = "esp_matter_command";

namespace esp_matter {
namespace command {

void DispatchSingleClusterCommandCommon(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    int endpoint_id = command_path.mEndpointId;
    int cluster_id = command_path.mClusterId;
    int command_id = command_path.mCommandId;
    ESP_LOGI(TAG, "Received command 0x%04X for enpoint 0x%04X's cluster 0x%08X", command_id, endpoint_id, cluster_id);

    node_t *node = node::get();
    endpoint_t *endpoint = endpoint::get(node, endpoint_id);
    cluster_t *cluster = cluster::get(endpoint, cluster_id);
    command_t *command = get(cluster, command_id);
    if (!command) {
        ESP_LOGE(TAG, "Command 0x%04X not found", command_id);
        return;
    }
    esp_err_t err = ESP_OK;
    callback_t callback = get_callback(command);
    if (callback) {
        err = callback(command_path, tlv_data, opaque_ptr);
    }
    int flags = get_flags(command);
    if (flags & COMMAND_FLAG_CUSTOM) {
        EmberAfStatus status = (err == ESP_OK) ? EMBER_ZCL_STATUS_SUCCESS : EMBER_ZCL_STATUS_FAILURE;
        emberAfSendImmediateDefaultResponse(status);
    }
}

} /* command */
} /* esp_matter */

namespace chip {
namespace app {

void DispatchSingleClusterCommand(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                  CommandHandler *command_obj)
{
    Compatibility::SetupEmberAfCommandHandler(command_obj, command_path);

    esp_matter::command::DispatchSingleClusterCommandCommon(command_path, tlv_data, command_obj);

    Compatibility::ResetEmberAfObjects();
}

} /* namespace app */
} /* namespace chip */

static esp_err_t esp_matter_command_callback_key_set_write(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::GroupKeyManagement::Commands::KeySetWrite::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupKeyManagementClusterKeySetWriteCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_key_set_read(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::GroupKeyManagement::Commands::KeySetRead::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupKeyManagementClusterKeySetReadCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_key_set_remove(const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::GroupKeyManagement::Commands::KeySetRemove::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupKeyManagementClusterKeySetRemoveCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_key_set_read_all_indices(const ConcreteCommandPath &command_path,
                                                                      TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::GroupKeyManagement::Commands::KeySetReadAllIndices::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupKeyManagementClusterKeySetReadAllIndicesCallback((CommandHandler *)opaque_ptr, command_path,
                                                                     command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_arm_fail_safe(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::GeneralCommissioning::Commands::ArmFailSafe::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGeneralCommissioningClusterArmFailSafeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_regulatory_config(const ConcreteCommandPath &command_path,
                                                                   TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::GeneralCommissioning::Commands::SetRegulatoryConfig::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGeneralCommissioningClusterSetRegulatoryConfigCallback((CommandHandler *)opaque_ptr, command_path,
                                                                      command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_commissioning_complete(const ConcreteCommandPath &command_path,
                                                                    TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::GeneralCommissioning::Commands::CommissioningComplete::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGeneralCommissioningClusterCommissioningCompleteCallback((CommandHandler *)opaque_ptr, command_path,
                                                                        command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_scan_networks(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::NetworkCommissioning::Commands::ScanNetworks::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterScanNetworksCallback((CommandHandler *)opaque_ptr, command_path,
                                                               command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_or_update_wifi_network(const ConcreteCommandPath &command_path,
                                                                        TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::NetworkCommissioning::Commands::AddOrUpdateWiFiNetwork::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterAddOrUpdateWiFiNetworkCallback((CommandHandler *)opaque_ptr, command_path,
                                                                         command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_or_update_thread_network(const ConcreteCommandPath &command_path,
                                                                          TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::NetworkCommissioning::Commands::AddOrUpdateThreadNetwork::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterAddOrUpdateThreadNetworkCallback((CommandHandler *)opaque_ptr, command_path,
                                                                           command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_network(const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::NetworkCommissioning::Commands::RemoveNetwork::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterRemoveNetworkCallback((CommandHandler *)opaque_ptr, command_path,
                                                                command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_connect_network(const ConcreteCommandPath &command_path,
                                                             TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::NetworkCommissioning::Commands::ConnectNetwork::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterConnectNetworkCallback((CommandHandler *)opaque_ptr, command_path,
                                                                 command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_reorder_network(const ConcreteCommandPath &command_path,
                                                             TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::NetworkCommissioning::Commands::ReorderNetwork::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterReorderNetworkCallback((CommandHandler *)opaque_ptr, command_path,
                                                                 command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_open_commissioning_window(const ConcreteCommandPath &command_path,
                                                                       TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::AdministratorCommissioning::Commands::OpenCommissioningWindow::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfAdministratorCommissioningClusterOpenCommissioningWindowCallback((CommandHandler *)opaque_ptr,
                                                                                command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_open_basic_commissioning_window(const ConcreteCommandPath &command_path,
                                                                             TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::AdministratorCommissioning::Commands::OpenBasicCommissioningWindow::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfAdministratorCommissioningClusterOpenBasicCommissioningWindowCallback((CommandHandler *)opaque_ptr,
                                                                                     command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_revoke_commissioning(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::AdministratorCommissioning::Commands::RevokeCommissioning::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfAdministratorCommissioningClusterRevokeCommissioningCallback((CommandHandler *)opaque_ptr, command_path,
                                                                            command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_attestation_request(const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OperationalCredentials::Commands::AttestationRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterAttestationRequestCallback((CommandHandler *)opaque_ptr, command_path,
                                                                       command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_certificate_chain_request(const ConcreteCommandPath &command_path,
                                                                       TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OperationalCredentials::Commands::CertificateChainRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterCertificateChainRequestCallback((CommandHandler *)opaque_ptr, command_path,
                                                                            command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_csr_request(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::OperationalCredentials::Commands::CSRRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterCSRRequestCallback((CommandHandler *)opaque_ptr, command_path,
                                                               command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_noc(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                     void *opaque_ptr)
{
    chip::app::Clusters::OperationalCredentials::Commands::AddNOC::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterAddNOCCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_update_noc(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::OperationalCredentials::Commands::UpdateNOC::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterUpdateNOCCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_update_fabric_label(const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OperationalCredentials::Commands::UpdateFabricLabel::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterUpdateFabricLabelCallback((CommandHandler *)opaque_ptr, command_path,
                                                                      command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_fabric(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::OperationalCredentials::Commands::RemoveFabric::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterRemoveFabricCallback((CommandHandler *)opaque_ptr, command_path,
                                                                 command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_trusted_root_certificate(const ConcreteCommandPath &command_path,
                                                                          TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OperationalCredentials::Commands::AddTrustedRootCertificate::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterAddTrustedRootCertificateCallback((CommandHandler *)opaque_ptr,
                                                                              command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_trusted_root_certificate(const ConcreteCommandPath &command_path,
                                                                             TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OperationalCredentials::Commands::RemoveTrustedRootCertificate::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterRemoveTrustedRootCertificateCallback((CommandHandler *)opaque_ptr,
                                                                                 command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_query_image(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::OtaSoftwareUpdateProvider::Commands::QueryImage::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOtaSoftwareUpdateProviderClusterQueryImageCallback((CommandHandler *)opaque_ptr, command_path,
                                                                  command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_apply_update_request(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OtaSoftwareUpdateProvider::Commands::ApplyUpdateRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOtaSoftwareUpdateProviderClusterApplyUpdateRequestCallback((CommandHandler *)opaque_ptr, command_path,
                                                                          command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_notify_update_applied(const ConcreteCommandPath &command_path,
                                                                   TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OtaSoftwareUpdateProvider::Commands::NotifyUpdateApplied::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOtaSoftwareUpdateProviderClusterNotifyUpdateAppliedCallback((CommandHandler *)opaque_ptr, command_path,
                                                                           command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_announce_ota_provider(const ConcreteCommandPath &command_path,
                                                                   TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OtaSoftwareUpdateRequestor::Commands::AnnounceOtaProvider::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOtaSoftwareUpdateRequestorClusterAnnounceOtaProviderCallback((CommandHandler *)opaque_ptr, command_path,
                                                                            command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_identify(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    chip::app::Clusters::Identify::Commands::Identify::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfIdentifyClusterIdentifyCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_identify_query(const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Identify::Commands::IdentifyQuery::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfIdentifyClusterIdentifyQueryCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_group(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::Groups::Commands::AddGroup::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterAddGroupCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_view_group(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::Groups::Commands::ViewGroup::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterViewGroupCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_group_membership(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Groups::Commands::GetGroupMembership::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterGetGroupMembershipCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_group(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::Groups::Commands::RemoveGroup::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterRemoveGroupCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_all_groups(const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Groups::Commands::RemoveAllGroups::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterRemoveAllGroupsCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_group_if_identifying(const ConcreteCommandPath &command_path,
                                                                      TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Groups::Commands::AddGroupIfIdentifying::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterAddGroupIfIdentifyingCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_scene(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::Scenes::Commands::AddScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterAddSceneCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_view_scene(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::Scenes::Commands::ViewScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterViewSceneCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_scene(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::Scenes::Commands::RemoveScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterRemoveSceneCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_all_scenes(const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Scenes::Commands::RemoveAllScenes::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterRemoveAllScenesCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_store_scene(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::Scenes::Commands::StoreScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterStoreSceneCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_recall_scene(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::Scenes::Commands::RecallScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterRecallSceneCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_scene_membership(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Scenes::Commands::GetSceneMembership::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterGetSceneMembershipCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_off(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                 void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::Off::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_on(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::On::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOnCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_toggle(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                    void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::Toggle::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterToggleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_off_with_effect(const ConcreteCommandPath &command_path,
                                                             TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::OffWithEffect::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOffWithEffectCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_on_with_recall_global_scene(const ConcreteCommandPath &command_path,
                                                                         TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::OnWithRecallGlobalScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOnWithRecallGlobalSceneCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_on_with_timed_off(const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::OnOff::Commands::OnWithTimedOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOnWithTimedOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_level(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::MoveToLevel::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveToLevelCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::Move::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::Step::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStepCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                  void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::Stop::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStopCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_level_with_on_off(const ConcreteCommandPath &command_path,
                                                                       TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::MoveToLevelWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveToLevelWithOnOffCallback((CommandHandler *)opaque_ptr, command_path,
                                                               command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_with_on_off(const ConcreteCommandPath &command_path,
                                                              TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::MoveWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveWithOnOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step_with_on_off(const ConcreteCommandPath &command_path,
                                                              TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::StepWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStepWithOnOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop_with_on_off(const ConcreteCommandPath &command_path,
                                                              TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::StopWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStopWithOnOffCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_hue(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveToHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToHueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_hue(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveHueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step_hue(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::StepHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStepHueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_saturation(const ConcreteCommandPath &command_path,
                                                                TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveToSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToSaturationCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_saturation(const ConcreteCommandPath &command_path,
                                                             TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveSaturationCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step_saturation(const ConcreteCommandPath &command_path,
                                                             TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::StepSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStepSaturationCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_hue_and_saturation(const ConcreteCommandPath &command_path,
                                                                        TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveToHueAndSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToHueAndSaturationCallback((CommandHandler *)opaque_ptr, command_path,
                                                                 command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop_move_step(const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::StopMoveStep::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStopMoveStepCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_lock_door(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::LockDoor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterLockDoorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_unlock_door(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::UnlockDoor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterUnlockDoorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_setpoint_raise_lower(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::SetpointRaiseLower::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterSetpointRaiseLowerCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {

namespace group_key_management {
namespace command {

command_t *create_key_set_write(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_KEY_SET_WRITE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_key_set_write);
}

command_t *create_key_set_read(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_KEY_SET_READ_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_key_set_read);
}

command_t *create_key_set_remove(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_KEY_SET_REMOVE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_key_set_remove);
}

command_t *create_key_set_read_all_indices(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_KEY_SET_READ_ALL_INDICES_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_key_set_read_all_indices);
}

command_t *create_key_set_read_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_KEY_SET_READ_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_key_set_read_all_indices_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_KEY_SET_READ_ALL_INDICES_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

} /* command */
} /* group_key_management */

namespace general_commissioning {
namespace command {

command_t *create_arm_fail_safe(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ARM_FAIL_SAFE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_arm_fail_safe);
}

command_t *create_set_regulatory_config(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_SET_REGULATORY_CONFIG_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_set_regulatory_config);
}

command_t *create_commissioning_complete(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_COMMISSIONING_COMPLETE_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_commissioning_complete);
}

command_t *create_arm_fail_safe_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ARM_FAIL_SAFE_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_set_regulatory_config_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_SET_REGULATORY_CONFIG_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_commissioning_complete_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_COMMISSIONING_COMPLETE_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

} /* command */
} /* general_commissioning */

namespace network_commissioning {
namespace command {

command_t *create_scan_networks(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_SCAN_NETWORKS_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_scan_networks);
}

command_t *create_add_or_update_wifi_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ADD_OR_UPDATE_WI_FI_NETWORK_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_add_or_update_wifi_network);
}

command_t *create_add_or_update_thread_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ADD_OR_UPDATE_THREAD_NETWORK_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_add_or_update_thread_network);
}

command_t *create_remove_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REMOVE_NETWORK_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_remove_network);
}

command_t *create_connect_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_CONNECT_NETWORK_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_connect_network);
}

command_t *create_reorder_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REORDER_NETWORK_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_reorder_network);
}

command_t *create_scan_networks_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_SCAN_NETWORKS_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_network_config_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_NETWORK_CONFIG_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_connect_network_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_CONNECT_NETWORK_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

} /* command */
} /* network_commissioning */

namespace administrator_commissioning {
namespace command {

command_t *create_open_commissioning_window(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_OPEN_COMMISSIONING_WINDOW_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_open_commissioning_window);
}

command_t *create_open_basic_commissioning_window(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_OPEN_BASIC_COMMISSIONING_WINDOW_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_open_basic_commissioning_window);
}

command_t *create_revoke_commissioning(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REVOKE_COMMISSIONING_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_revoke_commissioning);
}

} /* command */
} /* administrator_commissioning */

namespace operational_credentials {
namespace command {

command_t *create_attestation_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ATTESTATION_REQUEST_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_attestation_request);
}

command_t *create_certificate_chain_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_CERTIFICATE_CHAIN_REQUEST_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_certificate_chain_request);
}

command_t *create_csr_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_CSR_REQUEST_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_csr_request);
}

command_t *create_add_noc(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ADD_NOC_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_add_noc);
}

command_t *create_update_noc(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_UPDATE_NOC_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_update_noc);
}

command_t *create_update_fabric_label(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_UPDATE_FABRIC_LABEL_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_update_fabric_label);
}

command_t *create_remove_fabric(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REMOVE_FABRIC_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_remove_fabric);
}

command_t *create_add_trusted_root_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ADD_TRUSTED_ROOT_CERTIFICATE_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_add_trusted_root_certificate);
}

command_t *create_remove_trusted_root_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REMOVE_TRUSTED_ROOT_CERTIFICATE_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_remove_trusted_root_certificate);
}

command_t *create_attestation_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ATTESTATION_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_certificate_chain_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_CERTIFICATE_CHAIN_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_csr_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_CSR_RESPONSE_COMMAND_ID, COMMAND_FLAG_SERVER_GENERATED,
                                       NULL);
}

command_t *create_noc_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_NOC_RESPONSE_COMMAND_ID, COMMAND_FLAG_SERVER_GENERATED,
                                       NULL);
}

} /* command */
} /* operational_credentials */

namespace ota_provider {
namespace command {

command_t *create_query_image(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_QUERY_IMAGE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_query_image);
}

command_t *create_apply_update_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_APPLY_UPDATE_REQUEST_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_apply_update_request);
}

command_t *create_notify_update_applied(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_NOTIFY_UPDATE_APPLIED_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_notify_update_applied);
}

command_t *create_query_image_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_QUERY_IMAGE_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_apply_update_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_APPLY_UPDATE_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

} /* command */
} /* ota_provider */

namespace ota_requestor {
namespace command {

command_t *create_announce_ota_provider(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ANNOUNCE_OTA_PROVIDER_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_announce_ota_provider);
}

} /* command */
} /* ota_requestor */

namespace identify {
namespace command {

command_t *create_identify(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_IDENTIFY_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_identify);
}

command_t *create_identify_query(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_IDENTIFY_QUERY_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_identify_query);
}

command_t *create_identify_query_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_IDENTIFY_QUERY_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

} /* command */
} /* identify */

namespace groups {
namespace command {

command_t *create_add_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ADD_GROUP_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_add_group);
}

command_t *create_view_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_VIEW_GROUP_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_view_group);
}

command_t *create_get_group_membership(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_GET_GROUP_MEMBERSHIP_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_get_group_membership);
}

command_t *create_remove_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REMOVE_GROUP_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_remove_group);
}

command_t *create_remove_all_groups(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REMOVE_ALL_GROUPS_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_remove_all_groups);
}

command_t *create_add_group_if_identifying(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ADD_GROUP_IF_IDENTIFYING_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_add_group_if_identifying);
}

command_t *create_add_group_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ADD_GROUP_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_view_group_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_VIEW_GROUP_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_get_group_membership_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_GET_GROUP_MEMBERSHIP_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_remove_group_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REMOVE_GROUP_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

} /* command */
} /* groups */

namespace scenes {
namespace command {

command_t *create_add_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ADD_SCENE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_add_scene);
}

command_t *create_view_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_VIEW_SCENE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_view_scene);
}

command_t *create_remove_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REMOVE_SCENE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_remove_scene);
}

command_t *create_remove_all_scenes(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REMOVE_ALL_SCENES_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_remove_all_scenes);
}

command_t *create_store_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_STORE_SCENE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_store_scene);
}

command_t *create_recall_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_RECALL_SCENE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_recall_scene);
}

command_t *create_get_scene_membership(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_GET_SCENE_MEMBERSHIP_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_get_scene_membership);
}

command_t *create_add_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ADD_SCENE_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_view_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_VIEW_SCENE_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_remove_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REMOVE_SCENE_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_remove_all_scenes_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_REMOVE_ALL_SCENES_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_store_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_STORE_SCENE_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

command_t *create_get_scene_membership_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_GET_SCENE_MEMBERSHIP_RESPONSE_COMMAND_ID,
                                       COMMAND_FLAG_SERVER_GENERATED, NULL);
}

} /* command */
} /* scenes */

namespace on_off {
namespace command {

command_t *create_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_OFF_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_off);
}

command_t *create_on(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ON_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_on);
}

command_t *create_toggle(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_TOGGLE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_toggle);
}

command_t *create_off_with_effect(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_OFF_WITH_EFFECT_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_off_with_effect);
}

command_t *create_on_with_recall_global_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ON_WITH_RECALL_GLOBAL_SCENE_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_on_with_recall_global_scene);
}

command_t *create_on_with_timed_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_ON_WITH_TIMED_OFF_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_on_with_timed_off);
}

} /* command */
} /* on_off */

namespace level_control {
namespace command {

command_t *create_move_to_level(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_MOVE_TO_LEVEL_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_move_to_level);
}

command_t *create_move(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_MOVE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_move);
}

command_t *create_step(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_STEP_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_step);
}

command_t *create_stop(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_STOP_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_stop);
}

command_t *create_move_to_level_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_MOVE_TO_LEVEL_WITH_ON_OFF_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_move_to_level_with_on_off);
}

command_t *create_move_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_MOVE_WITH_ON_OFF_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_move_with_on_off);
}

command_t *create_step_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_STEP_WITH_ON_OFF_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_step_with_on_off);
}

command_t *create_stop_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_STOP_WITH_ON_OFF_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_stop_with_on_off);
}

} /* command */
} /* level_control */

namespace color_control {
namespace command {

command_t *create_move_to_hue(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_MOVE_TO_HUE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_move_to_hue);
}

command_t *create_move_hue(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_MOVE_HUE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_move_hue);
}

command_t *create_step_hue(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_STEP_HUE_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_step_hue);
}

command_t *create_move_to_saturation(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_MOVE_TO_SATURATION_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_move_to_saturation);
}

command_t *create_move_saturation(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_MOVE_SATURATION_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_move_saturation);
}

command_t *create_step_saturation(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_STEP_SATURATION_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_step_saturation);
}

command_t *create_move_to_hue_and_saturation(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_move_to_hue_and_saturation);
}

command_t *create_stop_move_step(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_STOP_MOVE_STEP_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_stop_move_step);
}

} /* command */
} /* color_control */

namespace thermostat {
namespace command {

command_t *create_setpoint_raise_lower(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_SETPOINT_RAISE_LOWER_COMMAND_ID,
                                       COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_setpoint_raise_lower);
}

} /* command */
} /* thermostat */

namespace door_lock {
namespace command {

command_t *create_lock_door(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_LOCK_DOOR_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_lock_door);
}

command_t *create_unlock_door(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ZCL_UNLOCK_DOOR_COMMAND_ID, COMMAND_FLAG_CLIENT_GENERATED,
                                       esp_matter_command_callback_unlock_door);
}

} /* command */
} /* door_lock */

} /* cluster */
} /* esp_matter */

#endif /* FIXED_ENDPOINT_COUNT */
