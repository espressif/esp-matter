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

#include <descriptor.h>
#include <identify.h>
#include <service_area.h>
#include <occupancy_sensing.h>
#include <ambient_context_sensing.h>

#include <esp_matter_core.h>

#define ESP_MATTER_AMBIENT_CONTEXT_SENSOR_DEVICE_TYPE_ID 0x0150
#define ESP_MATTER_AMBIENT_CONTEXT_SENSOR_DEVICE_TYPE_VERSION 2

#define AMBIENT_CONTEXT_SENSOR_OPTIONAL_CLUSTER_OCCUPANCY_SENSING  (1 << 0)
#define AMBIENT_CONTEXT_SENSOR_OPTIONAL_CLUSTER_AMBIENT_CONTEXT_SENSING  (1 << 1)

namespace esp_matter {
namespace endpoint {
namespace ambient_context_sensor {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::identify::config_t identify;
    cluster::service_area::config_t service_area;
    cluster::occupancy_sensing::config_t occupancy_sensing;
    cluster::ambient_context_sensing::config_t ambient_context_sensing;
    uint32_t optional_clusters_mask;

    cluster::occupancy_sensing::config_t &with_occupancy_sensing()
    {
        optional_clusters_mask |= AMBIENT_CONTEXT_SENSOR_OPTIONAL_CLUSTER_OCCUPANCY_SENSING;
        return occupancy_sensing;
    }

    cluster::ambient_context_sensing::config_t &with_ambient_context_sensing()
    {
        optional_clusters_mask |= AMBIENT_CONTEXT_SENSOR_OPTIONAL_CLUSTER_AMBIENT_CONTEXT_SENSING;
        return ambient_context_sensing;
    }

    config() : optional_clusters_mask(0) {}
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* ambient_context_sensor */
} /* endpoint */
} /* esp_matter */
