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

#include "esp_matter_attribute_utils.h"
#include "esp_matter_data_model.h"
#include "esp_matter_data_model_priv.h"
#include "esp_matter_data_model_provider.h"
#include <app/clusters/icd-management-server/ICDManagementCluster.h>
#include <string.h>
#include "app/server-cluster/ServerClusterInterfaceRegistry.h"
#include "app/server/Server.h"
#include "clusters/IcdManagement/Enums.h"
#include "core/DataModelTypes.h"
#include "lib/support/BitMask.h"
#include "lib/support/Span.h"
#include "support/CodeUtils.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::IcdManagement;

namespace {
#if CHIP_CONFIG_ENABLE_ICD_CIP
LazyRegisteredServerCluster<ICDManagementClusterWithCIP> gServer;
#else
LazyRegisteredServerCluster<ICDManagementCluster> gServer;
#endif

esp_err_t get_attr_val(esp_matter::cluster_t *cluster, uint32_t attribute_id, esp_matter_attr_val_t &val)
{
    esp_matter::attribute_t *attr = esp_matter::attribute::get(cluster, attribute_id);
    if (!attr) {
        return ESP_FAIL;
    }
    return esp_matter::attribute::get_val_internal(attr, &val);
}

CHIP_ERROR GetClusterConfig(EndpointId endpointId, ICDManagementCluster::OptionalAttributeSet &optionalAttrSet,
                            BitMask<OptionalCommands> &optionalCommands, BitMask<UserActiveModeTriggerBitmap> &uatHint,
                            MutableCharSpan &instructionSpan)
{
    esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpointId, IcdManagement::Id);
    if (!cluster) {
        return CHIP_ERROR_NOT_FOUND;
    }
    if (esp_matter::command::get(cluster, Commands::StayActiveRequest::Id, esp_matter::COMMAND_FLAG_ACCEPTED)) {
        optionalCommands.SetField(OptionalCommands::kStayActive, 1);
    } else {
        optionalCommands.SetField(OptionalCommands::kStayActive, 0);
    }
    esp_matter_attr_val_t attr_val;
    if (get_attr_val(cluster, Attributes::UserActiveModeTriggerHint::Id, attr_val) == ESP_OK &&
            attr_val.type == ESP_MATTER_VAL_TYPE_BITMAP32) {
        uatHint = BitMask<UserActiveModeTriggerBitmap>(attr_val.val.u32);
        if (get_attr_val(cluster, Attributes::UserActiveModeTriggerInstruction::Id, attr_val) == ESP_OK &&
                attr_val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING && instructionSpan.size() >= attr_val.val.a.s) {
            memcpy(instructionSpan.data(), (const char *)attr_val.val.a.b, attr_val.val.a.s);
            instructionSpan.reduce_size(attr_val.val.a.s);
            optionalAttrSet.Set<Attributes::UserActiveModeTriggerInstruction::Id>();
        }
    }
    return CHIP_NO_ERROR;
}

} // namespace

void ESPMatterIcdManagementClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    VerifyOrReturn(!gServer.IsConstructed());
    ICDManagementCluster::OptionalAttributeSet optionalAttrSet;
    BitMask<OptionalCommands> optionalCommands;
    BitMask<UserActiveModeTriggerBitmap> uatHint;
    char instructionBuffer[kUserActiveModeTriggerInstructionMaxLength];
    MutableCharSpan instructionSpan(instructionBuffer);
    CHIP_ERROR err = GetClusterConfig(endpointId, optionalAttrSet, optionalCommands, uatHint, instructionSpan);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to get config for IcdManagement - Error %" CHIP_ERROR_FORMAT, err.Format());
        return;
    }
    gServer.Create(endpointId, *Server::GetInstance().GetSessionKeystore(), Server::GetInstance().GetFabricTable(),
                   ICDConfigurationData::GetInstance(), optionalAttrSet, optionalCommands, uatHint, instructionSpan);
    err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register IcdManagement - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterIcdManagementClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    VerifyOrReturn(gServer.IsConstructed());
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister IcdManagement - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
    gServer.Destroy();
}
