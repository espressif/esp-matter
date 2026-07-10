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
namespace color_control {

const uint8_t k_max_compensation_text_length = 254u;
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
    config() : color_loop_active(0), color_loop_direction(0), color_loop_time(0), color_loop_start_enhanced_hue(0), color_loop_stored_enhanced_hue(0) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* color_loop */

namespace xy {
typedef struct config {
    uint16_t current_x;
    uint16_t current_y;
    config() : current_x(24939), current_y(24701) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* xy */

namespace color_temperature {
typedef struct config {
    uint16_t color_temperature_mireds;
    uint16_t color_temp_physical_min_mireds;
    uint16_t color_temp_physical_max_mireds;
    uint16_t couple_color_temp_to_level_min_mireds;
    nullable<uint16_t> start_up_color_temperature_mireds;
    config() : color_temperature_mireds(250), color_temp_physical_min_mireds(1), color_temp_physical_max_mireds(65279), couple_color_temp_to_level_min_mireds(1), start_up_color_temperature_mireds(250) {}
} config_t;
uint32_t get_id();
esp_err_t add(cluster_t *cluster, config_t *config);
} /* color_temperature */

} /* feature */

namespace attribute {
attribute_t *create_current_hue(cluster_t *cluster, uint8_t value);
attribute_t *create_current_saturation(cluster_t *cluster, uint8_t value);
attribute_t *create_remaining_time(cluster_t *cluster, uint16_t value);
attribute_t *create_current_x(cluster_t *cluster, uint16_t value);
attribute_t *create_current_y(cluster_t *cluster, uint16_t value);
attribute_t *create_drift_compensation(cluster_t *cluster, uint8_t value);
attribute_t *create_compensation_text(cluster_t *cluster, char * value, uint16_t length);
attribute_t *create_color_temperature_mireds(cluster_t *cluster, uint16_t value);
attribute_t *create_color_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_options(cluster_t *cluster, uint8_t value);
attribute_t *create_number_of_primaries(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_primary_1_x(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_1_y(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_1_intensity(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_primary_2_x(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_2_y(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_2_intensity(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_primary_3_x(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_3_y(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_3_intensity(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_primary_4_x(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_4_y(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_4_intensity(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_primary_5_x(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_5_y(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_5_intensity(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_primary_6_x(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_6_y(cluster_t *cluster, uint16_t value);
attribute_t *create_primary_6_intensity(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_white_point_x(cluster_t *cluster, uint16_t value);
attribute_t *create_white_point_y(cluster_t *cluster, uint16_t value);
attribute_t *create_color_point_rx(cluster_t *cluster, uint16_t value);
attribute_t *create_color_point_ry(cluster_t *cluster, uint16_t value);
attribute_t *create_color_point_r_intensity(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_color_point_gx(cluster_t *cluster, uint16_t value);
attribute_t *create_color_point_gy(cluster_t *cluster, uint16_t value);
attribute_t *create_color_point_g_intensity(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_color_point_bx(cluster_t *cluster, uint16_t value);
attribute_t *create_color_point_by(cluster_t *cluster, uint16_t value);
attribute_t *create_color_point_b_intensity(cluster_t *cluster, nullable<uint8_t> value);
attribute_t *create_enhanced_current_hue(cluster_t *cluster, uint16_t value);
attribute_t *create_enhanced_color_mode(cluster_t *cluster, uint8_t value);
attribute_t *create_color_loop_active(cluster_t *cluster, uint8_t value);
attribute_t *create_color_loop_direction(cluster_t *cluster, uint8_t value);
attribute_t *create_color_loop_time(cluster_t *cluster, uint16_t value);
attribute_t *create_color_loop_start_enhanced_hue(cluster_t *cluster, uint16_t value);
attribute_t *create_color_loop_stored_enhanced_hue(cluster_t *cluster, uint16_t value);
attribute_t *create_color_capabilities(cluster_t *cluster, uint16_t value);
attribute_t *create_color_temp_physical_min_mireds(cluster_t *cluster, uint16_t value);
attribute_t *create_color_temp_physical_max_mireds(cluster_t *cluster, uint16_t value);
attribute_t *create_couple_color_temp_to_level_min_mireds(cluster_t *cluster, uint16_t value);
attribute_t *create_start_up_color_temperature_mireds(cluster_t *cluster, nullable<uint16_t> value);
} /* attribute */

namespace command {
command_t *create_move_to_hue(cluster_t *cluster);
command_t *create_move_hue(cluster_t *cluster);
command_t *create_step_hue(cluster_t *cluster);
command_t *create_move_to_saturation(cluster_t *cluster);
command_t *create_move_saturation(cluster_t *cluster);
command_t *create_step_saturation(cluster_t *cluster);
command_t *create_move_to_hue_and_saturation(cluster_t *cluster);
command_t *create_move_to_color(cluster_t *cluster);
command_t *create_move_color(cluster_t *cluster);
command_t *create_step_color(cluster_t *cluster);
command_t *create_move_to_color_temperature(cluster_t *cluster);
command_t *create_enhanced_move_to_hue(cluster_t *cluster);
command_t *create_enhanced_move_hue(cluster_t *cluster);
command_t *create_enhanced_step_hue(cluster_t *cluster);
command_t *create_enhanced_move_to_hue_and_saturation(cluster_t *cluster);
command_t *create_color_loop_set(cluster_t *cluster);
command_t *create_stop_move_step(cluster_t *cluster);
command_t *create_move_color_temperature(cluster_t *cluster);
command_t *create_step_color_temperature(cluster_t *cluster);
} /* command */

typedef struct config {
    uint8_t color_mode;
    uint8_t options;
    nullable<uint8_t> number_of_primaries;
    uint8_t enhanced_color_mode;
    uint16_t color_capabilities;
    uint16_t primary_1_x;
    uint16_t primary_1_y;
    nullable<uint8_t> primary_1_intensity;
    uint16_t primary_2_x;
    uint16_t primary_2_y;
    nullable<uint8_t> primary_2_intensity;
    uint16_t primary_3_x;
    uint16_t primary_3_y;
    nullable<uint8_t> primary_3_intensity;
    uint16_t primary_4_x;
    uint16_t primary_4_y;
    nullable<uint8_t> primary_4_intensity;
    uint16_t primary_5_x;
    uint16_t primary_5_y;
    nullable<uint8_t> primary_5_intensity;
    uint16_t primary_6_x;
    uint16_t primary_6_y;
    nullable<uint8_t> primary_6_intensity;
    config() : color_mode(1), options(0), number_of_primaries(0), enhanced_color_mode(1), color_capabilities(0), primary_1_x(0), primary_1_y(0), primary_1_intensity(0), primary_2_x(0), primary_2_y(0), primary_2_intensity(0), primary_3_x(0), primary_3_y(0), primary_3_intensity(0), primary_4_x(0), primary_4_y(0), primary_4_intensity(0), primary_5_x(0), primary_5_y(0), primary_5_intensity(0), primary_6_x(0), primary_6_y(0), primary_6_intensity(0) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* color_control */
} /* cluster */
} /* esp_matter */
