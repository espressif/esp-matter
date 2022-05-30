// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
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
#include <sdkconfig.h>
#if CONFIG_ZB_ENABLED
#include <esp_matter_bridge.h>

using esp_matter::node_t;

typedef struct app_zigbee_bridge_device {
    esp_matter_bridge_device_t *dev;
    uint8_t zigbee_endpointid;
    uint16_t zigbee_shortaddr;
    struct app_zigbee_bridge_device *next;
} app_zigbee_bridge_device_t;

app_zigbee_bridge_device_t *app_bridge_create_zigbee_device(node_t *node, uint8_t zigbee_endpointid,
                                                            uint16_t zigbee_shortaddr);

uint16_t app_bridge_get_zigbee_shortaddr_by_matter_endpointid(uint16_t matter_endpointid);

uint16_t app_bridge_get_matter_endpointid_by_zigbee_shortaddr(uint16_t zigbee_shortaddr);

app_zigbee_bridge_device_t *app_bridge_get_zigbee_device_by_matter_endpointid(uint16_t matter_endpointid);

app_zigbee_bridge_device_t *app_bridge_get_zigbee_device_by_zigbee_shortaddr(uint16_t zigbee_shortaddr);

esp_err_t app_bridge_remove_zigbee_device(app_zigbee_bridge_device_t *bridged_device);
#endif
