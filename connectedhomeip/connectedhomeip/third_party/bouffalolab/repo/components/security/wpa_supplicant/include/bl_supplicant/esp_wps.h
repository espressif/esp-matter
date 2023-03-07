// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESP_WPS_H__
#define __ESP_WPS_H__

#include <stdint.h>
#include <stdbool.h>
/* #include "esp_err.h" */
/* #include "esp_wifi_crypto_types.h" */
/* #include "esp_compiler.h" */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    wps_type_t wps_type;
    wps_factory_information_t factory_info;
} esp_wps_config_t;

#define WPS_CONFIG_INIT_DEFAULT(type) { \
    .wps_type = type, \
    .factory_info = {   \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(manufacturer, "ESPRESSIF")  \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(model_number, "ESP32")  \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(model_name, "ESPRESSIF IOT")  \
        ESP_COMPILER_DESIGNATED_INIT_AGGREGATE_TYPE_STR(device_name, "ESP STATION")  \
    }  \
}

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WPS_H__ */
