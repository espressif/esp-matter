// Copyright 2025 Espressif Systems (Shanghai) PTE LTD
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

#include "esp_matter_data_model.h"
#include "esp_matter_data_model_priv.h"
#include <app/ClusterCallbacks.h>
#include <app/clusters/time-format-localization-server/TimeFormatLocalizationCluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/DeviceInfoProvider.h>
#include "clusters/TimeFormatLocalization/Enums.h"

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

namespace {
LazyRegisteredServerCluster<TimeFormatLocalizationCluster> gServer;

uint32_t get_feature_map(esp_matter::cluster_t *cluster)
{
    esp_matter::attribute_t *attribute = esp_matter::attribute::get(cluster, Globals::Attributes::FeatureMap::Id);
    if (attribute) {
        esp_matter_attr_val_t val;
        if (esp_matter::attribute::get_val_internal(attribute, &val) == ESP_OK &&
                val.type == ESP_MATTER_VAL_TYPE_BITMAP32) {
            return val.val.u32;
        }
    }
    return 0;
}

TimeFormatLocalization::HourFormatEnum get_default_hour_format(esp_matter::cluster_t *cluster)
{
    esp_matter::attribute_t *attribute =
        esp_matter::attribute::get(cluster, TimeFormatLocalization::Attributes::HourFormat::Id);
    if (attribute) {
        esp_matter_attr_val_t val;
        if (esp_matter::attribute::get_val_internal(attribute, &val) == ESP_OK &&
                val.type == ESP_MATTER_VAL_TYPE_ENUM8) {
            return TimeFormatLocalization::HourFormatEnum(val.val.u8);
        }
    }
    return TimeFormatLocalization::HourFormatEnum::kUseActiveLocale;
}

TimeFormatLocalization::CalendarTypeEnum get_default_calendar_type(esp_matter::cluster_t *cluster)
{
    esp_matter::attribute_t *attribute =
        esp_matter::attribute::get(cluster, TimeFormatLocalization::Attributes::ActiveCalendarType::Id);
    if (attribute) {
        esp_matter_attr_val_t val;
        if (esp_matter::attribute::get_val_internal(attribute, &val) == ESP_OK &&
                val.type == ESP_MATTER_VAL_TYPE_ENUM8) {
            return TimeFormatLocalization::CalendarTypeEnum(val.val.u8);
        }
    }
    return TimeFormatLocalization::CalendarTypeEnum::kUseActiveLocale;
}
} // namespace

void ESPMatterTimeFormatLocalizationClusterServerInitCallback(EndpointId endpointId)
{
    // This cluster should only exist in Root endpoint.
    VerifyOrReturn(endpointId == kRootEndpointId);
    if (!gServer.IsConstructed()) {
        esp_matter::cluster_t *cluster = esp_matter::cluster::get(endpointId, TimeFormatLocalization::Id);
        VerifyOrReturn(cluster != nullptr,
                       ChipLogError(AppServer,
                                    "TimeFormatLocalization: cluster missing in esp-matter data model for endpoint %u",
                                    endpointId));
        auto * deviceInfoProvider = DeviceLayer::GetDeviceInfoProvider();
        VerifyOrDie(deviceInfoProvider != nullptr);

        gServer.Create(endpointId, BitFlags<TimeFormatLocalization::Feature>(get_feature_map(cluster)),
                       get_default_hour_format(cluster), get_default_calendar_type(cluster),
                       TimeFormatLocalizationCluster::Context{ .deviceInfoProvider = *deviceInfoProvider });
    }
    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register TimeFormatLocalization - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterTimeFormatLocalizationClusterServerShutdownCallback(EndpointId endpointId,
                                                                  ClusterShutdownType shutdownType)
{
    // This cluster should only exist in Root endpoint.
    VerifyOrReturn(endpointId == kRootEndpointId);
    VerifyOrReturn(gServer.IsConstructed());
    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster(), shutdownType);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "TimeFormatLocalization unregister error: %" CHIP_ERROR_FORMAT, err.Format());
    }
    if (shutdownType == ClusterShutdownType::kPermanentRemove) {
        gServer.Destroy();
    }
}

void MatterTimeFormatLocalizationPluginServerInitCallback()
{
}
