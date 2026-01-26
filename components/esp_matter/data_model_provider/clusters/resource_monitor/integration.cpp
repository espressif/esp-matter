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
#include <app/clusters/resource-monitoring-server/ResourceMonitoringCluster.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_check.h>
#include <esp_matter_data_model.h>
#include <esp_matter_data_model_priv.h>
#include <unordered_map>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ResourceMonitoring;

namespace {
std::unordered_map<uint64_t, LazyRegisteredServerCluster<ResourceMonitoringCluster>> gServers;

esp_err_t get_attr_val(esp_matter::cluster_t *cluster, uint32_t attribute_id, esp_matter_attr_val_t &val)
{
    esp_matter::attribute_t *attr = esp_matter::attribute::get(cluster, attribute_id);
    if (!attr) {
        return ESP_FAIL;
    }
    return esp_matter::attribute::get_val_internal(attr, &val);
}

esp_err_t GetClusterConfig(EndpointId endpointId, ClusterId clusterId,
                           BitFlags<ResourceMonitoring::Feature> &enabledFeatures,
                           ResourceMonitoringCluster::OptionalAttributeSet &optionalAttributeSet,
                           Attributes::DegradationDirection::TypeInfo::Type &aDegradationDirection,
                           bool &aResetConditionCommandSupported)
{
    esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpointId, clusterId);
    if (!cluster) {
        return ESP_FAIL;
    }
    esp_matter_attr_val_t feature_val;
    ESP_RETURN_ON_ERROR(get_attr_val(cluster, Globals::Attributes::FeatureMap::Id, feature_val), "TimeSync",
                        "Failed to get feature map");
    VerifyOrReturnError(feature_val.type == ESP_MATTER_VAL_TYPE_BITMAP32, ESP_FAIL);
    enabledFeatures = BitFlags<ResourceMonitoring::Feature>(feature_val.val.u32);
    esp_matter_attr_val_t attr_val;
    ESP_RETURN_ON_ERROR(get_attr_val(cluster, Attributes::DegradationDirection::Id, attr_val), "ResourceMonitoring",
                        "Failed to get DegradationDirection");
    VerifyOrReturnError(attr_val.type == ESP_MATTER_VAL_TYPE_ENUM8, ESP_FAIL);
    aDegradationDirection = (Attributes::DegradationDirection::TypeInfo::Type)attr_val.val.u8;
    if (esp_matter::command::get(cluster, Commands::ResetCondition::Id, esp_matter::COMMAND_FLAG_ACCEPTED)) {
        aResetConditionCommandSupported = true;
    } else {
        aResetConditionCommandSupported = false;
    }

    if (esp_matter::attribute::get(cluster, Attributes::InPlaceIndicator::Id)) {
        optionalAttributeSet.Set<Attributes::InPlaceIndicator::Id>();
    }
    if (esp_matter::attribute::get(cluster, Attributes::LastChangedTime::Id)) {
        optionalAttributeSet.Set<Attributes::LastChangedTime::Id>();
    }
    return ESP_OK;
}

void ESPMatterResourceMonitoringClusterInitCallback(EndpointId endpointId, ClusterId clusterId)
{
    uint64_t index = ((uint64_t)(endpointId) << 32) + clusterId;
    if (gServers[index].IsConstructed()) {
        return;
    }
    BitFlags<ResourceMonitoring::Feature> enabledFeatures;
    ResourceMonitoringCluster::OptionalAttributeSet optionalAttributeSet;
    Attributes::DegradationDirection::TypeInfo::Type degradationDirection;
    bool resetConditionCommandSupported;
    if (GetClusterConfig(endpointId, clusterId, enabledFeatures, optionalAttributeSet, degradationDirection,
                         resetConditionCommandSupported) != ESP_OK) {
        ChipLogError(AppServer, "Failed to get config for Cluster %" PRIu32, clusterId);
        return;
    }
    gServers[index].Create(endpointId, clusterId, enabledFeatures, optionalAttributeSet, degradationDirection,
                           resetConditionCommandSupported);
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Register(gServers[index].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register Cluster %" PRIu32 " - Error %" CHIP_ERROR_FORMAT, clusterId,
                     err.Format());
    }
}

void ESPMatterResourceMonitoringClusterShutdownCallback(EndpointId endpointId, ClusterId clusterId,
                                                        ClusterShutdownType shutdownType)
{
    uint64_t index = ((uint64_t)(endpointId) << 32) + clusterId;
    if (!gServers[index].IsConstructed()) {
        return;
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&gServers[index].Cluster(),
                                                                                            shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister Cluster %" PRIu32 " - Error %" CHIP_ERROR_FORMAT, clusterId,
                     err.Format());
    }
    gServers[index].Destroy();
}
} // namespace

namespace chip::app::Clusters::ResourceMonitoring {

ResourceMonitoringCluster *GetClusterInstance(EndpointId endpointId, ClusterId clusterId)
{
    uint64_t index = ((uint64_t)(endpointId) << 32) + clusterId;
    if (gServers[index].IsConstructed()) {
        return &gServers[index].Cluster();
    }
    return nullptr;
}

CHIP_ERROR SetDefaultDelegate(EndpointId endpointId, ClusterId clusterId, Delegate *delegate)
{
    uint64_t index = ((uint64_t)(endpointId) << 32) + clusterId;
    if (gServers[index].IsConstructed()) {
        return gServers[index].Cluster().SetDelegate(delegate);
    }
    return CHIP_ERROR_INCORRECT_STATE;
}
} // namespace chip::app::Clusters::ResourceMonitoring

void ESPMatterActivatedCarbonFilterMonitoringClusterServerInitCallback(EndpointId endpointId)
{
    ESPMatterResourceMonitoringClusterInitCallback(endpointId, ActivatedCarbonFilterMonitoring::Id);
}

void ESPMatterActivatedCarbonFilterMonitoringClusterServerShutdownCallback(EndpointId endpointId,
                                                                           ClusterShutdownType shutdownType)
{
    ESPMatterResourceMonitoringClusterShutdownCallback(endpointId, ActivatedCarbonFilterMonitoring::Id, shutdownType);
}

void ESPMatterHepaFilterMonitoringClusterServerInitCallback(EndpointId endpointId)
{
    ESPMatterResourceMonitoringClusterInitCallback(endpointId, HepaFilterMonitoring::Id);
}

void ESPMatterHepaFilterMonitoringClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    ESPMatterResourceMonitoringClusterShutdownCallback(endpointId, HepaFilterMonitoring::Id, shutdownType);
}
