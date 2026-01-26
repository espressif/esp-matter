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
#include "esp_matter_attribute_utils.h"
#include "esp_matter_data_model.h"
#include "esp_matter_data_model_priv.h"
#include "esp_matter_data_model_provider.h"
#include <unordered_map>
#include "app/clusters/boolean-state-configuration-server/BooleanStateConfigurationCluster.h"
#include "app/server-cluster/ServerClusterInterfaceRegistry.h"
#include "clusters/BooleanStateConfiguration/Enums.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::BooleanStateConfiguration;

namespace {
std::unordered_map<EndpointId, LazyRegisteredServerCluster<BooleanStateConfigurationCluster>> gServers;

esp_err_t get_attr_val(esp_matter::cluster_t *cluster, uint32_t attribute_id, esp_matter_attr_val_t &val)
{
    esp_matter::attribute_t *attr = esp_matter::attribute::get(cluster, attribute_id);
    if (!attr) {
        return ESP_FAIL;
    }
    return esp_matter::attribute::get_val_internal(attr, &val);
}

CHIP_ERROR GetClusterConfig(EndpointId endpointId, BitMask<Feature> &featureMap, uint8_t &supportedSensitivityLevels,
                            uint8_t &defaultSensitivityLevel,
                            BooleanStateConfiguration::AlarmModeBitmap &alarmsSupported,
                            BooleanStateConfigurationCluster::OptionalAttributesSet &optionalAttrSet)
{
    esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpointId, BooleanStateConfiguration::Id);
    if (!cluster) {
        return CHIP_ERROR_NOT_FOUND;
    }
    esp_matter_attr_val_t tmp_attr_val;
    VerifyOrReturnError(get_attr_val(cluster, Globals::Attributes::FeatureMap::Id, tmp_attr_val) == ESP_OK &&
                            tmp_attr_val.type == ESP_MATTER_VAL_TYPE_BITMAP32,
                        CHIP_ERROR_INTERNAL);
    featureMap = BitMask<BooleanStateConfiguration::Feature>(tmp_attr_val.val.u32);
    if (featureMap.Has(Feature::kSensitivityLevel)) {
        VerifyOrReturnError(get_attr_val(cluster, Attributes::SupportedSensitivityLevels::Id, tmp_attr_val) == ESP_OK &&
                                tmp_attr_val.type == ESP_MATTER_VAL_TYPE_UINT8,
                            CHIP_ERROR_INTERNAL);
        supportedSensitivityLevels = tmp_attr_val.val.u8;
        if (get_attr_val(cluster, Attributes::DefaultSensitivityLevel::Id, tmp_attr_val) == ESP_OK &&
            tmp_attr_val.type == ESP_MATTER_VAL_TYPE_UINT8) {
            defaultSensitivityLevel = tmp_attr_val.val.u8;
            optionalAttrSet.Set<Attributes::DefaultSensitivityLevel::Id>();
        }
    }
    if (featureMap.Has(Feature::kAudible) || featureMap.Has(Feature::kVisual)) {
        VerifyOrReturnError(get_attr_val(cluster, Attributes::AlarmsSupported::Id, tmp_attr_val) == ESP_OK &&
                                tmp_attr_val.type == ESP_MATTER_VAL_TYPE_BITMAP8,
                            CHIP_ERROR_INTERNAL);
        alarmsSupported = AlarmModeBitmap(tmp_attr_val.val.u8);
        if (esp_matter::attribute::get(cluster, Attributes::AlarmsEnabled::Id)) {
            optionalAttrSet.Set<Attributes::AlarmsEnabled::Id>();
        }
    }
    if (esp_matter::attribute::get(cluster, Attributes::SensorFault::Id)) {
        optionalAttrSet.Set<Attributes::SensorFault::Id>();
    }
    return CHIP_NO_ERROR;
}

} // namespace

namespace chip::app::Clusters::BooleanStateConfiguration {

BooleanStateConfigurationCluster *FindClusterOnEndpoint(EndpointId endpointId)
{
    if (gServers[endpointId].IsConstructed()) {
        return &gServers[endpointId].Cluster();
    }
    return nullptr;
}
} // namespace chip::app::Clusters::BooleanStateConfiguration

void ESPMatterBooleanStateConfigurationClusterServerInitCallback(EndpointId endpointId)
{
    if (gServers[endpointId].IsConstructed()) {
        return;
    }
    BitMask<Feature> featureMap;
    uint8_t supportedSensitivityLevels = 0, defaultSensitivityLevel = 0;
    AlarmModeBitmap alarmsSupported = AlarmModeBitmap::kAudible;
    BooleanStateConfigurationCluster::OptionalAttributesSet optionalAttrSet;
    CHIP_ERROR err = GetClusterConfig(endpointId, featureMap, supportedSensitivityLevels, defaultSensitivityLevel,
                                      alarmsSupported, optionalAttrSet);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to get config of BooleanStateConfiguration - Error %" CHIP_ERROR_FORMAT,
                     err.Format());
        return;
    }
    gServers[endpointId].Create(endpointId, featureMap, optionalAttrSet,
                                BooleanStateConfigurationCluster::StartupConfiguration{
                                    .supportedSensitivityLevels = supportedSensitivityLevels,
                                    .defaultSensitivityLevel = defaultSensitivityLevel,
                                    .alarmsSupported = alarmsSupported,
                                });
    err = esp_matter::data_model::provider::get_instance().registry().Register(gServers[endpointId].Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register BooleanStateConfiguration - Error %" CHIP_ERROR_FORMAT,
                     err.Format());
    }
}

void ESPMatterBooleanStateConfigurationClusterServerShutdownCallback(EndpointId endpointId,
                                                                     ClusterShutdownType shutdownType)
{
    if (!gServers[endpointId].IsConstructed()) {
        return;
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Unregister(
        &gServers[endpointId].Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister BooleanStateConfiguration - Error %" CHIP_ERROR_FORMAT,
                     err.Format());
    }
    gServers[endpointId].Destroy();
}
