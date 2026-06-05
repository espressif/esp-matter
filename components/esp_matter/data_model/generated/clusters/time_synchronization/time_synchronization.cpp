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
#include <esp_matter_delegate_callbacks.h>
#include <time_synchronization.h>
#include <time_synchronization_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>
#include <app/ClusterCallbacks.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "time_synchronization_cluster";
constexpr uint16_t cluster_revision = 2;

namespace esp_matter {
namespace cluster {
namespace time_synchronization {

namespace feature {
namespace time_zone {
uint32_t get_id()
{
    return TimeZone::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_local_time(cluster, config->local_time);
    attribute::create_time_zone_database(cluster, config->time_zone_database);
    attribute::create_time_zone_list_max_size(cluster, config->time_zone_list_max_size);
    attribute::create_dst_offset_list_max_size(cluster, config->dst_offset_list_max_size);
    attribute::create_time_zone(cluster, NULL, 0, 0);
    attribute::create_dst_offset(cluster, NULL, 0, 0);
    command::create_set_time_zone(cluster);
    command::create_set_time_zone_response(cluster);
    command::create_set_dst_offset(cluster);
    event::create_dst_table_empty(cluster);
    event::create_dst_status(cluster);
    event::create_time_zone_status(cluster);

    return ESP_OK;
}
} /* time_zone */

namespace ntp_client {
uint32_t get_id()
{
    return NTPClient::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_default_ntp(cluster, config->default_ntp, sizeof(config->default_ntp));
    attribute::create_supports_dns_resolve(cluster, config->supports_dns_resolve);
    command::create_set_default_ntp(cluster);

    return ESP_OK;
}
} /* ntp_client */

namespace ntp_server {
uint32_t get_id()
{
    return NTPServer::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_ntp_server_available(cluster, config->ntp_server_available);

    return ESP_OK;
}
} /* ntp_server */

namespace time_sync_client {
uint32_t get_id()
{
    return TimeSyncClient::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_trusted_time_source(cluster, NULL, 0, 0);
    command::create_set_trusted_time_source(cluster);
    event::create_missing_trusted_time_source(cluster);

    return ESP_OK;
}
} /* time_sync_client */

} /* feature */

namespace attribute {
attribute_t *create_utc_time(cluster_t *cluster, nullable<uint64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, UTCTime::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint64_t>(0)), esp_matter_attr_val(nullable<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_granularity(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Granularity::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(4), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_time_source(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, TimeSource::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(16), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_trusted_time_source(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_sync_client), NULL);
    return esp_matter::attribute::create(cluster, TrustedTimeSource::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_default_ntp(cluster_t *cluster, char *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(ntp_client), NULL);
    VerifyOrReturnValue(length <= k_max_default_ntp_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, DefaultNTP::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length), k_max_default_ntp_length + 1);
}

attribute_t *create_time_zone(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    return esp_matter::attribute::create(cluster, TimeZone::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_dst_offset(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    return esp_matter::attribute::create(cluster, DSTOffset::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_local_time(cluster_t *cluster, nullable<uint64_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, LocalTime::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint64_t>(0)), esp_matter_attr_val(nullable<uint64_t>(4294967294)));
    return attribute;
}

attribute_t *create_time_zone_database(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TimeZoneDatabase::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_ntp_server_available(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(ntp_server), NULL);
    return esp_matter::attribute::create(cluster, NTPServerAvailable::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_time_zone_list_max_size(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, TimeZoneListMaxSize::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(1)), esp_matter_attr_val(static_cast<uint8_t>(2)));
    return attribute;
}

attribute_t *create_dst_offset_list_max_size(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, DSTOffsetListMaxSize::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(1)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_supports_dns_resolve(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(ntp_client), NULL);
    return esp_matter::attribute::create(cluster, SupportsDNSResolve::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

} /* attribute */
namespace command {
command_t *create_set_utc_time(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, SetUTCTime::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_trusted_time_source(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_sync_client), NULL);
    return esp_matter::command::create(cluster, SetTrustedTimeSource::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_time_zone(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    return esp_matter::command::create(cluster, SetTimeZone::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_time_zone_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    return esp_matter::command::create(cluster, SetTimeZoneResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_set_dst_offset(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    return esp_matter::command::create(cluster, SetDSTOffset::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_default_ntp(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(ntp_client), NULL);
    return esp_matter::command::create(cluster, SetDefaultNTP::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_dst_table_empty(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    return esp_matter::event::create(cluster, DSTTableEmpty::Id);
}

event_t *create_dst_status(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    return esp_matter::event::create(cluster, DSTStatus::Id);
}

event_t *create_time_zone_status(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_zone), NULL);
    return esp_matter::event::create(cluster, TimeZoneStatus::Id);
}

event_t *create_time_failure(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, TimeFailure::Id);
}

event_t *create_missing_trusted_time_source(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(time_sync_client), NULL);
    return esp_matter::event::create(cluster, MissingTrustedTimeSource::Id);
}

} /* event */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, time_synchronization::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, time_synchronization::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = TimeSynchronizationDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterTimeSynchronizationPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_utc_time(cluster, config->utc_time);
        attribute::create_granularity(cluster, config->granularity);
        command::create_set_utc_time(cluster);
        /* Events */
        event::create_time_failure(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterTimeSynchronizationClusterServerInitCallback,
                                                 ESPMatterTimeSynchronizationClusterServerShutdownCallback);
    }

    return cluster;
}

} /* time_synchronization */
} /* cluster */
} /* esp_matter */
