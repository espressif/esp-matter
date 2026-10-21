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
#include <user_label.h>
#include <power_topology.h>
#include <on_off.h>
#include <electrical_alarm.h>
#include <electrical_protection_alarm.h>

#include <esp_matter_core.h>

#define ESP_MATTER_ELECTRICAL_CIRCUIT_BREAKER_DEVICE_TYPE_ID 0x0516
#define ESP_MATTER_ELECTRICAL_CIRCUIT_BREAKER_DEVICE_TYPE_VERSION 1

#define ELECTRICAL_CIRCUIT_BREAKER_OPTIONAL_CLUSTER_ON_OFF  (1 << 0)
#define ELECTRICAL_CIRCUIT_BREAKER_OPTIONAL_CLUSTER_ELECTRICAL_ALARM  (1 << 1)
#define ELECTRICAL_CIRCUIT_BREAKER_OPTIONAL_CLUSTER_ELECTRICAL_PROTECTION_ALARM  (1 << 2)

namespace esp_matter {
namespace endpoint {
namespace electrical_circuit_breaker {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::user_label::config_t user_label;
    cluster::power_topology::config_t power_topology;
    cluster::on_off::config_t on_off;
    cluster::electrical_alarm::config_t electrical_alarm;
    cluster::electrical_protection_alarm::config_t electrical_protection_alarm;
    uint32_t optional_clusters_mask;

    cluster::on_off::config_t &with_on_off()
    {
        optional_clusters_mask |= ELECTRICAL_CIRCUIT_BREAKER_OPTIONAL_CLUSTER_ON_OFF;
        return on_off;
    }

    cluster::electrical_alarm::config_t &with_electrical_alarm()
    {
        optional_clusters_mask |= ELECTRICAL_CIRCUIT_BREAKER_OPTIONAL_CLUSTER_ELECTRICAL_ALARM;
        return electrical_alarm;
    }

    cluster::electrical_protection_alarm::config_t &with_electrical_protection_alarm()
    {
        optional_clusters_mask |= ELECTRICAL_CIRCUIT_BREAKER_OPTIONAL_CLUSTER_ELECTRICAL_PROTECTION_ALARM;
        return electrical_protection_alarm;
    }

    config() : optional_clusters_mask(0) {}
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* electrical_circuit_breaker */
} /* endpoint */
} /* esp_matter */
