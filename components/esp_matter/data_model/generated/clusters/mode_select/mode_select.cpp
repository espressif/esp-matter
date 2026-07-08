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
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <app/clusters/mode-base-server/mode-base-cluster-objects.h>
#include <esp_matter_delegate_callbacks.h>
#include <mode_select.h>
#include <mode_select_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 2;

static esp_err_t esp_matter_command_callback_change_to_mode(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                            void *opaque_ptr)
{
    chip::app::Clusters::ModeSelect::Commands::ChangeToMode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfModeSelectClusterChangeToModeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace mode_select {

namespace feature {
namespace on_off {
uint32_t get_id()
{
    return OnOff::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_on_mode(cluster, config->on_mode);

    return ESP_OK;
}
} /* on_off */

} /* feature */

namespace attribute {
attribute_t *create_description(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_description_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound. cluster_id: 0x%08" PRIX32, mode_select::Id));
    return esp_matter::attribute::create(cluster, Description::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_description_length + 1);
}

attribute_t *create_standard_namespace(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, StandardNamespace::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
}

attribute_t *create_supported_modes(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, SupportedModes::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, CurrentMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_start_up_mode(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, StartUpMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_on_mode(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(on_off), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, OnMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_change_to_mode(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, ChangeToMode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_change_to_mode);
}

} /* command */

const function_generic_t function_list[] = {
    (function_generic_t)emberAfModeSelectClusterServerInitCallback,
    (function_generic_t)MatterModeSelectClusterServerPreAttributeChangedCallback,
};

const int function_flags = CLUSTER_FLAG_INIT_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, mode_select::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, mode_select::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = ModeSelectDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterModeSelectPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_description(cluster, config->description, sizeof(config->description));
        attribute::create_standard_namespace(cluster, config->standard_namespace);
        attribute::create_current_mode(cluster, config->current_mode);
        attribute::create_supported_modes(cluster, NULL, 0, 0);
        command::create_change_to_mode(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* mode_select */
} /* cluster */
} /* esp_matter */
