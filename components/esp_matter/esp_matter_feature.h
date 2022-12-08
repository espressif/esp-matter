// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#include <esp_matter_core.h>
#include <stdint.h>

#define ESP_MATTER_NONE_FEATURE_ID 0x0000

/** Specific feature APIs
 *
 * These APIs also create the mandatory attributes and commands for the cluster for that particular feature. If the
 * mandatory attribute is not managed internally, then a config is present for that attribute. The constructor for the
 * config will set the attribute to the default value from the spec.
 *
 * If some standard feature is not present here, it can be added.
 */

namespace esp_matter {
namespace cluster {

namespace on_off {
namespace feature {
namespace lighting {

typedef struct config {
    bool global_scene_control;
    nullable<uint16_t> on_time;
    nullable<uint16_t> off_wait_time;
    nullable<uint8_t> start_up_on_off;
    config() : global_scene_control(1), on_time(0), off_wait_time(0), start_up_on_off(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* lighting */
} /* feature */
} /* on_off */

namespace level_control {
namespace feature {
namespace on_off {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* on_off */

namespace lighting {

typedef struct config {
    uint16_t remaining_time;
    uint8_t min_level;
    uint8_t max_level;
    nullable<uint8_t> start_up_current_level;
    config() : remaining_time(0), min_level(1), max_level(254), start_up_current_level(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* lighting */

namespace frequency {

typedef struct config {
    uint16_t current_frequency;
    uint16_t min_frequency;
    uint16_t max_frequency;
    config() : current_frequency(0), min_frequency(0), max_frequency(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* frequency */
} /* feature */
} /* level_control */

namespace color_control {
namespace feature {
namespace hue_saturation {

typedef struct config {
    uint8_t current_hue;
    uint8_t current_saturation;
    config() : current_hue(0), current_saturation(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* hue_saturation */

namespace color_temperature {

typedef struct config {
    uint16_t color_temperature_mireds;
    uint16_t color_temp_physical_min_mireds;
    uint16_t color_temp_physical_max_mireds;
    uint16_t couple_color_temp_to_level_min_mireds;
    nullable<uint16_t> startup_color_temperature_mireds;
    config() : color_temperature_mireds(0x00fa), color_temp_physical_min_mireds(0),
               color_temp_physical_max_mireds(0xfeff), couple_color_temp_to_level_min_mireds(0),
               startup_color_temperature_mireds(0x00fa) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* color_temperature */

namespace xy {

typedef struct config {
    uint16_t current_x;
    uint16_t current_y;
    config() : current_x(0x616b), current_y(0x607d) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* xy */

namespace enhanced_hue {

typedef struct config {
    uint16_t enhanced_current_hue;
    config() : enhanced_current_hue(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* enhanced_hue */

namespace color_loop {

typedef struct config {
    uint8_t color_loop_active;
    uint8_t color_loop_direction;
    uint16_t color_loop_time;
    uint16_t color_loop_start_enhanced_hue;
    uint16_t color_loop_stored_enhanced_hue;
    config() : color_loop_active(0), color_loop_direction(0), color_loop_time(0x19),
               color_loop_start_enhanced_hue(0x2300), color_loop_stored_enhanced_hue(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* color_loop */
} /* feature */
} /* color_control */

namespace window_covering {
namespace feature {
namespace lift {

typedef struct config {
    uint16_t number_of_actuations_lift;
    config() : number_of_actuations_lift(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* lift */

namespace tilt {

typedef struct config {
    uint16_t number_of_actuations_tilt;
    config() : number_of_actuations_tilt(0) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* tilt */

namespace position_aware_lift {

typedef struct config {
    uint16_t current_position_lift_percentage;
    uint16_t target_position_lift_percent_100ths;
    uint16_t current_position_lift_percent_100ths;
    config() : current_position_lift_percentage(), target_position_lift_percent_100ths(), current_position_lift_percent_100ths() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* position_aware_lift */

namespace absolute_position {

typedef struct config {
    uint16_t physical_closed_limit_lift;
    uint16_t current_position_lift;
    uint16_t installed_open_limit_lift;
    uint16_t installed_closed_limit_lift;
    uint16_t physical_closed_limit_tilt;
    uint16_t current_position_tilt;
    uint16_t installed_open_limit_tilt;
    uint16_t installed_closed_limit_tilt;
    config() : physical_closed_limit_lift(0), current_position_lift(0), installed_open_limit_lift(0), installed_closed_limit_lift(65534), physical_closed_limit_tilt(0), current_position_tilt(0), installed_open_limit_tilt(0), installed_closed_limit_tilt(65534) {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* absolute_position */

namespace position_aware_tilt {

typedef struct config {
    uint16_t current_position_tilt_percentage;
    uint16_t target_position_tilt_percent_100ths;
    uint16_t current_position_tilt_percent_100ths;
    config() : current_position_tilt_percentage(), target_position_tilt_percent_100ths(), current_position_tilt_percent_100ths() {}
} config_t;

uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);

} /* position_aware_tilt */
} /* feature */
} /* window_covering */

namespace diagnostics_network_wifi {
namespace feature {

namespace packets_counts {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* packets_counts */

namespace error_counts {

uint32_t get_id();
esp_err_t add(cluster_t *cluster);

} /* error_counts */

} /* feature */
} /* diagnostics_network_wifi */

} /* cluster */
} /* esp_matter */
