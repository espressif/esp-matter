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

#include "integration.h"
#include "esp_err.h"
#include <esp_check.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_priv.h>

#include <app/clusters/scenes-server/SceneTable.h>
#include <app/clusters/scenes-server/SceneTableImpl.h>
#include <app/clusters/scenes-server/ScenesManagementCluster.h>
#include <app/server/Server.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <filesystem>
#include <unordered_map>
#include "credentials/GroupDataProvider.h"
#include <data_model/esp_matter_attribute_helpers.h>

using SceneTable = chip::scenes::SceneTable<chip::scenes::ExtensionFieldSetsImpl>;

namespace chip::app::Clusters::ScenesManagement {

namespace {

class DefaultScenesManagementTableProvider : public ScenesManagementTableProvider {
public:
    void SetParameters(EndpointId endpointId, uint16_t endpointTableSize)
    {
        mEndpointId = endpointId;
        mEndpointTableSize = endpointTableSize;
    }
    EndpointId GetEndpointId() const
    {
        return mEndpointId;
    }

    ScenesManagementSceneTable *Take() override
    {
        return scenes::GetSceneTableImpl(mEndpointId, mEndpointTableSize);
    }
    void Release(ScenesManagementSceneTable *) override {}

private:
    EndpointId mEndpointId = kInvalidEndpointId;
    uint16_t mEndpointTableSize = scenes::kMaxScenesPerEndpoint;
};

std::unordered_map<EndpointId, LazyRegisteredServerCluster<ScenesManagementCluster>> gServers;
std::unordered_map<EndpointId, DefaultScenesManagementTableProvider> gTableProviders;

} // namespace

ScenesManagementCluster *FindClusterOnEndpoint(EndpointId endpointId)
{
    if (gServers[endpointId].IsConstructed()) {
        return &gServers[endpointId].Cluster();
    }
    return nullptr;
}

ScenesServer &ScenesServer::Instance()
{
    static ScenesServer gInstance;
    return gInstance;
}

bool ScenesServer::IsHandlerRegistered(EndpointId aEndpointId, scenes::SceneHandler *handler)
{
    SceneTable *sceneTable = scenes::GetSceneTableImpl(aEndpointId);
    return sceneTable->mHandlerList.Contains(handler);
}

void ScenesServer::RegisterSceneHandler(EndpointId aEndpointId, scenes::SceneHandler *handler)
{
    SceneTable *sceneTable = scenes::GetSceneTableImpl(aEndpointId);

    if (!IsHandlerRegistered(aEndpointId, handler)) {
        sceneTable->RegisterHandler(handler);
        ChipLogProgress(AppServer, "SCENE_DEBUG: RegisterSceneHandler ep=%u handler=%p listEmpty=%d",
                        aEndpointId, handler, sceneTable->mHandlerList.Empty());
    } else {
        ChipLogProgress(AppServer, "SCENE_DEBUG: RegisterSceneHandler ep=%u handler=%p ALREADY registered", aEndpointId, handler);
    }
}

void ScenesServer::UnregisterSceneHandler(EndpointId aEndpointId, scenes::SceneHandler *handler)
{
    SceneTable *sceneTable = scenes::GetSceneTableImpl(aEndpointId);

    if (IsHandlerRegistered(aEndpointId, handler)) {
        sceneTable->UnregisterHandler(handler);
    }
}

void ScenesServer::GroupWillBeRemoved(FabricIndex aFabricIx, EndpointId aEndpointId, GroupId aGroupId)
{
    ScenesManagementCluster *cluster = FindClusterOnEndpoint(aEndpointId);
    VerifyOrReturn(cluster != nullptr);

    TEMPORARY_RETURN_IGNORED cluster->GroupWillBeRemoved(aFabricIx, aGroupId);
}

void ScenesServer::MakeSceneInvalid(EndpointId aEndpointId, FabricIndex aFabricIx)
{
    ScenesManagementCluster *cluster = FindClusterOnEndpoint(aEndpointId);
    VerifyOrReturn(cluster != nullptr);

    TEMPORARY_RETURN_IGNORED cluster->MakeSceneInvalid(aFabricIx);
}

void ScenesServer::MakeSceneInvalidForAllFabrics(EndpointId aEndpointId)
{
    ScenesManagementCluster *cluster = FindClusterOnEndpoint(aEndpointId);
    VerifyOrReturn(cluster != nullptr);
    TEMPORARY_RETURN_IGNORED cluster->MakeSceneInvalidForAllFabrics();
}

void ScenesServer::StoreCurrentScene(FabricIndex aFabricIx, EndpointId aEndpointId, GroupId aGroupId, SceneId aSceneId)
{
    ScenesManagementCluster *cluster = FindClusterOnEndpoint(aEndpointId);
    VerifyOrReturn(cluster != nullptr);
    TEMPORARY_RETURN_IGNORED cluster->StoreCurrentScene(aFabricIx, aGroupId, aSceneId);
}

void ScenesServer::RecallScene(FabricIndex aFabricIx, EndpointId aEndpointId, GroupId aGroupId, SceneId aSceneId)
{
    ScenesManagementCluster *cluster = FindClusterOnEndpoint(aEndpointId);
    VerifyOrReturn(cluster != nullptr);
    TEMPORARY_RETURN_IGNORED cluster->RecallScene(aFabricIx, aGroupId, aSceneId);
}

void ScenesServer::RemoveFabric(EndpointId aEndpointId, FabricIndex aFabricIndex)
{
    ScenesManagementCluster *cluster = FindClusterOnEndpoint(aEndpointId);
    VerifyOrReturn(cluster != nullptr);
    TEMPORARY_RETURN_IGNORED cluster->RemoveFabric(aFabricIndex);
}

} // namespace chip::app::Clusters::ScenesManagement

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ScenesManagement;
using namespace esp_matter;

namespace {

esp_err_t GetScenesClusterContextParams(EndpointId endpointId, BitMask<ScenesManagement::Feature> &featureMap,
                                        bool &supportsCopyScene, uint16_t &tableSize)
{
    esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpointId, ScenesManagement::Id);
    featureMap = BitMask<ScenesManagement::Feature>(read_feature_map_u32(cluster));
    esp_matter_attr_val_t attr_val;
    supportsCopyScene =
        endpoint::is_command_enabled(endpointId, ScenesManagement::Id, Commands::CopyScene::Id);
    ESP_RETURN_ON_ERROR(get_stored_attr_val(cluster, Attributes::SceneTableSize::Id, attr_val), "Scenes",
                        "Failed to get scenes table size");
    VerifyOrReturnValue(attr_val.type == ESP_MATTER_VAL_TYPE_UINT16, ESP_ERR_INVALID_ARG);
    tableSize = attr_val.val.u16;
    return ESP_OK;
}

} // namespace

void ESPMatterScenesManagementClusterServerInitCallback(EndpointId endpointId)
{
    if (!gServers[endpointId].IsConstructed()) {
        BitMask<ScenesManagement::Feature> featureMap;
        bool supportsCopyScene;
        uint16_t tableSize;
        VerifyOrReturn(GetScenesClusterContextParams(endpointId, featureMap, supportsCopyScene, tableSize) == ESP_OK,
                       ChipLogError(AppServer, "Failed to get cluster context parameters"));
        gTableProviders[endpointId].SetParameters(endpointId, tableSize);
        gServers[endpointId].Create(endpointId,
        ScenesManagementCluster::Context{
            .groupDataProvider = Credentials::GetGroupDataProvider(),
            .fabricTable = &Server::GetInstance().GetFabricTable(),
            .features = featureMap,
            .sceneTableProvider = gTableProviders[endpointId],
            .supportsCopyScene = supportsCopyScene,
        });
    }
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register Scenes on endpoint %u - Error: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
}

void ESPMatterScenesManagementClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    auto serverIt = gServers.find(endpointId);
    VerifyOrReturn(serverIt != gServers.end());
    VerifyOrReturn(serverIt->second.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&serverIt->second.Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister Scenes on endpoint %u - Error: %" CHIP_ERROR_FORMAT, endpointId,
                     err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        serverIt->second.Destroy();
        gServers.erase(serverIt);
        auto tableProviderIt = gTableProviders.find(endpointId);
        VerifyOrReturn(tableProviderIt != gTableProviders.end());
        gTableProviders.erase(tableProviderIt);
    }
}

void MatterScenesManagementPluginServerInitCallback() {}
