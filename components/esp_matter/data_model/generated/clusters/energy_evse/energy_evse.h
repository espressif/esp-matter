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

#pragma once
#include <esp_matter_data_model.h>

namespace esp_matter {
namespace cluster {
namespace energy_evse {

const uint8_t k_max_vehicle_id_length = 32u;
namespace feature {
namespace charging_preferences {
typedef struct config {
    nullable<uint32_t> next_charge_start_time;
    nullable<uint32_t> next_charge_target_time;
    nullable<int64_t> next_charge_required_energy;
    nullable<uint8_t> next_charge_target_soc;
    config() : next_charge_start_time(), next_charge_target_time(), next_charge_required_energy(), next_charge_target_soc() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* charging_preferences */

namespace so_c_reporting {
typedef struct config {
    nullable<uint8_t> state_of_charge;
    nullable<int64_t> battery_capacity;
    config() : state_of_charge(), battery_capacity() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* so_c_reporting */

namespace plug_and_charge {
typedef struct config {
    char vehicle_id[k_max_vehicle_id_length + 1];
    config() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* plug_and_charge */

namespace rfid {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* rfid */

namespace v_2_x {
typedef struct config {
    nullable<uint32_t> discharging_enabled_until;
    int64_t maximum_discharge_current;
    nullable<int64_t> session_energy_discharged;
    config() : discharging_enabled_until(), maximum_discharge_current(0), session_energy_discharged() {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* v_2_x */

} /* feature */

namespace attribute {
attribute_t *create_state(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_supply_state(cluster_t *cluster, uint8_t value);
attribute_t *create_fault_state(cluster_t *cluster, uint8_t value);
attribute_t *create_charging_enabled_until(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_discharging_enabled_until(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_circuit_capacity(cluster_t *cluster, int64_t value);
attribute_t *create_minimum_charge_current(cluster_t *cluster, int64_t value);
attribute_t *create_maximum_charge_current(cluster_t *cluster, int64_t value);
attribute_t *create_maximum_discharge_current(cluster_t *cluster, int64_t value);
attribute_t *create_user_maximum_charge_current(cluster_t *cluster, int64_t value);
attribute_t *create_randomization_delay_window(cluster_t *cluster, uint32_t value);
attribute_t *create_next_charge_start_time(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_next_charge_target_time(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_next_charge_required_energy(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_next_charge_target_soc(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_approximate_ev_efficiency(cluster_t *cluster, nullable<uint16_t> value);
attribute_t *create_state_of_charge(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_battery_capacity(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_vehicle_id(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_session_id(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_session_duration(cluster_t *cluster, nullable<uint32_t> value);
attribute_t *create_session_energy_charged(cluster_t *cluster, nullable<int64_t> value);
attribute_t *create_session_energy_discharged(cluster_t *cluster, nullable<int64_t> value);
} /* attribute */

namespace command {
command_t *create_get_targets_response(cluster_t *cluster);
command_t *create_disable(cluster_t *cluster);
command_t *create_enable_charging(cluster_t *cluster);
command_t *create_enable_discharging(cluster_t *cluster);
command_t *create_start_diagnostics(cluster_t *cluster);
command_t *create_set_targets(cluster_t *cluster);
command_t *create_get_targets(cluster_t *cluster);
command_t *create_clear_targets(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_ev_connected(cluster_t *cluster);
event_t *create_ev_not_detected(cluster_t *cluster);
event_t *create_energy_transfer_started(cluster_t *cluster);
event_t *create_energy_transfer_stopped(cluster_t *cluster);
event_t *create_fault(cluster_t *cluster);
event_t *create_rfid(cluster_t *cluster);
} /* event */

typedef struct config {
    nullable<uint8_t> state;
    uint8_t supply_state;
    uint8_t fault_state;
    nullable<uint32_t> charging_enabled_until;
    int64_t circuit_capacity;
    int64_t minimum_charge_current;
    int64_t maximum_charge_current;
    nullable<uint32_t> session_id;
    nullable<uint32_t> session_duration;
    nullable<int64_t> session_energy_charged;
    void *delegate;
    config() : state(), supply_state(0), fault_state(0), charging_enabled_until(), circuit_capacity(0), minimum_charge_current(0), maximum_charge_current(0), session_id(), session_duration(), session_energy_charged(), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* energy_evse */
} /* cluster */
} /* esp_matter */
