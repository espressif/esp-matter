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
#include <bridged_device_basic_information.h>
#include <bridged_device_basic_information_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "bridged_device_basic_information_cluster";
constexpr uint16_t cluster_revision = 6;

namespace esp_matter {
namespace cluster {
namespace bridged_device_basic_information {

namespace feature {
namespace bridged_icd_support {
uint32_t get_id()
{
    return BridgedICDSupport::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_keep_active(cluster);
    event::create_active_changed(cluster);

    return ESP_OK;
}
} /* bridged_icd_support */

} /* feature */

namespace attribute {
attribute_t *create_vendor_name(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_vendor_name_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, VendorName::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_vendor_name_length + 1);
}

attribute_t *create_vendor_id(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, VendorID::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_product_name(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_product_name_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, ProductName::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_product_name_length + 1);
}

attribute_t *create_product_id(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ProductID::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_node_label(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_node_label_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, NodeLabel::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length), k_max_node_label_length + 1);
}

attribute_t *create_hardware_version(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, HardwareVersion::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_hardware_version_string(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, HardwareVersionString::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_hardware_version_string_length + 1);
}

attribute_t *create_software_version(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SoftwareVersion::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_software_version_string(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, SoftwareVersionString::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_software_version_string_length + 1);
}

attribute_t *create_manufacturing_date(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ManufacturingDate::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_manufacturing_date_length + 1);
}

attribute_t *create_part_number(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_part_number_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, PartNumber::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_part_number_length + 1);
}

attribute_t *create_product_url(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_product_url_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, ProductURL::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_product_url_length + 1);
}

attribute_t *create_product_label(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_product_label_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, ProductLabel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_product_label_length + 1);
}

attribute_t *create_serial_number(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_serial_number_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, SerialNumber::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_serial_number_length + 1);
}

attribute_t *create_reachable(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, Reachable::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_unique_id(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_unique_id_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, UniqueID::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, length), k_max_unique_id_length + 1);
}

attribute_t *create_product_appearance(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ProductAppearance::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_keep_active(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(bridged_icd_support), NULL);
    return esp_matter::command::create(cluster, KeepActive::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_start_up(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, StartUp::Id);
}

event_t *create_shut_down(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ShutDown::Id);
}

event_t *create_leave(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Leave::Id);
}

event_t *create_reachable_changed(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, ReachableChanged::Id);
}

event_t *create_active_changed(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(bridged_icd_support), NULL);
    return esp_matter::event::create(cluster, ActiveChanged::Id);
}

} /* event */

const function_generic_t function_list[] = {
    (function_generic_t)MatterBridgedDeviceBasicInformationClusterServerAttributeChangedCallback,
};

const int function_flags = CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, bridged_device_basic_information::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, bridged_device_basic_information::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterBridgedDeviceBasicInformationPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_reachable(cluster, config->reachable);
        attribute::create_unique_id(cluster, config->unique_id, sizeof(config->unique_id));
        /* Events */
        event::create_reachable_changed(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterBridgedDeviceBasicInformationClusterServerInitCallback,
                                                 ESPMatterBridgedDeviceBasicInformationClusterServerShutdownCallback);
    }

    return cluster;
}

} /* bridged_device_basic_information */
} /* cluster */
} /* esp_matter */
