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

#include <esp_matter_bridge.h>

using esp_matter::node_t;

/** Bridged Device Type */
typedef enum {
    /** ZigBee */
    ESP_MATTER_BRIDGED_DEVICE_TYPE_ZIGBEE = 0,
    /** BLE Mesh */
    ESP_MATTER_BRIDGED_DEVICE_TYPE_BLEMESH,
} app_bridged_device_type_t;

/* Bridged Device Address */
typedef union {
    /** ZigBee */
    struct {
        uint8_t zigbee_endpointid;
        uint16_t zigbee_shortaddr;
    };
    /** BLE Mesh */
    struct {
        uint16_t blemesh_addr;
    };
} app_bridged_device_address_t;

/* Bridged Device */
typedef struct app_bridged_device {
    /** Bridged Device */
    esp_matter_bridge_device_t *dev;
    /** Type of Bridged Device */
    app_bridged_device_type_t dev_type;
    /** Address of Bridged Device */
    app_bridged_device_address_t dev_addr;
    /** Pointer of Next Bridged Device */
    struct app_bridged_device *next;
} app_bridged_device_t;

/** Bridged Device's Address APIs */
app_bridged_device_address_t app_bridge_zigbee_address(uint8_t zigbee_endpointid, uint16_t zigbee_shortaddr);

app_bridged_device_address_t app_bridge_blemesh_address(uint16_t blemesh_addr);

/** Bridged Device APIs */
app_bridged_device_t *app_bridge_create_bridged_device(node_t *node,
                        app_bridged_device_type_t bridged_device_type, app_bridged_device_address_t bridged_device_address);

app_bridged_device_t *app_bridge_get_device_by_matter_endpointid(uint16_t matter_endpointid);

esp_err_t app_bridge_remove_device(app_bridged_device_t *bridged_device);

/** ZigBee Device APIs */
app_bridged_device_t *app_bridge_get_device_by_zigbee_shortaddr(uint16_t zigbee_shortaddr);

uint16_t app_bridge_get_matter_endpointid_by_zigbee_shortaddr(uint16_t zigbee_shortaddr);

uint16_t app_bridge_get_zigbee_shortaddr_by_matter_endpointid(uint16_t matter_endpointid);

/** BLE Mesh Device APIs */
app_bridged_device_t *app_bridge_get_device_by_blemesh_addr(uint16_t blemesh_addr);

uint16_t app_bridge_get_matter_endpointid_by_blemesh_addr(uint16_t blemesh_addr);

uint16_t app_bridge_get_blemesh_addr_by_matter_endpointid(uint16_t matter_endpointid);
