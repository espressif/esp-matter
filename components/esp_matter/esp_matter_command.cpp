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
#include <app-common/zap-generated/ids/Commands.h>
#include <app/InteractionModelEngine.h>
#include <app/util/basic-types.h>
#include <app/util/ember-compatibility-functions.h>

using chip::ByteSpan;
using chip::CharSpan;
using chip::app::CommandHandler;
using chip::app::CommandSender;
using chip::app::ConcreteCommandPath;
using chip::app::DataModel::Decode;
using chip::TLV::TagNumFromTag;
using chip::TLV::TLVReader;

#if (FIXED_ENDPOINT_COUNT == 0)

static const char *TAG = "esp_matter_command";

static esp_matter_command_custom_callback_t custom_callback = NULL;
static void *custom_callback_priv_data = NULL;

esp_err_t esp_matter_command_set_custom_callback(esp_matter_command_custom_callback_t callback, void *priv_data)
{
    custom_callback = callback;
    custom_callback_priv_data = priv_data;
    return ESP_OK;
}

void DispatchSingleClusterCommandCommon(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *command_obj)
{
    int endpoint_id = command_path.mEndpointId;
    int cluster_id = command_path.mClusterId;
    int command_id = command_path.mCommandId;
    ESP_LOGI(TAG, "Received command 0x%04X for enpoint 0x%04X's cluster 0x%04X", command_id, endpoint_id, cluster_id);

    esp_matter_node_t *node = esp_matter_node_get();
    esp_matter_endpoint_t *endpoint = esp_matter_endpoint_get(node, endpoint_id);
    esp_matter_cluster_t *cluster = esp_matter_cluster_get(endpoint, cluster_id);
    esp_matter_command_t *command = esp_matter_command_get(cluster, command_id);
    if (!command) {
        ESP_LOGE(TAG, "Command 0x%04X not found", command_id);
        return;
    }
    int flags = esp_matter_command_get_flags(command);
    if (flags & ESP_MATTER_COMMAND_FLAG_CUSTOM) {
        if (custom_callback) {
            custom_callback(endpoint_id, cluster_id, command_id, tlv_data, custom_callback_priv_data);
        }
    } else {
        esp_matter_command_callback_t callback = esp_matter_command_get_callback(command);
        callback(command_obj, command_path, tlv_data);
    }
}

namespace chip {
namespace app {

void DispatchSingleClusterCommand(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                  CommandHandler *command_obj)
{
    Compatibility::SetupEmberAfCommandHandler(command_obj, command_path);

    DispatchSingleClusterCommandCommon(command_path, tlv_data, command_obj);

    Compatibility::ResetEmberAfObjects();
}

void DispatchSingleClusterResponseCommand(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                          CommandSender *command_obj)
{
    Compatibility::SetupEmberAfCommandSender(command_obj, command_path);
    TLV::TLVType tlv_type;
    tlv_data.EnterContainer(tlv_type);

    DispatchSingleClusterCommandCommon(command_path, tlv_data, command_obj);

    tlv_data.ExitContainer(tlv_type);
    Compatibility::ResetEmberAfObjects();
}

} /* namespace app */
} /* namespace chip */

static void esp_matter_command_callback_key_set_write(void *command_obj, const ConcreteCommandPath &command_path,
                                               TLVReader &tlv_data)
{
    /* Not implemented
    chip::app::Clusters::GroupKeyManagement::Commands::KeySetWrite::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupKeyManagementClusterKeySetWriteCallback((CommandHandler *)command_obj, command_path, command_data);
    }
    */
}

static void esp_matter_command_callback_key_set_read(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data)
{
    /* Not implemented
    chip::app::Clusters::GroupKeyManagement::Commands::KeySetRead::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupKeyManagementClusterKeySetReadCallback((CommandHandler *)command_obj, command_path, command_data);
    }
    */
}

static void esp_matter_command_callback_key_set_remove(void *command_obj, const ConcreteCommandPath &command_path,
                                                TLVReader &tlv_data)
{
    /* Not implemented
    chip::app::Clusters::GroupKeyManagement::Commands::KeySetRemove::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupKeyManagementClusterKeySetRemoveCallback((CommandHandler *)command_obj, command_path, command_data);
    }
    */
}

static void esp_matter_command_callback_key_set_read_all_indices(void *command_obj, const ConcreteCommandPath &command_path,
                                                          TLVReader &tlv_data)
{
    /* Not implemented
    chip::app::Clusters::GroupKeyManagement::Commands::KeySetReadAllIndices::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupKeyManagementClusterKeySetReadAllIndicesCallback((CommandHandler *)command_obj, command_path,
                                                                     command_data);
    }
    */
}

static void esp_matter_command_callback_arm_fail_safe(void *command_obj, const ConcreteCommandPath &command_path,
                                               TLVReader &tlv_data)
{
    chip::app::Clusters::GeneralCommissioning::Commands::ArmFailSafe::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGeneralCommissioningClusterArmFailSafeCallback((CommandHandler *)command_obj, command_path,
                                                              command_data);
    }
}

static void esp_matter_command_callback_set_regulatory_config(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data)
{
    chip::app::Clusters::GeneralCommissioning::Commands::SetRegulatoryConfig::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGeneralCommissioningClusterSetRegulatoryConfigCallback((CommandHandler *)command_obj, command_path,
                                                                      command_data);
    }
}

static void esp_matter_command_callback_commissioning_complete(void *command_obj, const ConcreteCommandPath &command_path,
                                                        TLVReader &tlv_data)
{
    chip::app::Clusters::GeneralCommissioning::Commands::CommissioningComplete::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGeneralCommissioningClusterCommissioningCompleteCallback((CommandHandler *)command_obj, command_path,
                                                                        command_data);
    }
}

static void esp_matter_command_callback_scan_networks(void *command_obj, const ConcreteCommandPath &command_path,
                                               TLVReader &tlv_data)
{
    chip::app::Clusters::NetworkCommissioning::Commands::ScanNetworks::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterScanNetworksCallback((CommandHandler *)command_obj, command_path,
                                                               command_data);
    }
}

static void esp_matter_command_callback_add_or_update_wifi_network(void *command_obj, const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data)
{
    chip::app::Clusters::NetworkCommissioning::Commands::AddOrUpdateWiFiNetwork::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterAddOrUpdateWiFiNetworkCallback((CommandHandler *)command_obj, command_path,
                                                                         command_data);
    }
}

static void esp_matter_command_callback_add_or_update_thread_network(void *command_obj,
                                                              const ConcreteCommandPath &command_path,
                                                              TLVReader &tlv_data)
{
    chip::app::Clusters::NetworkCommissioning::Commands::AddOrUpdateThreadNetwork::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterAddOrUpdateThreadNetworkCallback((CommandHandler *)command_obj, command_path,
                                                                           command_data);
    }
}

static void esp_matter_command_callback_remove_network(void *command_obj, const ConcreteCommandPath &command_path,
                                                TLVReader &tlv_data)
{
    chip::app::Clusters::NetworkCommissioning::Commands::RemoveNetwork::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterRemoveNetworkCallback((CommandHandler *)command_obj, command_path,
                                                                command_data);
    }
}

static void esp_matter_command_callback_connect_network(void *command_obj, const ConcreteCommandPath &command_path,
                                                 TLVReader &tlv_data)
{
    chip::app::Clusters::NetworkCommissioning::Commands::ConnectNetwork::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterConnectNetworkCallback((CommandHandler *)command_obj, command_path,
                                                                 command_data);
    }
}

static void esp_matter_command_callback_reorder_network(void *command_obj, const ConcreteCommandPath &command_path,
                                                 TLVReader &tlv_data)
{
    chip::app::Clusters::NetworkCommissioning::Commands::ReorderNetwork::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfNetworkCommissioningClusterReorderNetworkCallback((CommandHandler *)command_obj, command_path,
                                                                 command_data);
    }
}

static void esp_matter_command_callback_open_commissioning_window(void *command_obj, const ConcreteCommandPath &command_path,
                                                           TLVReader &tlv_data)
{
    chip::app::Clusters::AdministratorCommissioning::Commands::OpenCommissioningWindow::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfAdministratorCommissioningClusterOpenCommissioningWindowCallback((CommandHandler *)command_obj,
                                                                                command_path, command_data);
    }
}

static void esp_matter_command_callback_open_basic_commissioning_window(void *command_obj,
                                                                 const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data)
{
    chip::app::Clusters::AdministratorCommissioning::Commands::OpenBasicCommissioningWindow::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfAdministratorCommissioningClusterOpenBasicCommissioningWindowCallback((CommandHandler *)command_obj,
                                                                                     command_path, command_data);
    }
}

static void esp_matter_command_callback_revoke_commissioning(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data)
{
    chip::app::Clusters::AdministratorCommissioning::Commands::RevokeCommissioning::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfAdministratorCommissioningClusterRevokeCommissioningCallback((CommandHandler *)command_obj, command_path,
                                                                            command_data);
    }
}

static void esp_matter_command_callback_attestation_request(void *command_obj, const ConcreteCommandPath &command_path,
                                                     TLVReader &tlv_data)
{
    chip::app::Clusters::OperationalCredentials::Commands::AttestationRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterAttestationRequestCallback((CommandHandler *)command_obj, command_path,
                                                                       command_data);
    }
}

static void esp_matter_command_callback_certificate_chain_request(void *command_obj, const ConcreteCommandPath &command_path,
                                                           TLVReader &tlv_data)
{
    chip::app::Clusters::OperationalCredentials::Commands::CertificateChainRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterCertificateChainRequestCallback((CommandHandler *)command_obj, command_path,
                                                                            command_data);
    }
}

static void esp_matter_command_callback_csr_request(void *command_obj, const ConcreteCommandPath &command_path,
                                             TLVReader &tlv_data)
{
    chip::app::Clusters::OperationalCredentials::Commands::OpCSRRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterOpCSRRequestCallback((CommandHandler *)command_obj, command_path,
                                                                 command_data);
    }
}

static void esp_matter_command_callback_add_noc(void *command_obj, const ConcreteCommandPath &command_path,
                                         TLVReader &tlv_data)
{
    chip::app::Clusters::OperationalCredentials::Commands::AddNOC::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterAddNOCCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_update_noc(void *command_obj, const ConcreteCommandPath &command_path,
                                            TLVReader &tlv_data)
{
    chip::app::Clusters::OperationalCredentials::Commands::UpdateNOC::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterUpdateNOCCallback((CommandHandler *)command_obj, command_path,
                                                              command_data);
    }
}

static void esp_matter_command_callback_update_fabric_label(void *command_obj, const ConcreteCommandPath &command_path,
                                                     TLVReader &tlv_data)
{
    chip::app::Clusters::OperationalCredentials::Commands::UpdateFabricLabel::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterUpdateFabricLabelCallback((CommandHandler *)command_obj, command_path,
                                                                      command_data);
    }
}

static void esp_matter_command_callback_remove_fabric(void *command_obj, const ConcreteCommandPath &command_path,
                                               TLVReader &tlv_data)
{
    chip::app::Clusters::OperationalCredentials::Commands::RemoveFabric::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterRemoveFabricCallback((CommandHandler *)command_obj, command_path,
                                                                 command_data);
    }
}

static void esp_matter_command_callback_add_trusted_root_certificate(void *command_obj,
                                                              const ConcreteCommandPath &command_path,
                                                              TLVReader &tlv_data)
{
    chip::app::Clusters::OperationalCredentials::Commands::AddTrustedRootCertificate::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterAddTrustedRootCertificateCallback((CommandHandler *)command_obj,
                                                                              command_path, command_data);
    }
}

static void esp_matter_command_callback_remove_trusted_root_certificate(void *command_obj,
                                                                 const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data)
{
    chip::app::Clusters::OperationalCredentials::Commands::RemoveTrustedRootCertificate::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOperationalCredentialsClusterRemoveTrustedRootCertificateCallback((CommandHandler *)command_obj,
                                                                                 command_path, command_data);
    }
}

static void esp_matter_command_callback_query_image(void *command_obj, const ConcreteCommandPath &command_path,
                                             TLVReader &tlv_data)
{
    chip::app::Clusters::OtaSoftwareUpdateProvider::Commands::QueryImage::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOtaSoftwareUpdateProviderClusterQueryImageCallback((CommandHandler *)command_obj, command_path,
                                                                  command_data);
    }
}

static void esp_matter_command_callback_apply_update_request(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data)
{
    chip::app::Clusters::OtaSoftwareUpdateProvider::Commands::ApplyUpdateRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOtaSoftwareUpdateProviderClusterApplyUpdateRequestCallback((CommandHandler *)command_obj, command_path,
                                                                          command_data);
    }
}

static void esp_matter_command_callback_notify_update_applied(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data)
{
    chip::app::Clusters::OtaSoftwareUpdateProvider::Commands::NotifyUpdateApplied::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOtaSoftwareUpdateProviderClusterNotifyUpdateAppliedCallback((CommandHandler *)command_obj, command_path,
                                                                           command_data);
    }
}

static void esp_matter_command_callback_announce_ota_provider(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data)
{
    chip::app::Clusters::OtaSoftwareUpdateRequestor::Commands::AnnounceOtaProvider::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOtaSoftwareUpdateRequestorClusterAnnounceOtaProviderCallback((CommandHandler *)command_obj, command_path,
                                                                            command_data);
    }
}

static void esp_matter_command_callback_identify(void *command_obj, const ConcreteCommandPath &command_path,
                                          TLVReader &tlv_data)
{
    chip::app::Clusters::Identify::Commands::Identify::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfIdentifyClusterIdentifyCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_identify_query(void *command_obj, const ConcreteCommandPath &command_path,
                                                TLVReader &tlv_data)
{
    chip::app::Clusters::Identify::Commands::IdentifyQuery::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfIdentifyClusterIdentifyQueryCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_add_group(void *command_obj, const ConcreteCommandPath &command_path,
                                           TLVReader &tlv_data)
{
    chip::app::Clusters::Groups::Commands::AddGroup::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterAddGroupCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_view_group(void *command_obj, const ConcreteCommandPath &command_path,
                                            TLVReader &tlv_data)
{
    chip::app::Clusters::Groups::Commands::ViewGroup::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterViewGroupCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_get_group_membership(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data)
{
    chip::app::Clusters::Groups::Commands::GetGroupMembership::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterGetGroupMembershipCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_remove_group(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data)
{
    chip::app::Clusters::Groups::Commands::RemoveGroup::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterRemoveGroupCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_remove_all_groups(void *command_obj, const ConcreteCommandPath &command_path,
                                                   TLVReader &tlv_data)
{
    chip::app::Clusters::Groups::Commands::RemoveAllGroups::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterRemoveAllGroupsCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_add_group_if_identifying(void *command_obj, const ConcreteCommandPath &command_path,
                                                          TLVReader &tlv_data)
{
    chip::app::Clusters::Groups::Commands::AddGroupIfIdentifying::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGroupsClusterAddGroupIfIdentifyingCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_add_scene(void *command_obj, const ConcreteCommandPath &command_path,
                                           TLVReader &tlv_data)
{
    chip::app::Clusters::Scenes::Commands::AddScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterAddSceneCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_view_scene(void *command_obj, const ConcreteCommandPath &command_path,
                                            TLVReader &tlv_data)
{
    chip::app::Clusters::Scenes::Commands::ViewScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterViewSceneCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_remove_scene(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data)
{
    chip::app::Clusters::Scenes::Commands::RemoveScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterRemoveSceneCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_remove_all_scenes(void *command_obj, const ConcreteCommandPath &command_path,
                                                   TLVReader &tlv_data)
{
    chip::app::Clusters::Scenes::Commands::RemoveAllScenes::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterRemoveAllScenesCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_store_scene(void *command_obj, const ConcreteCommandPath &command_path,
                                             TLVReader &tlv_data)
{
    chip::app::Clusters::Scenes::Commands::StoreScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterStoreSceneCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_recall_scene(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data)
{
    chip::app::Clusters::Scenes::Commands::RecallScene::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterRecallSceneCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_get_scene_membership(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data)
{
    chip::app::Clusters::Scenes::Commands::GetSceneMembership::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfScenesClusterGetSceneMembershipCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_bind(void *command_obj, const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data)
{
    chip::app::Clusters::Binding::Commands::Bind::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfBindingClusterBindCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_unbind(void *command_obj, const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data)
{
    chip::app::Clusters::Binding::Commands::Unbind::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfBindingClusterUnbindCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_off(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data)
{
    chip::app::Clusters::OnOff::Commands::Off::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOffCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_on(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data)
{
    chip::app::Clusters::OnOff::Commands::On::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterOnCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_toggle(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data)
{
    chip::app::Clusters::OnOff::Commands::Toggle::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOnOffClusterToggleCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_move_to_level(void *command_obj, const ConcreteCommandPath &command_path,
                                               TLVReader &tlv_data)
{
    chip::app::Clusters::LevelControl::Commands::MoveToLevel::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveToLevelCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_move(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data)
{
    chip::app::Clusters::LevelControl::Commands::Move::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_step(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data)
{
    chip::app::Clusters::LevelControl::Commands::Step::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStepCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_stop(void *command_obj, const ConcreteCommandPath &command_path, TLVReader &tlv_data)
{
    chip::app::Clusters::LevelControl::Commands::Stop::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStopCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_move_to_level_with_on_off(void *command_obj, const ConcreteCommandPath &command_path,
                                                           TLVReader &tlv_data)
{
    chip::app::Clusters::LevelControl::Commands::MoveToLevelWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveToLevelWithOnOffCallback((CommandHandler *)command_obj, command_path,
                                                               command_data);
    }
}

static void esp_matter_command_callback_move_with_on_off(void *command_obj, const ConcreteCommandPath &command_path,
                                                  TLVReader &tlv_data)
{
    chip::app::Clusters::LevelControl::Commands::MoveWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveWithOnOffCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_step_with_on_off(void *command_obj, const ConcreteCommandPath &command_path,
                                                  TLVReader &tlv_data)
{
    chip::app::Clusters::LevelControl::Commands::StepWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStepWithOnOffCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_stop_with_on_off(void *command_obj, const ConcreteCommandPath &command_path,
                                                  TLVReader &tlv_data)
{
    chip::app::Clusters::LevelControl::Commands::StopWithOnOff::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterStopWithOnOffCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_move_to_hue(void *command_obj, const ConcreteCommandPath &command_path,
                                             TLVReader &tlv_data)
{
    chip::app::Clusters::ColorControl::Commands::MoveToHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToHueCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_move_hue(void *command_obj, const ConcreteCommandPath &command_path,
                                          TLVReader &tlv_data)
{
    chip::app::Clusters::ColorControl::Commands::MoveHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveHueCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_step_hue(void *command_obj, const ConcreteCommandPath &command_path,
                                          TLVReader &tlv_data)
{
    chip::app::Clusters::ColorControl::Commands::StepHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStepHueCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_move_to_saturation(void *command_obj, const ConcreteCommandPath &command_path,
                                                    TLVReader &tlv_data)
{
    chip::app::Clusters::ColorControl::Commands::MoveToSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToSaturationCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_move_saturation(void *command_obj, const ConcreteCommandPath &command_path,
                                                 TLVReader &tlv_data)
{
    chip::app::Clusters::ColorControl::Commands::MoveSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveSaturationCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_step_saturation(void *command_obj, const ConcreteCommandPath &command_path,
                                                 TLVReader &tlv_data)
{
    chip::app::Clusters::ColorControl::Commands::StepSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStepSaturationCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_move_to_hue_and_saturation(void *command_obj, const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data)
{
    chip::app::Clusters::ColorControl::Commands::MoveToHueAndSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToHueAndSaturationCallback((CommandHandler *)command_obj, command_path,
                                                                 command_data);
    }
}

static void esp_matter_command_callback_setpoint_raise_lower(void *command_obj, const ConcreteCommandPath &command_path,
                                                             TLVReader &tlv_data)
{
    chip::app::Clusters::Thermostat::Commands::SetpointRaiseLower::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterSetpointRaiseLowerCallback((CommandHandler *)command_obj, command_path, command_data);
    }
}

static void esp_matter_command_callback_key_set_read_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data)
{
    /* Not implemented
    uint32_t tag_id = 0;
    ByteSpan group_key_set_struct;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
            case 0:
                tlv_data.Get(group_key_set_struct);
                break;

            default:
                break;
        }
    }

    emberAfGroupKeyManagementClusterKeySetReadResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj,
                                                               group_key_set_struct);
    */
}

static void esp_matter_command_callback_key_set_read_all_indices_response(void *command_obj,
                                                                   const ConcreteCommandPath &command_path,
                                                                   TLVReader &tlv_data)
{
    /* Not implemented
    uint32_t tag_id = 0;
    ByteSpan group_key_set_indices;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
            case 0:
                tlv_data.Get(group_key_set_indices);
                break;

            default:
                break;
        }
    }

    emberAfGroupKeyManagementClusterKeySetReadAllIndicesResponseCallback(command_path.mEndpointId,
                                                                         (CommandSender *)command_obj,
                                                                         group_key_set_indices);
    */
}

static void esp_matter_command_callback_arm_fail_safe_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                        TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t error_code;
    CharSpan debug_text;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(error_code);
            break;

        case 1:
            tlv_data.Get(debug_text);
            break;

        default:
            break;
        }
    }

    emberAfGeneralCommissioningClusterArmFailSafeResponseCallback(command_path.mEndpointId,
                                                                  (CommandSender *)command_obj, error_code, debug_text);
}

static void esp_matter_command_callback_set_regulatory_config_response(void *command_obj,
                                                                const ConcreteCommandPath &command_path,
                                                                TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t error_code;
    CharSpan debug_text;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(error_code);
            break;

        case 1:
            tlv_data.Get(debug_text);
            break;

        default:
            break;
        }
    }

    emberAfGeneralCommissioningClusterSetRegulatoryConfigResponseCallback(command_path.mEndpointId,
                                                                          (CommandSender *)command_obj, error_code,
                                                                          debug_text);
}

static void esp_matter_command_callback_commissioning_complete_response(void *command_obj,
                                                                 const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t error_code;
    CharSpan debug_text;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(error_code);
            break;

        case 1:
            tlv_data.Get(debug_text);
            break;

        default:
            break;
        }
    }

    emberAfGeneralCommissioningClusterCommissioningCompleteResponseCallback(command_path.mEndpointId,
                                                                            (CommandSender *)command_obj, error_code,
                                                                            debug_text);
}

static void esp_matter_command_callback_scan_networks_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                        TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t error_code;
    CharSpan debug_text;
    const uint8_t *wifi_scan_results;
    const uint8_t *thread_scan_results;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(error_code);
            break;

        case 1:
            tlv_data.Get(debug_text);
            break;

        case 2:
            tlv_data.GetDataPtr(wifi_scan_results);
            break;

        case 3:
            tlv_data.GetDataPtr(thread_scan_results);
            break;

        default:
            break;
        }
    }

    emberAfNetworkCommissioningClusterScanNetworksResponseCallback(command_path.mEndpointId,
                                                                   (CommandSender *)command_obj, error_code,
                                                                   debug_text, (uint8_t *)wifi_scan_results,
                                                                   (uint8_t *)thread_scan_results);
}

static void esp_matter_command_callback_network_config_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                         TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t networking_status;
    CharSpan debug_text;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(networking_status);
            break;

        case 1:
            tlv_data.Get(debug_text);
            break;

        default:
            break;
        }
    }

    emberAfNetworkCommissioningClusterNetworkConfigResponseCallback(command_path.mEndpointId,
                                                                    (CommandSender *)command_obj, networking_status,
                                                                    debug_text);
}

static void esp_matter_command_callback_connect_network_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                          TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t networking_status;
    CharSpan debug_text;
    int32_t error_value;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(networking_status);
            break;

        case 1:
            tlv_data.Get(debug_text);
            break;

        case 2:
            tlv_data.Get(error_value);
            break;

        default:
            break;
        }
    }

    emberAfNetworkCommissioningClusterConnectNetworkResponseCallback(command_path.mEndpointId,
                                                                     (CommandSender *)command_obj, networking_status,
                                                                     debug_text, error_value);
}

static void esp_matter_command_callback_attestation_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    ByteSpan attestation_elements;
    ByteSpan attestation_signature;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(attestation_elements);
            break;

        case 1:
            tlv_data.Get(attestation_signature);
            break;

        default:
            break;
        }
    }

    emberAfOperationalCredentialsClusterAttestationResponseCallback(command_path.mEndpointId,
                                                                    (CommandSender *)command_obj, attestation_elements,
                                                                    attestation_signature);
}

static void esp_matter_command_callback_certificate_chain_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    ByteSpan certificate;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(certificate);
            break;

        default:
            break;
        }
    }

    emberAfOperationalCredentialsClusterCertificateChainResponseCallback(command_path.mEndpointId,
                                                                         (CommandSender *)command_obj, certificate);
}

static void esp_matter_command_callback_csr_response(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    ByteSpan nocsr_elements;
    ByteSpan attestation_signature;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(nocsr_elements);
            break;

        case 1:
            tlv_data.Get(attestation_signature);
            break;

        default:
            break;
        }
    }

    emberAfOperationalCredentialsClusterOpCSRResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj,
                                                              nocsr_elements, attestation_signature);
}

static void esp_matter_command_callback_noc_response(void *command_obj, const ConcreteCommandPath &command_path,
                                              TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status_code;
    uint16_t fabric_index;
    CharSpan debug_text;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status_code);
            break;

        case 1:
            tlv_data.Get(fabric_index);
            break;

        case 2:
            tlv_data.Get(debug_text);
            break;

        default:
            break;
        }
    }

    emberAfOperationalCredentialsClusterNOCResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj,
                                                            status_code, fabric_index, debug_text);
}

static void esp_matter_command_callback_query_image_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status;
    uint32_t delayed_action_time;
    CharSpan image_uri;
    uint32_t software_vesion;
    CharSpan software_vesion_string;
    ByteSpan update_token;
    bool user_consent_needed;
    ByteSpan metadata_for_requestor;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status);
            break;

        case 1:
            tlv_data.Get(delayed_action_time);
            break;

        case 2:
            tlv_data.Get(image_uri);
            break;

        case 3:
            tlv_data.Get(software_vesion);
            break;

        case 4:
            tlv_data.Get(software_vesion_string);
            break;

        case 5:
            tlv_data.Get(update_token);
            break;

        case 6:
            tlv_data.Get(user_consent_needed);
            break;

        case 7:
            tlv_data.Get(metadata_for_requestor);
            break;

        default:
            break;
        }
    }

    emberAfOtaSoftwareUpdateProviderClusterQueryImageResponseCallback(command_path.mEndpointId,
                                                                      (CommandSender *)command_obj, status,
                                                                      delayed_action_time, image_uri, software_vesion,
                                                                      software_vesion_string, update_token,
                                                                      user_consent_needed, metadata_for_requestor);
}

static void esp_matter_command_callback_apply_update_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t action;
    uint32_t delayed_action_time;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(action);
            break;

        case 1:
            tlv_data.Get(delayed_action_time);
            break;

        default:
            break;
        }
    }

    emberAfOtaSoftwareUpdateProviderClusterApplyUpdateResponseCallback(command_path.mEndpointId,
                                                                       (CommandSender *)command_obj, action,
                                                                       delayed_action_time);
}

static void esp_matter_command_callback_identify_query_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                         TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint16_t timeout;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(timeout);
            break;

        default:
            break;
        }
    }

    emberAfIdentifyClusterIdentifyQueryResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj,
                                                        timeout);
}

static void esp_matter_command_callback_add_group_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                    TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status;
    uint16_t group_id;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status);
            break;

        case 1:
            tlv_data.Get(group_id);
            break;

        default:
            break;
        }
    }

    emberAfGroupsClusterAddGroupResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj, status,
                                                 group_id);
}

static void esp_matter_command_callback_view_group_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                     TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status;
    uint16_t group_id;
    CharSpan group_name;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status);
            break;

        case 1:
            tlv_data.Get(group_id);
            break;

        case 2:
            tlv_data.Get(group_name);
            break;

        default:
            break;
        }
    }

    emberAfGroupsClusterViewGroupResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj, status,
                                                  group_id, group_name);
}

static void esp_matter_command_callback_get_group_membership_response(void *command_obj,
                                                               const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t capacity;
    const uint8_t *group_list;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(capacity);
            break;

        case 2:
            tlv_data.GetDataPtr(group_list);
            break;

        default:
            break;
        }
    }

    emberAfGroupsClusterGetGroupMembershipResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj,
                                                           capacity, (uint8_t *)group_list);
}

static void esp_matter_command_callback_remove_group_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status;
    uint16_t group_id;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status);
            break;

        case 1:
            tlv_data.Get(group_id);
            break;

        default:
            break;
        }
    }

    emberAfGroupsClusterRemoveGroupResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj, status,
                                                    group_id);
}

static void esp_matter_command_callback_add_scene_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                    TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status;
    uint16_t group_id;
    uint8_t scene_id;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status);
            break;

        case 1:
            tlv_data.Get(group_id);
            break;

        case 2:
            tlv_data.Get(scene_id);
            break;

        default:
            break;
        }
    }

    emberAfScenesClusterAddSceneResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj, status,
                                                 group_id, scene_id);
}

static void esp_matter_command_callback_view_scene_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                     TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status;
    uint16_t group_id;
    uint8_t scene_id;
    uint16_t transition_time;
    CharSpan scene_name;
    const uint8_t *extension_field_sets;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status);
            break;

        case 1:
            tlv_data.Get(group_id);
            break;

        case 2:
            tlv_data.Get(scene_id);
            break;

        case 3:
            tlv_data.Get(transition_time);
            break;

        case 4:
            tlv_data.Get(scene_name);
            break;

        case 5:
            tlv_data.GetDataPtr(extension_field_sets);
            break;

        default:
            break;
        }
    }

    emberAfScenesClusterViewSceneResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj, status,
                                                  group_id, scene_id, transition_time, scene_name,
                                                  (uint8_t *)extension_field_sets);
}

static void esp_matter_command_callback_remove_scene_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                       TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status;
    uint16_t group_id;
    uint8_t scene_id;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status);
            break;

        case 1:
            tlv_data.Get(group_id);
            break;

        case 2:
            tlv_data.Get(scene_id);
            break;

        default:
            break;
        }
    }

    emberAfScenesClusterRemoveSceneResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj, status,
                                                    group_id, scene_id);
}

static void esp_matter_command_callback_remove_all_scenes_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status;
    uint16_t group_id;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status);
            break;

        case 1:
            tlv_data.Get(group_id);
            break;

        default:
            break;
        }
    }

    emberAfScenesClusterRemoveAllScenesResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj, status,
                                                        group_id);
}

static void esp_matter_command_callback_store_scene_response(void *command_obj, const ConcreteCommandPath &command_path,
                                                      TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status;
    uint16_t group_id;
    uint8_t scene_id;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status);
            break;

        case 1:
            tlv_data.Get(group_id);
            break;

        case 2:
            tlv_data.Get(scene_id);
            break;

        default:
            break;
        }
    }

    emberAfScenesClusterStoreSceneResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj, status,
                                                   group_id, scene_id);
}

static void esp_matter_command_callback_get_scene_membership_response(void *command_obj,
                                                               const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data)
{
    uint32_t tag_id = 0;
    uint8_t status;
    uint8_t capacity;
    uint16_t group_id;
    uint8_t scene_count;
    const uint8_t *scene_list;

    while (tlv_data.Next() == CHIP_NO_ERROR) {
        tag_id = TagNumFromTag(tlv_data.GetTag());

        switch (tag_id) {
        case 0:
            tlv_data.Get(status);
            break;

        case 1:
            tlv_data.Get(capacity);
            break;

        case 2:
            tlv_data.Get(group_id);
            break;

        case 3:
            tlv_data.Get(scene_count);
            break;

        case 4:
            tlv_data.GetDataPtr(scene_list);
            break;

        default:
            break;
        }
    }

    emberAfScenesClusterGetSceneMembershipResponseCallback(command_path.mEndpointId, (CommandSender *)command_obj,
                                                           status, capacity, group_id, scene_count,
                                                           (uint8_t *)scene_list);
}

esp_matter_command_t *esp_matter_command_create_key_set_write(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_KEY_SET_WRITE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_key_set_write);
}

esp_matter_command_t *esp_matter_command_create_key_set_read(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_KEY_SET_READ_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_key_set_read);
}

esp_matter_command_t *esp_matter_command_create_key_set_remove(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_KEY_SET_REMOVE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_key_set_remove);
}

esp_matter_command_t *esp_matter_command_create_key_set_read_all_indices(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_KEY_SET_READ_ALL_INDICES_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_key_set_read_all_indices);
}

esp_matter_command_t *esp_matter_command_create_arm_fail_safe(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ARM_FAIL_SAFE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_arm_fail_safe);
}

esp_matter_command_t *esp_matter_command_create_set_regulatory_config(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_SET_REGULATORY_CONFIG_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_set_regulatory_config);
}

esp_matter_command_t *esp_matter_command_create_commissioning_complete(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_COMMISSIONING_COMPLETE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_commissioning_complete);
}

esp_matter_command_t *esp_matter_command_create_scan_networks(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_SCAN_NETWORKS_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_scan_networks);
}

esp_matter_command_t *esp_matter_command_create_add_or_update_wifi_network(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ADD_OR_UPDATE_WI_FI_NETWORK_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_add_or_update_wifi_network);
}

esp_matter_command_t *esp_matter_command_create_add_or_update_thread_network(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ADD_OR_UPDATE_THREAD_NETWORK_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_add_or_update_thread_network);
}

esp_matter_command_t *esp_matter_command_create_remove_network(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REMOVE_NETWORK_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_remove_network);
}

esp_matter_command_t *esp_matter_command_create_connect_network(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_CONNECT_NETWORK_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_connect_network);
}

esp_matter_command_t *esp_matter_command_create_reorder_network(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REORDER_NETWORK_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_reorder_network);
}

esp_matter_command_t *esp_matter_command_create_open_commissioning_window(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_OPEN_COMMISSIONING_WINDOW_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_open_commissioning_window);
}

esp_matter_command_t *esp_matter_command_create_open_basic_commissioning_window(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_OPEN_BASIC_COMMISSIONING_WINDOW_COMMAND_ID,
                                     ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_open_basic_commissioning_window);
}

esp_matter_command_t *esp_matter_command_create_revoke_commissioning(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REVOKE_COMMISSIONING_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_revoke_commissioning);
}

esp_matter_command_t *esp_matter_command_create_attestation_request(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ATTESTATION_REQUEST_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_attestation_request);
}

esp_matter_command_t *esp_matter_command_create_certificate_chain_request(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_CERTIFICATE_CHAIN_REQUEST_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_certificate_chain_request);
}

esp_matter_command_t *esp_matter_command_create_csr_request(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_OP_CSR_REQUEST_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_csr_request);
}

esp_matter_command_t *esp_matter_command_create_add_noc(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ADD_NOC_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_add_noc);
}

esp_matter_command_t *esp_matter_command_create_update_noc(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_UPDATE_NOC_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_update_noc);
}

esp_matter_command_t *esp_matter_command_create_update_fabric_label(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_UPDATE_FABRIC_LABEL_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_update_fabric_label);
}

esp_matter_command_t *esp_matter_command_create_remove_fabric(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REMOVE_FABRIC_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_remove_fabric);
}

esp_matter_command_t *esp_matter_command_create_add_trusted_root_certificate(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ADD_TRUSTED_ROOT_CERTIFICATE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_add_trusted_root_certificate);
}

esp_matter_command_t *esp_matter_command_create_remove_trusted_root_certificate(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REMOVE_TRUSTED_ROOT_CERTIFICATE_COMMAND_ID,
                                     ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_remove_trusted_root_certificate);
}

esp_matter_command_t *esp_matter_command_create_query_image(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_QUERY_IMAGE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_query_image);
}

esp_matter_command_t *esp_matter_command_create_apply_update_request(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_APPLY_UPDATE_REQUEST_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_apply_update_request);
}

esp_matter_command_t *esp_matter_command_create_notify_update_applied(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_NOTIFY_UPDATE_APPLIED_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_notify_update_applied);
}

esp_matter_command_t *esp_matter_command_create_announce_ota_provider(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ANNOUNCE_OTA_PROVIDER_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_announce_ota_provider);
}

esp_matter_command_t *esp_matter_command_create_identify(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_IDENTIFY_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_identify);
}

esp_matter_command_t *esp_matter_command_create_identify_query(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_IDENTIFY_QUERY_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_identify_query);
}

esp_matter_command_t *esp_matter_command_create_add_group(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ADD_GROUP_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_add_group);
}

esp_matter_command_t *esp_matter_command_create_view_group(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_VIEW_GROUP_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_view_group);
}

esp_matter_command_t *esp_matter_command_create_get_group_membership(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_GET_GROUP_MEMBERSHIP_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_get_group_membership);
}

esp_matter_command_t *esp_matter_command_create_remove_group(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REMOVE_GROUP_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_remove_group);
}

esp_matter_command_t *esp_matter_command_create_remove_all_groups(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REMOVE_ALL_GROUPS_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_remove_all_groups);
}

esp_matter_command_t *esp_matter_command_create_add_group_if_identifying(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ADD_GROUP_IF_IDENTIFYING_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_add_group_if_identifying);
}

esp_matter_command_t *esp_matter_command_create_add_scene(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ADD_SCENE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_add_scene);
}

esp_matter_command_t *esp_matter_command_create_view_scene(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_VIEW_SCENE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_view_scene);
}

esp_matter_command_t *esp_matter_command_create_remove_scene(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REMOVE_SCENE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_remove_scene);
}

esp_matter_command_t *esp_matter_command_create_remove_all_scenes(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REMOVE_ALL_SCENES_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_remove_all_scenes);
}

esp_matter_command_t *esp_matter_command_create_store_scene(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_STORE_SCENE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_store_scene);
}

esp_matter_command_t *esp_matter_command_create_recall_scene(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_RECALL_SCENE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_recall_scene);
}

esp_matter_command_t *esp_matter_command_create_get_scene_membership(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_GET_SCENE_MEMBERSHIP_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_get_scene_membership);
}

esp_matter_command_t *esp_matter_command_create_bind(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_BIND_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_bind);
}

esp_matter_command_t *esp_matter_command_create_unbind(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_UNBIND_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_unbind);
}

esp_matter_command_t *esp_matter_command_create_off(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_OFF_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_off);
}

esp_matter_command_t *esp_matter_command_create_on(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ON_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_on);
}

esp_matter_command_t *esp_matter_command_create_toggle(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_TOGGLE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_toggle);
}

esp_matter_command_t *esp_matter_command_create_move_to_level(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_MOVE_TO_LEVEL_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_move_to_level);
}

esp_matter_command_t *esp_matter_command_create_move(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_MOVE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_move);
}

esp_matter_command_t *esp_matter_command_create_step(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_STEP_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_step);
}

esp_matter_command_t *esp_matter_command_create_stop(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_STOP_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_stop);
}

esp_matter_command_t *esp_matter_command_create_move_to_level_with_on_off(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_MOVE_TO_LEVEL_WITH_ON_OFF_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_move_to_level_with_on_off);
}

esp_matter_command_t *esp_matter_command_create_move_with_on_off(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_MOVE_WITH_ON_OFF_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_move_with_on_off);
}

esp_matter_command_t *esp_matter_command_create_step_with_on_off(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_STEP_WITH_ON_OFF_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_step_with_on_off);
}

esp_matter_command_t *esp_matter_command_create_stop_with_on_off(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_STOP_WITH_ON_OFF_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_stop_with_on_off);
}

esp_matter_command_t *esp_matter_command_create_move_to_hue(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_MOVE_TO_HUE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_move_to_hue);
}

esp_matter_command_t *esp_matter_command_create_move_hue(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_MOVE_HUE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_move_hue);
}

esp_matter_command_t *esp_matter_command_create_step_hue(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_STEP_HUE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_step_hue);
}

esp_matter_command_t *esp_matter_command_create_move_to_saturation(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_MOVE_TO_SATURATION_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_move_to_saturation);
}

esp_matter_command_t *esp_matter_command_create_move_saturation(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_MOVE_SATURATION_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_move_saturation);
}

esp_matter_command_t *esp_matter_command_create_step_saturation(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_STEP_SATURATION_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_step_saturation);
}

esp_matter_command_t *esp_matter_command_create_move_to_hue_and_saturation(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_MOVE_TO_HUE_AND_SATURATION_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_move_to_hue_and_saturation);
}

esp_matter_command_t *esp_matter_command_create_setpoint_raise_lower(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_SETPOINT_RAISE_LOWER_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_setpoint_raise_lower);
}

esp_matter_command_t *esp_matter_command_create_key_set_read_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_KEY_SET_READ_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_key_set_read_response);
}

esp_matter_command_t *esp_matter_command_create_key_set_read_all_indices_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_KEY_SET_READ_ALL_INDICES_RESPONSE_COMMAND_ID,
                                     ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_key_set_read_all_indices_response);
}

esp_matter_command_t *esp_matter_command_create_arm_fail_safe_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ARM_FAIL_SAFE_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_arm_fail_safe_response);
}

esp_matter_command_t *esp_matter_command_create_set_regulatory_config_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_SET_REGULATORY_CONFIG_RESPONSE_COMMAND_ID,
                                     ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_set_regulatory_config_response);
}

esp_matter_command_t *esp_matter_command_create_commissioning_complete_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_COMMISSIONING_COMPLETE_RESPONSE_COMMAND_ID,
                                     ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_commissioning_complete_response);
}

esp_matter_command_t *esp_matter_command_create_scan_networks_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_SCAN_NETWORKS_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_scan_networks_response);
}

esp_matter_command_t *esp_matter_command_create_network_config_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_NETWORK_CONFIG_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_network_config_response);
}

esp_matter_command_t *esp_matter_command_create_connect_network_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_CONNECT_NETWORK_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_connect_network_response);
}

esp_matter_command_t *esp_matter_command_create_attestation_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ATTESTATION_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_attestation_response);
}

esp_matter_command_t *esp_matter_command_create_certificate_chain_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_CERTIFICATE_CHAIN_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_certificate_chain_response);
}

esp_matter_command_t *esp_matter_command_create_csr_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_OP_CSR_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_csr_response);
}

esp_matter_command_t *esp_matter_command_create_noc_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_NOC_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_noc_response);
}

esp_matter_command_t *esp_matter_command_create_query_image_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_QUERY_IMAGE_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_query_image_response);
}

esp_matter_command_t *esp_matter_command_create_apply_update_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_APPLY_UPDATE_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_apply_update_response);
}

esp_matter_command_t *esp_matter_command_create_identify_query_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_IDENTIFY_QUERY_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_identify_query_response);
}

esp_matter_command_t *esp_matter_command_create_add_group_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ADD_GROUP_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_add_group_response);
}

esp_matter_command_t *esp_matter_command_create_view_group_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_VIEW_GROUP_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_view_group_response);
}

esp_matter_command_t *esp_matter_command_create_get_group_membership_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_GET_GROUP_MEMBERSHIP_RESPONSE_COMMAND_ID,
                                     ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_get_group_membership_response);
}

esp_matter_command_t *esp_matter_command_create_remove_group_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REMOVE_GROUP_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_remove_group_response);
}

esp_matter_command_t *esp_matter_command_create_add_scene_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_ADD_SCENE_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_add_scene_response);
}

esp_matter_command_t *esp_matter_command_create_view_scene_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_VIEW_SCENE_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_view_scene_response);
}

esp_matter_command_t *esp_matter_command_create_remove_scene_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REMOVE_SCENE_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_remove_scene_response);
}

esp_matter_command_t *esp_matter_command_create_remove_all_scenes_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_REMOVE_ALL_SCENES_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_remove_all_scenes_response);
}

esp_matter_command_t *esp_matter_command_create_store_scene_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_STORE_SCENE_RESPONSE_COMMAND_ID, ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_store_scene_response);
}

esp_matter_command_t *esp_matter_command_create_get_scene_membership_response(esp_matter_cluster_t *cluster)
{
    return esp_matter_command_create(cluster, ZCL_GET_SCENE_MEMBERSHIP_RESPONSE_COMMAND_ID,
                                     ESP_MATTER_COMMAND_FLAG_NONE,
                                     esp_matter_command_callback_get_scene_membership_response);
}

#endif /* FIXED_ENDPOINT_COUNT */
