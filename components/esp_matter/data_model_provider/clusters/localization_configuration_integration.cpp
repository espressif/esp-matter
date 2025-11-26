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

#include <app/clusters/localization-configuration-server/localization-configuration-cluster.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_matter_attribute_utils.h>
#include <esp_matter_data_model.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/Span.h>
#include <platform/DeviceInfoProvider.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

namespace {
LazyRegisteredServerCluster<LocalizationConfigurationCluster> gServer;
}

void ESPMatterLocalizationConfigurationClusterServerInitCallback(EndpointId endpointId)
{
    if (endpointId != kRootEndpointId) {
        return;
    }
    esp_matter::attribute_t *active_locale = esp_matter::attribute::get(
        endpointId, LocalizationConfiguration::Id, LocalizationConfiguration::Attributes::ActiveLocale::Id);
    esp_matter_attr_val_t attr_val;
    if (active_locale && esp_matter::attribute::get_val(active_locale, &attr_val) == ESP_OK &&
        attr_val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING &&
        attr_val.val.a.s <= LocalizationConfiguration::Attributes::ActiveLocale::TypeInfo::MaxLength()) {
        ChipLogError(AppServer, "Failed to get active locale on endpoint %u", endpointId);
    }
    gServer.Create(*DeviceLayer::GetDeviceInfoProvider(), CharSpan((const char *)attr_val.val.a.b, attr_val.val.a.s));

    CHIP_ERROR err = esp_matter::data_model::provider::get_instance().registry().Register(gServer.Registration());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to register LocalizationConfiguration - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

void ESPMatterLocalizationConfigurationClusterServerShutdownCallback(EndpointId endpointId)
{
    if (endpointId != kRootEndpointId) {
        return;
    }

    CHIP_ERROR err =
        esp_matter::data_model::provider::get_instance().registry().Unregister(&gServer.Cluster());
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to unregister LocalizationConfiguration - Error: %" CHIP_ERROR_FORMAT, err.Format());
    }
}
