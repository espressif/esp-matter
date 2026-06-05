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
#include <energy_evse.h>
#include <energy_evse_ids.h>
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

static const char *TAG = "energy_evse_cluster";
constexpr uint16_t cluster_revision = 4;

namespace esp_matter {
namespace cluster {
namespace energy_evse {

namespace feature {
namespace charging_preferences {
uint32_t get_id()
{
    return ChargingPreferences::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_next_charge_start_time(cluster, config->next_charge_start_time);
    attribute::create_next_charge_target_time(cluster, config->next_charge_target_time);
    attribute::create_next_charge_required_energy(cluster, config->next_charge_required_energy);
    attribute::create_next_charge_target_soc(cluster, config->next_charge_target_soc);
    command::create_get_targets_response(cluster);
    command::create_set_targets(cluster);
    command::create_get_targets(cluster);
    command::create_clear_targets(cluster);

    return ESP_OK;
}
} /* charging_preferences */

namespace so_c_reporting {
uint32_t get_id()
{
    return SoCReporting::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_state_of_charge(cluster, config->state_of_charge);
    attribute::create_battery_capacity(cluster, config->battery_capacity);

    return ESP_OK;
}
} /* so_c_reporting */

namespace plug_and_charge {
uint32_t get_id()
{
    return PlugAndCharge::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_vehicle_id(cluster, config->vehicle_id, sizeof(config->vehicle_id));

    return ESP_OK;
}
} /* plug_and_charge */

namespace rfid {
uint32_t get_id()
{
    return RFID::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());

    return ESP_OK;
}
} /* rfid */

namespace v_2_x {
uint32_t get_id()
{
    return V2X::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_discharging_enabled_until(cluster, config->discharging_enabled_until);
    attribute::create_maximum_discharge_current(cluster, config->maximum_discharge_current);
    attribute::create_session_energy_discharged(cluster, config->session_energy_discharged);
    command::create_enable_discharging(cluster);

    return ESP_OK;
}
} /* v_2_x */

} /* feature */

namespace attribute {
attribute_t *create_state(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, State::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(6), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_supply_state(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SupplyState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(5), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_fault_state(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, FaultState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(16), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_charging_enabled_until(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ChargingEnabledUntil::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_discharging_enabled_until(cluster_t *cluster, nullable<uint32_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(v_2_x), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, DischargingEnabledUntil::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_circuit_capacity(cluster_t *cluster, int64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, CircuitCapacity::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int64_t>(0)), esp_matter_attr_val(static_cast<int64_t>(2147483646)));
    return attribute;
}

attribute_t *create_minimum_charge_current(cluster_t *cluster, int64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinimumChargeCurrent::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int64_t>(0)), esp_matter_attr_val(static_cast<int64_t>(2147483646)));
    return attribute;
}

attribute_t *create_maximum_charge_current(cluster_t *cluster, int64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaximumChargeCurrent::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int64_t>(0)), esp_matter_attr_val(static_cast<int64_t>(2147483646)));
    return attribute;
}

attribute_t *create_maximum_discharge_current(cluster_t *cluster, int64_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(v_2_x), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaximumDischargeCurrent::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int64_t>(0)), esp_matter_attr_val(static_cast<int64_t>(2147483646)));
    return attribute;
}

attribute_t *create_user_maximum_charge_current(cluster_t *cluster, int64_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, UserMaximumChargeCurrent::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<int64_t>(-2147483648)), esp_matter_attr_val(static_cast<int64_t>(2147483646)));
    return attribute;
}

attribute_t *create_randomization_delay_window(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, RandomizationDelayWindow::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(86400)));
    return attribute;
}

attribute_t *create_next_charge_start_time(cluster_t *cluster, nullable<uint32_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(charging_preferences), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NextChargeStartTime::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_next_charge_target_time(cluster_t *cluster, nullable<uint32_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(charging_preferences), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NextChargeTargetTime::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_next_charge_required_energy(cluster_t *cluster, nullable<int64_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(charging_preferences), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NextChargeRequiredEnergy::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(0)), esp_matter_attr_val(nullable<int64_t>(2147483646)));
    return attribute;
}

attribute_t *create_next_charge_target_soc(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(charging_preferences), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NextChargeTargetSoC::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_approximate_ev_efficiency(cluster_t *cluster, nullable<uint16_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, ApproximateEVEfficiency::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint16_t>(0)), esp_matter_attr_val(nullable<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_state_of_charge(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(so_c_reporting), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, StateOfCharge::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0)), esp_matter_attr_val(nullable<uint8_t>(254)));
    return attribute;
}

attribute_t *create_battery_capacity(cluster_t *cluster, nullable<int64_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(so_c_reporting), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, BatteryCapacity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(0)), esp_matter_attr_val(nullable<int64_t>(2147483646)));
    return attribute;
}

attribute_t *create_vehicle_id(cluster_t *cluster, char *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(plug_and_charge), NULL);
    VerifyOrReturnValue(length <= k_max_vehicle_id_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, VehicleID::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length), k_max_vehicle_id_length + 1);
}

attribute_t *create_session_id(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SessionID::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_session_duration(cluster_t *cluster, nullable<uint32_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SessionDuration::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint32_t>(0)), esp_matter_attr_val(nullable<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_session_energy_charged(cluster_t *cluster, nullable<int64_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SessionEnergyCharged::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(0)), esp_matter_attr_val(nullable<int64_t>(2147483646)));
    return attribute;
}

attribute_t *create_session_energy_discharged(cluster_t *cluster, nullable<int64_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(v_2_x), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, SessionEnergyDischarged::Id, ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<int64_t>(0)), esp_matter_attr_val(nullable<int64_t>(2147483646)));
    return attribute;
}

} /* attribute */
namespace command {
command_t *create_get_targets_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(charging_preferences), NULL);
    return esp_matter::command::create(cluster, GetTargetsResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_disable(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, Disable::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_enable_charging(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, EnableCharging::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_enable_discharging(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(v_2_x), NULL);
    return esp_matter::command::create(cluster, EnableDischarging::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_start_diagnostics(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, StartDiagnostics::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_set_targets(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(charging_preferences), NULL);
    return esp_matter::command::create(cluster, SetTargets::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_get_targets(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(charging_preferences), NULL);
    return esp_matter::command::create(cluster, GetTargets::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

command_t *create_clear_targets(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(charging_preferences), NULL);
    return esp_matter::command::create(cluster, ClearTargets::Id, COMMAND_FLAG_ACCEPTED, NULL);
}

} /* command */

namespace event {
event_t *create_ev_connected(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, EVConnected::Id);
}

event_t *create_ev_not_detected(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, EVNotDetected::Id);
}

event_t *create_energy_transfer_started(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, EnergyTransferStarted::Id);
}

event_t *create_energy_transfer_stopped(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, EnergyTransferStopped::Id);
}

event_t *create_fault(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, Fault::Id);
}

event_t *create_rfid(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, RFID::Id);
}

} /* event */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, energy_evse::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, energy_evse::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = EnergyEvseDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterEnergyEvsePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_state(cluster, config->state);
        attribute::create_supply_state(cluster, config->supply_state);
        attribute::create_fault_state(cluster, config->fault_state);
        attribute::create_charging_enabled_until(cluster, config->charging_enabled_until);
        attribute::create_circuit_capacity(cluster, config->circuit_capacity);
        attribute::create_minimum_charge_current(cluster, config->minimum_charge_current);
        attribute::create_maximum_charge_current(cluster, config->maximum_charge_current);
        attribute::create_session_id(cluster, config->session_id);
        attribute::create_session_duration(cluster, config->session_duration);
        attribute::create_session_energy_charged(cluster, config->session_energy_charged);
        command::create_disable(cluster);
        command::create_enable_charging(cluster);
        /* Events */
        event::create_ev_connected(cluster);
        event::create_ev_not_detected(cluster);
        event::create_energy_transfer_started(cluster);
        event::create_energy_transfer_stopped(cluster);
        event::create_fault(cluster);

        cluster::set_init_and_shutdown_callbacks(cluster, ESPMatterEnergyEvseClusterServerInitCallback,
                                                 ESPMatterEnergyEvseClusterServerShutdownCallback);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* energy_evse */
} /* cluster */
} /* esp_matter */
