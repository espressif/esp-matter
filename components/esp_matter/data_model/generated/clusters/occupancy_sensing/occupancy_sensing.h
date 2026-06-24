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
namespace occupancy_sensing {

namespace feature {
namespace other {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* other */

namespace passive_infrared {
typedef struct config {
    uint16_t pir_unoccupied_to_occupied_delay;
    uint8_t pir_unoccupied_to_occupied_threshold;
    config() : pir_unoccupied_to_occupied_delay(0), pir_unoccupied_to_occupied_threshold(1) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* passive_infrared */

namespace ultrasonic {
typedef struct config {
    uint16_t ultrasonic_unoccupied_to_occupied_delay;
    uint8_t ultrasonic_unoccupied_to_occupied_threshold;
    config() : ultrasonic_unoccupied_to_occupied_delay(0), ultrasonic_unoccupied_to_occupied_threshold(1) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* ultrasonic */

namespace physical_contact {
typedef struct config {
    uint16_t physical_contact_unoccupied_to_occupied_delay;
    uint8_t physical_contact_unoccupied_to_occupied_threshold;
    config() : physical_contact_unoccupied_to_occupied_delay(0), physical_contact_unoccupied_to_occupied_threshold(1) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* physical_contact */

namespace active_infrared {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* active_infrared */

namespace radar {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* radar */

namespace rf_sensing {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* rf_sensing */

namespace vision {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* vision */

namespace occupancy_event {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* occupancy_event */

} /* feature */

namespace attribute {
attribute_t *create_occupancy(cluster_t *cluster, uint8_t value);
attribute_t *create_occupancy_sensor_type(cluster_t *cluster, uint8_t value);
attribute_t *create_occupancy_sensor_type_bitmap(cluster_t *cluster, uint8_t value);
attribute_t *create_hold_time(cluster_t *cluster, uint16_t value);
attribute_t *create_hold_time_limits(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_pir_occupied_to_unoccupied_delay(cluster_t *cluster, uint16_t value);
attribute_t *create_pir_unoccupied_to_occupied_delay(cluster_t *cluster, uint16_t value);
attribute_t *create_pir_unoccupied_to_occupied_threshold(cluster_t *cluster, uint8_t value);
attribute_t *create_ultrasonic_occupied_to_unoccupied_delay(cluster_t *cluster, uint16_t value);
attribute_t *create_ultrasonic_unoccupied_to_occupied_delay(cluster_t *cluster, uint16_t value);
attribute_t *create_ultrasonic_unoccupied_to_occupied_threshold(cluster_t *cluster, uint8_t value);
attribute_t *create_physical_contact_occupied_to_unoccupied_delay(cluster_t *cluster, uint16_t value);
attribute_t *create_physical_contact_unoccupied_to_occupied_delay(cluster_t *cluster, uint16_t value);
attribute_t *create_physical_contact_unoccupied_to_occupied_threshold(cluster_t *cluster, uint8_t value);
} /* attribute */

namespace event {
event_t *create_occupancy_changed(cluster_t *cluster);
} /* event */

typedef struct config {
    uint8_t occupancy;
    uint8_t occupancy_sensor_type;
    uint8_t occupancy_sensor_type_bitmap;
    struct {
        feature::passive_infrared::config_t passive_infrared;
        feature::ultrasonic::config_t ultrasonic;
        feature::physical_contact::config_t physical_contact;
    } features;
    uint32_t feature_flags;
    config() : occupancy(0), occupancy_sensor_type(0), occupancy_sensor_type_bitmap(0), feature_flags(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* occupancy_sensing */
} /* cluster */
} /* esp_matter */
