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
#include <basic_information.h>
#include <basic_information_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "basic_information_cluster";
constexpr uint16_t cluster_revision = 6;

namespace esp_matter {
namespace cluster {
namespace basic_information {

namespace attribute {
attribute_t *create_data_model_revision(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DataModelRevision::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

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

attribute_t *create_location(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, Location::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length), k_max_location_length + 1);
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

attribute_t *create_local_config_disabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, LocalConfigDisabled::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
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

attribute_t *create_capability_minima(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CapabilityMinima::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_product_appearance(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ProductAppearance::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_specification_version(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SpecificationVersion::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_max_paths_per_invoke(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxPathsPerInvoke::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(1)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

} /* attribute */

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
    VerifyOrReturnValue(has_attribute(Reachable), NULL);
    return esp_matter::event::create(cluster, ReachableChanged::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, basic_information::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, basic_information::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterBasicInformationPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_data_model_revision(cluster, config->data_model_revision);
        attribute::create_vendor_name(cluster, config->vendor_name, sizeof(config->vendor_name));
        attribute::create_vendor_id(cluster, config->vendor_id);
        attribute::create_product_name(cluster, config->product_name, sizeof(config->product_name));
        attribute::create_product_id(cluster, config->product_id);
        attribute::create_node_label(cluster, config->node_label, sizeof(config->node_label));
        attribute::create_location(cluster, config->location, sizeof(config->location));
        attribute::create_hardware_version(cluster, config->hardware_version);
        attribute::create_hardware_version_string(cluster, config->hardware_version_string, sizeof(config->hardware_version_string));
        attribute::create_software_version(cluster, config->software_version);
        attribute::create_software_version_string(cluster, config->software_version_string, sizeof(config->software_version_string));
        attribute::create_specification_version(cluster, config->specification_version);
        attribute::create_max_paths_per_invoke(cluster, config->max_paths_per_invoke);
        attribute::create_capability_minima(cluster, NULL, 0, 0);
        /* Events */
        event::create_start_up(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterBasicInformationClusterServerInitCallback,
                                                 ESPMatterBasicInformationClusterServerShutdownCallback);
    }

    return cluster;
}

} /* basic_information */
} /* cluster */
} /* esp_matter */
