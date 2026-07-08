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
#include <commodity_price.h>
#include <commodity_tariff.h>

#include <esp_matter_core.h>

#define ESP_MATTER_ELECTRICAL_ENERGY_TARIFF_DEVICE_TYPE_ID 0x0513
#define ESP_MATTER_ELECTRICAL_ENERGY_TARIFF_DEVICE_TYPE_VERSION 1

#define ELECTRICAL_ENERGY_TARIFF_OPTIONAL_CLUSTER_COMMODITY_PRICE  (1 << 0)
#define ELECTRICAL_ENERGY_TARIFF_OPTIONAL_CLUSTER_COMMODITY_TARIFF  (1 << 1)

namespace esp_matter {
namespace endpoint {
namespace electrical_energy_tariff {

typedef struct config {
    cluster::descriptor::config_t descriptor;
    cluster::commodity_price::config_t commodity_price;
    cluster::commodity_tariff::config_t commodity_tariff;
    uint32_t optional_clusters_mask;

    cluster::commodity_price::config_t &with_commodity_price()
    {
        optional_clusters_mask |= ELECTRICAL_ENERGY_TARIFF_OPTIONAL_CLUSTER_COMMODITY_PRICE;
        return commodity_price;
    }

    cluster::commodity_tariff::config_t &with_commodity_tariff()
    {
        optional_clusters_mask |= ELECTRICAL_ENERGY_TARIFF_OPTIONAL_CLUSTER_COMMODITY_TARIFF;
        return commodity_tariff;
    }

    config() : optional_clusters_mask(0) {}
} config_t;

uint32_t get_device_type_id();
uint8_t get_device_type_version();
endpoint_t *create(node_t *node, config_t *config, uint8_t flags, void *priv_data);
esp_err_t add(endpoint_t *endpoint, config_t *config);
} /* electrical_energy_tariff */
} /* endpoint */
} /* esp_matter */
