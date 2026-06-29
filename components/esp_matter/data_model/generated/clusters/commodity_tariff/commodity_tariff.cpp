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
#include <esp_matter_delegate_callbacks.h>
#include <commodity_tariff.h>
#include <commodity_tariff_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "esp_matter_cluster";
constexpr uint16_t cluster_revision = 1;

namespace esp_matter {
namespace cluster {
namespace commodity_tariff {

namespace feature {
namespace pricing {
uint32_t get_id()
{
    return Pricing::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* pricing */

namespace friendly_credit {
uint32_t get_id()
{
    return FriendlyCredit::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* friendly_credit */

namespace auxiliary_load {
uint32_t get_id()
{
    return AuxiliaryLoad::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* auxiliary_load */

namespace peak_period {
uint32_t get_id()
{
    return PeakPeriod::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* peak_period */

namespace power_threshold {
uint32_t get_id()
{
    return PowerThreshold::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* power_threshold */

namespace randomization {
uint32_t get_id()
{
    return Randomization::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_default_randomization_offset(cluster, 0);
    attribute::create_default_randomization_type(cluster, 0);

    return ESP_OK;
}
} /* randomization */

} /* feature */

namespace attribute {
attribute_t *create_tariff_info(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, TariffInfo::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_tariff_unit(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, TariffUnit::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
}

attribute_t *create_start_date(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, StartDate::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_day_entries(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, DayEntries::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_day_patterns(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, DayPatterns::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_calendar_periods(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CalendarPeriods::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_individual_days(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, IndividualDays::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_day(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CurrentDay::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_next_day(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NextDay::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_day_entry(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CurrentDayEntry::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_day_entry_date(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, CurrentDayEntryDate::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_next_day_entry(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NextDayEntry::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_next_day_entry_date(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, NextDayEntryDate::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_tariff_components(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, TariffComponents::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_tariff_periods(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, TariffPeriods::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_current_tariff_components(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, CurrentTariffComponents::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_next_tariff_components(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NextTariffComponents::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_default_randomization_offset(cluster_t *cluster, nullable<int16_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(randomization), NULL);
    return esp_matter::attribute::create(cluster, DefaultRandomizationOffset::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
}

attribute_t *create_default_randomization_type(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(randomization), NULL);
    return esp_matter::attribute::create(cluster, DefaultRandomizationType::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
}

} /* attribute */
namespace command {
command_t *create_get_tariff_component(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetTariffComponent::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_tariff_component_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetTariffComponentResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_get_day_entry(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetDayEntry::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_day_entry_response(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, GetDayEntryResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, commodity_tariff::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, commodity_tariff::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = CommodityTariffDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterCommodityTariffPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, config->feature_flags);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_tariff_info(cluster, NULL, 0, 0);
        attribute::create_tariff_unit(cluster, 0);
        attribute::create_start_date(cluster, 0);
        attribute::create_day_entries(cluster, NULL, 0, 0);
        attribute::create_day_patterns(cluster, NULL, 0, 0);
        attribute::create_calendar_periods(cluster, NULL, 0, 0);
        attribute::create_individual_days(cluster, NULL, 0, 0);
        attribute::create_current_day(cluster, NULL, 0, 0);
        attribute::create_next_day(cluster, NULL, 0, 0);
        attribute::create_current_day_entry(cluster, NULL, 0, 0);
        attribute::create_current_day_entry_date(cluster, 0);
        attribute::create_next_day_entry(cluster, NULL, 0, 0);
        attribute::create_next_day_entry_date(cluster, 0);
        attribute::create_tariff_components(cluster, NULL, 0, 0);
        attribute::create_tariff_periods(cluster, NULL, 0, 0);
        attribute::create_current_tariff_components(cluster, NULL, 0, 0);
        attribute::create_next_tariff_components(cluster, NULL, 0, 0);

        uint32_t feature_map = config->feature_flags;
        VALIDATE_FEATURES_AT_LEAST_ONE("Pricing,FriendlyCredit,AuxiliaryLoad",
                                       feature::pricing::get_id(), feature::friendly_credit::get_id(), feature::auxiliary_load::get_id());
        if (feature_map & feature::pricing::get_id()) {
            VerifyOrReturnValue(feature::pricing::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::friendly_credit::get_id()) {
            VerifyOrReturnValue(feature::friendly_credit::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::auxiliary_load::get_id()) {
            VerifyOrReturnValue(feature::auxiliary_load::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::peak_period::get_id()) {
            VerifyOrReturnValue(feature::peak_period::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::power_threshold::get_id()) {
            VerifyOrReturnValue(feature::power_threshold::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        if (feature_map & feature::randomization::get_id()) {
            VerifyOrReturnValue(feature::randomization::add(cluster) == ESP_OK, ABORT_CLUSTER_CREATE(cluster));
        }
        command::create_get_tariff_component(cluster);
        command::create_get_tariff_component_response(cluster);
        command::create_get_day_entry(cluster);
        command::create_get_day_entry_response(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* commodity_tariff */
} /* cluster */
} /* esp_matter */
