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

/* This is a Generated File */

#pragma once
#include <esp_matter_data_model.h>

#include <descriptor.h>
#include <temperature_control.h>
#include <temperature_measurement.h>

#include <esp_matter_core.h>

#define ESP_MATTER_COOK_SURFACE_DEVICE_TYPE_ID 0x0077
#define ESP_MATTER_COOK_SURFACE_DEVICE_TYPE_VERSION 2

#define COOK_SURFACE_OPTIONAL_CLUSTER_TEMPERATURE_CONTROL  (1 << 0)
#define COOK_SURFACE_OPTIONAL_CLUSTER_TEMPERATURE_MEASUREMENT  (1 << 1)

namespace esp_matter {
namespace endpoint {
namespace cook_surface {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::temperature_control::config_t temperature_control;
    cluster::temperature_measurement::config_t temperature_measurement;
    uint32_t optional_clusters_mask;

    cluster::temperature_control::config_t &with_temperature_control()
    {
        optional_clusters_mask |= COOK_SURFACE_OPTIONAL_CLUSTER_TEMPERATURE_CONTROL;
        return temperature_control;
    }

    cluster::temperature_measurement::config_t &with_temperature_measurement()
    {
        optional_clusters_mask |= COOK_SURFACE_OPTIONAL_CLUSTER_TEMPERATURE_MEASUREMENT;
        return temperature_measurement;
    }

    config() : optional_clusters_mask(0) {}
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* cook_surface */
} /* endpoint */
} /* esp_matter */
