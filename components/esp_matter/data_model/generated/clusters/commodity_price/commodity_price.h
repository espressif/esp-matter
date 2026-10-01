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
namespace commodity_price {

namespace feature {
namespace forecasting {
uint32_t get_id();
esp_err_t add(cluster_t *cluster);
} /* forecasting */

} /* feature */

namespace attribute {
attribute_t *create_tariff_unit(cluster_t *cluster, uint8_t value);
attribute_t *create_currency(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_current_price(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
attribute_t *create_price_forecast(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count);
} /* attribute */

namespace command {
command_t *create_get_detailed_price_request(cluster_t *cluster);
command_t *create_get_detailed_price_response(cluster_t *cluster);
command_t *create_get_detailed_forecast_request(cluster_t *cluster);
command_t *create_get_detailed_forecast_response(cluster_t *cluster);
} /* command */

namespace event {
event_t *create_price_change(cluster_t *cluster);
} /* event */

typedef struct config {
    uint8_t tariff_unit;
    void *delegate;
    config() : tariff_unit(0), delegate(nullptr) {}
} config_t;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags);

} /* commodity_price */
} /* cluster */
} /* esp_matter */
