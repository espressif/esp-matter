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
#include <app/clusters/mode-base-server/mode-base-cluster-objects.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;

#if (FIXED_ENDPOINT_COUNT == 0)

static const char *TAG = "esp_matter_command";

namespace esp_matter {
namespace command {

static callback_t get_cluster_accepted_command(uint32_t cluster_id, uint32_t command_id);

void DispatchSingleClusterCommandCommon(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    uint16_t endpoint_id = command_path.mEndpointId;
    uint32_t cluster_id = command_path.mClusterId;
    uint32_t command_id = command_path.mCommandId;
    ESP_LOGI(TAG, "Received command 0x%08" PRIX32 " for endpoint 0x%04" PRIX16 "'s cluster 0x%08" PRIX32 "", command_id, endpoint_id, cluster_id);

    cluster_t *cluster = cluster::get(endpoint_id, cluster_id);
    VerifyOrReturn(cluster);
    command_t *command = get(cluster, command_id, COMMAND_FLAG_ACCEPTED);
    callback_t standard_callback = get_cluster_accepted_command(cluster_id, command_id);
    VerifyOrReturn((command || standard_callback), ESP_LOGE(TAG, "Command 0x%08" PRIX32 " not found", command_id));
    esp_err_t err = ESP_OK;
    TLVReader tlv_reader;
    tlv_reader.Init(tlv_data);
    if (standard_callback) {
        standard_callback(command_path, tlv_data, opaque_ptr);
    }
    if (command) {
        callback_t callback = get_user_callback(command);
        if (callback) {
            err = callback(command_path, tlv_reader, opaque_ptr);
        }
        callback = get_callback(command);
        if ((err == ESP_OK) && callback) {
            err = callback(command_path, tlv_data, opaque_ptr);
        }
        int flags = get_flags(command);
        if (flags & COMMAND_FLAG_CUSTOM) {
            chip::app::CommandHandler *command_obj = (chip::app::CommandHandler *)opaque_ptr;
            if (!command_obj) {
                ESP_LOGE(TAG, "Command Object cannot be NULL");
                return;
            }
            command_obj->AddStatus(command_path, err == ESP_OK ? chip::Protocols::InteractionModel::Status::Success :
                                                                chip::Protocols::InteractionModel::Status::Failure);
        }
    }
}

} /* command */
} /* esp_matter */

namespace chip {
namespace app {

void DispatchSingleClusterCommand(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                  CommandHandler *command_obj)
{
    esp_matter::command::DispatchSingleClusterCommandCommon(command_path, tlv_data, command_obj);
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
    chip::app::Clusters::OtaSoftwareUpdateRequestor::Commands::AnnounceOTAProvider::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfOtaSoftwareUpdateRequestorClusterAnnounceOTAProviderCallback((CommandHandler *)opaque_ptr, command_path,
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

static esp_err_t esp_matter_command_callback_trigger_effect(const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Identify::Commands::TriggerEffect::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfIdentifyClusterTriggerEffectCallback((CommandHandler *)opaque_ptr, command_path, command_data);
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

static esp_err_t esp_matter_command_callback_register_client(const ConcreteCommandPath &command_path,
                                                             TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::IcdManagement::Commands::RegisterClient::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
#if CONFIG_ENABLE_ICD_SERVER
        emberAfIcdManagementClusterRegisterClientCallback((CommandHandler *)opaque_ptr, command_path, command_data);
#endif
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_unregister_client(const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::IcdManagement::Commands::UnregisterClient::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
#if CONFIG_ENABLE_ICD_SERVER
        emberAfIcdManagementClusterUnregisterClientCallback((CommandHandler *)opaque_ptr, command_path, command_data);
#endif
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stay_active_request(const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::IcdManagement::Commands::StayActiveRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
#if CONFIG_ENABLE_ICD_SERVER
        emberAfIcdManagementClusterStayActiveRequestCallback((CommandHandler *)opaque_ptr, command_path, command_data);
#endif
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

static esp_err_t esp_matter_command_callback_move_to_closest_frequency(const ConcreteCommandPath &command_path,
                                                                       TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::LevelControl::Commands::MoveToClosestFrequency::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfLevelControlClusterMoveToClosestFrequencyCallback((CommandHandler *)opaque_ptr, command_path, command_data);
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

static esp_err_t esp_matter_command_callback_move_to_color_temperature(const ConcreteCommandPath &command_path,
                                                                       TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveToColorTemperature::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToColorTemperatureCallback((CommandHandler *)opaque_ptr, command_path,
                                                                 command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_color_temperature(const ConcreteCommandPath &command_path,
                                                                    TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveColorTemperature::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveColorTemperatureCallback((CommandHandler *)opaque_ptr, command_path,
                                                               command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step_color_temperature(const ConcreteCommandPath &command_path,
                                                                    TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::StepColorTemperature::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStepColorTemperatureCallback((CommandHandler *)opaque_ptr, command_path,
                                                               command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enhanced_move_to_hue(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::EnhancedMoveToHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterEnhancedMoveToHueCallback((CommandHandler *)opaque_ptr, command_path,
                                                            command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enhanced_move_hue(const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::EnhancedMoveHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterEnhancedMoveHueCallback((CommandHandler *)opaque_ptr, command_path,
                                                          command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enhanced_step_hue(const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::EnhancedStepHue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterEnhancedStepHueCallback((CommandHandler *)opaque_ptr, command_path,
                                                          command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enhanced_move_to_hue_and_saturation(const ConcreteCommandPath &command_path,
                                                                                 TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::EnhancedMoveToHueAndSaturation::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterEnhancedMoveToHueAndSaturationCallback((CommandHandler *)opaque_ptr, command_path,
                                                                         command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_color_loop_set(const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::ColorLoopSet::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterColorLoopSetCallback((CommandHandler *)opaque_ptr, command_path,
                                                       command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_to_color(const ConcreteCommandPath &command_path,
                                                           TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveToColor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveToColorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_move_color(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::MoveColor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterMoveColorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_step_color(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::ColorControl::Commands::StepColor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfColorControlClusterStepColorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_self_test_request(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::SmokeCoAlarm::Commands::SelfTestRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfSmokeCoAlarmClusterSelfTestRequestCallback((CommandHandler *)opaque_ptr, command_path, command_data);
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

static esp_err_t esp_matter_command_callback_unlock_with_timeout(const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::UnlockWithTimeout::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterUnlockWithTimeoutCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_weekday_schedule(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::SetWeekDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterSetWeekDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_weekday_schedule(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::GetWeekDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterGetWeekDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_weekday_schedule(const ConcreteCommandPath &command_path,
                                                                    TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::ClearWeekDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterClearWeekDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_year_day_schedule(const ConcreteCommandPath &command_path,
                                                                   TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::SetYearDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterSetYearDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_year_day_schedule(const ConcreteCommandPath &command_path,
                                                                   TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::GetYearDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterGetYearDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_year_day_schedule(const ConcreteCommandPath &command_path,
                                                                     TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::ClearYearDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterClearYearDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_holiday_schedule(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::SetHolidaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterSetHolidayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_holiday_schedule(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::GetHolidaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterGetHolidayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_holiday_schedule(const ConcreteCommandPath &command_path,
                                                                    TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::ClearHolidaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterClearHolidayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_user(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::SetUser::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterSetUserCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_user(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                      void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::GetUser::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterGetUserCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_user(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                        void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::ClearUser::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterClearUserCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_credential(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                            void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::SetCredential::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterSetCredentialCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_credential_status(const ConcreteCommandPath &command_path,
                                                                   TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::GetCredentialStatus::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterGetCredentialStatusCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_credential(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                              void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::ClearCredential::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterClearCredentialCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_unbolt_door(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::UnboltDoor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterUnboltDoorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_aliro_reader_config(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::SetAliroReaderConfig::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterSetAliroReaderConfigCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_aliro_reader_config(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::ClearAliroReaderConfig::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterClearAliroReaderConfigCallback((CommandHandler *)opaque_ptr, command_path, command_data);
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

static esp_err_t esp_matter_command_callback_set_weekly_schedule(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::SetWeeklySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterSetWeeklyScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_weekly_schedule(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::GetWeeklySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterGetWeeklyScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_weekly_schedule(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::ClearWeeklySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterClearWeeklyScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_active_schedule_request(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::SetActiveScheduleRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterSetActiveScheduleRequestCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_active_preset_request(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::Thermostat::Commands::SetActivePresetRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThermostatClusterSetActivePresetRequestCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_thread_reset_counts(const ConcreteCommandPath &command_path,
                                                                 TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ThreadNetworkDiagnostics::Commands::ResetCounts::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfThreadNetworkDiagnosticsClusterResetCountsCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_wifi_reset_counts(const ConcreteCommandPath &command_path,
                                                               TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::WiFiNetworkDiagnostics::Commands::ResetCounts::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWiFiNetworkDiagnosticsClusterResetCountsCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_ethernet_reset_counts(const ConcreteCommandPath &command_path,
								   TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::EthernetNetworkDiagnostics::Commands::ResetCounts::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
	emberAfEthernetNetworkDiagnosticsClusterResetCountsCallback((CommandHandler *)opaque_ptr, command_path,
								    command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_retrieve_logs_request(const ConcreteCommandPath &command_path,
                                                                    TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::DiagnosticLogs::Commands::RetrieveLogsRequest::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDiagnosticLogsClusterRetrieveLogsRequestCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_test_event_trigger(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::GeneralDiagnostics::Commands::TestEventTrigger::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGeneralDiagnosticsClusterTestEventTriggerCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_time_snap_shot(const ConcreteCommandPath &command_path,
                                                                  TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::GeneralDiagnostics::Commands::TimeSnapshot::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfGeneralDiagnosticsClusterTimeSnapshotCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_up_or_open(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::UpOrOpen::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterUpOrOpenCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_down_or_close(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::DownOrClose::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterDownOrCloseCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop_motion(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::StopMotion::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterStopMotionCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_go_to_lift_value(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::GoToLiftValue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterGoToLiftValueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_go_to_lift_percentage(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::GoToLiftPercentage::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterGoToLiftPercentageCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_go_to_tilt_value(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::GoToTiltValue::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterGoToTiltValueCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_go_to_tilt_percentage(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::WindowCovering::Commands::GoToTiltPercentage::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfWindowCoveringClusterGoToTiltPercentageCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_change_to_mode(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ModeSelect::Commands::ChangeToMode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfModeSelectClusterChangeToModeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_temperature(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::TemperatureControl::Commands::SetTemperature::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfTemperatureControlClusterSetTemperatureCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_fan_step(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::FanControl::Commands::Step::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfFanControlClusterStepCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_instance_action(const ConcreteCommandPath &command_path,
                                                             TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_instance_action_with_transition(const ConcreteCommandPath &command_path,
                                                                             TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_start_action(const ConcreteCommandPath &command_path,
                                                          TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_start_action_with_duration(const ConcreteCommandPath &command_path,
                                                                        TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_stop_action(const ConcreteCommandPath &command_path,
                                                         TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_pause_action(const ConcreteCommandPath &command_path,
                                                          TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_pause_action_with_duration(const ConcreteCommandPath &command_path,
                                                                        TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_resume_action(const ConcreteCommandPath &command_path,
                                                           TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enable_action(const ConcreteCommandPath &command_path,
                                                           TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enable_action_with_duration(const ConcreteCommandPath &command_path,
                                                                         TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_disable_action(const ConcreteCommandPath &command_path,
                                                            TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_disable_action_with_duration(const ConcreteCommandPath &command_path,
                                                                          TLVReader &tlv_data, void *opaque_ptr)
{
    // No actions are implemented, just return status NotFound.
    // TODO: Add action callbacks for actions cluster.
    CommandHandler *command_handler = (CommandHandler *)opaque_ptr;
    command_handler->AddStatus(command_path, chip::Protocols::InteractionModel::Status::NotFound);

    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_review_fabric_restrictions(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::AccessControl::Commands::ReviewFabricRestrictions::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfAccessControlClusterReviewFabricRestrictionsCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_keep_active(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::BridgedDeviceBasicInformation::Commands::KeepActive::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfBridgedDeviceBasicInformationClusterKeepActiveCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_send_key(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::KeypadInput::Commands::SendKey::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfKeypadInputClusterSendKeyCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_suppress_alarm(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::BooleanStateConfiguration::Commands::SuppressAlarm::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfBooleanStateConfigurationClusterSuppressAlarmCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_enable_disable_alarm(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::BooleanStateConfiguration::Commands::EnableDisableAlarm::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfBooleanStateConfigurationClusterEnableDisableAlarmCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_open(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ValveConfigurationAndControl::Commands::Open::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfValveConfigurationAndControlClusterOpenCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_close(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::ValveConfigurationAndControl::Commands::Close::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfValveConfigurationAndControlClusterCloseCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_utc_time(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::TimeSynchronization::Commands::SetUTCTime::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfTimeSynchronizationClusterSetUTCTimeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_trusted_time_source(const ConcreteCommandPath &command_path,
                                                                     TLVReader &tlv_data, void *opaque_ptr)
{
    chip::app::Clusters::TimeSynchronization::Commands::SetTrustedTimeSource::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfTimeSynchronizationClusterSetTrustedTimeSourceCallback((CommandHandler *)opaque_ptr, command_path,
                                                                      command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_time_zone(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                           void *opaque_ptr)
{
    chip::app::Clusters::TimeSynchronization::Commands::SetTimeZone::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfTimeSynchronizationClusterSetTimeZoneCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_dst_offset(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                            void *opaque_ptr)
{
    chip::app::Clusters::TimeSynchronization::Commands::SetDSTOffset::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfTimeSynchronizationClusterSetDSTOffsetCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_default_ntp(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                             void *opaque_ptr)
{
    chip::app::Clusters::TimeSynchronization::Commands::SetDefaultNTP::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfTimeSynchronizationClusterSetDefaultNTPCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {

namespace actions {
namespace command {
command_t *create_instant_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::InstantAction::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_instance_action);
}

command_t *create_instant_action_with_transition(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::InstantActionWithTransition::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_instance_action_with_transition);
}

command_t *create_start_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::StartAction::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_start_action);
}

command_t *create_start_action_with_duration(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::StartActionWithDuration::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_start_action_with_duration);
}

command_t *create_stop_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::StopAction::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_stop_action);
}

command_t *create_pause_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::PauseAction::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_pause_action);
}

command_t *create_pause_action_with_duration(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::PauseActionWithDuration::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_pause_action_with_duration);
}

command_t *create_resume_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::ResumeAction::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_resume_action);
}

command_t *create_enable_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::EnableAction::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_enable_action);
}

command_t *create_enable_action_with_duration(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::EnableActionWithDuration::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_enable_action_with_duration);
}

command_t *create_disable_action(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::DisableAction::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_disable_action);
}

command_t *create_disable_action_with_duration(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Actions::Commands::DisableActionWithDuration::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_disable_action_with_duration);
}

} /* command */
} /* actions */

namespace access_control {
namespace command {
command_t *create_review_fabric_restrictions(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AccessControl::Commands::ReviewFabricRestrictions::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_review_fabric_restrictions);
}

command_t *create_review_fabric_restrictions_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AccessControl::Commands::ReviewFabricRestrictionsResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* access_control */

namespace bridged_device_basic_information {
namespace command {
command_t *create_keep_active(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, BridgedDeviceBasicInformation::Commands::KeepActive::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_keep_active);
}

} /* command */
} /* bridged_device_basic_information */

namespace thread_network_diagnostics {
namespace command {

command_t *create_reset_counts(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ThreadNetworkDiagnostics::Commands::ResetCounts::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_thread_reset_counts);
}

} /* command */
} /* thread_network_diagnostics */

namespace wifi_network_diagnotics {
namespace command {

command_t *create_reset_counts(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WiFiNetworkDiagnostics::Commands::ResetCounts::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_wifi_reset_counts);
}

} /* command */
} /* wifi_network_diagnotics */

namespace ethernet_network_diagnostics {
namespace command {

command_t *create_reset_counts(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EthernetNetworkDiagnostics::Commands::ResetCounts::Id, COMMAND_FLAG_ACCEPTED,
				       esp_matter_command_callback_ethernet_reset_counts);
}

} /* command */
} /* ethernet_network_diagnostics */

namespace diagnostic_logs {
namespace command {

command_t *create_retrieve_logs_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DiagnosticLogs::Commands::RetrieveLogsRequest::Id, COMMAND_FLAG_ACCEPTED,
                                        esp_matter_command_callback_retrieve_logs_request);
}

command_t *create_retrieve_logs_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DiagnosticLogs::Commands::RetrieveLogsResponse::Id,
                                        COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* diagnostic_logs */

namespace general_diagnostics {
namespace command {

constexpr const command_entry_t accepted_command_list[] = {
    {GeneralDiagnostics::Commands::TestEventTrigger::Id, COMMAND_FLAG_ACCEPTED,
     esp_matter_command_callback_test_event_trigger},
    {GeneralDiagnostics::Commands::TimeSnapshot::Id, COMMAND_FLAG_ACCEPTED,
     esp_matter_command_callback_time_snap_shot},
};

constexpr const command_entry_t generated_command_list[] = {
    {GeneralDiagnostics::Commands::TimeSnapshotResponse::Id, COMMAND_FLAG_GENERATED, NULL},
};

command_t *create_test_event_trigger(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralDiagnostics::Commands::TestEventTrigger::Id, COMMAND_FLAG_ACCEPTED,
                                        esp_matter_command_callback_test_event_trigger);
}

command_t *create_time_snap_shot(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralDiagnostics::Commands::TimeSnapshot::Id, COMMAND_FLAG_ACCEPTED,
                                        esp_matter_command_callback_time_snap_shot);
}

command_t *create_time_snap_shot_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralDiagnostics::Commands::TimeSnapshotResponse::Id, COMMAND_FLAG_GENERATED,
                                        NULL);
}

} /* command */
} /* general_diagnostics */

namespace software_diagnostics {
namespace command {

command_t *create_reset_watermarks(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SoftwareDiagnostics::Commands::ResetWatermarks::Id,
                                        COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */
} /* software_diagnostics */

namespace group_key_management {
namespace command {

constexpr const command_entry_t accepted_command_list[] = {
    {GroupKeyManagement::Commands::KeySetWrite::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_key_set_write},
    {GroupKeyManagement::Commands::KeySetRead::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_key_set_read},
    {GroupKeyManagement::Commands::KeySetRemove::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_key_set_remove},
    {GroupKeyManagement::Commands::KeySetReadAllIndices::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_key_set_read_all_indices},
};

constexpr const command_entry_t generated_command_list[] = {
    {GroupKeyManagement::Commands::KeySetReadResponse::Id, COMMAND_FLAG_GENERATED, NULL},
    {GroupKeyManagement::Commands::KeySetReadAllIndicesResponse::Id, COMMAND_FLAG_GENERATED, NULL},
};

command_t *create_key_set_write(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GroupKeyManagement::Commands::KeySetWrite::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_key_set_write);
}

command_t *create_key_set_read(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GroupKeyManagement::Commands::KeySetRead::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_key_set_read);
}

command_t *create_key_set_remove(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GroupKeyManagement::Commands::KeySetRemove::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_key_set_remove);
}

command_t *create_key_set_read_all_indices(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GroupKeyManagement::Commands::KeySetReadAllIndices::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_key_set_read_all_indices);
}

command_t *create_key_set_read_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GroupKeyManagement::Commands::KeySetReadResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_key_set_read_all_indices_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GroupKeyManagement::Commands::KeySetReadAllIndicesResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* group_key_management */

namespace general_commissioning {
namespace command {

constexpr const command_entry_t accepted_command_list[] = {
    {GeneralCommissioning::Commands::ArmFailSafe::Id, COMMAND_FLAG_ACCEPTED, nullptr},
    {GeneralCommissioning::Commands::SetRegulatoryConfig::Id, COMMAND_FLAG_ACCEPTED, nullptr},
    {GeneralCommissioning::Commands::CommissioningComplete::Id, COMMAND_FLAG_ACCEPTED, nullptr},
};

constexpr const command_entry_t generated_command_list[] = {
    {GeneralCommissioning::Commands::ArmFailSafeResponse::Id, COMMAND_FLAG_GENERATED, NULL},
    {GeneralCommissioning::Commands::SetRegulatoryConfigResponse::Id, COMMAND_FLAG_GENERATED, NULL},
    {GeneralCommissioning::Commands::CommissioningCompleteResponse::Id, COMMAND_FLAG_GENERATED, NULL},
};

command_t *create_arm_fail_safe(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralCommissioning::Commands::ArmFailSafe::Id, COMMAND_FLAG_ACCEPTED,
                                       nullptr);
}

command_t *create_set_regulatory_config(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralCommissioning::Commands::SetRegulatoryConfig::Id,
                                       COMMAND_FLAG_ACCEPTED, nullptr);
}

command_t *create_commissioning_complete(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralCommissioning::Commands::CommissioningComplete::Id,
                                       COMMAND_FLAG_ACCEPTED, nullptr);
}

command_t *create_arm_fail_safe_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralCommissioning::Commands::ArmFailSafeResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_set_regulatory_config_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralCommissioning::Commands::SetRegulatoryConfigResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_commissioning_complete_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralCommissioning::Commands::CommissioningCompleteResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_set_tc_acknowledgements(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralCommissioning::Commands::SetTCAcknowledgements::Id,
                                       COMMAND_FLAG_ACCEPTED, nullptr);
}

command_t *create_set_tc_acknowledgements_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GeneralCommissioning::Commands::SetTCAcknowledgementsResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* general_commissioning */

namespace network_commissioning {
namespace command {

constexpr const command_entry_t accepted_command_list[] = {
    {NetworkCommissioning::Commands::ScanNetworks::Id, COMMAND_FLAG_ACCEPTED, NULL},
    {NetworkCommissioning::Commands::RemoveNetwork::Id, COMMAND_FLAG_ACCEPTED, NULL},
    {NetworkCommissioning::Commands::ConnectNetwork::Id, COMMAND_FLAG_ACCEPTED, NULL},
    {NetworkCommissioning::Commands::ReorderNetwork::Id, COMMAND_FLAG_ACCEPTED, NULL},
};

constexpr const command_entry_t generated_command_list[] = {
    {NetworkCommissioning::Commands::ScanNetworksResponse::Id, COMMAND_FLAG_GENERATED, NULL},
    {NetworkCommissioning::Commands::NetworkConfigResponse::Id, COMMAND_FLAG_GENERATED, NULL},
    {NetworkCommissioning::Commands::ConnectNetworkResponse::Id, COMMAND_FLAG_GENERATED, NULL},
};

command_t *create_scan_networks(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NetworkCommissioning::Commands::ScanNetworks::Id, COMMAND_FLAG_ACCEPTED,
                                       NULL);
}

command_t *create_add_or_update_wifi_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NetworkCommissioning::Commands::AddOrUpdateWiFiNetwork::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_or_update_thread_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NetworkCommissioning::Commands::AddOrUpdateThreadNetwork::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NetworkCommissioning::Commands::RemoveNetwork::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_connect_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NetworkCommissioning::Commands::ConnectNetwork::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_reorder_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NetworkCommissioning::Commands::ReorderNetwork::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_scan_networks_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NetworkCommissioning::Commands::ScanNetworksResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_network_config_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NetworkCommissioning::Commands::NetworkConfigResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_connect_network_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, NetworkCommissioning::Commands::ConnectNetworkResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* network_commissioning */

namespace administrator_commissioning {
namespace command {

constexpr const command_entry_t accepted_command_list[] = {
    {AdministratorCommissioning::Commands::OpenCommissioningWindow::Id, COMMAND_FLAG_ACCEPTED,
     esp_matter_command_callback_open_commissioning_window},
    {AdministratorCommissioning::Commands::RevokeCommissioning::Id, COMMAND_FLAG_ACCEPTED,
     esp_matter_command_callback_revoke_commissioning},
};

constexpr const command_entry_t generated_command_list[] = {};

command_t *create_open_commissioning_window(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AdministratorCommissioning::Commands::OpenCommissioningWindow::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_open_commissioning_window);
}

command_t *create_open_basic_commissioning_window(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AdministratorCommissioning::Commands::OpenBasicCommissioningWindow::Id,
                                       COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_open_basic_commissioning_window);
}

command_t *create_revoke_commissioning(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AdministratorCommissioning::Commands::RevokeCommissioning::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_revoke_commissioning);
}

} /* command */
} /* administrator_commissioning */

namespace operational_credentials {
namespace command {

constexpr const command_entry_t accepted_command_list[] = {
    {OperationalCredentials::Commands::AttestationRequest::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_attestation_request},
    {OperationalCredentials::Commands::CertificateChainRequest::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_certificate_chain_request},
    {OperationalCredentials::Commands::CSRRequest::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_csr_request},
    {OperationalCredentials::Commands::AddNOC::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_noc},
    {OperationalCredentials::Commands::UpdateNOC::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_update_noc},
    {OperationalCredentials::Commands::UpdateFabricLabel::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_update_fabric_label},
    {OperationalCredentials::Commands::RemoveFabric::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_fabric},
    {OperationalCredentials::Commands::AddTrustedRootCertificate::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_trusted_root_certificate},
};

constexpr const command_entry_t generated_command_list[] = {
    {OperationalCredentials::Commands::AttestationResponse::Id, COMMAND_FLAG_GENERATED, NULL},
    {OperationalCredentials::Commands::CertificateChainResponse::Id, COMMAND_FLAG_GENERATED, NULL},
    {OperationalCredentials::Commands::CSRResponse::Id, COMMAND_FLAG_GENERATED, NULL},
    {OperationalCredentials::Commands::NOCResponse::Id, COMMAND_FLAG_GENERATED, NULL},
};

command_t *create_attestation_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::AttestationRequest::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_attestation_request);
}

command_t *create_certificate_chain_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::CertificateChainRequest::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_certificate_chain_request);
}

command_t *create_csr_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::CSRRequest::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_csr_request);
}

command_t *create_add_noc(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::AddNOC::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_add_noc);
}

command_t *create_update_noc(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::UpdateNOC::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_update_noc);
}

command_t *create_update_fabric_label(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::UpdateFabricLabel::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_update_fabric_label);
}

command_t *create_remove_fabric(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::RemoveFabric::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_fabric);
}

command_t *create_add_trusted_root_certificate(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::AddTrustedRootCertificate::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_trusted_root_certificate);
}

command_t *create_attestation_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::AttestationResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_certificate_chain_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::CertificateChainResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_csr_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::CSRResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_noc_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalCredentials::Commands::NOCResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* operational_credentials */

namespace ota_provider {
namespace command {

command_t *create_query_image(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OtaSoftwareUpdateProvider::Commands::QueryImage::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_query_image);
}

command_t *create_apply_update_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OtaSoftwareUpdateProvider::Commands::ApplyUpdateRequest::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_apply_update_request);
}

command_t *create_notify_update_applied(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OtaSoftwareUpdateProvider::Commands::NotifyUpdateApplied::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_notify_update_applied);
}

command_t *create_query_image_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OtaSoftwareUpdateProvider::Commands::QueryImageResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_apply_update_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OtaSoftwareUpdateProvider::Commands::ApplyUpdateResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* ota_provider */

namespace ota_requestor {
namespace command {

command_t *create_announce_ota_provider(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OtaSoftwareUpdateRequestor::Commands::AnnounceOTAProvider::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_announce_ota_provider);
}

} /* command */
} /* ota_requestor */

namespace identify {
namespace command {

command_t *create_identify(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Identify::Commands::Identify::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_identify);
}

command_t *create_trigger_effect(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Identify::Commands::TriggerEffect::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_trigger_effect);
}

} /* command */
} /* identify */

namespace groups {
namespace command {

command_t *create_add_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Groups::Commands::AddGroup::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_add_group);
}

command_t *create_view_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Groups::Commands::ViewGroup::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_view_group);
}

command_t *create_get_group_membership(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Groups::Commands::GetGroupMembership::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_get_group_membership);
}

command_t *create_remove_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Groups::Commands::RemoveGroup::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_remove_group);
}

command_t *create_remove_all_groups(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Groups::Commands::RemoveAllGroups::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_remove_all_groups);
}

command_t *create_add_group_if_identifying(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Groups::Commands::AddGroupIfIdentifying::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_add_group_if_identifying);
}

command_t *create_add_group_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Groups::Commands::AddGroupResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_view_group_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Groups::Commands::ViewGroupResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_get_group_membership_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Groups::Commands::GetGroupMembershipResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_remove_group_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Groups::Commands::RemoveGroupResponse::Id, COMMAND_FLAG_GENERATED,
                                       NULL);
}

} /* command */
} /* groups */

namespace icd_management {
namespace command {
command_t *create_register_client(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, IcdManagement::Commands::RegisterClient::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_register_client);
}

command_t *create_register_client_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, IcdManagement::Commands::RegisterClientResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_unregister_client(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, IcdManagement::Commands::UnregisterClient::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_unregister_client);
}

command_t *create_stay_active_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, IcdManagement::Commands::StayActiveRequest::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_stay_active_request);
}

} /* command */
} /* icd_management */

namespace scenes_management {
namespace command {

command_t *create_add_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::AddScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_view_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::ViewScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::RemoveScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_all_scenes(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::RemoveAllScenes::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_store_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::StoreScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_recall_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::RecallScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_scene_membership(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::GetSceneMembership::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_copy_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::CopyScene::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::AddSceneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_view_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::ViewSceneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_remove_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::RemoveSceneResponse::Id, COMMAND_FLAG_GENERATED,
                                       NULL);
}

command_t *create_remove_all_scenes_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::RemoveAllScenesResponse::Id, COMMAND_FLAG_GENERATED,
                                       NULL);
}

command_t *create_store_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::StoreSceneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_get_scene_membership_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::GetSceneMembershipResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_copy_scene_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ScenesManagement::Commands::CopySceneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* scenes_management */

namespace on_off {
namespace command {

command_t *create_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OnOff::Commands::Off::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_off);
}

command_t *create_on(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OnOff::Commands::On::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_on);
}

command_t *create_toggle(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OnOff::Commands::Toggle::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_toggle);
}

command_t *create_off_with_effect(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OnOff::Commands::OffWithEffect::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_off_with_effect);
}

command_t *create_on_with_recall_global_scene(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OnOff::Commands::OnWithRecallGlobalScene::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_on_with_recall_global_scene);
}

command_t *create_on_with_timed_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OnOff::Commands::OnWithTimedOff::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_on_with_timed_off);
}

} /* command */
} /* on_off */

namespace level_control {
namespace command {

command_t *create_move_to_level(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LevelControl::Commands::MoveToLevel::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move_to_level);
}

command_t *create_move(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LevelControl::Commands::Move::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move);
}

command_t *create_step(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LevelControl::Commands::Step::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_step);
}

command_t *create_stop(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LevelControl::Commands::Stop::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_stop);
}

command_t *create_move_to_level_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LevelControl::Commands::MoveToLevelWithOnOff::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move_to_level_with_on_off);
}

command_t *create_move_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LevelControl::Commands::MoveWithOnOff::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move_with_on_off);
}

command_t *create_step_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LevelControl::Commands::StepWithOnOff::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_step_with_on_off);
}

command_t *create_stop_with_on_off(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LevelControl::Commands::StopWithOnOff::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_stop_with_on_off);
}

command_t *create_move_to_closest_frequency(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LevelControl::Commands::MoveToClosestFrequency::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move_to_closest_frequency);
}

} /* command */
} /* level_control */

namespace color_control {
namespace command {

command_t *create_move_to_hue(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::MoveToHue::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move_to_hue);
}

command_t *create_move_hue(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::MoveHue::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move_hue);
}

command_t *create_step_hue(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::StepHue::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_step_hue);
}

command_t *create_move_to_saturation(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::MoveToSaturation::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move_to_saturation);
}

command_t *create_move_saturation(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::MoveSaturation::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move_saturation);
}

command_t *create_step_saturation(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::StepSaturation::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_step_saturation);
}

command_t *create_move_to_hue_and_saturation(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::MoveToHueAndSaturation::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_to_hue_and_saturation);
}

command_t *create_stop_move_step(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::StopMoveStep::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_stop_move_step);
}

command_t *create_move_to_color_temperature(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::MoveToColorTemperature::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_to_color_temperature);
}

command_t *create_move_color_temperature(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::MoveColorTemperature::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_move_color_temperature);
}

command_t *create_step_color_temperature(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::StepColorTemperature::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_step_color_temperature);
}

command_t *create_move_to_color(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::MoveToColor::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move_to_color);
}

command_t *create_move_color(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::MoveColor::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_move_color);
}

command_t *create_step_color(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::StepColor::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_step_color);
}

command_t *create_enhanced_move_to_hue(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::EnhancedMoveToHue::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_enhanced_move_to_hue);
}

command_t *create_enhanced_move_hue(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::EnhancedMoveHue::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_enhanced_move_hue);
}

command_t *create_enhanced_step_hue(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::EnhancedStepHue::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_enhanced_step_hue);
}

command_t *create_enhanced_move_to_hue_and_saturation(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::EnhancedMoveToHueAndSaturation::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_enhanced_move_to_hue_and_saturation);
}

command_t *create_color_loop_set(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ColorControl::Commands::ColorLoopSet::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_color_loop_set);
}

} /* command */
} /* color_control */

namespace thermostat {
namespace command {

command_t *create_setpoint_raise_lower(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Thermostat::Commands::SetpointRaiseLower::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_setpoint_raise_lower);
}

command_t *create_set_weekly_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Thermostat::Commands::SetWeeklySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_weekly_schedule);
}

command_t *create_get_weekly_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Thermostat::Commands::GetWeeklySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_get_weekly_schedule);
}

command_t *create_clear_weekly_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Thermostat::Commands::ClearWeeklySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_clear_weekly_schedule);
}

command_t *create_get_weekly_schedule_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Thermostat::Commands::GetWeeklyScheduleResponse::Id, COMMAND_FLAG_ACCEPTED,
                                       NULL);
}

command_t *create_set_active_schedule_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Thermostat::Commands::SetActiveScheduleRequest::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_active_schedule_request);
}

command_t *create_set_active_preset_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Thermostat::Commands::SetActivePresetRequest::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_active_preset_request);
}

} /* command */
} /* thermostat */

namespace operational_state {
namespace command {
command_t *create_pause(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalState::Commands::Pause::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_stop(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalState::Commands::Stop::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_start(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalState::Commands::Start::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_resume(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalState::Commands::Resume::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_operational_command_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, OperationalState::Commands::OperationalCommandResponse::Id,
				       COMMAND_FLAG_GENERATED, NULL);
}
} /* command */
} /* operational_state */

namespace smoke_co_alarm {
namespace command {

command_t *create_self_test_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SmokeCoAlarm::Commands::SelfTestRequest::Id, COMMAND_FLAG_ACCEPTED,
				       esp_matter_command_callback_self_test_request);
}

} /* command */
} /* smoke_co_alarm */

namespace door_lock {
namespace command {

command_t *create_lock_door(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::LockDoor::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_lock_door);
}

command_t *create_unlock_door(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::UnlockDoor::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_unlock_door);
}

command_t *create_unlock_with_timeout(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::UnlockWithTimeout::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_unlock_with_timeout);
}

command_t *create_set_weekday_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::SetWeekDaySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_weekday_schedule);
}

command_t *create_get_weekday_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::GetWeekDaySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_get_weekday_schedule);
}

command_t *create_get_weekday_schedule_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::GetWeekDayScheduleResponse::Id, COMMAND_FLAG_GENERATED,
                                       NULL);
}

command_t *create_clear_weekday_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::ClearWeekDaySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_clear_weekday_schedule);
}

command_t *create_set_year_day_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::SetYearDaySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_year_day_schedule);
}

command_t *create_get_year_day_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::GetYearDaySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_get_year_day_schedule);
}

command_t *create_get_year_day_schedule_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::GetYearDayScheduleResponse::Id, COMMAND_FLAG_GENERATED,
                                       NULL);
}

command_t *create_clear_year_day_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::ClearYearDaySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_clear_year_day_schedule);
}

command_t *create_set_holiday_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::SetHolidaySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_holiday_schedule);
}

command_t *create_get_holiday_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::GetHolidaySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_get_holiday_schedule);
}

command_t *create_get_holiday_schedule_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::GetHolidayScheduleResponse::Id, COMMAND_FLAG_GENERATED,
                                       NULL);
}

command_t *create_clear_holiday_schedule(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::ClearHolidaySchedule::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_clear_holiday_schedule);
}

command_t *create_set_user(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::SetUser::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_user);
}

command_t *create_get_user(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::GetUser::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_get_user);
}

command_t *create_get_user_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::GetUserResponse::Id, COMMAND_FLAG_GENERATED,
                                       NULL);
}

command_t *create_clear_user(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::ClearUser::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_clear_user);
}

command_t *create_set_credential(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::SetCredential::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_credential);
}

command_t *create_set_credential_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::SetCredentialResponse::Id, COMMAND_FLAG_GENERATED,
                                       NULL);
}

command_t *create_get_credential_status(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::GetCredentialStatus::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_get_credential_status);
}

command_t *create_get_credential_status_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::GetCredentialStatusResponse::Id, COMMAND_FLAG_GENERATED,
                                       NULL);
}

command_t *create_clear_credential(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::ClearCredential::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_clear_credential);
}

command_t *create_unbolt_door(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::UnboltDoor::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_unbolt_door);
}

command_t *create_set_aliro_reader_config(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::SetAliroReaderConfig::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_aliro_reader_config);
}

command_t *create_clear_aliro_reader_config(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DoorLock::Commands::ClearAliroReaderConfig::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_clear_aliro_reader_config);
}

} /* command */
} /* door_lock */

namespace window_covering {
namespace command {

command_t *create_up_or_open(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WindowCovering::Commands::UpOrOpen::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_up_or_open);
}

command_t *create_down_or_close(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WindowCovering::Commands::DownOrClose::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_down_or_close);
}

command_t *create_stop_motion(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WindowCovering::Commands::StopMotion::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_stop_motion);
}

command_t *create_go_to_lift_value(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WindowCovering::Commands::GoToLiftValue::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_go_to_lift_value);
}

command_t *create_go_to_lift_percentage(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WindowCovering::Commands::GoToLiftPercentage::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_go_to_lift_percentage);
}

command_t *create_go_to_tilt_value(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WindowCovering::Commands::GoToTiltValue::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_go_to_tilt_value);
}

command_t *create_go_to_tilt_percentage(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WindowCovering::Commands::GoToTiltPercentage::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_go_to_tilt_percentage);
}

} /* command */
} /* window_covering */

namespace mode_select {
namespace command {

command_t *create_change_to_mode(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ModeSelect::Commands::ChangeToMode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_change_to_mode);
}

} /* command */
} /* mode_select */

namespace temperature_control {
namespace command {

command_t *create_set_temperature(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TemperatureControl::Commands::SetTemperature::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_temperature);
}

} /* command */
} /* temperature_control */

namespace fan_control {
namespace command {
command_t *create_step(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, FanControl::Commands::Step::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_fan_step);
}

} /* command */
} /* fan_control */

namespace hepa_filter_monitoring {
namespace command {
command_t *create_reset_condition(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, HepaFilterMonitoring::Commands::ResetCondition::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */
} /* hepa_filter_monitoring */

namespace activated_carbon_filter_monitoring {
namespace command {
command_t *create_reset_condition(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ActivatedCarbonFilterMonitoring::Commands::ResetCondition::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */
} /* activated_carbon_filter_monitoring */

namespace mode_base {
namespace command {

// command response is null because of InvokeCommandHandler is overriden in srs/app/clusters/mode-base
command_t *create_change_to_mode(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ModeBase::Commands::ChangeToMode::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_change_to_mode_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ModeBase::Commands::ChangeToModeResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* mode_base */

namespace keypad_input {
namespace command {

command_t *create_send_key(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, KeypadInput::Commands::SendKey::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_send_key);
}

command_t *create_send_key_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, KeypadInput::Commands::SendKeyResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* keypad_input */

namespace boolean_state_configuration {
namespace command {
command_t *create_suppress_alarm(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, BooleanStateConfiguration::Commands::SuppressAlarm::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_suppress_alarm);
}

command_t *create_enable_disable_alarm(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, BooleanStateConfiguration::Commands::EnableDisableAlarm::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_enable_disable_alarm);
}

} /* command */
} /* boolean_state_configuration */

namespace energy_evse {
namespace command {

command_t *create_disable(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnergyEvse::Commands::Disable::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_enable_charging(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnergyEvse::Commands::EnableCharging::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_enable_discharging(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnergyEvse::Commands::EnableDischarging::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_start_diagnostics(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnergyEvse::Commands::StartDiagnostics::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_targets(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnergyEvse::Commands::SetTargets::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_targets(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnergyEvse::Commands::GetTargets::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_clear_targets(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnergyEvse::Commands::ClearTargets::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_targets_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnergyEvse::Commands::GetTargetsResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* energy_evse */

namespace microwave_oven_control {
namespace command {

command_t *create_set_cooking_parameters(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, MicrowaveOvenControl::Commands::SetCookingParameters::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_add_more_time(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, MicrowaveOvenControl::Commands::AddMoreTime::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */
} /* microwave_oven_control */

namespace valve_configuration_and_control {
namespace command {

command_t *create_open(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ValveConfigurationAndControl::Commands::Open::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_open);
}

command_t *create_close(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ValveConfigurationAndControl::Commands::Close::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_close);
}

} /* command */
} /* valve_configuration_and_control */

namespace device_energy_management {
namespace command {
command_t *create_power_adjust_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DeviceEnergyManagement::Commands::PowerAdjustRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_cancel_power_adjust_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DeviceEnergyManagement::Commands::CancelPowerAdjustRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_start_time_adjust_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DeviceEnergyManagement::Commands::StartTimeAdjustRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_pause_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DeviceEnergyManagement::Commands::PauseRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_resume_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DeviceEnergyManagement::Commands::ResumeRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_modify_forecast_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DeviceEnergyManagement::Commands::ModifyForecastRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_request_constraint_based_forecast(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DeviceEnergyManagement::Commands::RequestConstraintBasedForecast::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_cancel_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, DeviceEnergyManagement::Commands::CancelRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */
} /* device_energy_management */

namespace thread_border_router_management {
namespace command {

command_t *create_get_active_dataset_request(cluster_t *cluster)
{
     return esp_matter::command::create(cluster, ThreadBorderRouterManagement::Commands::GetActiveDatasetRequest::Id,
                                        COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_pending_dataset_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ThreadBorderRouterManagement::Commands::GetPendingDatasetRequest::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_dataset_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ThreadBorderRouterManagement::Commands::DatasetResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_set_active_dataset_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ThreadBorderRouterManagement::Commands::SetActiveDatasetRequest::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_pending_dataset_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ThreadBorderRouterManagement::Commands::SetPendingDatasetRequest::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */
} /* thread_border_router_management */

namespace wifi_network_management {
namespace command {

command_t *create_network_passphrase_request(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WiFiNetworkManagement::Commands::NetworkPassphraseRequest::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_network_passphrase_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WiFiNetworkManagement::Commands::NetworkPassphraseResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* wifi_network_management */

namespace thread_network_directory {
namespace command {

command_t *create_add_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ThreadNetworkDirectory::Commands::AddNetwork::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_remove_network(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ThreadNetworkDirectory::Commands::RemoveNetwork::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_operational_dataset(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ThreadNetworkDirectory::Commands::GetOperationalDataset::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_operational_dataset_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ThreadNetworkDirectory::Commands::OperationalDatasetResponse::Id,
                                       COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* thread_network_directory */

namespace service_area {
namespace command {

command_t *create_select_areas(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ServiceArea::Commands::SelectAreas::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_select_areas_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ServiceArea::Commands::SelectAreasResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_skip_area(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ServiceArea::Commands::SkipAreaResponse::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_skip_area_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ServiceArea::Commands::SkipAreaResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */
} /* service_area */

namespace water_heater_management {
namespace command {

command_t *create_boost(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WaterHeaterManagement::Commands::Boost::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_cancel_boost(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, WaterHeaterManagement::Commands::CancelBoost::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */
} /* water_heater_management */

namespace commissioner_control {
namespace command {

command_t *create_request_commissioning_approval(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CommissionerControl::Commands::RequestCommissioningApproval::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_commission_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CommissionerControl::Commands::CommissionNode::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_reverse_open_commissioning_window(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, CommissionerControl::Commands::ReverseOpenCommissioningWindow::Id,
                                       COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */
} /* commissioner_control */

namespace time_synchronization {
namespace command {

command_t *create_set_utc_time(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TimeSynchronization::Commands::SetUTCTime::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_utc_time);
}

command_t *create_set_trusted_time_source(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TimeSynchronization::Commands::SetTrustedTimeSource::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_trusted_time_source);
}

command_t *create_set_time_zone(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TimeSynchronization::Commands::SetTimeZone::Id, COMMAND_FLAG_ACCEPTED,
                                       esp_matter_command_callback_set_time_zone);
}

command_t *create_set_time_zone_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TimeSynchronization::Commands::SetTimeZoneResponse::Id,
                                       COMMAND_FLAG_GENERATED, nullptr);
}

command_t *create_set_dst_offset(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TimeSynchronization::Commands::SetDSTOffset::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_dst_offset);
}

command_t *create_set_default_ntp(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TimeSynchronization::Commands::SetDefaultNTP::Id,
                                       COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_default_ntp);
}

} /* command */
} /* time_synchronization */

} /* cluster */
} /* esp_matter */

namespace esp_matter {
namespace command {
#define GET_ACCEPTED_COMMAND_COUNT(cluster) sizeof(cluster::command::accepted_command_list) / sizeof(command_entry_t)
#define GET_ACCEPTED_COMMAND_LIST(cluster) \
    static_cast<const command_entry_t *>(&cluster::command::accepted_command_list[0])

#define GET_GENERATED_COMMAND_COUNT(cluster) sizeof(cluster::command::generated_command_list) / sizeof(command_entry_t)
#define GET_GENERATED_COMMAND_LIST(cluster) \
    static_cast<const command_entry_t *>(&cluster::command::generated_command_list[0])

#define GET_COMMAND_COUNT_LIST(cluster)                                                                            \
    GET_ACCEPTED_COMMAND_COUNT(cluster), GET_GENERATED_COMMAND_COUNT(cluster), GET_ACCEPTED_COMMAND_LIST(cluster), \
        GET_GENERATED_COMMAND_LIST(cluster)

/* Instead of adding the standard commands dynamically, place them in the .rodata section to save RAM. */
constexpr const cluster_command_t cluster_command_table[] = {
    {GeneralCommissioning::Id, GET_COMMAND_COUNT_LIST(cluster::general_commissioning)},
    {NetworkCommissioning::Id, GET_COMMAND_COUNT_LIST(cluster::network_commissioning)},
    {GeneralDiagnostics::Id, GET_COMMAND_COUNT_LIST(cluster::general_diagnostics)},
    {AdministratorCommissioning::Id, GET_COMMAND_COUNT_LIST(cluster::administrator_commissioning)},
    {OperationalCredentials::Id, GET_COMMAND_COUNT_LIST(cluster::operational_credentials)},
    {GroupKeyManagement::Id, GET_COMMAND_COUNT_LIST(cluster::group_key_management)},
};

#if defined(CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER) && defined(CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
const command_entry_t *get_cluster_accepted_command_list(uint32_t cluster_id)
{
    for (auto const &cluster_command : cluster_command_table) {
        if (cluster_command.cluster_id == cluster_id) {
            return cluster_command.accepted_command_list;
        }
    }
    return nullptr;
}

size_t get_cluster_accepted_command_count(uint32_t cluster_id)
{
    for (auto const &cluster_command : cluster_command_table) {
        if (cluster_command.cluster_id == cluster_id) {
            return cluster_command.accepted_command_count;
        }
    }
    return 0;
}

const command_entry_t *get_cluster_generated_command_list(uint32_t cluster_id)
{
    for (auto const &cluster_command : cluster_command_table) {
        if (cluster_command.cluster_id == cluster_id) {
            return cluster_command.generated_command_list;
        }
    }
    return nullptr;
}

size_t get_cluster_generated_command_count(uint32_t cluster_id)
{
    for (auto const &cluster_command : cluster_command_table) {
        if (cluster_command.cluster_id == cluster_id) {
            return cluster_command.generated_command_count;
        }
    }
    return 0;
}
#endif // defined(CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER) && defined(CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)

static callback_t get_cluster_accepted_command(uint32_t cluster_id, uint32_t command_id)
{
#if defined(CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER) && defined(CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    for (auto const &cluster_command : cluster_command_table) {
        if (cluster_command.cluster_id == cluster_id) {
            for (size_t index = 0; index < cluster_command.accepted_command_count; ++index){
                if (cluster_command.accepted_command_list[index].command_id == command_id) {
                    return cluster_command.accepted_command_list[index].callback;
                }
            }
        }
    }
#endif
    return nullptr;
}

} /* command */
} /* esp_matter */

#endif /* FIXED_ENDPOINT_COUNT */
