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
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <general_diagnostics.h>
#include <general_diagnostics_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 3;

namespace esp_matter {
namespace cluster {
namespace general_diagnostics {

namespace feature {
namespace data_model_test {
uint32_t get_id()
{
    return DataModelTest::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_payload_test_request(cluster);
    command::create_payload_test_response(cluster);

    return ESP_OK;
}
} /* data_model_test */

} /* feature */

namespace attribute {
attribute_t *create_network_interfaces(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NetworkInterfaces::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_reboot_count(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RebootCount::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_up_time(cluster_t *cluster, uint64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, UpTime::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint64_t>(0)), esp_matter_attr_val(static_cast<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_total_operational_hours(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TotalOperationalHours::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_boot_reason(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, BootReason::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(6), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_active_hardware_faults(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActiveHardwareFaults::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_active_radio_faults(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActiveRadioFaults::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_active_network_faults(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActiveNetworkFaults::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_test_event_triggers_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, TestEventTriggersEnabled::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_device_load_status(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, DeviceLoadStatus::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_test_event_trigger(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TestEventTrigger::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_time_snapshot(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TimeSnapshot::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_time_snapshot_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, TimeSnapshotResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_payload_test_request(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(data_model_test), NULL);
    return esp_matter::command::create(cluster, PayloadTestRequest::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_payload_test_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(data_model_test), NULL);
    return esp_matter::command::create(cluster, PayloadTestResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

namespace event {
event_t *create_hardware_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, HardwareFaultChange::Id);
}

event_t *create_radio_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, RadioFaultChange::Id);
}

event_t *create_network_fault_change(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, NetworkFaultChange::Id);
}

event_t *create_boot_reason(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, BootReason::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, general_diagnostics::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, general_diagnostics::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterGeneralDiagnosticsPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_reboot_count(cluster, config->reboot_count);
        attribute::create_up_time(cluster, config->up_time);
        attribute::create_test_event_triggers_enabled(cluster, config->test_event_triggers_enabled);
        attribute::create_network_interfaces(cluster, NULL, 0, 0);
        attribute::create_device_load_status(cluster, NULL, 0, 0);
        command::create_test_event_trigger(cluster);
        command::create_time_snapshot(cluster);
        command::create_time_snapshot_response(cluster);
        /* Events */
        event::create_boot_reason(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterGeneralDiagnosticsClusterServerInitCallback,
                                                 ESPMatterGeneralDiagnosticsClusterServerShutdownCallback);
    }

    return cluster;
}

} /* general_diagnostics */
} /* cluster */
} /* esp_matter */
