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
#include <esp_matter_attribute_utils.h>
#include <esp_matter_data_model_priv.h>
#include <app/clusters/time-synchronization-server/TimeSynchronizationCluster.h>
#include <app/server-cluster/ServerClusterInterfaceRegistry.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_matter_data_model.h>
#include <lib/support/BitFlags.h>
#include <clusters/TimeSynchronization/Enums.h>
#include <esp_check.h>
#include "support/CodeUtils.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::TimeSynchronization;

namespace {
LazyRegisteredServerCluster<TimeSynchronizationCluster> gServer;
TimeSynchronization::Delegate * gDelegate = nullptr;

esp_err_t get_attr_val(esp_matter::cluster_t *cluster, uint32_t attribute_id, esp_matter_attr_val_t &val)
{
    esp_matter::attribute_t *attr = esp_matter::attribute::get(cluster, attribute_id);
    if (!attr) {
      return ESP_FAIL;
    }
    return esp_matter::attribute::get_val_internal(attr, &val);
}

esp_err_t GetClusterConfig(EndpointId endpointId, TimeSynchronizationCluster::OptionalAttributeSet &attrSet,
                           TimeSynchronizationCluster::StartupConfiguration &startupConfig,
                           BitFlags<TimeSynchronization::Feature> &featureMap)
{
    esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpointId, TimeSynchronization::Id);
    if (!cluster) {
        return ESP_FAIL;
    }
    esp_matter_attr_val_t feature_val;
    ESP_RETURN_ON_ERROR(get_attr_val(cluster, Globals::Attributes::FeatureMap::Id, feature_val), "TimeSync",
        "Failed to get feature map");
    VerifyOrReturnError(feature_val.type == ESP_MATTER_VAL_TYPE_BITMAP32, ESP_FAIL);
    featureMap = BitFlags<TimeSynchronization::Feature>(feature_val.val.u32);
    esp_matter_attr_val_t attr_val;
    if (featureMap.Has(Feature::kNTPClient) || featureMap.Has(Feature::kNTPServer)) {
        ESP_RETURN_ON_ERROR(get_attr_val(cluster, Attributes::SupportsDNSResolve::Id, attr_val), "TimeSync",
        "Failed to get SupportsDNSResolve");
        VerifyOrReturnError(attr_val.type == ESP_MATTER_VAL_TYPE_BOOLEAN, ESP_FAIL);
        if (featureMap.Has(Feature::kNTPClient)) {
            startupConfig.supportsDNSResolve = attr_val.val.b;
        }
        if (featureMap.Has(Feature::kNTPServer)) {
            startupConfig.ntpServerAvailable = attr_val.val.b;
        }
    }
    if (featureMap.Has(Feature::kTimeZone)) {
        ESP_RETURN_ON_ERROR(get_attr_val(cluster, Attributes::TimeZoneDatabase::Id, attr_val), "TimeSync",
        "Failed to get TimeZoneDatabase");
        VerifyOrReturnError(attr_val.type == ESP_MATTER_VAL_TYPE_ENUM8, ESP_FAIL);
        startupConfig.timeZoneDatabase = (TimeZoneDatabaseEnum)attr_val.val.u8; 
    }
    if (get_attr_val(cluster, Attributes::TimeSource::Id, attr_val) == ESP_OK && 
        attr_val.type == ESP_MATTER_VAL_TYPE_ENUM8) {
        attrSet.Set<Attributes::TimeSource::Id>();
        startupConfig.timeSource = (TimeSourceEnum)attr_val.val.u8;        
    }
    startupConfig.delegate = GetDefaultDelegate();
    return ESP_OK;
}
}

void ESPMatterTimeSynchronizationClusterServerInitCallback(EndpointId endpointId)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    TimeSynchronizationCluster::OptionalAttributeSet attrSet;
    TimeSynchronizationCluster::StartupConfiguration startupConfig;
    BitFlags<TimeSynchronization::Feature> featureMap;
    VerifyOrReturn(GetClusterConfig(endpointId, attrSet, startupConfig, featureMap) == ESP_OK);
    gServer.Create(endpointId, featureMap, attrSet, startupConfig);
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register TimeSynchronization - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterTimeSynchronizationClusterServerShutdownCallback(EndpointId endpointId, ClusterShutdownType shutdownType)
{
    VerifyOrReturn(endpointId == kRootEndpointId);
    VerifyOrReturn(gServer.IsConstructed());
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister TimeSynchronization - Error %" CHIP_ERROR_FORMAT, err.Format());
    }
    gServer.Destroy();
}

void MatterTimeSynchronizationPluginServerInitCallback() {}

namespace chip::app::Clusters::TimeSynchronization {

TimeSynchronizationCluster * GetClusterInstance()
{
    VerifyOrReturnValue(gServer.IsConstructed(), nullptr);
    return &gServer.Cluster();
}

void SetDefaultDelegate(Delegate * delegate)
{
    VerifyOrReturn(delegate != nullptr);
    gDelegate                = delegate;
    auto timeSynchronization = GetClusterInstance();
    if (timeSynchronization != nullptr)
    {
        timeSynchronization->SetDelegate(gDelegate);
    }
}

Delegate * GetDefaultDelegate()
{
    auto timeSynchronization = GetClusterInstance();
    if (timeSynchronization != nullptr)
    {
        return timeSynchronization->GetDelegate();
    }
    if (gDelegate == nullptr)
    {
        static DefaultTimeSyncDelegate delegate;
        gDelegate = &delegate;
    }
    return gDelegate;
}

} // namespace chip::app::Clusters::TimeSynchronization
