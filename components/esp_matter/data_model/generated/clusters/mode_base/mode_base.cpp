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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#include <esp_log.h>
#include <esp_matter_core.h>
#include <esp_matter.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <app-common/zap-generated/callback.h>
#include <app/InteractionModelEngine.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <app/clusters/mode-base-server/mode-base-cluster-objects.h>
#include <esp_matter_delegate_callbacks.h>
#include <mode_base.h>
#include <mode_base_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "mode_base_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace mode_base {

namespace feature {
namespace on_off {
uint32_t get_id()
{
    return OnOff::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_on_mode(cluster, 0);

    return ESP_OK;
}
} /* on_off */

} /* feature */

namespace attribute {
attribute_t *create_supported_modes(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, SupportedModes::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CurrentMode::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_start_up_mode(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, StartUpMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_on_mode(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(on_off), NULL);
    return esp_matter::attribute::create(cluster, OnMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

} /* attribute */
namespace command {
command_t *create_change_to_mode(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ChangeToMode::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_change_to_mode_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ChangeToModeResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

} /* mode_base */
} /* cluster */
} /* esp_matter */
